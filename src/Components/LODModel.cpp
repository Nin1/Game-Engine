#include "stdafx.h"
#include "LODModel.h"
#include "Camera.h"
#include <Core\FrameTime.h>
#include <Core\GameObject.h>
#include <Core\Input.h>
#include <GL/glew.h>
#include <glm/gtx/euler_angles.hpp>
#include <algorithm>
#include <fstream>

namespace snes
{
	const float LODModel::STIPPLE_PATTERN[16] = {
		1.0f/17.0f,  9.0f/17.0f,  3.0f/17.0f,  11.0f/17.0f,
		13.0f/17.0f, 5.0f/17.0f,  15.0f/17.0f, 7.0f/17.0f,
		4.0f/17.0f,  12.0f/17.0f, 2.0f/17.0f,  10.0f/17.0f,
		16.0f/17.0f, 8.0f/17.0f,  14.0f/17.0f, 6.0f/17.0f
	};
	const float LODModel::TRANSITION_DURATION_S = 0.0f;

	std::vector<LODValue> LODModel::m_lodValues;
	uint LODModel::m_instanceCount = 0;
	float LODModel::m_totalCost = 0;
	float LODModel::m_maxCost = 10000;
	std::weak_ptr<GameObject> LODModel::m_referenceObj;
	bool LODModel::m_useReferenceObj = false;
	
	void LODModel::Load(std::string modelName)
	{
		// Load the .lod file for this model
		std::string lodPath = modelName + ".lod";
		std::ifstream lodFile(lodPath, std::ios::in);

		if (!lodFile)
		{
			std::cout << "Error opening LOD file: " << lodPath << std::endl;
			return;
		}

		// Find how many LOD levels exist for this model
		std::string line;
		int totalModels;
		if (std::getline(lodFile, line))
		{
			totalModels = std::stoi(line);
		}

		for (int i = 0; i < totalModels; i++)
		{
			// Read current LOD mesh file
			std::getline(lodFile, line);
			auto mesh = Mesh::GetMesh(line.c_str());
			if (mesh)
			{
				m_meshes.push_back(mesh);

				float costCoefficient = 1;
				float costCoefficient2 = 1;

				float cost = mesh->GetNumFaces() * costCoefficient + mesh->GetVertexCount() * costCoefficient2;
				m_costs.push_back(cost);
			}

			// Read current LOD material file
			std::getline(lodFile, line);
			m_materials.push_back(Material::CreateMaterial(line.c_str()));
			m_shadowMaterials.push_back(Material::CreateShadowMaterial(line.c_str()));
		}

		if (m_meshes.size() == 0)
		{
			// No meshes were loaded
			std::cout << "Error: No LOD meshes loaded for model name " << modelName << std::endl;
			return;
		}

		m_currentMesh = m_meshes.size() - 1;
	}

	int LODModel::GetCurrentLOD() const
	{
		if (m_meshes.size() == 0 || m_camera.expired())
		{
			return -1;
		}

		// Use heuristics to determine the best mesh to show
		glm::vec3 cameraPos;
		if (m_useReferenceObj)
		{
			cameraPos = m_referenceObj.lock()->GetTransform().GetWorldPosition();
		}
		else
		{
			cameraPos = m_camera.lock()->GetTransform().GetWorldPosition();
		}

		float distanceToCamera = glm::length(m_transform.GetWorldPosition() - cameraPos);
		float distancePerLOD = (m_distanceLow - m_distanceHigh) / m_meshes.size();

		uint lodToShow = (uint)std::abs((distanceToCamera + m_distanceHigh) / distancePerLOD);
		lodToShow = std::max(lodToShow, (uint)0);
		lodToShow = std::min(lodToShow, (m_meshes.size() - 1));

		// Return that mesh
		return lodToShow;
	}

	void LODModel::FixedLogic()
	{
		m_currentMesh = 0;
		m_shownMeshCost = 0;
		CalculateEachLODValue();
		//PickBestMesh();
	}

	void LODModel::MainLogic()
	{
		// Update LOD transition
		if (m_transitionRemainingS > 0.0f)
		{
			m_transitionRemainingS -= FrameTime::GetLastFrameDuration();
		}
	}

	void LODModel::MainDraw(RenderPass renderPass, Camera& camera)
	{
		if (renderPass == GEOMETRY_PASS)
		{
			if (m_currentMesh != m_lastRenderedMesh && m_transitionRemainingS <= 0.0f)
			{
				// The displayed mesh has changed - begin a transition
				m_transitioningFromMesh = m_lastRenderedMesh;	// Set the last mesh as one to fade out
				m_lastRenderedMesh = m_currentMesh;
				m_transitionRemainingS = TRANSITION_DURATION_S;
			}
		}

		DrawCurrentMesh(renderPass, camera);

		if (m_transitionRemainingS > 0.0f)
		{
			// Draw previously selected mesh if transitioning
			DrawLastMesh(renderPass, camera);
		}

		glDisable(GL_POLYGON_STIPPLE);

		// Unbind the VBO and VAO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void LODModel::DrawCurrentMesh(RenderPass renderPass, Camera& camera)
	{
		Material* material = m_materials[m_lastRenderedMesh].get();

		if (renderPass == SHADOW_PASS)
		{
			//material = m_shadowMaterials[m_lastRenderedMesh].get();
		}

		PrepareTransformUniforms(camera, material);
		material->PrepareForRendering(m_transform, *m_camera.lock(), *m_meshes[m_lastRenderedMesh]);
		m_meshes[m_lastRenderedMesh]->PrepareForRendering();

		if (m_transitionRemainingS > 0.0f)
		{
			glEnable(GL_POLYGON_STIPPLE);
			float opacity = 1.0f - (m_transitionRemainingS / TRANSITION_DURATION_S);
			GenerateStipplePattern(opacity, m_stipplePattern);
			glPolygonStipple(m_stipplePattern);
		}

		// Draw the mesh
		if (material->GetUsePatches())
		{
			glDrawArrays(GL_PATCHES, 0, m_meshes[m_lastRenderedMesh]->GetVertexCount());
		}
		else
		{
			glDrawArrays(GL_TRIANGLES, 0, m_meshes[m_lastRenderedMesh]->GetVertexCount());
		}

	}

	void LODModel::DrawLastMesh(RenderPass renderPass, Camera& camera)
	{
		Material* material = m_materials[m_transitioningFromMesh].get();

		if (renderPass == SHADOW_PASS)
		{
			//material = m_shadowMaterials[m_transitioningFromMesh].get();
		}

		PrepareTransformUniforms(camera, material);
		material->PrepareForRendering(m_transform, camera, *m_meshes[m_transitioningFromMesh]);
		m_meshes[m_transitioningFromMesh]->PrepareForRendering();

		if (m_transitionRemainingS > 0.0f)
		{
			glEnable(GL_POLYGON_STIPPLE);
			float opacity = m_transitionRemainingS / TRANSITION_DURATION_S;
			// Render last mesh with an inverted stipple pattern so that it can fade out while the other mesh fades in, while keeping the object as a whole opaque
			InvertStipplePattern(m_stipplePattern);
			glPolygonStipple(m_stipplePattern);
		}

		// Draw the mesh
		if (material->GetUsePatches())
		{
			glDrawArrays(GL_PATCHES, 0, m_meshes[m_transitioningFromMesh]->GetVertexCount());
		}
		else
		{
			glDrawArrays(GL_TRIANGLES, 0, m_meshes[m_transitioningFromMesh]->GetVertexCount());
		}
	}

	void LODModel::GenerateStipplePattern(float opacity, GLubyte patternOut[128])
	{
		// Generate a 32x32 stipple pattern from the 4x4 ordered dithering pattern and opacity
		for (int l = 0; l < 8; l++)
		{
			for (int k = 0; k < 4; k++)
			{
				for (int i = 0; i < 4; i++)
				{
					GLubyte eightBits = 0;
					for (int j = 0; j < 4; j++)
					{
						if (opacity > STIPPLE_PATTERN[(4 * k) + j])
						{
							eightBits = eightBits | (1 << j);
							eightBits = eightBits | (1 << (j + 4));
						}
					}
					patternOut[(16 * l) + (4 * k) + i] = eightBits;
				}
			}
		}
	}

	void LODModel::InvertStipplePattern(GLubyte patternOut[128])
	{
		// Invert the bits of the stipple pattern
		for (int i = 0; i < 128; i++)
		{
			patternOut[i] = patternOut[i] ^ 255;
		}
	}

	void LODModel::PrepareTransformUniforms(Camera& camera, Material* mat)
	{
		auto transform = m_gameObject.GetTransform();
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), transform.GetWorldScale());
		glm::mat4 translate = glm::translate(glm::mat4(1.0f), transform.GetWorldPosition());
		glm::vec3 eulerAngles = transform.GetWorldRotationRadians();
		glm::mat4 eulerRotation = glm::eulerAngleYXZ(eulerAngles.y, eulerAngles.x, eulerAngles.z);

		glm::mat4 modelMat = translate * eulerRotation * scale;
		glm::mat4 viewMat = camera.GetViewMatrix();
		glm::mat4 projMat = camera.GetProjMatrix();

		mat->ApplyTransformUniforms(modelMat, viewMat, projMat);
	}

	const std::weak_ptr<Mesh> LODModel::GetMesh(uint lodLevel) const
	{
		if (lodLevel < m_meshes.size())
		{
			return m_meshes[lodLevel];
		}
		else
		{
			// No valid mesh chosen, return null pointer
			return std::weak_ptr<Mesh>();
		}
	}

	void LODModel::PickBestMesh()
	{
		float distanceFromCamera = glm::length(m_camera.lock()->GetTransform().GetWorldPosition() - m_transform.GetWorldPosition());

		m_currentMesh = m_meshes.size() - 1;
		if (distanceFromCamera < 200)
			m_currentMesh = m_currentMesh - 1;
		if (distanceFromCamera < 30)
			m_currentMesh = m_currentMesh - 1;
		if (distanceFromCamera < 20)
			m_currentMesh = m_currentMesh - 1;
		if (distanceFromCamera < 10)
			m_currentMesh = m_currentMesh - 1;

		m_currentMesh = std::max((int)m_currentMesh, 0);
	}

	int LODModel::CalculateEachLODValue()
	{
		float size = GetScreenSizeOfMesh(0);

		int bestIndex = 0;
		float bestValue = 0;
		
		for (uint i = 0; i < m_meshes.size(); i++)
		{
			uint numFaces = m_meshes[i]->GetNumFaces();	// @TODO: Calculate a proper cost heuristic
			uint numVertices = m_meshes[i]->GetVertexCount();

			float baseError = 0.5f;
			float accuracy = size / numFaces; //1.0f - ((baseError / numFaces) * (baseError / numFaces)); //1 - (BaseError / number of faces)^2
			float importance = 1.0f;
			float focus = 1.0f; // Proportional to distance from object to screen center
			float motion = 1.0f; // proportional to the ratio of the object's apparent speed to the size of an average polygon
			
			float benefit = size * accuracy * importance * focus * motion; // * hysteresis

			float value = benefit / m_costs[i];

			LODValue valueEntry = { this, i, m_costs[i], value };
			m_lodValues.push_back(valueEntry);
			// @TODO: Find the maximum "cost" for a frame, then select LODs for each model in descending order of value until the cost is completely claimed.
			// Maybe calculate all the "value"s in MainLogic(), then do LOD selection once.
		}
		
		return bestIndex;
	}

	float LODModel::GetScreenSizeOfMesh(int index)
	{
		//https://stackoverflow.com/questions/21648630/radius-of-projected-sphere-in-screen-space
		float fovy = m_camera.lock()->GetVerticalFoV();

		// Get the radius of the mesh's encapsulating sphere
		glm::vec3 worldScale = m_transform.GetWorldScale();
		float maxScale = std::max(std::max(worldScale.x, worldScale.y), worldScale.z);
		float r = m_meshes[index]->GetSize() * maxScale / 2.0f;

		// Get the distance between the camera and the mesh's origin
		/** Find distance from camera or distance from reference object? */
		glm::vec3 cameraPos;
		if (m_useReferenceObj)
		{
			cameraPos = m_referenceObj.lock()->GetTransform().GetWorldPosition();
		}
		else
		{
			cameraPos = m_camera.lock()->GetTransform().GetWorldPosition();
		}

		float d = glm::length(m_transform.GetWorldPosition() - cameraPos);
		if (r > d)
		{
			// Camera is inside object's bounding sphere
			// Display highest LOD
			return -1.0f;
		}

		// Find the "projected radius" of the mesh in normalized screen space
		float l = sqrtf(d*d - r*r);
		float pr = (1 / tanf(fovy / 2) * (r / l));
		return pr;
	}

	void LODModel::StartNewFrame()
	{
		m_lodValues.clear();
		m_lodValues.reserve(m_instanceCount * 3);
		m_totalCost = 0;
	}

	void LODModel::SortAndSetLODValues()
	{
		if (Input::GetKeyDown('-'))
		{
			m_maxCost -= 100000;
		}
		if (Input::GetKeyDown('='))
		{
			m_maxCost += 100000;
		}
		if (Input::GetKeyDown('o'))
		{
			m_useReferenceObj = !m_useReferenceObj;
		}
		
		// Sort m_lodValues by value
		std::sort(m_lodValues.begin(), m_lodValues.end(), [](const LODValue& a, const LODValue& b)
		{
			return a.value > b.value;
		});

		uint i = 0;
		while (m_totalCost < m_maxCost && i < m_lodValues.size())
		{
			m_lodValues[i].model->SetCurrentLOD(m_lodValues[i]);
			++i;
		}
	}

	void LODModel::SetCurrentLOD(LODValue& lodValue)
	{
		m_totalCost -= m_shownMeshCost;

		m_currentMesh = lodValue.meshIndex;
		m_shownMeshCost = lodValue.cost;

		m_totalCost += m_shownMeshCost;
		
	}

	void LODModel::SetCurrentLOD(uint index)
	{
		m_totalCost -= m_shownMeshCost;

		m_currentMesh = index;
		m_shownMeshCost = m_costs[index];

		m_totalCost += m_shownMeshCost;

	}
}

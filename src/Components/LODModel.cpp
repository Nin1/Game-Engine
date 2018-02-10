#include "stdafx.h"
#include "LODModel.h"
#include "Camera.h"
#include <Core\GameObject.h>
#include <Core\Input.h>
#include <GL/glew.h>
#include <glm/gtx/euler_angles.hpp>
#include <algorithm>
#include <fstream>

namespace snes
{
	std::vector<LODValue> LODModel::m_lodValues;
	uint LODModel::m_instanceCount = 0;
	float LODModel::m_totalCost = 0;
	float LODModel::m_maxCost = 5000000;
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
			}

			// Read current LOD material file
			std::getline(lodFile, line);
			m_materials.push_back(Material::CreateMaterial(line.c_str()));
		}

		if (m_meshes.size() == 0)
		{
			// No meshes were loaded
			std::cout << "Error: No LOD meshes loaded for model name " << modelName << std::endl;
			return;
		}

		m_meshToShow = m_meshes.size() - 1;
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
		m_meshToShow = 0;
		m_shownMeshCost = 0;
		CalculateEachLODValue();
	}

	void LODModel::MainDraw()
	{
		auto camera = m_camera.lock();
		if (!camera.get())
		{
			return;
		}

		m_materials[m_meshToShow]->PrepareForRendering();
		m_meshes[m_meshToShow]->PrepareForRendering();
		PrepareTransformUniforms(*camera, *m_materials[m_meshToShow]);

		// Draw the mesh
		glDrawArrays(GL_TRIANGLES, 0, m_meshes[m_meshToShow]->GetVertexCount());

		// Unbind the VBO and VAO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}


	void LODModel::PrepareTransformUniforms(Camera& camera, Material& mat)
	{
		auto transform = m_gameObject.GetTransform();
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), transform.GetWorldScale());
		glm::mat4 translate = glm::translate(glm::mat4(1.0f), transform.GetWorldPosition());
		glm::vec3 eulerAngles = transform.GetWorldRotationRadians();
		glm::mat4 eulerRotation = glm::eulerAngleYXZ(eulerAngles.y, eulerAngles.x, eulerAngles.z);

		glm::mat4 modelMat = translate * eulerRotation * scale;
		glm::mat4 viewMat = camera.GetViewMatrix();
		glm::mat4 projMat = camera.GetProjMatrix();

		mat.ApplyTransformUniforms(modelMat, viewMat, projMat);
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

	int LODModel::CalculateEachLODValue()
	{
		float costCoefficient = 1;
		float costCoefficient2 = 1;

		float size = GetScreenSizeOfMesh(0);

		int bestIndex = 0;
		float bestValue = 0;
		
		for (int i = 0; i < m_meshes.size(); i++)
		{
			uint numFaces = m_meshes[i]->GetNumFaces();	// @TODO: Calculate a proper cost heuristic
			uint numVertices = m_meshes[i]->GetVertexCount();

			float cost = numFaces * costCoefficient + numVertices * costCoefficient2;

			float baseError = 0.5f;
			float accuracy = size / numFaces; //1.0f - ((baseError / numFaces) * (baseError / numFaces)); //1 - (BaseError / number of faces)^2
			float importance = 1.0f;
			float focus = 1.0f; // Proportional to distance from object to screen center
			float motion = 1.0f; // proportional to the ratio of the object's apparent speed to the size of an average polygon
			
			float benefit = size * accuracy * importance * focus * motion; // * hysteresis

			float value = benefit / cost;

			LODValue valueEntry = { this, i, cost, value };
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

		int i = 0;
		while (m_totalCost < m_maxCost && i < m_lodValues.size())
		{
			m_lodValues[i].model->SetCurrentLOD(m_lodValues[i]);
			++i;
		}
	}

	void LODModel::SetCurrentLOD(LODValue& lodValue)
	{
		m_totalCost -= m_shownMeshCost;

		m_meshToShow = lodValue.meshIndex;
		m_shownMeshCost = lodValue.cost;

		m_totalCost += m_shownMeshCost;
	}
}

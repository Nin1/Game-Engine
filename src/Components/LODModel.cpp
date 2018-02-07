#include "stdafx.h"
#include "LODModel.h"
#include "Camera.h"
#include <Core\GameObject.h>
#include <GL/glew.h>
#include <glm/gtx/euler_angles.hpp>
#include <fstream>

namespace snes
{
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
	}

	int LODModel::GetCurrentLOD() const
	{
		if (m_meshes.size() == 0)
		{
			return -1;
		}

		// Use heuristics to determine the best mesh to show
		int lodToShow = 0;

		if (m_transform.GetWorldPosition().z < 0)
		{
			lodToShow = m_meshes.size() - 1;
		}

		// Return that mesh
		return lodToShow;
	}

	void LODModel::MainDraw()
	{
		auto camera = m_camera.lock();
		if (!camera.get())
		{
			return;
		}

		int lod = GetCurrentLOD();
		if (lod == -1)
		{
			return;
		}

		m_materials[lod]->PrepareForRendering();
		m_meshes[lod]->PrepareForRendering();
		PrepareTransformUniforms(*camera, *m_materials[lod]);

		// Draw the mesh
		glDrawArrays(GL_TRIANGLES, 0, m_meshes[lod]->GetVertexCount());

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

}

#include "stdafx.h"
#include "TessModel.h"
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
	std::weak_ptr<GameObject> TessModel::m_referenceObj;

	bool TessModel::m_useReferenceObj = false;
	
	void TessModel::Load(std::string modelName)
	{
		// Load the .tess file for this model
		std::string modelPath = modelName + ".tess";
		std::ifstream modelFile(modelPath, std::ios::in);

		if (!modelFile)
		{
			std::cout << "Error opening TessModel file: " << modelPath << std::endl;
			return;
		}
		
		// Read current LOD mesh file
		std::string line;
		std::getline(modelFile, line);
		m_mesh = Mesh::GetMesh(line.c_str());
		if (!m_mesh)
		{
			std::cout << "Error opening mesh file: " << line << std::endl;
			return;
		}

		m_mesh->GenNeighbourData();

		// Read current LOD material file
		std::getline(modelFile, line);
		m_material = Material::CreateMaterial(line.c_str());
		if (!m_material)
		{
			std::cout << "Error opening material file: " << line << std::endl;
			return;
		}
		m_shadowMaterial = Material::CreateShadowMaterial(line.c_str());
	}

	void TessModel::FixedLogic()
	{
	}

	void TessModel::MainLogic()
	{
	}

	void TessModel::MainDraw(RenderPass renderPass, Camera& camera)
	{
		Material* material = m_material.get();

		if (renderPass == SHADOW_PASS)
		{
			//material = m_shadowMaterial.get();
		}

		PrepareTransformUniforms(camera, material);
		material->PrepareForRendering(m_transform, camera);
		m_mesh->PrepareForRendering();

		// Draw the mesh
		if (material->GetUsePatches())
		{
			glDrawArrays(GL_PATCHES, 0, m_mesh->GetVertexCount());
		}
		else
		{
			glDrawArrays(GL_TRIANGLES, 0, m_mesh->GetVertexCount());
		}

		// Unbind the VBO and VAO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void TessModel::PrepareTransformUniforms(Camera& camera, Material* mat)
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
}

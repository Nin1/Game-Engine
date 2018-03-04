#include "stdafx.h"
#include "MeshRenderer.h"
#include <Core\GameObject.h>
#include <GL/glew.h>
#include <glm/gtx/euler_angles.hpp>
#include <fstream>

namespace snes
{

	MeshRenderer::MeshRenderer(GameObject& gameObject)
		: Component(gameObject)
	{
	}


	MeshRenderer::~MeshRenderer()
	{
	}

	void MeshRenderer::MainDraw(RenderPass renderPass, Camera& camera)
	{
		if ( !m_material.get() || !m_mesh.get())
		{
			return;
		}

		m_mesh->PrepareForRendering();

		// Set up shaders and uniforms
		m_material->PrepareForRendering();
		PrepareTransformUniforms(camera);

		// Draw the mesh
		glDrawArrays(GL_TRIANGLES, 0, m_mesh->GetVertexCount());

		/** Unbind the VBO and VAO */
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void MeshRenderer::PrepareTransformUniforms(Camera& camera)
	{
		auto transform = m_gameObject.GetTransform();
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), transform.GetWorldScale());
		glm::mat4 translate = glm::translate(glm::mat4(1.0f), transform.GetWorldPosition());
		glm::vec3 eulerAngles = transform.GetWorldRotationRadians();
		glm::mat4 eulerRotation = glm::eulerAngleYXZ(eulerAngles.y, eulerAngles.x, eulerAngles.z);

		glm::mat4 modelMat = translate * eulerRotation * scale;
		glm::mat4 viewMat = camera.GetViewMatrix();
		glm::mat4 projMat = camera.GetProjMatrix();

		m_material->ApplyTransformUniforms(modelMat, viewMat, projMat);
	}
}

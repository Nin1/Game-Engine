#include "stdafx.h"
#include "TessellatedMat.h"
#include "Components\Camera.h"
#include <Rendering\Mesh.h>
#include <SOIL/SOIL.h>
#include <algorithm>

namespace snes
{
	bool TessellatedMat::m_useTessellation = true;

	TessellatedMat::TessellatedMat() : Material(TESSELLATED_TEXTURED)
	{
		SetUniformSampler2D("tex1", 0);
		SetUniformSampler2D("dispMap", 1);
		SetUniformFloat("innerTessLevel", m_maxInnerTessLevel);
		SetUniformFloat("outerTessLevel", m_maxOuterTessLevel);
		SetUniformFloat("magnitude", m_displacementMagnitude);
		m_usePatches = true;
	}

	TessellatedMat::TessellatedMat(std::ifstream& params) : Material(TESSELLATED_TEXTURED)
	{
		SetUniformSampler2D("tex1", 0);
		SetUniformSampler2D("dispMap", 1);
		SetUniformFloat("innerTessLevel", m_maxInnerTessLevel);
		SetUniformFloat("outerTessLevel", m_maxOuterTessLevel);
		SetUniformFloat("magnitude", m_displacementMagnitude);
		SetUniformBool("hasDispMap", false);
		m_usePatches = true;

		params >> m_maxInnerTessLevel;
		m_maxOuterTessLevel = m_maxInnerTessLevel;
		SetUniformFloat("innerTessLevel", m_maxInnerTessLevel);
		SetUniformFloat("outerTessLevel", m_maxOuterTessLevel);

		if (params.eof())
		{
			return;
		}

		params >> m_pixelsPerPolygon;

		if (params.eof())
		{
			return;
		}

		std::string texturePath;
		params >> texturePath;

		m_textureID = SOIL_load_OGL_texture(
			texturePath.c_str(),
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
		);

		if (params.eof())
		{
			return;
		}

		params >> texturePath;

		m_dispMapID = SOIL_load_OGL_texture(
			texturePath.c_str(),
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
		);

		SetUniformBool("hasDispMap", true);

		if (params.eof())
		{
			return;
		}

		params >> m_displacementMagnitude;
		SetUniformFloat("magnitude", m_displacementMagnitude);
	}


	TessellatedMat::~TessellatedMat()
	{
	}

	void TessellatedMat::PrepareForRendering(Transform& transform, Camera& camera, Mesh& mesh)
	{
		// Toggle for turning tessellation on/off
		if (m_useTessellation)
		{
			float normalizedMeshRadius = std::fmin(1.0f, GetScreenSizeOfMesh(transform, camera, mesh));
			float pixelMeshRadius = normalizedMeshRadius * 1024.0f / 2.0f;
			float circleArea = 3.14159f * (pixelMeshRadius * pixelMeshRadius);
			int pixelsPerPolygon = (int)(circleArea / mesh.GetNumFaces());
			float desiredOuterTessLevel = std::fmax(1.0f, std::fmin(64, sqrt((float)pixelsPerPolygon / m_pixelsPerPolygon)));	// Max tessellation level is 64
			float desiredInnerTessLevel = std::fmax(1.0f, desiredOuterTessLevel - 1.0f);

			SetUniformFloat("innerTessLevel", std::fmin(m_maxInnerTessLevel, desiredInnerTessLevel));
			SetUniformFloat("outerTessLevel", std::fmin(m_maxOuterTessLevel, desiredOuterTessLevel));

			// Adjust displacement amount based on tessellation level

			float tessMultiplier = std::fmax(desiredOuterTessLevel / 64, normalizedMeshRadius);
			SetUniformFloat("magnitude", m_displacementMagnitude * tessMultiplier);
		}
		else
		{
			SetUniformFloat("innerTessLevel", 1);
			SetUniformFloat("outerTessLevel", 1);
		}

		Material::PrepareForRendering();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_textureID);
		glActiveTexture(GL_TEXTURE1);
		if (m_dispMapID != -1)
		{
			glBindTexture(GL_TEXTURE_2D, m_dispMapID);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		glPatchParameteri(GL_PATCH_VERTICES, 3);
	}

	void TessellatedMat::SetTexture(const char* texturePath)
	{
		m_textureID = SOIL_load_OGL_texture(
			texturePath,
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
		);
	}

	float TessellatedMat::GetScreenSizeOfMesh(Transform& transform, Camera& camera, Mesh& mesh)
	{
		//https://stackoverflow.com/questions/21648630/radius-of-projected-sphere-in-screen-space
		float fovy = camera.GetVerticalFoV();
		fovy *= 3.14159f / 180;

		// Get the radius of the mesh's encapsulating sphere
		glm::vec3 worldScale = transform.GetWorldScale();
		float maxScale = std::max(std::max(worldScale.x, worldScale.y), worldScale.z);
		float r = mesh.GetSize() * maxScale / 2.0f;

		// Get the distance between the camera and the mesh's origin
		/** Find distance from camera or distance from reference object? */
		glm::vec3 cameraPos;
		//		if (m_useReferenceObj)
		//		{
		//			cameraPos = m_referenceObj.lock()->GetTransform().GetWorldPosition();
		//		}
		//		else
		{
			cameraPos = camera.GetTransform().GetWorldPosition();
		}

		float d = glm::length(transform.GetWorldPosition() - cameraPos);
		if (r >= d)
		{
			return 1.0f;
		}

		// Find the "projected radius" of the mesh in normalized screen space
		float l = sqrtf(d*d - r*r);
		float pr = 1 / tanf(fovy) * r / l;
		return pr;
	}
}

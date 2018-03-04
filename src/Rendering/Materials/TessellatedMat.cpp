#include "stdafx.h"
#include "TessellatedMat.h"
#include <SOIL/SOIL.h>

namespace snes
{
	TessellatedMat::TessellatedMat() : Material(TESSELLATED_TEXTURED)
	{
		SetUniformSampler2D("tex1", 0);
		SetUniformSampler2D("dispMap", 1);
		SetUniformFloat("innerTessLevel", m_innerTessLevel);
		SetUniformFloat("outerTessLevel", m_outerTessLevel);
		SetUniformFloat("magnitude", m_displacementMagnitude);
		m_usePatches = true;
	}

	TessellatedMat::TessellatedMat(std::ifstream& params) : Material(TESSELLATED_TEXTURED)
	{
		SetUniformSampler2D("tex1", 0);
		SetUniformSampler2D("dispMap", 1);
		SetUniformFloat("innerTessLevel", m_innerTessLevel);
		SetUniformFloat("outerTessLevel", m_outerTessLevel);
		SetUniformFloat("magnitude", m_displacementMagnitude);

		std::string texturePath;
		std::getline(params, texturePath);

		m_textureID = SOIL_load_OGL_texture(
			texturePath.c_str(),
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
		);

		std::getline(params, texturePath);

		m_dispMapID = SOIL_load_OGL_texture(
			texturePath.c_str(),
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
		);
		m_usePatches = true;
	}


	TessellatedMat::~TessellatedMat()
	{
	}

	void TessellatedMat::PrepareForRendering()
	{
		Material::PrepareForRendering();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_textureID);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_dispMapID);
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
}

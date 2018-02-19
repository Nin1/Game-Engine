#include "stdafx.h"
#include "BillboardMat.h"
#include <Components\Transform.h>
#include <SOIL/SOIL.h>

namespace snes
{
	BillboardMat::BillboardMat() : Material(BILLBOARD)
	{
		SetUniformSampler2D("albedo", 0);
		SetUniformSampler2D("normal", 1);
	}

	BillboardMat::BillboardMat(std::ifstream& params) : Material(BILLBOARD)
	{
		SetUniformSampler2D("albedo", 0);
		SetUniformSampler2D("normal", 1);
		std::string line;

		// First line of material file is the size of the billboard in world units
		if (std::getline(params, line))
		{
			m_worldSize = std::stof(line);
		}

		// Second line is the path to the texture
		if (std::getline(params, line))
		{
			m_textureID = SOIL_load_OGL_texture(
				line.c_str(),
				SOIL_LOAD_AUTO,
				SOIL_CREATE_NEW_ID,
				SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
			);
		}

		// Third line is the path to the normal map
		if (std::getline(params, line))
		{
			m_normalTextureID = SOIL_load_OGL_texture(
				line.c_str(),
				SOIL_LOAD_AUTO,
				SOIL_CREATE_NEW_ID,
				SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
			);
		}
	}


	BillboardMat::~BillboardMat()
	{
	}

	void BillboardMat::PrepareForRendering(Transform& transform)
	{
		SetUniformVec3("centerWorldPos", transform.GetWorldPosition());
		SetUniformVec3("worldScale", transform.GetWorldScale() * m_worldSize);
		SetUniformBool("useNormalMap", m_normalTextureID != 0);

		Material::PrepareForRendering();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_normalTextureID);
	}

}

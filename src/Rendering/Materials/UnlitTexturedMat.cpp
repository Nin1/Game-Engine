#include "stdafx.h"
#include "UnlitTexturedMat.h"
#include <SOIL/SOIL.h>

namespace snes
{
	UnlitTexturedMat::UnlitTexturedMat() : Material(UNLIT_TEXTURED)
	{
		SetUniformSampler2D("tex1", 0);
	}

	UnlitTexturedMat::UnlitTexturedMat(std::ifstream& params) : Material(UNLIT_TEXTURED)
	{
		SetUniformSampler2D("tex1", 0);

		std::string texturePath;
		std::getline(params, texturePath);

		m_textureID = SOIL_load_OGL_texture(
			texturePath.c_str(),
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
		);
	}


	UnlitTexturedMat::~UnlitTexturedMat()
	{
	}

	void UnlitTexturedMat::PrepareForRendering()
	{
		Material::PrepareForRendering();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_textureID);
	}

	void UnlitTexturedMat::SetTexture(const char* texturePath)
	{
		m_textureID = SOIL_load_OGL_texture(
			texturePath,
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
		);
	}
}

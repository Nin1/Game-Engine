#pragma once
#include "..\Material.h"
#include <fstream>

namespace snes
{
	/** Tessellated Material
	  * Renders a textured mesh with tessellation. */
	class TessellatedMat : public Material
	{
	public:
		TessellatedMat();
		TessellatedMat(std::ifstream& params);
		~TessellatedMat();
		
		void PrepareForRendering() override;

		/** Load the texture for this mesh into OpenGL */
		void SetTexture(const char* texturePath);

	private:
		GLuint m_textureID = -1;
		GLuint m_dispMapID = -1;
		GLuint m_innerTessLevel = 2048;
		GLuint m_outerTessLevel = 2048;
		float m_displacementMagnitude = 10.0f;
	};
}

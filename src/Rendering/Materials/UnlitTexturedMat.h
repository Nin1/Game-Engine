#pragma once
#include "..\Material.h"
#include <fstream>

namespace snes
{
	/** Unlit Textured Material
	  * Renders a mesh with a texture.
	  * Perhaps incorrectly named, since the deferred lighting pipeline causes these to be lit quite nicely now :) */
	class UnlitTexturedMat : public Material
	{
	public:
		UnlitTexturedMat();
		UnlitTexturedMat(std::ifstream& params);
		~UnlitTexturedMat();
		
		void PrepareForRendering() override;

		/** Load the texture for this mesh into OpenGL */
		void SetTexture(const char* texturePath);

	private:
		GLuint m_textureID = -1;
	};
}

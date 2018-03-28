#pragma once
#include "..\Material.h"
#include <fstream>

namespace snes
{
	/** Tessellated Material
	  * Renders a textured mesh with tessellation. */
	class SilhouetteTessellatedMat : public Material
	{
	public:
		SilhouetteTessellatedMat();
		SilhouetteTessellatedMat(std::ifstream& params);
		~SilhouetteTessellatedMat();
		
		void PrepareForRendering(Transform& transform, Camera& camera) override;

		/** Load the texture for this mesh into OpenGL */
		void SetTexture(const char* texturePath);

		static void ToggleTessellation() { m_useTessellation = !m_useTessellation; }

	private:
		GLuint m_textureID = -1;
		GLuint m_dispMapID = -1;
		GLuint m_innerTessLevel = 1;
		GLuint m_outerTessLevel = 1;
		float m_displacementMagnitude = 1.0f;

		static bool m_useTessellation;
	};
}

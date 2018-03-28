#pragma once
#include "..\Material.h"
#include <fstream>

namespace snes
{
	/** Billboard Material
	  * A material always faces the camera */
	class BillboardMat : public Material
	{
	public:
		BillboardMat();
		BillboardMat(std::ifstream& params);
		~BillboardMat();

		void PrepareForRendering(Transform& transform, Camera& camera) override;

	private:
		GLuint m_textureID = 0;
		GLuint m_normalTextureID = 0;
		/** The size of the billboard in world units (before scale) */
		float m_worldSize;
	};
}

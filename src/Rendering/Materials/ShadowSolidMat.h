#pragma once
#include "..\Material.h"
#include <fstream>

namespace snes
{
	/** Solid Colour Material
	  * Renders an object untextured with a solid colour. */
	class ShadowSolidMat : public Material
	{
	public:
		ShadowSolidMat();
		ShadowSolidMat(std::ifstream& params);
		~ShadowSolidMat();

		void ApplyTransformUniforms(glm::mat4& model, glm::mat4& view, glm::mat4& proj) override;
	};
}

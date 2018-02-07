#pragma once
#include "..\Material.h"
#include <fstream>

namespace snes
{
	/** Solid Colour Material
	  * Renders an object untextured with a solid colour. */
	class LitColourMat : public Material
	{
	public:
		LitColourMat();
		LitColourMat(std::ifstream& params);
		~LitColourMat();

		/** Set the colour of the material (0 .. 1) */
		void SetColour(glm::vec3 colour);
	};
}

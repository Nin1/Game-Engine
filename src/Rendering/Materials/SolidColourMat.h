#pragma once
#include "..\Material.h"
#include <fstream>

namespace snes
{
	/** Solid Colour Material
	  * Renders an object untextured with a solid colour. */
	class SolidColourMat : public Material
	{
	public:
		SolidColourMat();
		SolidColourMat(std::ifstream& params);
		~SolidColourMat();

		/** Set the colour of the material (0 .. 1) */
		void SetColour(glm::vec3 colour);
	};
}

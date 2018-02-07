#include "stdafx.h"
#include "SolidColourMat.h"

namespace snes
{
	SolidColourMat::SolidColourMat() : Material(SOLID_COLOUR)
	{
		// Default to magenta
		glm::vec3 colour;
		colour.r = 1.0;
		colour.g = 0.0;
		colour.b = 1.0;
		SetUniformVec3("colour", colour);
	}

	SolidColourMat::SolidColourMat(std::ifstream& params) : Material(SOLID_COLOUR)
	{
		std::string line;
		glm::vec3 colour;

		std::getline(params, line);
		colour.r = std::stoi(line) / 255.0f;

		std::getline(params, line);
		colour.g = std::stoi(line) / 255.0f;

		std::getline(params, line);
		colour.b = std::stoi(line) / 255.0f;

		SetUniformVec3("colour", colour);
	}


	SolidColourMat::~SolidColourMat()
	{
	}

	void SolidColourMat::SetColour(glm::vec3 colour)
	{
		SetUniformVec3("colour", colour);
	}
}

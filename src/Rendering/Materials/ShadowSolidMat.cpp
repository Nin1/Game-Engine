#include "stdafx.h"
#include "ShadowSolidMat.h"

namespace snes
{
	ShadowSolidMat::ShadowSolidMat() : Material(SHADOW_SOLID)
	{
	}

	ShadowSolidMat::ShadowSolidMat(std::ifstream& params) : Material(SHADOW_SOLID)
	{
	}

	ShadowSolidMat::~ShadowSolidMat()
	{
	}

	void ShadowSolidMat::ApplyTransformUniforms(glm::mat4& model, glm::mat4& view, glm::mat4& proj)
	{
		SetUniformMat4("depthMVP", proj * view * model);
	}
}

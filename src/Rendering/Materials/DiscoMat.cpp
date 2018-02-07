#include "stdafx.h"
#include "DiscoMat.h"
#include <Core\FrameTime.h>

namespace snes
{
	glm::vec3 RgbFromHsv(float h, float s, float v)
	{
		// @TODO: Source code https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
		float hh, p, q, t, ff;
		long i;
		glm::vec3 out;

		if (s <= 0.0) {       // < is bogus, just shuts up warnings
			out.r = v;
			out.g = v;
			out.b = v;
			return out;
		}
		hh = h;
		if (hh >= 360.0f) hh = 0.0f;
		hh /= 60.0f;
		i = (long)hh;
		ff = hh - i;
		p = v * (1.0f - s);
		q = v * (1.0f - (s * ff));
		t = v * (1.0f - (s * (1.0f - ff)));

		switch (i) {
		case 0:
			out.r = v;
			out.g = t;
			out.b = p;
			break;
		case 1:
			out.r = q;
			out.g = v;
			out.b = p;
			break;
		case 2:
			out.r = p;
			out.g = v;
			out.b = t;
			break;

		case 3:
			out.r = p;
			out.g = q;
			out.b = v;
			break;
		case 4:
			out.r = t;
			out.g = p;
			out.b = v;
			break;
		case 5:
		default:
			out.r = v;
			out.g = p;
			out.b = q;
			break;
		}
		return out;
	}

	DiscoMat::DiscoMat() : Material(SOLID_COLOUR)
	{
		m_hue = 0;
	}

	DiscoMat::DiscoMat(std::ifstream& /*params*/) : Material(SOLID_COLOUR)
	{
		m_hue = 0;
	}


	DiscoMat::~DiscoMat()
	{
	}

	void DiscoMat::PrepareForRendering()
	{
		SetUniformVec3("colour", RgbFromHsv(m_hue, 1.0f, 1.0f));
		m_hue += FrameTime::GetLastFrameDuration() * 100;
		if (m_hue > 360.0f)
		{
			m_hue -= 360.0f;
		}

		Material::PrepareForRendering();
	}

}

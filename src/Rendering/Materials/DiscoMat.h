#pragma once
#include "..\Material.h"

namespace snes
{
	/** Disco Material
	  * A material that changes hue over time */
	class DiscoMat : public Material
	{
	public:
		DiscoMat();
		DiscoMat(std::ifstream& params);
		~DiscoMat();

		void PrepareForRendering() override;

	private:
		float m_hue;
	};
}

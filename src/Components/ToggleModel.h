#pragma once
#include "LODModel.h"
#include "TessModel.h"
#include <glm\glm.hpp>

namespace snes
{
	/** Axis-Aligned Bounding Box */
	class ToggleModel : public Component
	{
	public:
		ToggleModel(GameObject& gameObject) : Component(gameObject) {};
		~ToggleModel() {};

		void FixedLogic() override;
		void MainLogic() override;

		void MainDraw(RenderPass renderPass, Camera& camera) override;

		void SetModels(std::weak_ptr<LODModel> lodModel, std::weak_ptr<TessModel> tessModel);

	private:
		std::weak_ptr<LODModel> m_lodModel;
		std::weak_ptr<TessModel> m_tessModel;
		int m_lodIndex = 0;
		bool m_showTessModel = false;
	};
}

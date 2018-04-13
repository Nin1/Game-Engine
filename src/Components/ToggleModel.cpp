#include "stdafx.h"
#include "ToggleModel.h"
#include "LODModel.h"
#include "TessModel.h"
#include <Core\GameObject.h>
#include <Core\Input.h>
#include <glm/gtx/euler_angles.hpp>
#include <algorithm>

namespace snes
{
	void ToggleModel::FixedLogic()
	{
		if (Input::GetKeyDown('t'))
		{
			m_showTessModel = true;
		}
		if (Input::GetKeyDown('1'))
		{
			m_lodIndex = 0;
			m_showTessModel = false;
		}
		if (Input::GetKeyDown('2'))
		{
			m_lodIndex = 1;
			m_showTessModel = false;
		}
		if (Input::GetKeyDown('3'))
		{
			m_lodIndex = 2;
			m_showTessModel = false;
		}
		if (Input::GetKeyDown('4'))
		{
			m_lodIndex = 3;
			m_showTessModel = false;
		}
		if (Input::GetKeyDown('5'))
		{
			m_lodIndex = 4;
			m_showTessModel = false;
		}
		if (Input::GetKeyDown('6'))
		{
			m_lodIndex = 5;
			m_showTessModel = false;
		}
		if (Input::GetKeyDown('7'))
		{
			m_lodIndex = 6;
			m_showTessModel = false;
		}
		if (Input::GetKeyDown('8'))
		{
			m_lodIndex = 7;
			m_showTessModel = false;
		}
		if (Input::GetKeyDown('9'))
		{
			m_lodIndex = 8;
			m_showTessModel = false;
		}
		if (Input::GetKeyDown('0'))
		{
			m_lodIndex = 9;
			m_showTessModel = false;
		}
	}

	void ToggleModel::MainLogic()
	{
		m_lodModel.lock()->MainLogic();
	}

	void ToggleModel::MainDraw(RenderPass renderPass, Camera& camera)
	{
		if (m_showTessModel)
		{
			m_tessModel.lock()->MainDraw(renderPass, camera);
		}
		else
		{
			auto lodModel = m_lodModel.lock();
			int indexToRender = std::min(m_lodIndex, lodModel->GetLODCount() - 1);
			lodModel->SetCurrentLOD(indexToRender);
			lodModel->MainDraw(renderPass, camera);
		}
	}

	void ToggleModel::SetModels(std::weak_ptr<LODModel> lodModel, std::weak_ptr<TessModel> tessModel)
	{
		m_lodModel = lodModel;
		m_tessModel = tessModel;
		m_lodModel.lock()->Disable();
		m_tessModel.lock()->Disable();
	}
}

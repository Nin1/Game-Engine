#include "stdafx.h"
#include "Input.h"
#include <GL/freeglut.h>

namespace snes
{
	std::vector<int> Input::m_downKeys;
	std::vector<int> Input::m_heldKeys;
	std::vector<int> Input::m_upKeys;
	std::vector<int> Input::m_downMouseButtons;
	std::vector<int> Input::m_heldMouseButtons;
	std::vector<int> Input::m_upMouseButtons;
	glm::vec2 Input::m_mousePos(0, 0);
	glm::vec2 Input::m_lastMouseOffset(0, 0);
	bool Input::m_isWarping = false;

	Input::Input()
	{
	}

	Input::~Input()
	{
	}

	void Input::RefreshInputs()
	{
		m_upKeys.clear();
		m_downKeys.clear();
		m_upMouseButtons.clear();
		m_downMouseButtons.clear();
		m_lastMouseOffset = glm::vec2(0, 0);
	}

	void Input::SetKeyDown(int keyCode)
	{
		m_downKeys.push_back(keyCode);
		m_heldKeys.push_back(keyCode);
	}

	void Input::SetKeyUp(int keyCode)
	{
		m_upKeys.push_back(keyCode);
		// Remove key from heldKeys
		for (unsigned int i = 0; i < m_heldKeys.size(); ++i)
		{
			if (m_heldKeys[i] == keyCode)
			{
				m_heldKeys.erase(m_heldKeys.begin() + i);
			}
		}
	}

	void Input::SetMouseDown(int button)
	{
		m_downMouseButtons.push_back(button);
		m_heldMouseButtons.push_back(button);
	}

	void Input::SetMouseUp(int button)
	{
		m_upMouseButtons.push_back(button);
		// Remove key from heldKeys
		for (unsigned int i = 0; i < m_heldMouseButtons.size(); ++i)
		{
			if (m_heldMouseButtons[i] == button)
			{
				m_heldMouseButtons.erase(m_heldMouseButtons.begin() + i);
			}
		}
	}

	void Input::WarpMousePos(int x, int y)
	{
		m_isWarping = true;
		glutWarpPointer(x, y);
		m_mousePos = glm::vec2(x, y);
	}

	void Input::UpdateMousePos(int x, int y)
	{
		glm::vec2 newMousePos(x, y);
		m_lastMouseOffset = newMousePos - m_mousePos;
		m_mousePos = newMousePos;
	}

	bool Input::GetKeyDown(int keyCode)
	{
		return std::find(m_downKeys.begin(), m_downKeys.end(), keyCode) != m_downKeys.end();
	}

	bool Input::GetKeyHeld(int keyCode)
	{
		return std::find(m_heldKeys.begin(), m_heldKeys.end(), keyCode) != m_heldKeys.end();
	}

	bool Input::GetKeyUp(int keyCode)
	{
		return std::find(m_upKeys.begin(), m_upKeys.end(), keyCode) != m_upKeys.end();
	}

	bool Input::GetMouseDown(int button)
	{
		return std::find(m_downMouseButtons.begin(), m_downMouseButtons.end(), button) != m_downMouseButtons.end();
	}

	bool Input::GetMouseHeld(int button)
	{
		return std::find(m_heldMouseButtons.begin(), m_heldMouseButtons.end(), button) != m_heldMouseButtons.end();
	}

	bool Input::GetMouseUp(int button)
	{
		return std::find(m_upMouseButtons.begin(), m_upMouseButtons.end(), button) != m_upMouseButtons.end();
	}

	const glm::vec2& Input::GetMousePos()
	{
		return m_mousePos;
	}

	const glm::vec2& Input::GetLastMouseOffset()
	{
		return m_lastMouseOffset;
	}
}
#pragma once
#include <glm\vec2.hpp>

namespace snes
{
	class Input
	{
	public:
		Input();
		~Input();

		/** Call once-per-frame to refresh the up/down key states */
		void RefreshInputs();

		/** Set the new state of a given key */
		void SetKeyDown(int keyCode);
		void SetKeyUp(int keyCode);

		/** Set the new state of a given mouse button */
		void SetMouseDown(int button);
		void SetMouseUp(int button);
		/** Set the new position of the mouse */
		void UpdateMousePos(int x, int y);

		/** Set the new position of the mouse (Does not update the mouse offset) */
		static void WarpMousePos(int x, int y);

		/** @return true if the given key is in the desired state */
		static bool GetKeyDown(int keyCode);
		static bool GetKeyHeld(int keyCode);
		static bool GetKeyUp(int keyCode);

		/** @return true if the given mouse button is in the desired state */
		static bool GetMouseDown(int button);
		static bool GetMouseHeld(int button);
		static bool GetMouseUp(int button);

		/** @return the current mouse position */
		static const glm::vec2& GetMousePos();
		/** @return the difference in mouse position between this frame and the last */
		static const glm::vec2& GetLastMouseOffset();

		static bool m_isWarping;

	private:
		static std::vector<int> m_downKeys;
		static std::vector<int> m_heldKeys;
		static std::vector<int> m_upKeys;

		static std::vector<int> m_downMouseButtons;
		static std::vector<int> m_heldMouseButtons;
		static std::vector<int> m_upMouseButtons;

		static glm::vec2 m_mousePos;
		static glm::vec2 m_lastMouseOffset;
	};
}


#pragma once
#include <Core\Component.h>

namespace snes
{
	class CharController : public Component
	{
	public:
		CharController(GameObject& gameObject);
		~CharController();

		void Awake() override;
		void MainLogic() override;

		/** Set whether this character is being controlled by the keyboard */
		void SetCharControl(bool on) { m_charControl = on; }

	private:
		bool m_charControl = true;
	};
}

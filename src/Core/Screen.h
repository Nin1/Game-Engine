#pragma once

namespace snes
{
	class Application;

	class Screen
	{
	public:
		Screen() {};
		~Screen() {};

		/** @return the width of the window */
		int GetWidth() const { return m_width; }
		/** @return the height of the window */
		int GetHeight() const { return m_height; }
		/** Set the width and height of the window */
		void SetResolution(int width, int height);

	private:
		int m_width;
		int m_height;
	};
}


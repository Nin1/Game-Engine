#pragma once
#include "FrameTime.h"
#include "Input.h"
#include "Scene.h"
#include "Screen.h"

namespace snes
{
    class Application
    {
    public:
        Application(int argc, char* argv[]);
        ~Application();

		/** Start the glut main loop */
        void Run();
		/** Exit the glut main loop and terminate the program */
        void Exit();

		/** Return the window width and height in pixels */
		static void GetScreenSize(uint& outWidth, uint& outHeight);

    private:
		/** Resize the screen */
        static void Resize(int width, int height);
		/** Glut Display callback function */
        static void Display();
		/** Glut Main Loop callback function */
		static void MainLogic();
		/** Glut Keypress down callback function */
		static void KeyboardDown(unsigned char key, int x, int y);
		/** Glut Keypress up callback function */
		static void KeyboardUp(unsigned char key, int x, int y);
		/** Glut Mouse click callback function */
		static void MouseButton(int button, int state, int x, int y);
		/** Glut Mouse move callback function */
		static void MouseMove(int x, int y);

		static FrameTime m_frameTime;
		static Input m_input;
		static Scene m_currentScene;
		static Screen m_screen;

        int m_argc;
        std::vector<std::string> m_argv;

        /** Application state */
        bool m_running;

    };
}


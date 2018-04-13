#include "stdafx.h"
#include "Application.h"
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

#define OPENGL_DEBUG 1

namespace snes
{
	static void APIENTRY openglCallbackFunction(
		GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const void* userParam
	) {
		(void)source; (void)type; (void)id;
		(void)severity; (void)length; (void)userParam;
		fprintf(stderr, "%s\n", message);
		if (severity == GL_DEBUG_SEVERITY_HIGH) {
			fprintf(stderr, "Aborting...\n");
			//abort();
		}
	}

	FrameTime Application::m_frameTime;
	Input Application::m_input;
	Scene Application::m_currentScene;
	Screen Application::m_screen;

	GLuint vertexBuffer;

    Application::Application(int argc, char* argv[])
    {
        m_running = false;
        m_argc = argc;
        for (int i = 0; i < argc; ++i)
        {
            m_argv.push_back(argv[i]);
        }

        // Get window size from args
        int windowWidth = 1280;
        int windowHeight = 1024;
		m_screen.SetResolution(windowWidth, windowHeight);
        if (argc > 1)
        {
            windowWidth = std::stoi(argv[0]);
            windowHeight = std::stoi(argv[1]);
        }

        // Initialise glut and display window
        glutInit(&argc, argv);
        glutInitWindowSize(windowWidth, windowHeight);
		glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);	// GLUT_DOUBLE does V-Sync
        glutCreateWindow("James Engine");
        glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

		// Initialise glew
		if (glewInit() != GLEW_OK)
		{
			std::cout << "ERROR: Cannot initialize glew!";
			exit(1);
		}

		// Set glut input settings
		glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
		glutSetCursor(GLUT_CURSOR_NONE);

		// Set glut functions
		glutKeyboardFunc(KeyboardDown);
		glutKeyboardUpFunc(KeyboardUp);
        glutDisplayFunc(Display);
        glutReshapeFunc(Resize);
		glutIdleFunc(MainLogic);
		glutMotionFunc(MouseMove);
		glutPassiveMotionFunc(MouseMove);
		glutMouseFunc(MouseButton);


        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

#if OPENGL_DEBUG == 1
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(openglCallbackFunction, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);
#endif

		m_currentScene.InitialiseScene();
    }


    Application::~Application()
    {
    }

	void Application::Run()
	{
		if (m_running)
		{
			return;
		}

		m_running = true;

		glutMainLoop();

		m_running = false;
	}

	void Application::Exit()
	{
		m_running = false;

		glutLeaveMainLoop();
	}

    void Application::Resize(int width, int height)
    {
		m_screen.SetResolution(width, height);
    }

	void Application::GetScreenSize(uint& outWidth, uint& outHeight)
	{
		outWidth = m_screen.GetWidth();
		outHeight = m_screen.GetHeight();
	}

    void Application::Display()
    {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, m_screen.GetWidth(), m_screen.GetHeight());

		m_currentScene.MainDraw();

        glutSwapBuffers();
    }

	void Application::MainLogic()
	{
		m_frameTime.StartNewFrame();

		// Run FixedLogic on all components in the scene as many times as necessary
		for (uint i = 0; i < m_frameTime.GetPendingFixedLogicLoops(); ++i)
		{
			m_currentScene.FixedLogic();
		}

		// Run MainLogic on all components in the scene
		m_currentScene.MainLogic();

		// Clear keys pressed up/down
		m_input.RefreshInputs();

		if (m_input.GetKeyDown(GLUT_KEY_DELETE))
		{
			glutExit();
		}
		
		glutPostRedisplay();
	}

	void Application::KeyboardDown(unsigned char key, int /*x*/, int /*y*/)
	{
		m_input.SetKeyDown(key);
	}

	void Application::KeyboardUp(unsigned char key, int /*x*/, int /*y*/)
	{
		m_input.SetKeyUp(key);
	}

	void Application::MouseButton(int button, int state, int /*x*/, int /*y*/)
	{
		if (state == GLUT_UP)
		{
			m_input.SetMouseUp(button);
		}
		else
		{
			m_input.SetMouseDown(button);
		}
	}

	void Application::MouseMove(int x, int y)
	{
		if (!Input::m_isWarping)
		{
			m_input.UpdateMousePos(x, y);
		}
		else
		{
			Input::m_isWarping = false;
		}
	}
}
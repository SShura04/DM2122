
#include "Application.h"

//Include GLEW
#include <GL/glew.h>

//Include GLFW
#include <GLFW/glfw3.h>

//Include the standard C++ headers
#include <stdio.h>
#include <stdlib.h>

#include "ShootingMinigame.h"
#include "SP2.h"
#include "Jigglypuff_Minigame.h"

GLFWwindow* m_window;
const unsigned char FPS = 60; // FPS of this game
const unsigned int frameTime = 1000 / FPS; // time for each frame



unsigned Application::m_width;
unsigned Application::m_height;
unsigned Application::GameScene;
float Application::Minigame_timer = 0;
int Application::Cash = 100;

bool Application::isMuted = false;
bool Application::ismusicPlaying = false;

void resize_callback(GLFWwindow* window, int w, int h)
{
	Application::m_width = w;
	Application::m_height = h;
	glViewport(0, 0, w, h);
}

bool Application::IsMousePressed(unsigned short key) //0 - Left, 1 - Right, 2 - Middle
{
	return glfwGetMouseButton(m_window, key) != 0;
}

void Application::GetCursorPos(double* xpos, double* ypos)
{
	glfwGetCursorPos(m_window, xpos, ypos);
}
int Application::GetWindowWidth()
{
	return m_width;
}
int Application::GetWindowHeight()
{
	return m_height;
}


//Define an error callback
static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
	_fgetchar();
}

//Define the key input callback
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

bool Application::IsKeyPressed(unsigned short key)
{
	return ((GetAsyncKeyState(key) & 0x8001) != 0);
}

Application::Application()
{
}

Application::~Application()
{
}

//void resize_callback(GLFWwindow* window, int w, int h)
//{
//	glViewport(0, 0, w, h); //update opengl the new window size
//}


void Application::Init()
{
	//Set the error callback
	glfwSetErrorCallback(error_callback);

	//Initialize GLFW
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	//Set the GLFW window creation hints - these are optional
	glfwWindowHint(GLFW_SAMPLES, 4); //Request 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Request a specific OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //Request a specific OpenGL version
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL 


	//Create a window and create its OpenGL context
	//m_window = glfwCreateWindow(800, 600, "Test Window", NULL, NULL);


	m_width = 800;//1024;
	m_height = 600;//768;
	m_window = glfwCreateWindow(m_width, m_height, "Test Window", NULL, NULL);


	//If the window couldn't be created
	if (!m_window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	//This function makes the context of the specified window current on the calling thread. 
	glfwMakeContextCurrent(m_window);

	//Sets the key callback
	//glfwSetKeyCallback(m_window, key_callback);

	glewExperimental = true; // Needed for core profile
	//Initialize GLEW
	GLenum err = glewInit();

	//If GLEW hasn't initialized
	if (err != GLEW_OK)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		//return -1;
	}

	glfwSetWindowSizeCallback(m_window, resize_callback);
	GameScene = gs_game;
}

void Application::Run()
{
	//Main Loop
	Scene* scene1 = new SP2();
	Scene* scene2 = new Sp2_Minigame();
	Scene* scene3 = new shootingminigame();
	Scene* scene = scene1;
	scene1->Init();
	scene2->Init();
	scene3->Init();

	m_timer.startTimer();    // Start timer to calculate how long it takes to render this frame

	while (!glfwWindowShouldClose(m_window))
	{
		if (GameScene == gs_quit)
			break;
		else if (GameScene == gs_game)
		{
			scene->UseScene();
			scene = scene1;
		}
		else if (GameScene == gs_jigglypuffgame)
		{
			scene->UseScene();
			scene = scene2;
		}
		else if (GameScene == gs_shootingminigame)
		{
			scene->UseScene();
			scene = scene3;
		}

		bool quit = scene->Update(m_timer.getElapsedTime());
		scene->Render();
		//Swap buffers
		glfwSwapBuffers(m_window);
		//Get and organize events, like keyboard and mouse input, window resizing, etc...
		glfwPollEvents();
		m_timer.waitUntil(frameTime);       // Frame rate limiter. Limits each frame to a specified time in ms.   
		if (quit)
		{
			break;
		}

	} //Check if the ESC key had been pressed or if the window had been closed
	scene1->Exit();
	scene2->Exit();
	scene3->Exit();
	delete scene1;
	delete scene2;
	delete scene3;
}

void Application::Exit()
{
	//Close OpenGL window and terminate GLFW
	glfwDestroyWindow(m_window);
	//Finalize and clean up GLFW
	glfwTerminate();
}

void Application::ChangeScene(unsigned change2scene)
{
	GameScene = change2scene;
}

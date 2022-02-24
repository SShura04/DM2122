#ifndef APPLICATION_H
#define APPLICATION_H

#include "timer.h"

class Application
{
public:
	enum Gamestates
	{
		gs_quit,
		gs_mainmenu,
		gs_game,
		gs_jigglypuffgame,
		gs_shootingminigame,
	};
	Application();
	~Application();
	void Init();
	void Run();
	void Exit();
	static void ChangeScene(unsigned change2scene);
	static bool IsKeyPressed(unsigned short key);
	static bool IsMousePressed(unsigned short key);
	static void GetCursorPos(double* xpos, double* ypos);
	static int GetWindowWidth();
	static int GetWindowHeight();
	static unsigned m_width;
	static unsigned m_height;
	static unsigned GameScene;

	// Minigame timer
	static float Minigame_timer;
	static bool isMuted;
	static bool ismusicPlaying;

	// Cash
	static int Cash;
private:
	//Declare a window object
	StopWatch m_timer;
};

#endif
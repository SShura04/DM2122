#ifndef SHOOTING_H
#define SHOOTING_H

#include "Scene.h"
#include "CameraTest.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "Light.h"
#include "timer.h"
#include "Objects.h"
#include "Navmesh.h"
#include "PoliceAI.h"
#include <sstream>
#include <stdlib.h>

class shootingminigame : public Scene
{
	enum GEOMETRY_TYPE
	{
		GEO_NULL,
		GEO_AXES,
		GEO_QUAD,
		GEO_NAVINODES,
		GEO_NAVINODES2,
		GEO_LIGHTBALL,

		//skybox
		GEO_LEFT,
		GEO_RIGHT,
		GEO_TOP,
		GEO_BOTTOM,
		GEO_FRONT,
		GEO_BACK,

		GEO_TARGET,
		GEO_HANGAR,
		GEO_WALL,
		GEO_GUN,

		//Stamina
		GEO_STAMINA_BLACK,
		GEO_STAMINA_BAR,

		//text
		GEO_TEXT,
		GEO_GAMEOVER,
		GEO_GAMEOVERBACKGROUND,
		GEO_GAMEOVERBUTTON,
		NUM_GEOMETRY,
	};
	enum UNIFORM_TYPE
	{
		U_MVP = 0,
		U_MODELVIEW,
		U_MODELVIEW_INVERSE_TRANSPOSE,
		U_MATERIAL_AMBIENT,
		U_MATERIAL_DIFFUSE,
		U_MATERIAL_SPECULAR,
		U_MATERIAL_SHININESS,
		U_COLOR_TEXTURE_ENABLED,
		U_COLOR_TEXTURE,
		//add these enum in UNIFORM_TYPE before U_TOTAL
		//light 1
		U_LIGHT0_POSITION,
		U_LIGHT0_COLOR,
		U_LIGHT0_POWER,
		U_LIGHT0_KC,
		U_LIGHT0_KL,
		U_LIGHT0_KQ,
		U_LIGHT0_TYPE,
		U_LIGHT0_SPOTDIRECTION,
		U_LIGHT0_COSCUTOFF,
		U_LIGHT0_COSINNER,
		U_LIGHT0_EXPONENT,

		//light 2
		U_LIGHT1_POSITION,
		U_LIGHT1_COLOR,
		U_LIGHT1_POWER,
		U_LIGHT1_KC,
		U_LIGHT1_KL,
		U_LIGHT1_KQ,
		U_LIGHT1_TYPE,
		U_LIGHT1_SPOTDIRECTION,
		U_LIGHT1_COSCUTOFF,
		U_LIGHT1_COSINNER,
		U_LIGHT1_EXPONENT,

		U_LIGHTENABLED,
		U_NUMLIGHTS,

		//for text
		U_TEXT_ENABLED,
		U_TEXT_COLOR,
		U_TOTAL,
	};
	enum GAME_STATES
	{
		gs_mainmenu,
		gs_env,
		gs_credits,
		gs_states
	};
	enum GAME_OBJECTS //game objects
	{
		hb_hangar,
		//collision of hangar's insides
		hb_wall1,
		hb_wall2,
		hb_wall3,
		hb_wall4,
		hb_hangarwall,
		hb_table,
		hb_table1,
		hb_target1,
		hb_target2,
		hb_target3,
		hb_count,
		hb_gun,

		hb_total,
	};
	enum MENUBUTTON
	{
		mb_none,
		mb_start,
		mb_credits,
		mb_quit,
	};
	enum INTERACTIONS
	{
		i_nthin,
		i_count,
	};
public:
	shootingminigame();
	~shootingminigame();

	virtual void Init();
	virtual bool Update(double dt);
	virtual bool UpdateMainMenu();
	virtual void UpdateENV(double dt);
	virtual void UpdateCredits(double dt);
	virtual void Render();
	virtual void RenderMainmenu();
	virtual void RenderENV();
	virtual void RenderCredits();
	virtual void Exit();
	virtual void UseScene();


private:
	unsigned m_vertexArrayID;
	unsigned gamestate;
	unsigned interact;
	unsigned count;
	Mesh* meshList[NUM_GEOMETRY];
	Objects objectlist[hb_total];
	Objects interactionhitboxes[i_count];
	Objects player;
	Vector3 savedposition, savedtarget;
	std::vector <Objects> targets;
	float shootcooldown;

	unsigned score = 0;
	float timer;
	unsigned playergold, playerpunchpower, playerwater, shopselect, textprogress, pricemultiplier, playerwood, treegrowthstage;
	bool playerhasseed, playerhaskey, lackofmoney, playerboughtitem, lackofwood, playerlost, lackofwater;
	bool house2locked, treeplanted;
	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];
	bool resume, escbuttonstate, triedtoopendoor, rendernodes, gameover;
	unsigned buttonhover;

	float NPClookAtPlayerAngle = 0, ShopKeeperlookAtPlayerAngle = 0, PlayerandNpcRotationSpeed = 0, PlayerandShopKeeperRotationSpeed = 0;
	float PrevRotation = 0;


	float rotateAngle, gamemovementspeed, playerscore;
	float timesincelastbuttonpress = 0, timesincelastpunch;
	bool DrawAxis, InWindow, mousehidden, renderhitboxes, checkcollision, inshop, ingame, chatting;
	HWND window; RECT rect;
	LPPOINT mousepos;
	Vector3 camerapos;
	bool debug;


	std::ostringstream UIstringoutput;

	CameraTest camera;

	MS modelStack, viewStack, projectionStack;

	float fps;
	std::string fpstext;

	Light light[2];

	Vector3 imagepos;
	Vector3 imagedimensions;

	//functions for rendering
	void RenderMesh(Mesh* mesh, bool enableLight);
	void RenderSkybox(Vector3 skyboxoffset);
	void RenderText(Mesh* mesh, std::string text, Color color);
	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);
	void RenderMeshOnScreen(Mesh* mesh, Vector3 size, float rotate, float x, float y);

	// npc and police
	void EnemyMove(unsigned ObjectID, float& x, float& z, double dt, float speed);
	void NPCMove(unsigned ObjectID, float& x, float& z, double dt, int choice);
	float NPCX, NPCZ;




	float DayandNightSkyboxTransitioning = 0.5;

	// Interaction dialogue
	bool DialogueBoxOpen = false;
	std::string GD_PrintLine1 = "";
	std::string GD_PrintLine2 = "";
	std::string GD_PrintLine3 = "";
	int Dialogue = 1;
	int randomtext = 0;
	int randomscam = 0;
	int randomgreet = 0;
	int randomsuccess = 0;
	int randomfail = 0;
	std::string person;
	// TV
	bool checkedtv = false;



	//other
	bool isinhouse = false;
	float rotateSkybox = 0;
	float rotateSunandMoon = 0;
	float SizeofStamina = 40;

	// Spacing of text
	int textMaxWidth;
	int textSpacing[256];
	Vector3 PlayerCollision(unsigned count, float x, float z);
	Vector3 CollisionCircleRect(float cx, float cz, float radius, float rx, float rz, float rw, float rb);
	bool CollisionCircleRect1(float cx, float cz, float radius, float rx, float rz, float rw, float rb);

	Vector3 prev;

	// Stars wanted
	int Stars = 0;
	float timer_blinking = 0;
	float timer_wanted = 0;
	float timer_Wanted_Chase = 0;
	float speed_police = 2;
};

#endif
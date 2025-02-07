#ifndef SCENE_UI_H
#define SCENE_UI_H

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

class SP2 : public Scene
{
	enum GEOMETRY_TYPE
	{
		GEO_CUBE1,
		GEO_AXES,
		GEO_QUAD,
		GEO_SHOPUI,
		GEO_SOUND_MUTE,
		GEO_SOUND_UNMUTE,
		GEO_GAMEOVERBACKGROUND,
		GEO_GAMEOVER,
		GEO_GAMEOVERBUTTON,
		GEO_LIGHTBALL,

		//skybox
		GEO_LEFT,
		GEO_RIGHT,
		GEO_TOP,
		GEO_BOTTOM,
		GEO_FRONT,
		GEO_BACK,

		//houses
		GEO_HOUSE1, //player's house
		GEO_HOUSE2,
		GEO_HOUSE3,
		GEO_HOUSE4,
		GEO_HOUSE5,
		GEO_NPC1,
		GEO_NPC2,
		GEO_POLICE,

		GEO_WATCH,
		GEO_NECKLACE,
		GEO_RING,
		GEO_DIALOGUEUI,

		//walls
		//GEO_OUTERWALLS,

		//others
		GEO_CONCRETE_PAVEMENT,
		GEO_ROAD,
		GEO_CITY_CENTRE_FLOOR,
		GEO_BIN,
		GEO_BENCH,
		GEO_LAMPPOST,

		//Home furniture
		GEO_BED,
		GEO_DESK,
		GEO_LAPTOP,
		GEO_TABLE,
		GEO_TV,
		GEO_CEILING,

		//Shop furnitutre
		GEO_SHOPPROPSHELF,
		GEO_COUNTER,
		GEO_TABLESHOP,
		GEO_RADIO,
		GEO_WASHERDRYER,


		//hitboxes
		GEO_HITBOX,
		GEO_HITBOX2,


		GEO_CONTROLSTAB,
		GEO_CONTROLSCREEN,

		//garbage
		GEO_GARBAGE,

		//Stars
	    GEO_STAR1,
		GEO_STAR2,
		GEO_STAR3,
		GEO_STAR4,
		GEO_STAR5,
		GEO_STAR1_Grey,
		GEO_STAR2_Grey,
		GEO_STAR3_Grey,
		GEO_STAR4_Grey,
		GEO_STAR5_Grey,

		//Stamina
		GEO_STAMINA_BLACK,
		GEO_STAMINA_BAR,

		// Phone
		GEO_PHONEBODY,
		GEO_PHONESCREEN,

		//Cash
		GEO_CASH,

		//Inv
		GEO_HOTBAR,


		//text
		GEO_TEXT,
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

		U_LIGHT2_POSITION,
		U_LIGHT2_COLOR,
		U_LIGHT2_POWER,
		U_LIGHT2_KC,
		U_LIGHT2_KL,
		U_LIGHT2_KQ,
		U_LIGHT2_TYPE,
		U_LIGHT2_SPOTDIRECTION,
		U_LIGHT2_COSCUTOFF,
		U_LIGHT2_COSINNER,
		U_LIGHT2_EXPONENT,

		U_LIGHT3_POSITION,
		U_LIGHT3_COLOR,
		U_LIGHT3_POWER,
		U_LIGHT3_KC,
		U_LIGHT3_KL,
		U_LIGHT3_KQ,
		U_LIGHT3_TYPE,
		U_LIGHT3_SPOTDIRECTION,
		U_LIGHT3_COSCUTOFF,
		U_LIGHT3_COSINNER,
		U_LIGHT3_EXPONENT,

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

		//houses
		hb_HOUSE1,
		hb_HOUSE2,
		hb_HOUSE3,
		hb_HOUSE4,
		hb_HOUSE5,
		hb_HOUSE6,
		hb_HOUSE7,
		hb_HOUSE8,
		hb_HOUSE9,
		hb_HOUSE10,
		hb_HOUSE11,
		hb_HOUSE12,
		hb_HOUSE13,
		hb_HOUSE14,
		hb_HOUSE15,
		hb_HOUSE16,
		hb_HOUSE17,
		hb_HOUSE18,
		hb_HOUSE19,
		hb_HOUSE20,
		hb_HOUSE21,
		hb_HOUSE22,
		hb_HOUSE23,
		hb_HOUSE24,
		hb_HOUSE25,
		hb_HOUSE26,
		hb_HOUSE27,
		hb_HOUSE28,
		hb_HOUSE29,
		hb_HOUSE30,
		hb_HOUSE31,
		hb_HOUSE32,
		hb_HOUSE33,

		hb_HOUSE_TEST,
		hb_HOUSE_TEST2,
		hb_HOUSE_TEST3,
		hb_HOUSE_TEST4,


		hb_BENCH1,
		hb_BENCH2,
		hb_BENCH3,
		hb_BENCH4,
		hb_BENCH5,


		hb_BIN1,
		hb_BIN2,
		hb_BIN3,
		hb_BIN4,
		hb_BIN5,
		hb_BIN6,
		hb_BIN7,

		//Npc
		hb_NPC1,
		hb_NPC2,
		hb_NPC3,
		hb_POLICE1,
		hb_POLICE2,
		hb_POLICE3,
		hb_POLICE4,
		hb_POLICE5,

		//End of collision
		hb_count,


		//interior of player house
		hb_HOUSE34,
		// Shop Interior
		hb_HOUSE35,

		//furniture
		hb_CEILINGHOUSE,
		hb_CEILINGSHOP,
		hb_BED,
		hb_DESK,
		hb_LAPTOP,
		hb_TABLE,
		hb_TV,
		hb_PHONE,
		hb_WALLSHOP,
		hb_SHOPSELLTABLE,
		hb_SHOPTABLE,
		hb_SHOPPROPTV,
		hb_SHOPPROPSHELF,
		hb_SHOPPROPRADIO,
		hb_SHOPPROPWASHERDRYER,
		hb_ShopKeeper,

		// Collision of the wall interior of the house
		hb_Wall,
		hb_Wall2,
		hb_Wall3,
		hb_Wall4,
		hb_WallSHOP1,
		hb_WallSHOP2,
		hb_WallSHOP3,
		hb_WallSHOP4,


		// Bed collision
		hb_WallBed,
		hb_WallDesk,
		hb_WallTV,


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
	SP2();
	~SP2();

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
	Hitbox fist1, fist2;
	float playerfist1, playerfist2;
	bool playerfist1extend, playerfist2extend;
	Vector3 savedposition, savedtarget;
	Navmesh* navigationmesh;
	std::vector <PoliceAI*> polices;


	unsigned playergold, playerpunchpower, playerwater, shopselect, textprogress, pricemultiplier, playerwood, treegrowthstage;
	bool playerhasseed, playerhaskey, lackofmoney, playerboughtitem, lackofwood, playerlost, lackofwater;
	bool house2locked, treeplanted;
	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];
	bool resume, escbuttonstate, triedtoopendoor, rendernodes;
	unsigned buttonhover;

	float ShopKeeperlookAtPlayerAngle = 0;
	float PlayerandShopKeeperRotationSpeed = 0;

	float NPC1lookAtPlayerAngle = 0, PlayerandNpc1RotationSpeed = 0;
	float NPC2lookAtPlayerAngle = 0, PlayerandNpc2RotationSpeed = 0;
	float NPC3lookAtPlayerAngle = 0, PlayerandNpc3RotationSpeed = 0;

	// Npc movement
	float NPCX, NPCZ;
	bool NPC1Front = true, NPC1Back = false;
	float PrevRotationNPC1 = 0;
	float NPCX2, NPCZ2;
	bool NPC2Front = true, NPC2Back = false;
	float PrevRotationNPC2 = 0;
	float NPCX3, NPCZ3;
	bool NPC3Front = true, NPC3Back = false;
	float PrevRotationNPC3 = 90;
	bool AllowNPC1Movement = true;
	bool AllowNPC2Movement = true;
	bool AllowNPC3Movement = true;


	//bin cooldowns
	float bin1cooldown = 0;
	float bin2cooldown = 0;
	float bin3cooldown = 0;
	float bin4cooldown = 0;
	float bin5cooldown = 0;
	float bin6cooldown = 0;
	float bin7cooldown = 0;

	float ArrowRotate = 0;

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

	Light light[4];

	Vector3 imagepos;
	Vector3 imagedimensions;

	//functions for rendering
	void RenderMesh(Mesh* mesh, bool enableLight);
	void RenderSkybox(Vector3 skyboxoffset);
	void RenderText(Mesh* mesh, std::string text, Color color);
	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);
	void RenderMeshOnScreen(Mesh* mesh, Vector3 size, float rotate, float x, float y);



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
	int failshop = 0;
	int failNPC = 0;
	std::string person;
	// TV
	bool checkedtv = false;
	bool isShopOpen = false;

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

	// Stars wanted
	int Stars = 0;
	float timer_blinking = 0;
	float timer_wanted = 0;
	float timer_Wanted_Chase = 0;
	float speed_police = 2;
	bool canseeplayer = false;
	bool gameover = false;


	// bin
	int finditemchance;
	bool talkshopkeep;
	bool ringfound;
	int rings = 0;
	int watches = 0;
	int necklace = 0;
	bool ringscam = false;
	bool watchscam = false;
	bool necklacescam = false;
	bool inventoryopen = false;

	//Inv
	int item1 = 0, item2 = 0, item3 = 0;
	bool isSufficient = false;
	bool ShopUI_Status = false;
	bool controlopen;

	//Power ups
	float item1Activetime = 0;
	float item2Activetime = 0;
	bool item1_active = false;
	bool item2_active = false;
};

#endif

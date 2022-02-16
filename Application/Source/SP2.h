#ifndef SCENE_SP2_H
#define SCENE_SP2_H

#include "Scene.h"
#include "CameraTest.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "Light.h"
#include "timer.h"
#include "Objects.h"
#include <sstream>
#include <stdlib.h>

class SP2 : public Scene
{
	enum GEOMETRY_TYPE
	{
		GEO_AXES,
		GEO_QUAD,
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
		GEO_DIALOGUEUI,

		//walls
		//GEO_OUTERWALLS,

		//others
		GEO_CONCRETE_PAVEMENT,
		GEO_ROAD,
		GEO_CITY_CENTRE_FLOOR,

		//hitboxes
		GEO_HITBOX,
		GEO_HITBOX2,

		//text
		GEO_TEXT_CALIBRI,
		GEO_TEXT,
		GEO_TEXT_COMICSANS,
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

		//light 3
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

		//light 4
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

		//light 5
		U_LIGHT4_POSITION,
		U_LIGHT4_COLOR,
		U_LIGHT4_POWER,
		U_LIGHT4_KC,
		U_LIGHT4_KL,
		U_LIGHT4_KQ,
		U_LIGHT4_TYPE,
		U_LIGHT4_SPOTDIRECTION,
		U_LIGHT4_COSCUTOFF,
		U_LIGHT4_COSINNER,
		U_LIGHT4_EXPONENT,

		//light 6
		U_LIGHT5_POSITION,
		U_LIGHT5_COLOR,
		U_LIGHT5_POWER,
		U_LIGHT5_KC,
		U_LIGHT5_KL,
		U_LIGHT5_KQ,
		U_LIGHT5_TYPE,
		U_LIGHT5_SPOTDIRECTION,
		U_LIGHT5_COSCUTOFF,
		U_LIGHT5_COSINNER,
		U_LIGHT5_EXPONENT,

		//light 7
		U_LIGHT6_POSITION,
		U_LIGHT6_COLOR,
		U_LIGHT6_POWER,
		U_LIGHT6_KC,
		U_LIGHT6_KL,
		U_LIGHT6_KQ,
		U_LIGHT6_TYPE,
		U_LIGHT6_SPOTDIRECTION,
		U_LIGHT6_COSCUTOFF,
		U_LIGHT6_COSINNER,
		U_LIGHT6_EXPONENT,

		//light 8
		U_LIGHT7_POSITION,
		U_LIGHT7_COLOR,
		U_LIGHT7_POWER,
		U_LIGHT7_KC,
		U_LIGHT7_KL,
		U_LIGHT7_KQ,
		U_LIGHT7_TYPE,
		U_LIGHT7_SPOTDIRECTION,
		U_LIGHT7_COSCUTOFF,
		U_LIGHT7_COSINNER,
		U_LIGHT7_EXPONENT,

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

		//Npc
		hb_NPC1,

		//walls
		//hb_OUTERWALLS,
		hb_count,
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
private:
	unsigned m_vertexArrayID;
	unsigned gamestate;
	unsigned interact;
	unsigned count;
	Mesh* meshList[NUM_GEOMETRY];
	Objects objectlist[hb_count];
	Objects interactionhitboxes[i_count];
	Objects player;
	Hitbox fist1, fist2;
	float playerfist1, playerfist2;
	bool playerfist1extend, playerfist2extend;
	Vector3 savedposition, savedtarget;

	float playermovementspeed;
	unsigned playergold, playerpunchpower, playerwater, shopselect, textprogress, pricemultiplier, playerwood, treegrowthstage;
	bool playerhasseed, playerhaskey, lackofmoney, playerboughtitem, lackofwood, playerlost, lackofwater;
	bool house2locked, treeplanted;
	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];
	bool resume, escbuttonstate, triedtoopendoor;
	unsigned buttonhover;

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

	Light light[8];

	Vector3 imagepos;
	Vector3 imagedimensions;

	//functions for rendering
	void RenderMesh(Mesh* mesh, bool enableLight);
	void RenderSkybox(Vector3 skyboxoffset);
	void RenderText(Mesh* mesh, std::string text, Color color);
	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);
	void RenderMeshOnScreen(Mesh* mesh, Vector3 size, Vector3 rotate, float x, float y);
	void RenderMudkip(Vector3 position = Vector3(0, 8, 0), Vector3 rotation = Vector3(0, 0, 0));

	// Scrolling of text
	bool isRead = false;
	bool DialogueBoxOpen = false;
	std::string GameDialogueLINE;
	std::string GD_PrintLine1;
	std::string GD_PrintLine2;
	std::string GD_PrintLine3;
	int x = 0;
	int ScrollingText;


	// Spacing of text
	int textMaxWidth;
	int textSpacing[256];
	Vector3 PlayerCollision(unsigned count, CameraTest camera);
	Vector3 CollisionCircleRect(float cx, float cy, float radius, float rx, float ry, float rw, float rh);
};

#endif

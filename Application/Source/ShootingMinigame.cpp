#include "shootingminigame.h"
#include "GL\glew.h"

#include "shader.hpp"
#include "Mtx44.h"

#include "Application.h"
#include "MeshBuilder.h"

#include "Utility.h"
#include "LoadTGA.h"
#include <fstream>
#include <iostream>
using namespace std;

void shootingminigame::UseScene()
{
	glBindVertexArray(m_vertexArrayID);
	glUseProgram(m_programID);

	Mesh::SetMaterialLoc(m_parameters[U_MATERIAL_AMBIENT],
		m_parameters[U_MATERIAL_DIFFUSE],
		m_parameters[U_MATERIAL_SPECULAR],
		m_parameters[U_MATERIAL_SHININESS]);
}
Vector3 shootingminigame::CollisionCircleRect(float cx, float cz, float radius, float rx, float rz, float rw, float rb) {
	//assume all y = 0 since y not needed
	Vector3 endPos = Vector3(cx, 0, cz);
	//Work out nearest point to future player position, around perimeter of cell rectangle. 
	//We can test the distance to this point to see if we have collided. 
	Vector3 nearestPoint;
	nearestPoint.x = Math::Clamp(cx, rx - 0.5f * rw, rx + 0.5f * rw);
	nearestPoint.y = 0;
	nearestPoint.z = Math::Clamp(cz, rz - 0.5f * rb, rz + 0.5f * rb);
	Vector3 rayToNearest = nearestPoint - endPos;
	float overlap = radius - rayToNearest.Length();
	if (std::isnan(overlap))
	{
		overlap = 0; //in case ray same pos as nearestPoint
	}

	// If overlap is positive, then a collision has occurred, so we displace backwards by the overlap amount. 
	// The potential position is then tested against other tiles in the area therefore "statically" resolving the collision

	if (overlap > 0)
	{
		if (rayToNearest == Vector3(0, 0, 0)) //to prevent crashes
		{
			return endPos;
		}
		// Statically resolve the collision
		endPos = endPos - rayToNearest.Normalized() * overlap;
	}

	return endPos;
}



bool shootingminigame::CollisionCircleRect1(float cx, float cz, float radius, float rx, float rz, float rw, float rb) {
	//assume all y = 0 since y not needed
	Vector3 endPos = Vector3(cx, 1, cz);
	//Work out nearest point to future player position, around perimeter of cell rectangle. 
	//We can test the distance to this point to see if we have collided. 
	Vector3 nearestPoint;
	nearestPoint.x = Math::Clamp(cx, rx - 0.5f * rw, rx + 0.5f * rw);
	nearestPoint.y = 0;
	nearestPoint.z = Math::Clamp(cz, rz - 0.5f * rb, rz + 0.5f * rb);
	Vector3 rayToNearest = nearestPoint - endPos;
	float overlap = radius - rayToNearest.Length();
	if (std::isnan(overlap))
	{
		overlap = 0; //in case ray same pos as nearestPoint
	}

	// If overlap is positive, then a collision has occurred, so we displace backwards by the overlap amount. 
	// The potential position is then tested against other tiles in the area therefore "statically" resolving the collision

	if (overlap > 0)
	{
		return true;
	}

	return false;
}

Vector3 shootingminigame::PlayerCollision(unsigned count, float x, float z) {
	if (count == count) {
		return CollisionCircleRect(x, z, 1.5, objectlist[count].getposition().x, objectlist[count].getposition().z, objectlist[count].gethitboxcollisionsize().x /*HitboxX*/, objectlist[count].gethitboxcollisionsize().z /*HitboxZ*/);
	}
	else
	{
		return Vector3(x, 0, z); //wont trigger
	}
}

shootingminigame::shootingminigame()
{
}

shootingminigame::~shootingminigame()
{
}

void shootingminigame::Init()
{
	srand((unsigned)time(NULL));

	textMaxWidth = 64; //initial value
	ifstream fileStream;
	//for text spacing (check)
	const char* file_path;
	file_path = "Image//FontData.csv"; //name of font data csv file
	fileStream.open(file_path, std::ios::binary);
	if (!fileStream.is_open())
	{
		std::cout << "Impossible to open " << file_path << ". Are you in the right directory ?\n";
	}
	else {
		while (!fileStream.eof()) {
			std::string buf;
			std::getline(fileStream, buf, ' ');
			if (buf == "Char") {
				std::getline(fileStream, buf, ' ');
				unsigned i = stoi(buf);
				std::getline(fileStream, buf, ',');
				if (buf == "Base Width") {
					std::getline(fileStream, buf);
					unsigned width = stoi(buf);
					textSpacing[i] = width;
				}
				else {
					std::getline(fileStream, buf);
				}
			}
			else if (buf == "Cell") {
				std::getline(fileStream, buf, ',');
				if (buf == "Width") {
					std::getline(fileStream, buf);
					textMaxWidth = stoi(buf);
				}
				else {
					std::getline(fileStream, buf);
				}
			}
			else {
				std::getline(fileStream, buf);
			}
		}
	}
	fileStream.close();


	{// Set background color to black



		//Enable depth buffer and depth testing
		glEnable(GL_DEPTH_TEST);

		//Enable back face culling
		glEnable(GL_CULL_FACE);

		//Enable blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//Default to fill mode
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// Generate a default VAO for now
		glGenVertexArrays(1, &m_vertexArrayID);
		glBindVertexArray(m_vertexArrayID);

		//Load vertex and fragment shaders
		m_programID = LoadShaders("Shader//Texture.vertexshader", "Shader//Text.fragmentshader");
		m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");
		m_parameters[U_MODELVIEW] = glGetUniformLocation(m_programID, "MV");
		m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(m_programID, "MV_inverse_transpose");
		m_parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(m_programID, "material.kAmbient");
		m_parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(m_programID, "material.kDiffuse");
		m_parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(m_programID, "material.kSpecular");
		m_parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(m_programID, "material.kShininess");

		//light 1
		m_parameters[U_LIGHT0_POSITION] = glGetUniformLocation(m_programID, "lights[0].position_cameraspace");
		m_parameters[U_LIGHT0_COLOR] = glGetUniformLocation(m_programID, "lights[0].color");
		m_parameters[U_LIGHT0_POWER] = glGetUniformLocation(m_programID, "lights[0].power");
		m_parameters[U_LIGHT0_KC] = glGetUniformLocation(m_programID, "lights[0].kC");
		m_parameters[U_LIGHT0_KL] = glGetUniformLocation(m_programID, "lights[0].kL");
		m_parameters[U_LIGHT0_KQ] = glGetUniformLocation(m_programID, "lights[0].kQ");
		m_parameters[U_LIGHT0_TYPE] = glGetUniformLocation(m_programID, "lights[0].type");
		m_parameters[U_LIGHT0_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[0].spotDirection");
		m_parameters[U_LIGHT0_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[0].cosCutoff");
		m_parameters[U_LIGHT0_COSINNER] = glGetUniformLocation(m_programID, "lights[0].cosInner");
		m_parameters[U_LIGHT0_EXPONENT] = glGetUniformLocation(m_programID, "lights[0].exponent");

		//light 2
		m_parameters[U_LIGHT1_POSITION] = glGetUniformLocation(m_programID, "lights[1].position_cameraspace");
		m_parameters[U_LIGHT1_COLOR] = glGetUniformLocation(m_programID, "lights[1].color");
		m_parameters[U_LIGHT1_POWER] = glGetUniformLocation(m_programID, "lights[1].power");
		m_parameters[U_LIGHT1_KC] = glGetUniformLocation(m_programID, "lights[1].kC");
		m_parameters[U_LIGHT1_KL] = glGetUniformLocation(m_programID, "lights[1].kL");
		m_parameters[U_LIGHT1_KQ] = glGetUniformLocation(m_programID, "lights[1].kQ");
		m_parameters[U_LIGHT1_TYPE] = glGetUniformLocation(m_programID, "lights[1].type");
		m_parameters[U_LIGHT1_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[1].spotDirection");
		m_parameters[U_LIGHT1_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[1].cosCutoff");
		m_parameters[U_LIGHT1_COSINNER] = glGetUniformLocation(m_programID, "lights[1].cosInner");
		m_parameters[U_LIGHT1_EXPONENT] = glGetUniformLocation(m_programID, "lights[1].exponent");

		m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
		m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");
		// Get a handle for our "colorTexture" uniform
		m_parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_programID, "colorTextureEnabled");
		m_parameters[U_COLOR_TEXTURE] = glGetUniformLocation(m_programID, "colorTexture");

		// Get a handle for our "textColor" uniform
		m_parameters[U_TEXT_ENABLED] = glGetUniformLocation(m_programID, "textEnabled");
		m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID, "textColor");

		glUseProgram(m_programID);
		glUniform1i(m_parameters[U_NUMLIGHTS], 2);

		//light 2
		light[0].type = Light::LIGHT_DIRECTIONAL;
		light[0].position.Set(-100, 100, -100);
		light[0].color.Set(253.0f / 255.0f, 251.0f / 255.0f, 241.0f / 255.0f);
		light[0].power = 0.6f;
		light[0].kC = 1.f;
		light[0].kL = 0.01f;
		light[0].kQ = 0.001f;
		light[0].cosCutoff = cos(Math::DegreeToRadian(45));
		light[0].cosInner = cos(Math::DegreeToRadian(30));
		light[0].exponent = 3.f;
		light[0].spotDirection.Set(0.f, 1.f, 0.f);
		glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &light[0].color.r);
		glUniform1f(m_parameters[U_LIGHT0_POWER], light[0].power);
		glUniform1f(m_parameters[U_LIGHT0_KC], light[0].kC);
		glUniform1f(m_parameters[U_LIGHT0_KL], light[0].kL);
		glUniform1f(m_parameters[U_LIGHT0_KQ], light[0].kQ);
		glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
		glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &light[0].color.r);
		glUniform1f(m_parameters[U_LIGHT0_POWER], light[0].power);
		glUniform1f(m_parameters[U_LIGHT0_KC], light[0].kC);
		glUniform1f(m_parameters[U_LIGHT0_KL], light[0].kL);
		glUniform1f(m_parameters[U_LIGHT0_KQ], light[0].kQ);
		glUniform1f(m_parameters[U_LIGHT0_COSCUTOFF], light[0].cosCutoff);
		glUniform1f(m_parameters[U_LIGHT0_COSINNER], light[0].cosInner);
		glUniform1f(m_parameters[U_LIGHT0_EXPONENT], light[0].exponent);

		//light 3
		light[1].type = Light::LIGHT_DIRECTIONAL;
		light[1].position.Set(100, 100, 100);
		light[1].color.Set(253.0f / 255.0f, 251.0f / 255.0f, 241.0f / 255.0f);
		light[1].power = 0.4f;
		light[1].kC = 1.f;
		light[1].kL = 0.01f;
		light[1].kQ = 0.001f;
		light[1].cosCutoff = cos(Math::DegreeToRadian(45));
		light[1].cosInner = cos(Math::DegreeToRadian(30));
		light[1].exponent = 3.f;
		light[1].spotDirection.Set(0.f, 1.f, 0.f);
		glUniform3fv(m_parameters[U_LIGHT1_COLOR], 1, &light[1].color.r);
		glUniform1f(m_parameters[U_LIGHT1_POWER], light[1].power);
		glUniform1f(m_parameters[U_LIGHT1_KC], light[1].kC);
		glUniform1f(m_parameters[U_LIGHT1_KL], light[1].kL);
		glUniform1f(m_parameters[U_LIGHT1_KQ], light[1].kQ);
		glUniform1i(m_parameters[U_LIGHT1_TYPE], light[1].type);
		glUniform3fv(m_parameters[U_LIGHT1_COLOR], 1, &light[1].color.r);
		glUniform1f(m_parameters[U_LIGHT1_KC], light[1].kC);
		glUniform1f(m_parameters[U_LIGHT1_POWER], light[1].power);
		glUniform1f(m_parameters[U_LIGHT1_KL], light[1].kL);
		glUniform1f(m_parameters[U_LIGHT1_KQ], light[1].kQ);
		glUniform1f(m_parameters[U_LIGHT1_COSCUTOFF], light[1].cosCutoff);
		glUniform1f(m_parameters[U_LIGHT1_COSINNER], light[1].cosInner);
		glUniform1f(m_parameters[U_LIGHT1_EXPONENT], light[1].exponent);



		Mesh::SetMaterialLoc(m_parameters[U_MATERIAL_AMBIENT], m_parameters[U_MATERIAL_DIFFUSE], m_parameters[U_MATERIAL_SPECULAR], m_parameters[U_MATERIAL_SHININESS]);
	}

	//variable to rotate geometry
	rotateAngle = 0;

	//Initialize camera settings
	camera.Init(Vector3(0, 2, -10), Vector3(1, 2, 0), Vector3(0, 1, 0));
	camera.Position_Y = 2;

	// Init VBO
	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		meshList[i] = nullptr;
	}
	meshList[GEO_AXES] = MeshBuilder::GenerateAxes("Axes", 100, 100, 100);
	meshList[GEO_LIGHTBALL] = MeshBuilder::GenerateSphere("sphere", Color(0, 1, 0), 10, 10, 1);
	meshList[GEO_NAVINODES] = MeshBuilder::GenerateCube("navicubes", Color(1, 1, 1), 0.5);
	meshList[GEO_NAVINODES2] = MeshBuilder::GenerateCube("navicubes", Color(0, 1, 0), 0.5);

	//ground
	meshList[GEO_QUAD] = MeshBuilder::GenerateQuad("quad", Color(1, 1, 1), 1, 20);
	meshList[GEO_QUAD]->material.kAmbient.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_QUAD]->material.kDiffuse.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_QUAD]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_QUAD]->material.kShininess = 1.f;
	meshList[GEO_QUAD]->textureID = LoadTGA("Image//Brick_03.tga");

	//sky box
	meshList[GEO_FRONT] = MeshBuilder::GenerateQuad("front", Color(1, 1, 1), 1.f);
	meshList[GEO_FRONT]->textureID = LoadTGA("Image//DaylightBox_Front.tga");
	meshList[GEO_BACK] = MeshBuilder::GenerateQuad("back", Color(0, 1, 1), 1.f);
	meshList[GEO_BACK]->textureID = LoadTGA("Image//DaylightBox_Back.tga");
	meshList[GEO_LEFT] = MeshBuilder::GenerateQuad("left", Color(0, 1, 1), 1.f);
	meshList[GEO_LEFT]->textureID = LoadTGA("Image//DaylightBox_Left.tga");
	meshList[GEO_RIGHT] = MeshBuilder::GenerateQuad("right", Color(0, 1, 1), 1.f);
	meshList[GEO_RIGHT]->textureID = LoadTGA("Image//DaylightBox_Right.tga");
	meshList[GEO_TOP] = MeshBuilder::GenerateQuad("top", Color(0, 1, 1), 1.f);
	meshList[GEO_TOP]->textureID = LoadTGA("Image//DaylightBox_Top.tga");
	meshList[GEO_BOTTOM] = MeshBuilder::GenerateQuad("bottom", Color(0, 1, 1), 1.f);
	meshList[GEO_BOTTOM]->textureID = LoadTGA("Image//DaylightBox_Bottom.tga");

	// Stamina
	meshList[GEO_STAMINA_BLACK] = MeshBuilder::GenerateQuad("stamina_black", Color(0, 0, 0), 1.f);
	meshList[GEO_STAMINA_BAR] = MeshBuilder::GenerateQuad("stamina_bar", Color(1, 1, 0), 1.f);

	meshList[GEO_TARGET] = MeshBuilder::GenerateOBJMTL("tree", "OBJ//targetA.obj", "OBJ//targetA.mtl");
	objectlist[hb_target1].setmesh(GEO_TARGET);
	objectlist[hb_target1].setproperties(Vector3(rand() % 16 - 8, rand() % 6 + 1, 12), Vector3(0, -90, 0), Vector3(2, 2, 2)); //0,rand() % 6 + 1, 12
	objectlist[hb_target1].sethitboxcollisionsize(Vector3(0.7, 0.7, 0.7));
	objectlist[hb_target2].setmesh(GEO_TARGET);
	objectlist[hb_target2].setproperties(Vector3(rand() % 16 - 8, rand() % 6 + 1, 12), Vector3(0, -90, 0), Vector3(2, 2, 2));
	objectlist[hb_target2].sethitboxcollisionsize(Vector3(0.7, 0.7, 0.7));
	objectlist[hb_target3].setmesh(GEO_TARGET);
	objectlist[hb_target3].setproperties(Vector3(rand() % 16 - 8, rand() % 6 + 1, 12), Vector3(0, -90, 0), Vector3(2, 2, 2));
	objectlist[hb_target3].sethitboxcollisionsize(Vector3(0.7, 0.7, 0.7));
	targets.push_back(objectlist[hb_target1]);
	targets.push_back(objectlist[hb_target2]);
	targets.push_back(objectlist[hb_target3]);

	meshList[GEO_HANGAR] = MeshBuilder::GenerateOBJMTL("hangar", "OBJ//hangar_largeB.obj", "OBJ//hangar_largeB.mtl");
	objectlist[hb_hangar].setmesh(GEO_HANGAR);
	objectlist[hb_hangar].setproperties(Vector3(0, 0, 0), Vector3(0, 0, 0), Vector3(-10, 10, -10));

	meshList[GEO_WALL] = MeshBuilder::GenerateOBJMTL("wall", "OBJ//corridor_wall.obj", "OBJ//corridor_wall.mtl");
	objectlist[hb_hangarwall].setmesh(GEO_WALL);
	objectlist[hb_hangarwall].setproperties(Vector3(0, 0, 6.9), Vector3(0, 90, 0), Vector3(15, 15, 20));
	objectlist[hb_table].setmesh(GEO_NULL);
	objectlist[hb_table].setproperties(Vector3(0, 0, -3), Vector3(0, 90, 0), Vector3(20, 2, 2));
	objectlist[hb_table].sethitboxcollisionsize(Vector3(20, 0, 1));
	objectlist[hb_table1].setmesh(GEO_WALL);
	objectlist[hb_table1].setproperties(Vector3(0, 0, -11.5), Vector3(0, 90, 0), Vector3(20, 1, 20));


	meshList[GEO_GUN] = MeshBuilder::GenerateOBJMTL("gun", "OBJ//blasterD.obj", "OBJ//blasterD.mtl");
	objectlist[hb_gun].setmesh(GEO_GUN);
	objectlist[hb_gun].setproperties(Vector3(0, 0, 0), Vector3(0, 180, 0), Vector3(1, 1, 1));

	objectlist[hb_wall1].setproperties(Vector3(-10, 0, 0), Vector3(0, 0, 0), Vector3(1, 0, 27));
	objectlist[hb_wall1].sethitboxcollisionsize(Vector3(2, 0, 27));
	objectlist[hb_wall2].setproperties(Vector3(10, 0, 0), Vector3(0, 0, 0), Vector3(1, 0, 27));
	objectlist[hb_wall2].sethitboxcollisionsize(Vector3(2, 0, 27));
	objectlist[hb_wall3].setproperties(Vector3(0, 0, 13.5), Vector3(0, 0, 0), Vector3(0.1, 1, 6));
	objectlist[hb_wall3].sethitboxcollisionsize(Vector3(20, 0, 2));
	objectlist[hb_wall4].setproperties(Vector3(0, 0, -13.5), Vector3(0, 0, 0), Vector3(20, 0, 1));
	objectlist[hb_wall4].sethitboxcollisionsize(Vector3(20, 0, 2));

	//text
	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Image//DimboFont.tga");

	meshList[GEO_GAMEOVERBACKGROUND] = MeshBuilder::GenerateQuad("gameover", Color(1, 1, 1), 1.f);
	meshList[GEO_GAMEOVERBACKGROUND]->material.kAmbient.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_GAMEOVERBACKGROUND]->material.kDiffuse.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_GAMEOVERBACKGROUND]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_GAMEOVERBACKGROUND]->material.kShininess = 1.f;
	meshList[GEO_GAMEOVERBACKGROUND]->textureID = LoadTGA("Image//GameOverRedScreen.tga");

	meshList[GEO_GAMEOVER] = MeshBuilder::GenerateQuad("gameovertext", Color(1, 1, 1), 1.f);
	meshList[GEO_GAMEOVER]->material.kAmbient.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_GAMEOVER]->material.kDiffuse.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_GAMEOVER]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_GAMEOVER]->material.kShininess = 1.f;
	meshList[GEO_GAMEOVER]->textureID = LoadTGA("Image//GameOver.tga");

	meshList[GEO_GAMEOVERBUTTON] = MeshBuilder::GenerateQuad("Retry", Color(1, 1, 1), 1.f);
	meshList[GEO_GAMEOVERBUTTON]->material.kAmbient.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_GAMEOVERBUTTON]->material.kDiffuse.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_GAMEOVERBUTTON]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_GAMEOVERBUTTON]->material.kShininess = 1.f;
	meshList[GEO_GAMEOVERBUTTON]->textureID = LoadTGA("Image//Retry.tga");

	//setup player's hitbox
	player.Setposition(Vector3(0, 2, 0));

	Mtx44 projection;
	projection.SetToPerspective(45.f, 4.f / 3.f, 0.1f, 10000.f);
	projectionStack.LoadMatrix(projection);

	timesincelastbuttonpress = 0; DrawAxis = false; InWindow = true;
	window = GetFocus();

	//set mouse to centre of window
	if (GetWindowRect(window, &rect))
	{
		SetCursorPos((rect.right + rect.left) * 0.5, (rect.bottom + rect.top) * 0.5);
	}

	Mesh::SetMaterialLoc(m_parameters[U_MATERIAL_AMBIENT],
		m_parameters[U_MATERIAL_DIFFUSE],
		m_parameters[U_MATERIAL_SPECULAR],
		m_parameters[U_MATERIAL_SHININESS]);
	//giving all variables used the starting value
	gamestate = gs_env;
	debug = false; mousehidden = false; InWindow = true; DrawAxis = false; renderhitboxes = false; inshop = false; ingame = false;
	checkcollision = true; shopselect = 0;
	interact = i_count;
	playergold = 0; playerwater = 0; playerhasseed = false; playerwood = 0; playerhaskey = false;
	lackofmoney = false; playerboughtitem = false; resume = false; triedtoopendoor = false; playerlost = false;
	chatting = false; house2locked = false;
	pricemultiplier = 1;
	playerpunchpower = 1;
	buttonhover = mb_none;
	gamemovementspeed = 0.5; timesincelastpunch = 0;
	textprogress = 0;
	treegrowthstage = 0;
	treeplanted = false; lackofwater = false;
	imagepos = Vector3(40, 30, 0);
	imagedimensions = Vector3(20, 20, 1);
	gameover = false;
	rendernodes = false;
	shootcooldown = 0.2;
	std::cout << "init done\n";
	timer = 60;
}

bool shootingminigame::Update(double dt)
{
	//Mouse Inputs
	static bool bLButtonState = false;
	if (!bLButtonState && Application::IsMousePressed(0))
	{
		bLButtonState = true;
		//std::cout << "LBUTTON DOWN" << std::endl;
		//Converting Viewport space to UI space
		double x, y;
		Application::GetCursorPos(&x, &y);
		unsigned w = Application::GetWindowWidth();
		unsigned h = Application::GetWindowHeight();
		float posX = x / w * 80.f; //convert (0,800) to (0,80)
		float posY = -(y / h * 60.f) + 60; //convert (600,0) to (0,60)
		//std::cout << "posX:" << posX << " , posY:" << posY << std::endl;
		if (posX > imagepos.x - imagedimensions.x * 0.5 && posX < imagepos.x + imagedimensions.x * 0.5 && posY > imagepos.y - imagedimensions.y * 0.5 && posY < imagepos.y + imagedimensions.y * 0.5)
		{
		}
		else
		{
		}
	}
	else if (bLButtonState && !Application::IsMousePressed(0))
	{
		bLButtonState = false;
		//std::cout << "LBUTTON UP" << std::endl;
	}
	static bool RightClick = false;
	if (!RightClick && Application::IsMousePressed(1))
	{
		gameover = true;
		RightClick = true;
	}
	else if (RightClick && !Application::IsMousePressed(1))
		RightClick = false;


	static bool quit = false;
	switch (gamestate)
	{
	case gs_mainmenu:
		quit = UpdateMainMenu();
		if (quit == true)
			return true;
		break;
	case gs_env:
		UpdateENV(dt);
		break;
	case gs_credits:
		UpdateCredits(dt);
		break;
	}
	return false;
}

bool shootingminigame::UpdateMainMenu()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	bool click = GetAsyncKeyState(0x01);
	timesincelastbuttonpress = 0;
	DialogueBoxOpen = false;

	if (GetAsyncKeyState(VK_ESCAPE))
	{

	}

	if (window == GetFocus())
	{
		if (GetWindowRect(window, &rect))
		{
			POINT initmousepos;
			mousepos = &initmousepos;//init mousepos
			GetCursorPos(mousepos); //get mouse position
			mousepos->x -= rect.left;
			mousepos->y -= rect.top;

			if ((mousepos->x > 8 && mousepos->x < 181 && mousepos->y > 191 && mousepos->y < 221) || (mousepos->x > 8 && mousepos->x < 228 && mousepos->y > 191 && mousepos->y < 221 && resume))
			{
				buttonhover = mb_start;
				if (click)
				{
					SetCursorPos((rect.right + rect.left) * 0.5, (rect.bottom + rect.top) * 0.5);
					gamestate = gs_env;
				}
			}
			else if (mousepos->x > 8 && mousepos->x < 262 && mousepos->y > 241 && mousepos->y < 275)
			{
				buttonhover = mb_credits;
				if (click)
				{
					rotateAngle = 0;
					gamestate = gs_credits;
				}
			}
			else if (mousepos->x > 8 && mousepos->x < 139 && mousepos->y > 290 && mousepos->y < 334)
			{
				buttonhover = mb_quit;
				if (GetAsyncKeyState(0x01))
					return true;
			}
			else
				buttonhover = mb_none;
		}
	}
	return false;
}

#define LSPEED 10
void shootingminigame::UpdateENV(double dt)
{
	bool click = GetAsyncKeyState(0x01);
	static bool timerstart = false;
	if (timerstart)
	{
		timer -= dt;
	}
	if (timer <= 0)
	{
		gameover = true;
		timer = 0;
	}
	if (gameover)
	{
		if (mousehidden == true)
		{
			mousehidden = false;
			ShowCursor(true);
		}
		double x, y;
		Application::GetCursorPos(&x, &y);
		unsigned w = Application::GetWindowWidth();
		unsigned h = Application::GetWindowHeight();
		float posX = x / w * 80.f; //convert (0,800) to (0,80)
		float posY = -(y / h * 60.f) + 60; //convert (600,0) to (0,60)
		if (posX > 31.5 && posX < 49 && posY > 7 && posY < 13.6 && GetAsyncKeyState(0x01))
		{
			Application::GameScene = Application::gs_game;
			if (mousehidden)
			{
				mousehidden = false;
				ShowCursor(true);
			}
			score = 0;
			camera.Reset();
			timerstart = false;
			timer = 60;
		}
		if (GetAsyncKeyState(VK_ESCAPE))
		{
			Application::GameScene = Application::gs_game;
			if (mousehidden)
			{
				mousehidden = false;
				ShowCursor(true);
			}
			score = 0;
			camera.Reset();
			timerstart = false;
			timer = 60;
		}
		return;
	}


	// Skybox
	static bool isDay = true;
	static bool isNight = false;
	static float timer_dayandNight = 20;

	if (timer_dayandNight >= 20) {
		if (isDay == true and isNight == false) {
			DayandNightSkyboxTransitioning += 0.000095;
		}
		if (isDay == false and isNight == true) {
			DayandNightSkyboxTransitioning -= 0.000095;
		}
		if (isDay == true and isNight == false and DayandNightSkyboxTransitioning >= 1) {
			isDay = false;
			isNight = true;
			timer_dayandNight = 0;
			DayandNightSkyboxTransitioning = 1;
		}
		if (isDay == false and isNight == true and DayandNightSkyboxTransitioning <= 0) {
			isDay = true;
			isNight = false;
			timer_dayandNight = 0;
			DayandNightSkyboxTransitioning = 0;
		}
	}
	timer_dayandNight += dt;

	glClearColor(0.0f, 0.0f + DayandNightSkyboxTransitioning * 0.8, 0.0f + DayandNightSkyboxTransitioning, 0.0f);

	//calculate the fps
	Vector3 tempplayerpos = camerapos;
	fps = 1.f / dt;



	//check if window is in focus
	if (window == GetFocus())
	{
		if (interact == i_count)
		{
			triedtoopendoor = false;
			lackofwater = false;
		}
		interact = i_count;
		bool x1 = false, y1 = false, z1 = false;
		bool* x = &x1, * y = &y1, * z = &z1;
		static bool boatalive = true, punch = false;
		//check if mouse is in window
		if (GetWindowRect(window, &rect))
		{
			POINT initmousepos;
			mousepos = &initmousepos;//init mousepos
			GetCursorPos(mousepos); //get mouse position
			if (mousepos->x > rect.left && mousepos->x < rect.right && mousepos->y > rect.top && mousepos->y < rect.bottom && InWindow == true)
			{
				//mouse inside window
				if (!mousehidden)
				{
					ShowCursor(false);
					mousehidden = true;
				}
				if (!ingame || chatting)
				{
					camerapos = Vector3(camera.position.x, camera.position.y, camera.position.z);
					camera.Update(dt, initmousepos, !inshop);
				}

				//Buldings etc.
				if (camera.position.y != -18) {
					for (int i = 0; i < hb_count; ++i) {
						if (i == hb_gun)
							continue;
						Vector3 finalPos = PlayerCollision(i, camera.position.x, camera.position.z); //ignore y
						camera.position.x = finalPos.x;
						camera.position.z = finalPos.z;
					}
					camera.target = camera.position + camera.view;
				}
			} //else do nothing with camera
		}

		//calculate the gun position

		Vector3 playerviewdirection;
		playerviewdirection = camera.view;
		playerviewdirection.Normalize();
		if (click && shootcooldown > 0.2)
		{
			shootcooldown = 0;
			timerstart = true;
			bool hit = false;
			unsigned times = 1;
			Vector3 testvector3;
			//check for walls
			while (true)
			{
				bool done = false;
				testvector3 = playerviewdirection * 0.1 * times;
				//get inworldposition
				Vector3 Pos = testvector3 + camerapos;
				Hitbox testhitbox;
				testhitbox.sethitboxcenterdimensions(Vector3(0, 0, 0), Pos);
				for (unsigned idx = 0; idx < targets.size(); idx++)
				{
					if (targets[idx].gethitbox().Checkforcollision(testhitbox))
					{
						hit = true;
						done = true;
						times = 401;
					}
					else if (testhitbox.Checkforcollision(targets[idx].gethitbox()))
					{
						hit = true;
						done = true;
						times = 401;
					}

					if (hit)
					{
						score++;
						objectlist[hb_target1 + idx].Setposition(Vector3(rand() % 16 - 8, rand() % 6 + 1, 12));
						targets[idx].Setposition(objectlist[hb_target1 + idx].getposition());
						break;
					}
				}
				if (done || hit || times >= 400)
					break;
				times++;
			}
			/*if (hit)
			{
				std::cout << "hit\n";
			}
			else
				std::cout << "miss\n";*/
		}


		if (debug)
		{
			if (Application::IsKeyPressed('1')) //enable back face culling
				glEnable(GL_CULL_FACE);
			if (Application::IsKeyPressed('2')) //disable back face culling
				glDisable(GL_CULL_FACE);
			if (Application::IsKeyPressed('3'))
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //default fill mode
			if (Application::IsKeyPressed('4'))
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //wireframe mode

			static bool OEM4buttonstate = false, OEM6buttonstate = false, rctrlbuttonstate = false;
			if (GetAsyncKeyState(VK_OEM_4) && !OEM4buttonstate) //'['
			{
				checkcollision = !checkcollision;
				OEM4buttonstate = true;
			}
			else if (!GetAsyncKeyState(VK_OEM_4) && OEM4buttonstate)
			{
				OEM4buttonstate = false;
			}
			if (GetAsyncKeyState(VK_OEM_6) && !OEM6buttonstate) //']'
			{
				renderhitboxes = !renderhitboxes;
				OEM6buttonstate = true;
			}
			else if (!GetAsyncKeyState(VK_OEM_6) && OEM6buttonstate)
			{
				OEM6buttonstate = false;
			}
			//for axis
			if (Application::IsKeyPressed(0xA3) == true && !rctrlbuttonstate)
			{
				DrawAxis = !DrawAxis;
				rctrlbuttonstate = true;
			}
			else if (!Application::IsKeyPressed(0xA3) && rctrlbuttonstate)
			{
				rctrlbuttonstate = false;
			}
			static bool enterbuttonstate = false;
			if (GetAsyncKeyState(VK_RETURN) && !enterbuttonstate)
			{
				enterbuttonstate = true;
				rendernodes = !rendernodes;
			}
			else if (!GetAsyncKeyState(VK_RETURN) && enterbuttonstate)
			{
				enterbuttonstate = false;
			}
		}
		static bool OEM5buttonstate = false;
		if (Application::IsKeyPressed(VK_OEM_5) && !OEM5buttonstate) //'\'
		{
			debug = !debug;
			OEM5buttonstate = true;
		}
		else if (!GetAsyncKeyState(VK_OEM_5) && OEM5buttonstate)
		{
			OEM5buttonstate = false;
		}

		if (GetAsyncKeyState(VK_ESCAPE) && !escbuttonstate) //to exit env
		{
			Application::GameScene = Application::gs_game;
			if (mousehidden)
			{
				mousehidden = false;
				ShowCursor(true);
			}
			score = 0;
			camera.Reset();
			timerstart = false;
			timer = 60;
			escbuttonstate = true;
		}
		else if (!GetAsyncKeyState(VK_ESCAPE) && escbuttonstate)
		{
			escbuttonstate = false;
		}

		if (playerboughtitem && timesincelastbuttonpress > 4)
		{
			playerboughtitem = false;
			timesincelastbuttonpress = 0;
		}


		//for inwindow
		if (Application::IsKeyPressed(VK_BACK) == true && timesincelastbuttonpress > 0.2)
		{
			if (InWindow == false) InWindow = true;
			else InWindow = false;
			timesincelastbuttonpress = 0;
			ShowCursor(true);
			mousehidden = false;
		}
		//update the player position
		player.Setposition(camerapos);
	}
	else
	{
		if (mousehidden)
		{
			ShowCursor(true);
			mousehidden = false;
		}
	}
	camera.center.x = (camera.windowcenter.left + camera.windowcenter.right) * 0.5;
	camera.center.y = (camera.windowcenter.top + camera.windowcenter.bottom) * 0.5;

	// Stamina Consumption
	static float timer_regen = 0;
	if (Application::IsKeyPressed(VK_SHIFT) and (Application::IsKeyPressed('W') or Application::IsKeyPressed('A') or Application::IsKeyPressed('S') or Application::IsKeyPressed('D')) and SizeofStamina > 0 and DialogueBoxOpen == false) {
		if (not(Application::IsKeyPressed('W') and Application::IsKeyPressed('S')) and not(Application::IsKeyPressed('A') and Application::IsKeyPressed('D'))) {
			camera.ZOOM_SPEED = 10.f;
			SizeofStamina -= (float)(5 * dt);
			timer_regen = 0;
		}
	}
	else {
		if (SizeofStamina > 0) {
			camera.ZOOM_SPEED = 7.f;
		}
		else {
			camera.ZOOM_SPEED = 5.f;
		}
	}
	if ((not Application::IsKeyPressed('W') and not Application::IsKeyPressed('A') and not Application::IsKeyPressed('S') and not Application::IsKeyPressed('D')) or (Application::IsKeyPressed('W') and Application::IsKeyPressed('S')) or (Application::IsKeyPressed('A') and Application::IsKeyPressed('D'))) {
		if (timer_regen >= 1) {
			SizeofStamina += (float)(6.5 * dt);
			if (SizeofStamina >= 40) {
				SizeofStamina = 40;
			}
		}
		timer_regen += dt;
	}
	float gunoffset = shootcooldown;
	if (gunoffset > 0.2)
	{
		gunoffset = 0.2;
	}
	gunoffset *= 2;
	Vector3 gunpos;
	gunpos = camera.position + camera.view * (0.5 + gunoffset) + camera.right * 0.3 + camera.right.Cross(camera.view) * -0.3;
	objectlist[hb_gun].Setposition(gunpos);


	//make the gun rotate with the camera
	Vector3 playerforward(0, 0, -1);
	Vector3 playerforward1(0, 0, 1);
	gunpos = camera.view * 0.5 + camera.right * 0.3 + camera.right.Cross(camera.view) * -0.3;
	float angle_x = atan2(gunpos.z * playerforward.x - gunpos.x * playerforward.z, gunpos.x * playerforward.x + gunpos.z * playerforward.z) * (180 / Math::PI);
	angle_x = -angle_x;
	angle_x += 30;
	/*float angle_y = atan2(playerforward1.z * gunpos.y - playerforward1.y * gunpos.z, playerforward1.y * gunpos.y + playerforward1.z * gunpos.z) * (180 / Math::PI);
	angle_y = -angle_y;
	angle_y -= 30;*/
	objectlist[hb_gun].setrotationparentonly(Vector3(0, angle_x, 0));

	//cout << angle_y << endl;
	rotateSkybox += dt;
	rotateSunandMoon += dt;
	shootcooldown += dt;
}

void shootingminigame::UpdateCredits(double dt)
{
	rotateAngle += 10 * dt;
	if (rotateAngle > 225 || GetAsyncKeyState(0x1B))
	{
		rotateAngle = 0;
		gamestate = gs_mainmenu;
	}
}

void shootingminigame::Render()
{
	switch (gamestate)
	{
	case gs_mainmenu:
		RenderMainmenu();
		break;
	case gs_env:
		RenderENV();
		break;
	case gs_credits:
		RenderCredits();
		break;
	}
}

void shootingminigame::RenderMainmenu()
{
	Color color = Color(0, 0, 0);
	GetWindowRect(window, &rect);
	POINT initmousepos;
	mousepos = &initmousepos;//init mousepos
	GetCursorPos(mousepos); //get mouse position
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Render title
	RenderTextOnScreen(meshList[GEO_TEXT], "Scam Life", Color(1, 1, 1), 10, 0, 45);

	//Render buttons
	if (buttonhover == mb_start)
		color = Color(0, 1, 0);
	else
		color = Color(1, 1, 1);
	if (resume)
		RenderTextOnScreen(meshList[GEO_TEXT], "Resume", color, 5, 0, 40);
	else
		RenderTextOnScreen(meshList[GEO_TEXT], "Start", color, 5, 0, 40);
	if (buttonhover == mb_credits)
		color = Color(0, 1, 0);
	else
		color = Color(1, 1, 1);
	RenderTextOnScreen(meshList[GEO_TEXT], "Credits", color, 5, 0, 35);
	if (buttonhover == mb_quit)
		color = Color(0, 1, 0);
	else
		color = Color(1, 1, 1);
	RenderTextOnScreen(meshList[GEO_TEXT], "Quit", color, 5, 0, 30);

	//Render mouse pos
	UIstringoutput.str("");
	UIstringoutput.precision(4);
	UIstringoutput << "Mousepos X:" << mousepos->x - rect.left << "Y:" << mousepos->y - rect.top;
	RenderTextOnScreen(meshList[GEO_TEXT], UIstringoutput.str(), Color(0, 1, 0), 2, 0, 0);
	UIstringoutput.str("");

	//RenderMeshOnScreen(meshList[GEO_UIBACKGROUND], imagedimensions, Vector3(0, 0, 0), imagepos.x, imagepos.y);
}

void shootingminigame::RenderENV()
{
	// Render VBO here
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//light 1
	if (light[0].type == Light::LIGHT_DIRECTIONAL)
	{
		Vector3 lightDir(light[0].position.x, light[0].position.y, light[0].position.z);
		Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightDirection_cameraspace.x);
	}
	else if (light[0].type == Light::LIGHT_SPOT)
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[0].position;
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * light[0].spotDirection;
		glUniform3fv(m_parameters[U_LIGHT0_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[0].position;
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightPosition_cameraspace.x);
	}

	//light 2
	if (light[1].type == Light::LIGHT_DIRECTIONAL)
	{
		Vector3 lightDir(light[1].position.x, light[1].position.y, light[1].position.z);
		Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
		glUniform3fv(m_parameters[U_LIGHT1_POSITION], 1, &lightDirection_cameraspace.x);
	}
	else if (light[1].type == Light::LIGHT_SPOT)
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[1].position;
		glUniform3fv(m_parameters[U_LIGHT1_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * light[1].spotDirection;
		glUniform3fv(m_parameters[U_LIGHT1_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[1].position;
		glUniform3fv(m_parameters[U_LIGHT1_POSITION], 1, &lightPosition_cameraspace.x);
	}

	viewStack.LoadIdentity();
	viewStack.LookAt(
		camera.position.x, camera.position.y,
		camera.position.z, camera.target.x, camera.target.y,
		camera.target.z, camera.up.x, camera.up.y, camera.up.z
	);
	modelStack.LoadIdentity();

	Position lightPosition_cameraspace = viewStack.Top() * light[0].position;
	glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightPosition_cameraspace.x);

	RenderSkybox(camerapos);
	if (DrawAxis == true) RenderMesh(meshList[GEO_AXES], false);

	modelStack.PushMatrix(); //ground
	modelStack.Rotate(-90, 1, 0, 0);
	modelStack.Scale(100, 100, 100);
	RenderMesh(meshList[GEO_QUAD], true);
	modelStack.PopMatrix();

	//render objects
	for (unsigned idx = 0; idx < hb_total; idx++) //for objects
	{
		if (!objectlist[idx].getissetup() || objectlist[idx].getmodel() == 0 || !objectlist[idx].getrenderobject())
			continue;
		modelStack.PushMatrix();
		modelStack.Translate(objectlist[idx].getposition().x, objectlist[idx].getposition().y, objectlist[idx].getposition().z);
		modelStack.Rotate(objectlist[idx].getrotationparentchild().x, 1, 0, 0);
		modelStack.Rotate(objectlist[idx].getrotationparentchild().y, 0, 1, 0);
		modelStack.Rotate(objectlist[idx].getrotationparentchild().z, 0, 0, 1);
		modelStack.Scale(objectlist[idx].getscaleparentchild().x, objectlist[idx].getscaleparentchild().y, objectlist[idx].getscaleparentchild().z);

		if (objectlist[idx].GetChildlist().size() != 0) //deal with child of the object
		{
			std::vector<std::vector<Objects*>> parentchildlistlist;
			parentchildlistlist.push_back(objectlist[idx].GetChildlist());
			for (unsigned idx1 = 0; idx1 < parentchildlistlist.size(); idx1++)
			{
				modelStack.PushMatrix();
				modelStack.Translate(parentchildlistlist[0][idx1]->getposition().x, parentchildlistlist[0][idx1]->getposition().y, parentchildlistlist[0][idx1]->getposition().z);
				modelStack.Rotate(parentchildlistlist[0][idx1]->getrotationparentchild().x, 1, 0, 0);
				modelStack.Rotate(parentchildlistlist[0][idx1]->getrotationparentchild().y, 0, 1, 0);
				modelStack.Rotate(parentchildlistlist[0][idx1]->getrotationparentchild().z, 0, 0, 1);
				modelStack.Scale(parentchildlistlist[0][idx1]->getscaleparentchild().x, parentchildlistlist[0][idx1]->getscaleparentchild().y, parentchildlistlist[0][idx1]->getscaleparentchild().z);

				if (parentchildlistlist[0][idx1]->GetChildlist().size() != 0)
				{
					parentchildlistlist.push_back(parentchildlistlist[0][idx1]->GetChildlist());
					for (unsigned idx2 = 0; idx2 < parentchildlistlist[0][idx1]->GetChildlist().size(); idx2++)
					{
						modelStack.PushMatrix();
						modelStack.Translate(parentchildlistlist[1][idx2]->getposition().x, parentchildlistlist[1][idx2]->getposition().y, parentchildlistlist[1][idx2]->getposition().z);
						modelStack.Rotate(parentchildlistlist[1][idx2]->getrotationparentchild().x, 1, 0, 0);
						modelStack.Rotate(parentchildlistlist[1][idx2]->getrotationparentchild().y, 0, 1, 0);
						modelStack.Rotate(parentchildlistlist[1][idx2]->getrotationparentchild().z, 0, 0, 1);
						modelStack.Scale(parentchildlistlist[1][idx2]->getscaleparentchild().x, parentchildlistlist[1][idx2]->getscaleparentchild().y, parentchildlistlist[1][idx2]->getscaleparentchild().z);

						if (parentchildlistlist[1][idx2]->GetChildlist().size() != 0)
						{
							parentchildlistlist.push_back(parentchildlistlist[1][idx2]->GetChildlist());
							for (unsigned idx3 = 0; idx3 < parentchildlistlist[1][idx2]->GetChildlist().size(); idx3++)
							{
								modelStack.PushMatrix();
								modelStack.Translate(parentchildlistlist[2][idx3]->getposition().x, parentchildlistlist[2][idx3]->getposition().y, parentchildlistlist[2][idx3]->getposition().z);
								modelStack.Rotate(parentchildlistlist[2][idx3]->getrotationparentchild().x, 1, 0, 0);
								modelStack.Rotate(parentchildlistlist[2][idx3]->getrotationparentchild().y, 0, 1, 0);
								modelStack.Rotate(parentchildlistlist[2][idx3]->getrotationparentchild().z, 0, 0, 1);
								modelStack.Scale(parentchildlistlist[2][idx3]->getscaleparentchild().x, parentchildlistlist[2][idx3]->getscaleparentchild().y, parentchildlistlist[2][idx3]->getscaleparentchild().z);

								if (parentchildlistlist[2][idx3]->GetChildlist().size() != 0)
								{
									parentchildlistlist.push_back(parentchildlistlist[2][idx3]->GetChildlist());
									for (unsigned idx4 = 0; idx4 < parentchildlistlist[2][idx3]->GetChildlist().size(); idx4++)
									{
										modelStack.PushMatrix();
										modelStack.Translate(parentchildlistlist[3][idx4]->getposition().x, parentchildlistlist[3][idx4]->getposition().y, parentchildlistlist[3][idx4]->getposition().z);
										modelStack.Rotate(parentchildlistlist[3][idx4]->getrotationparentchild().x, 1, 0, 0);
										modelStack.Rotate(parentchildlistlist[3][idx4]->getrotationparentchild().y, 0, 1, 0);
										modelStack.Rotate(parentchildlistlist[3][idx4]->getrotationparentchild().z, 0, 0, 1);
										modelStack.Scale(parentchildlistlist[3][idx4]->getscaleparentchild().x, parentchildlistlist[3][idx4]->getscaleparentchild().y, parentchildlistlist[3][idx4]->getscaleparentchild().z);

										if (parentchildlistlist[3][idx4]->GetChildlist().size() != 0)
										{
											parentchildlistlist.push_back(parentchildlistlist[3][idx4]->GetChildlist());
											for (unsigned idx5 = 0; idx5 < parentchildlistlist[3][idx4]->GetChildlist().size(); idx5++)
											{
												modelStack.PushMatrix();
												modelStack.Translate(parentchildlistlist[4][idx5]->getposition().x, parentchildlistlist[4][idx5]->getposition().y, parentchildlistlist[4][idx5]->getposition().z);
												modelStack.Rotate(parentchildlistlist[4][idx5]->getrotationparentchild().x, 1, 0, 0);
												modelStack.Rotate(parentchildlistlist[4][idx5]->getrotationparentchild().y, 0, 1, 0);
												modelStack.Rotate(parentchildlistlist[4][idx5]->getrotationparentchild().z, 0, 0, 1);
												modelStack.Scale(parentchildlistlist[4][idx5]->getscaleparentchild().x, parentchildlistlist[4][idx5]->getscaleparentchild().y, parentchildlistlist[4][idx5]->getscaleparentchild().z);



												modelStack.Rotate(parentchildlistlist[4][idx5]->getrotationparentonly().x, 1, 0, 0);
												modelStack.Rotate(parentchildlistlist[4][idx5]->getrotationparentonly().y, 0, 1, 0);
												modelStack.Rotate(parentchildlistlist[4][idx5]->getrotationparentonly().z, 0, 0, 1);
												modelStack.Scale(parentchildlistlist[4][idx5]->getscaleparentonly().x, parentchildlistlist[4][idx5]->getscaleparentonly().y, parentchildlistlist[4][idx5]->getscaleparentonly().z);
												RenderMesh(meshList[parentchildlistlist[4][idx5]->getmodel()], objectlist[idx].getlighting());
												modelStack.PopMatrix();
											}
											parentchildlistlist.pop_back();
										}

										modelStack.Rotate(parentchildlistlist[3][idx4]->getrotationparentonly().x, 1, 0, 0);
										modelStack.Rotate(parentchildlistlist[3][idx4]->getrotationparentonly().y, 0, 1, 0);
										modelStack.Rotate(parentchildlistlist[3][idx4]->getrotationparentonly().z, 0, 0, 1);
										modelStack.Scale(parentchildlistlist[3][idx4]->getscaleparentonly().x, parentchildlistlist[3][idx4]->getscaleparentonly().y, parentchildlistlist[3][idx4]->getscaleparentonly().z);
										RenderMesh(meshList[parentchildlistlist[3][idx4]->getmodel()], objectlist[idx].getlighting());
										modelStack.PopMatrix();
									}
									parentchildlistlist.pop_back();
								}

								modelStack.Rotate(parentchildlistlist[2][idx3]->getrotationparentonly().x, 1, 0, 0);
								modelStack.Rotate(parentchildlistlist[2][idx3]->getrotationparentonly().y, 0, 1, 0);
								modelStack.Rotate(parentchildlistlist[2][idx3]->getrotationparentonly().z, 0, 0, 1);
								modelStack.Scale(parentchildlistlist[2][idx3]->getscaleparentonly().x, parentchildlistlist[0][idx3]->getscaleparentonly().y, parentchildlistlist[0][idx3]->getscaleparentonly().z);
								RenderMesh(meshList[parentchildlistlist[2][idx3]->getmodel()], objectlist[idx].getlighting());
								modelStack.PopMatrix();
							}
							parentchildlistlist.pop_back();
						}

						modelStack.Rotate(parentchildlistlist[1][idx2]->getrotationparentonly().x, 1, 0, 0);
						modelStack.Rotate(parentchildlistlist[1][idx2]->getrotationparentonly().y, 0, 1, 0);
						modelStack.Rotate(parentchildlistlist[1][idx2]->getrotationparentonly().z, 0, 0, 1);
						modelStack.Scale(parentchildlistlist[1][idx2]->getscaleparentonly().x, parentchildlistlist[0][idx2]->getscaleparentonly().y, parentchildlistlist[0][idx2]->getscaleparentonly().z);
						RenderMesh(meshList[parentchildlistlist[1][idx2]->getmodel()], objectlist[idx].getlighting());
						modelStack.PopMatrix();
					}
					parentchildlistlist.pop_back();
				}

				modelStack.Rotate(parentchildlistlist[0][idx1]->getrotationparentonly().x, 1, 0, 0);
				modelStack.Rotate(parentchildlistlist[0][idx1]->getrotationparentonly().y, 0, 1, 0);
				modelStack.Rotate(parentchildlistlist[0][idx1]->getrotationparentonly().z, 0, 0, 1);
				modelStack.Scale(parentchildlistlist[0][idx1]->getscaleparentonly().x, parentchildlistlist[0][idx1]->getscaleparentonly().y, parentchildlistlist[0][idx1]->getscaleparentonly().z);
				RenderMesh(meshList[parentchildlistlist[0][idx1]->getmodel()], objectlist[idx].getlighting());
				modelStack.PopMatrix();
			}
			parentchildlistlist.pop_back();
		}

		modelStack.Rotate(objectlist[idx].getrotationparentonly().x, 1, 0, 0);
		modelStack.Rotate(objectlist[idx].getrotationparentonly().y, 0, 1, 0);
		modelStack.Rotate(objectlist[idx].getrotationparentonly().z, 0, 0, 1);
		modelStack.Scale(objectlist[idx].getscaleparentonly().x, objectlist[idx].getscaleparentonly().y, objectlist[idx].getscaleparentonly().z);
		RenderMesh(meshList[objectlist[idx].getmodel()], objectlist[idx].getlighting());
		modelStack.PopMatrix();
	}

	if (debug)
	{
		UIstringoutput << "FPS:" << fps;
		UIstringoutput.precision(4);
		RenderTextOnScreen(meshList[GEO_TEXT], UIstringoutput.str(), Color(0, 1, 0), 4, 0, 0);
		UIstringoutput.str("");

		UIstringoutput << "Pos x:" << player.getposition().x << " y:" << player.getposition().y << " z:" << player.getposition().z;
		UIstringoutput.precision(3);
		RenderTextOnScreen(meshList[GEO_TEXT], UIstringoutput.str(), Color(1, 1, 1), 4, 0, 50);
		UIstringoutput.str("");
	}

	RenderMeshOnScreen(meshList[GEO_LIGHTBALL], Vector3(0.5, 0.5, 0.5), 0, 40, 30);
	//RenderMesh(meshList[GEO_LIGHTBALL])

	UIstringoutput << "Time:" << (int)timer;
	RenderTextOnScreen(meshList[GEO_TEXT], UIstringoutput.str(), Color(0, 1, 0), 5, 0, 50);
	UIstringoutput.str("");


	if (gameover)
	{
		RenderMeshOnScreen(meshList[GEO_GAMEOVERBACKGROUND], Vector3(100, 100, 1), 0, 40, 30);
		RenderMeshOnScreen(meshList[GEO_GAMEOVER], Vector3(60, 10, 0), 0, 40, 50);
		RenderMeshOnScreen(meshList[GEO_GAMEOVERBUTTON], Vector3(21, 9, 0), 0, 40, 10);
	}

	UIstringoutput << "Score:" << score;
	RenderTextOnScreen(meshList[GEO_TEXT], UIstringoutput.str(), Color(0, 1, 0), 5, 0, 0);
	UIstringoutput.str("");

}


void shootingminigame::RenderCredits()
{

}

void shootingminigame::Exit()
{
	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		if (meshList[i])
		{
			delete meshList[i];
		}
	}
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}

void shootingminigame::RenderMesh(Mesh* mesh, bool enableLight)
{
	Mtx44 MVP, modelView, modelView_inverse_transpose;

	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	modelView = viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MODELVIEW], 1, GL_FALSE, &modelView.a[0]);
	if (enableLight)
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 1);
		modelView_inverse_transpose = modelView.GetInverse().GetTranspose();
		glUniformMatrix4fv(m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE, &modelView_inverse_transpose.a[0]);

		//load material
		glUniform3fv(m_parameters[U_MATERIAL_AMBIENT], 1, &mesh->material.kAmbient.r);
		glUniform3fv(m_parameters[U_MATERIAL_DIFFUSE], 1, &mesh->material.kDiffuse.r);
		glUniform3fv(m_parameters[U_MATERIAL_SPECULAR], 1, &mesh->material.kSpecular.r);
		glUniform1f(m_parameters[U_MATERIAL_SHININESS], mesh->material.kShininess);
	}
	else
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	}

	if (mesh->textureID > 0)
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->textureID);
		glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	}
	else
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 0);
	}


	mesh->Render();

	if (mesh->textureID > 0)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

}

void shootingminigame::RenderSkybox(Vector3 skyboxoffset)
{
	modelStack.PushMatrix();
	{
		modelStack.Translate(camera.position.x, camera.position.y, camera.position.z);
		modelStack.Rotate(rotateSkybox, 0, 1, 0);
		{
			modelStack.PushMatrix();
			modelStack.Rotate(0, 0, 1, 0);
			modelStack.Translate(0, 0, 499);
			modelStack.Rotate(180, 0, 1, 0);
			modelStack.Scale(1000, 1000, 1000);
			RenderMesh(meshList[GEO_FRONT], false);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Rotate(0, 0, 1, 0);
			modelStack.Translate(499, 0, 0);
			modelStack.Rotate(-90, 0, 1, 0);
			modelStack.Scale(1000, 1000, 1000);
			RenderMesh(meshList[GEO_LEFT], false);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Rotate(0, 0, 1, 0);
			modelStack.Translate(-499, 0, 0);
			modelStack.Rotate(90, 0, 1, 0);
			modelStack.Scale(1000, 1000, 1000);
			RenderMesh(meshList[GEO_RIGHT], false);
			modelStack.PopMatrix();


			modelStack.PushMatrix();
			modelStack.Rotate(0, 0, 1, 0);
			modelStack.Translate(0, 499, 0);
			modelStack.Rotate(90, 1, 0, 0);
			modelStack.Rotate(-180, 0, 0, 1);
			modelStack.Scale(1000, 1000, 1000);
			RenderMesh(meshList[GEO_TOP], false);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Rotate(0, 0, 1, 0);
			modelStack.Translate(0, -499, 0);
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(1000, 1000, 1000);
			RenderMesh(meshList[GEO_BOTTOM], false);
			modelStack.PopMatrix();


			modelStack.PushMatrix();
			modelStack.Rotate(0, 0, 1, 0);
			modelStack.Translate(0, 0, -499);
			modelStack.Scale(1000, 1000, 1000);
			RenderMesh(meshList[GEO_BACK], false);
			modelStack.PopMatrix();
		}
		modelStack.Rotate(0, 0, 0, 1);
		modelStack.Scale(1, 1, 1);
	}
	modelStack.PopMatrix();
}

void shootingminigame::RenderText(Mesh* mesh, std::string text, Color color)
{
	if (!mesh || mesh->textureID <= 0) //Proper error check
		return;
	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	float offset = 0;
	for (unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(0.5f + offset, 0.5f, 0);
		offset += (textSpacing[(int)(text[i])] / (float)textMaxWidth);

		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
}

void shootingminigame::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
{
	if (!mesh || mesh->textureID <= 0) //Proper error check
		return;
	glDisable(GL_DEPTH_TEST); //uncomment for RenderTextOnScreen
	Mtx44 ortho;
	ortho.SetToOrtho(0, 80, 0, 60, -10, 10); //size of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	modelStack.PushMatrix();
	modelStack.LoadIdentity(); //Reset modelStack
	modelStack.Translate(x, y, 0);
	modelStack.Scale(size, size, size);

	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	float offset = 0;
	for (unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(0.5f + offset, 0.5f, 0);
		offset += (textSpacing[(int)(text[i])] / (float)textMaxWidth);

		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();
	glEnable(GL_DEPTH_TEST); //uncomment for RenderTextOnScreen

}


void shootingminigame::RenderMeshOnScreen(Mesh* mesh, Vector3 size, float rotate, float x, float y)
{
	glDisable(GL_DEPTH_TEST);
	Mtx44 ortho;
	ortho.SetToOrtho(0, 80, 0, 60, -10, 10); //size of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	modelStack.PushMatrix();
	modelStack.LoadIdentity(); //Reset modelStack
	modelStack.Translate(x, y, 0);
	modelStack.Rotate(rotate, 0, 0, 1);
	modelStack.Scale(size.x, size.y, size.z);

	Mtx44 MVP, modelView, modelView_inverse_transpose;
	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	modelView = viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MODELVIEW], 1, GL_FALSE, &modelView.a[0]);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	if (mesh->textureID > 0) {
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->textureID);
		glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	}
	else {
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 0);
	}
	mesh->Render();
	if (mesh->textureID > 0) {
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();
	glEnable(GL_DEPTH_TEST);
}


// Will be modified

void shootingminigame::NPCMove(unsigned ObjectID, float& x, float& z, double dt, int choice)
{
	if (CollisionCircleRect1(camera.position.x, camera.position.z, 1.9, x, z, objectlist[ObjectID].gethitboxcollisionsize().x /*HitboxX*/, objectlist[ObjectID].gethitboxcollisionsize().z /*HitboxZ*/) == 0) {
		if (choice == 1) {
			x -= (float)(1 * dt);
		}
		else if (choice == 2) {
			x += (float)(1 * dt);
		}
		else if (choice == 3) {
			z -= (float)(1 * dt);
		}
		else if (choice == 4) {
			z += (float)(1 * dt);
		}

	}
	objectlist[ObjectID].Setposition(Vector3(x, 0, z));
}


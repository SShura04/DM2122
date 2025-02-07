#include "SP2.h"
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

float DistanceParameter(float EnemyXPos, float EnemyZPos, float OriginalPosX, float OriginalPosZ) {
	float squareofxz = (pow(EnemyXPos - OriginalPosX, 2) + pow(EnemyZPos - OriginalPosZ, 2));
	float magnitude = sqrt(squareofxz);

	return magnitude;
}

void SP2::UseScene()
{
	glBindVertexArray(m_vertexArrayID);
	glUseProgram(m_programID);

	Mesh::SetMaterialLoc(m_parameters[U_MATERIAL_AMBIENT],
		m_parameters[U_MATERIAL_DIFFUSE],
		m_parameters[U_MATERIAL_SPECULAR],
		m_parameters[U_MATERIAL_SHININESS]);
}
Vector3 SP2::CollisionCircleRect(float cx, float cz, float radius, float rx, float rz, float rw, float rb) {
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
		if (rayToNearest == Vector3(0, 0, 0))
		{
			return endPos;
		}
		// Statically resolve the collision
		endPos = endPos - rayToNearest.Normalized() * overlap;
	}

	return endPos;
}



bool SP2::CollisionCircleRect1(float cx, float cz, float radius, float rx, float rz, float rw, float rb) {
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

Vector3 SP2::PlayerCollision(unsigned count, float x, float z) {
	if (count == count) {
		return CollisionCircleRect(x, z, 1.5, objectlist[count].getposition().x, objectlist[count].getposition().z, objectlist[count].gethitboxcollisionsize().x /*HitboxX*/, objectlist[count].gethitboxcollisionsize().z /*HitboxZ*/);
	}
	else
	{
		return Vector3(x, 0, z); //wont trigger
	}
}


bool interactableObjectRect(int Px, int Pz, int ObjectX, int ObjectZ, int x, int z)
{
	for (int i = 0; i <= x; i++) {
		for (int j = 0; j <= z; j++) {
			if (Px == ObjectX + i and Pz == ObjectZ + j) {
				return true;
			}
			if (Px == ObjectX - i and Pz == ObjectZ - j) {
				return true;
			}
			if (Px == ObjectX + i and Pz == ObjectZ - j) {
				return true;
			}
			if (Px == ObjectX - i and Pz == ObjectZ + j) {
				return true;
			}
		}
	}
	return false;
}

SP2::SP2()
{
}

SP2::~SP2()
{
}

void SP2::Init()
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


	{
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

		//light 3
		m_parameters[U_LIGHT2_POSITION] = glGetUniformLocation(m_programID, "lights[2].position_cameraspace");
		m_parameters[U_LIGHT2_COLOR] = glGetUniformLocation(m_programID, "lights[2].color");
		m_parameters[U_LIGHT2_POWER] = glGetUniformLocation(m_programID, "lights[2].power");
		m_parameters[U_LIGHT2_KC] = glGetUniformLocation(m_programID, "lights[2].kC");
		m_parameters[U_LIGHT2_KL] = glGetUniformLocation(m_programID, "lights[2].kL");
		m_parameters[U_LIGHT2_KQ] = glGetUniformLocation(m_programID, "lights[2].kQ");
		m_parameters[U_LIGHT2_TYPE] = glGetUniformLocation(m_programID, "lights[2].type");
		m_parameters[U_LIGHT2_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[2].spotDirection");
		m_parameters[U_LIGHT2_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[2].cosCutoff");
		m_parameters[U_LIGHT2_COSINNER] = glGetUniformLocation(m_programID, "lights[2].cosInner");
		m_parameters[U_LIGHT2_EXPONENT] = glGetUniformLocation(m_programID, "lights[2].exponent");

		//light 4
		m_parameters[U_LIGHT3_POSITION] = glGetUniformLocation(m_programID, "lights[3].position_cameraspace");
		m_parameters[U_LIGHT3_COLOR] = glGetUniformLocation(m_programID, "lights[3].color");
		m_parameters[U_LIGHT3_POWER] = glGetUniformLocation(m_programID, "lights[3].power");
		m_parameters[U_LIGHT3_KC] = glGetUniformLocation(m_programID, "lights[3].kC");
		m_parameters[U_LIGHT3_KL] = glGetUniformLocation(m_programID, "lights[3].kL");
		m_parameters[U_LIGHT3_KQ] = glGetUniformLocation(m_programID, "lights[3].kQ");
		m_parameters[U_LIGHT3_TYPE] = glGetUniformLocation(m_programID, "lights[3].type");
		m_parameters[U_LIGHT3_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[3].spotDirection");
		m_parameters[U_LIGHT3_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[3].cosCutoff");
		m_parameters[U_LIGHT3_COSINNER] = glGetUniformLocation(m_programID, "lights[3].cosInner");
		m_parameters[U_LIGHT3_EXPONENT] = glGetUniformLocation(m_programID, "lights[3].exponent");

		m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
		m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");

		// Get a handle for our "colorTexture" uniform
		m_parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_programID, "colorTextureEnabled");
		m_parameters[U_COLOR_TEXTURE] = glGetUniformLocation(m_programID, "colorTexture");

		// Get a handle for our "textColor" uniform
		m_parameters[U_TEXT_ENABLED] = glGetUniformLocation(m_programID, "textEnabled");
		m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID, "textColor");

		glUseProgram(m_programID);
		glUniform1i(m_parameters[U_NUMLIGHTS], 4);

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

		//light 3
		light[2].type = Light::LIGHT_SPOT;
		light[2].position.Set(17.6, 1, 7.25);
		light[2].color.Set(255.0f / 255.0f, 255.0f / 255.0f, 1.f / 255.0f);
		light[2].power = 0.4f;
		light[2].kC = 1.f;
		light[2].kL = 0.01f;
		light[2].kQ = 0.001f;
		light[2].cosCutoff = cos(Math::DegreeToRadian(45));
		light[2].cosInner = cos(Math::DegreeToRadian(30));
		light[2].exponent = 3.f;
		light[2].spotDirection.Set(0.f, 1.f, 0.f);
		glUniform3fv(m_parameters[U_LIGHT2_COLOR], 1, &light[2].color.r);
		glUniform1f(m_parameters[U_LIGHT2_POWER], light[2].power);
		glUniform1f(m_parameters[U_LIGHT2_KC], light[2].kC);
		glUniform1f(m_parameters[U_LIGHT2_KL], light[2].kL);
		glUniform1f(m_parameters[U_LIGHT2_KQ], light[2].kQ);
		glUniform1i(m_parameters[U_LIGHT2_TYPE], light[2].type);
		glUniform3fv(m_parameters[U_LIGHT2_COLOR], 1, &light[2].color.r);
		glUniform1f(m_parameters[U_LIGHT2_KC], light[2].kC);
		glUniform1f(m_parameters[U_LIGHT2_POWER], light[2].power);
		glUniform1f(m_parameters[U_LIGHT2_KL], light[2].kL);
		glUniform1f(m_parameters[U_LIGHT2_KQ], light[2].kQ);
		glUniform1f(m_parameters[U_LIGHT2_COSCUTOFF], light[2].cosCutoff);
		glUniform1f(m_parameters[U_LIGHT2_COSINNER], light[2].cosInner);
		glUniform1f(m_parameters[U_LIGHT2_EXPONENT], light[2].exponent);

		//light 4
		light[3].type = Light::LIGHT_SPOT;
		light[3].position.Set(3.5, 1, -28);
		light[3].color.Set(255.0f / 255.0f, 255.0f / 255.0f, 1.f / 255.0f);
		light[3].power = 0.4f;
		light[3].kC = 1.f;
		light[3].kL = 0.01f;
		light[3].kQ = 0.001f;
		light[3].cosCutoff = cos(Math::DegreeToRadian(45));
		light[3].cosInner = cos(Math::DegreeToRadian(30));
		light[3].exponent = 3.f;
		light[3].spotDirection.Set(0.f, 1.f, 0.f);
		glUniform3fv(m_parameters[U_LIGHT3_COLOR], 1, &light[3].color.r);
		glUniform1f(m_parameters[U_LIGHT3_POWER], light[3].power);
		glUniform1f(m_parameters[U_LIGHT3_KC], light[3].kC);
		glUniform1f(m_parameters[U_LIGHT3_KL], light[3].kL);
		glUniform1f(m_parameters[U_LIGHT3_KQ], light[3].kQ);
		glUniform1i(m_parameters[U_LIGHT3_TYPE], light[3].type);
		glUniform3fv(m_parameters[U_LIGHT3_COLOR], 1, &light[3].color.r);
		glUniform1f(m_parameters[U_LIGHT3_KC], light[3].kC);
		glUniform1f(m_parameters[U_LIGHT3_POWER], light[3].power);
		glUniform1f(m_parameters[U_LIGHT3_KL], light[3].kL);
		glUniform1f(m_parameters[U_LIGHT3_KQ], light[3].kQ);
		glUniform1f(m_parameters[U_LIGHT3_COSCUTOFF], light[3].cosCutoff);
		glUniform1f(m_parameters[U_LIGHT3_COSINNER], light[3].cosInner);
		glUniform1f(m_parameters[U_LIGHT3_EXPONENT], light[3].exponent);

		Mesh::SetMaterialLoc(m_parameters[U_MATERIAL_AMBIENT], m_parameters[U_MATERIAL_DIFFUSE], m_parameters[U_MATERIAL_SPECULAR], m_parameters[U_MATERIAL_SHININESS]);
	}

	//variable to rotate geometry
	rotateAngle = 0;

	//Initialize camera settings
	camera.Init(Vector3(19.3, -18, 1.246), Vector3(19.3, -18, 2.246), Vector3(0, 1, 0));


	// Init VBO
	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		meshList[i] = nullptr;
	}
	meshList[GEO_AXES] = MeshBuilder::GenerateAxes("Axes", 100, 100, 100);
	meshList[GEO_LIGHTBALL] = MeshBuilder::GenerateSphere("sphere", Color(1, 1, 1), 10, 20, 1);

	//ground
	meshList[GEO_QUAD] = MeshBuilder::GenerateQuad("quad", Color(1, 1, 1), 1.f, 8);
	meshList[GEO_QUAD]->textureID = LoadTGA("Image//grass.tga");

	//ground
	meshList[GEO_CASH] = MeshBuilder::GenerateQuad("cash", Color(1, 1, 1), 1.f);
	meshList[GEO_CASH]->textureID = LoadTGA("Image//Cash.tga");

	meshList[GEO_CUBE1] = MeshBuilder::GenerateCube("cube1", Color(1, 1, 0), 2.f);

	//Garbage
	meshList[GEO_GARBAGE] = MeshBuilder::GenerateQuad("garbage", Color(1, 1, 1), 1.f);
	meshList[GEO_GARBAGE]->textureID = LoadTGA("Image//garbage2.tga");

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

	//Npc
	meshList[GEO_NPC1] = MeshBuilder::GenerateOBJMTL("npc1", "OBJ//advancedCharacter.obj", "OBJ//advancedCharacter.obj.mtl");
	meshList[GEO_NPC1]->textureID = LoadTGA("Image//skin_adventurer.tga");
	objectlist[hb_NPC1].setmesh(GEO_NPC1);
	objectlist[hb_NPC1].setproperties(Vector3(7.5, 0, -8), Vector3(0, -90, 0), Vector3(0.16, 0.16, 0.16));
	objectlist[hb_NPC1].sethitboxcollisionsize(Vector3(1, 0, 1));
	objectlist[hb_NPC1].setlighting(false);
	NPCX = objectlist[hb_NPC1].getposition().x;
	NPCZ = objectlist[hb_NPC1].getposition().z;



	meshList[GEO_NPC2] = MeshBuilder::GenerateOBJMTL("npc2", "OBJ//advancedCharacter.obj", "OBJ//advancedCharacter.obj.mtl");
	meshList[GEO_NPC2]->textureID = LoadTGA("Image//skin_man.tga");
	objectlist[hb_ShopKeeper].setmesh(GEO_NPC2);
	objectlist[hb_ShopKeeper].setproperties(Vector3(3.5, -20, -36.5), Vector3(0, 0, 0), Vector3(0.16, 0.16, 0.16));
	objectlist[hb_ShopKeeper].sethitboxcollisionsize(Vector3(1, 0, 1));
	objectlist[hb_ShopKeeper].setlighting(false);
	objectlist[hb_NPC2].setmesh(GEO_NPC2);
	objectlist[hb_NPC2].setproperties(Vector3(0, 0, 4), Vector3(0, 90, 0), Vector3(0.16, 0.16, 0.16));
	objectlist[hb_NPC2].sethitboxcollisionsize(Vector3(1, 0, 1));
	objectlist[hb_NPC2].setlighting(false);
	NPCX2 = objectlist[hb_NPC2].getposition().x;
	NPCZ2 = objectlist[hb_NPC2].getposition().z;

	objectlist[hb_NPC3].setmesh(GEO_NPC2);
	objectlist[hb_NPC3].setproperties(Vector3(-20, 0, 15), Vector3(0, 90, 0), Vector3(0.16, 0.16, 0.16));
	objectlist[hb_NPC3].sethitboxcollisionsize(Vector3(1, 0, 1));
	objectlist[hb_NPC3].setlighting(false);
	NPCX3 = objectlist[hb_NPC3].getposition().x;
	NPCZ3 = objectlist[hb_NPC3].getposition().z;

	meshList[GEO_POLICE] = MeshBuilder::GenerateOBJMTL("police", "OBJ//advancedCharacter.obj", "OBJ//advancedCharacter.obj.mtl");
	meshList[GEO_POLICE]->textureID = LoadTGA("Image//skin_police.tga");
	objectlist[hb_POLICE1].setmesh(GEO_POLICE);
	objectlist[hb_POLICE1].setproperties(Vector3(5, 0, -200), Vector3(0, 0, 0), Vector3(0.16, 0.16, 0.16));
	objectlist[hb_POLICE1].sethitboxcollisionsize(Vector3(1, 0, 1));
	objectlist[hb_POLICE1].setlighting(false);
	objectlist[hb_POLICE2].setmesh(GEO_POLICE);
	objectlist[hb_POLICE2].setproperties(Vector3(5, 0, -200), Vector3(0, 0, 0), Vector3(0.16, 0.16, 0.16));
	objectlist[hb_POLICE2].sethitboxcollisionsize(Vector3(1, 0, 1));
	objectlist[hb_POLICE2].setlighting(false);
	objectlist[hb_POLICE3].setmesh(GEO_POLICE);
	objectlist[hb_POLICE3].setproperties(Vector3(5, 0, -200), Vector3(0, 0, 0), Vector3(0.16, 0.16, 0.16));
	objectlist[hb_POLICE3].sethitboxcollisionsize(Vector3(1, 0, 1));
	objectlist[hb_POLICE3].setlighting(false);
	objectlist[hb_POLICE4].setmesh(GEO_POLICE);
	objectlist[hb_POLICE4].setproperties(Vector3(5, 0, -200), Vector3(0, 0, 0), Vector3(0.16, 0.16, 0.16));
	objectlist[hb_POLICE4].sethitboxcollisionsize(Vector3(1, 0, 1));
	objectlist[hb_POLICE4].setlighting(false);
	objectlist[hb_POLICE5].setmesh(GEO_POLICE);
	objectlist[hb_POLICE5].setproperties(Vector3(5, 0, -200), Vector3(0, 0, 0), Vector3(0.16, 0.16, 0.16));
	objectlist[hb_POLICE5].sethitboxcollisionsize(Vector3(1, 0, 1));
	objectlist[hb_POLICE5].setlighting(false);

	meshList[GEO_DIALOGUEUI] = MeshBuilder::GenerateQuad("dialoguebox", Color(1, 1, 1), 1.f);
	meshList[GEO_DIALOGUEUI]->textureID = LoadTGA("Image//DialogueBox.tga");


	//Sound
	meshList[GEO_SOUND_UNMUTE] = MeshBuilder::GenerateQuad("audio", Color(1, 1, 1), 1.f);
	meshList[GEO_SOUND_UNMUTE]->textureID = LoadTGA("Image//AudioIcon.tga");
	meshList[GEO_SOUND_MUTE] = MeshBuilder::GenerateQuad("mueaudio", Color(1, 1, 1), 1.f);
	meshList[GEO_SOUND_MUTE]->textureID = LoadTGA("Image//MuteIcon.tga");


	// Stars
	meshList[GEO_STAR1] = MeshBuilder::GenerateQuad("star1", Color(1, 1, 1), 1.f);
	meshList[GEO_STAR1]->textureID = LoadTGA("Image//1_Star.tga");
	meshList[GEO_STAR2] = MeshBuilder::GenerateQuad("star2", Color(1, 1, 1), 1.f);
	meshList[GEO_STAR2]->textureID = LoadTGA("Image//2_Star.tga");
	meshList[GEO_STAR3] = MeshBuilder::GenerateQuad("star3", Color(1, 1, 1), 1.f);
	meshList[GEO_STAR3]->textureID = LoadTGA("Image//3_Star.tga");
	meshList[GEO_STAR4] = MeshBuilder::GenerateQuad("star4", Color(1, 1, 1), 1.f);
	meshList[GEO_STAR4]->textureID = LoadTGA("Image//4_Star.tga");
	meshList[GEO_STAR5] = MeshBuilder::GenerateQuad("star1", Color(1, 1, 1), 1.f);
	meshList[GEO_STAR5]->textureID = LoadTGA("Image//5_Star.tga");

	meshList[GEO_STAR1_Grey] = MeshBuilder::GenerateQuad("star1", Color(1, 1, 1), 1.f);
	meshList[GEO_STAR1_Grey]->textureID = LoadTGA("Image//1_Star_Grey.tga");
	meshList[GEO_STAR2_Grey] = MeshBuilder::GenerateQuad("star2", Color(1, 1, 1), 1.f);
	meshList[GEO_STAR2_Grey]->textureID = LoadTGA("Image//2_Star_Grey.tga");
	meshList[GEO_STAR3_Grey] = MeshBuilder::GenerateQuad("star3", Color(1, 1, 1), 1.f);
	meshList[GEO_STAR3_Grey]->textureID = LoadTGA("Image//3_Star_Grey.tga");
	meshList[GEO_STAR4_Grey] = MeshBuilder::GenerateQuad("star4", Color(1, 1, 1), 1.f);
	meshList[GEO_STAR4_Grey]->textureID = LoadTGA("Image//4_Star_Grey.tga");
	meshList[GEO_STAR5_Grey] = MeshBuilder::GenerateQuad("star1", Color(1, 1, 1), 1.f);
	meshList[GEO_STAR5_Grey]->textureID = LoadTGA("Image//5_Star_Grey.tga");

	//houses
	//fat tall
	meshList[GEO_HOUSE1] = MeshBuilder::GenerateOBJMTL("modelhouse1", "OBJ//large_buildingB.obj", "OBJ//large_buildingB.mtl");
	objectlist[hb_HOUSE1].setmesh(GEO_HOUSE1);
	objectlist[hb_HOUSE1].setproperties(Vector3(-19, 0, 21.6), Vector3(0, 0, 0), Vector3(7, 10, 11));
	objectlist[hb_HOUSE1].sethitboxcollisionsize(Vector3(4, 0, 6.7));

	objectlist[hb_HOUSE6].setmesh(GEO_HOUSE1);
	objectlist[hb_HOUSE6].setproperties(Vector3(-13, 0, 3), Vector3(0, 180, 0), Vector3(7, 10, 9));
	objectlist[hb_HOUSE6].sethitboxcollisionsize(Vector3(4, 0, 6.7));

	objectlist[hb_HOUSE11].setmesh(GEO_HOUSE1);
	objectlist[hb_HOUSE11].setproperties(Vector3(-13, 0, -19), Vector3(0, -90, 0), Vector3(9, 13, 7));
	objectlist[hb_HOUSE11].sethitboxcollisionsize(Vector3(4, 0, 6.7));

	objectlist[hb_HOUSE17].setmesh(GEO_HOUSE1);
	objectlist[hb_HOUSE17].setproperties(Vector3(25.6, 0, 20), Vector3(0, 0, 0), Vector3(7, 10, 7));
	objectlist[hb_HOUSE17].sethitboxcollisionsize(Vector3(4, 0, 4));

	objectlist[hb_HOUSE20].setmesh(GEO_HOUSE1);
	objectlist[hb_HOUSE20].setproperties(Vector3(19, 0, -5), Vector3(0, 90, 0), Vector3(7, 10, 7));
	objectlist[hb_HOUSE20].sethitboxcollisionsize(Vector3(4, 0, 4));

	objectlist[hb_HOUSE22].setmesh(GEO_HOUSE1);
	objectlist[hb_HOUSE22].setproperties(Vector3(19, 0, -10.5), Vector3(0, 90, 0), Vector3(7, 10, 7));
	objectlist[hb_HOUSE22].sethitboxcollisionsize(Vector3(4, 0, 6.7));

	objectlist[hb_HOUSE29].setmesh(GEO_HOUSE1);
	objectlist[hb_HOUSE29].setproperties(Vector3(-23, 0, -35.8), Vector3(0, 90, 0), Vector3(7, 10, 7));
	objectlist[hb_HOUSE29].sethitboxcollisionsize(Vector3(4, 0, 4));

	objectlist[hb_HOUSE30].setmesh(GEO_HOUSE1);
	objectlist[hb_HOUSE30].setproperties(Vector3(-13, 0, -35.4), Vector3(0, -90, 0), Vector3(7, 13, 7));
	objectlist[hb_HOUSE30].sethitboxcollisionsize(Vector3(4, 0, 4));

	//long
	meshList[GEO_HOUSE2] = MeshBuilder::GenerateOBJMTL("modelhouse2", "OBJ//large_buildingF.obj", "OBJ//large_buildingF.mtl");
	objectlist[hb_HOUSE2].setmesh(GEO_HOUSE2);
	objectlist[hb_HOUSE2].setproperties(Vector3(-9.2, 0, 20), Vector3(0, 0, 0), Vector3(7, 10, 7));
	objectlist[hb_HOUSE2].sethitboxcollisionsize(Vector3(12, 0, 4));
	objectlist[hb_HOUSE_TEST].setmesh(GEO_HOUSE2);
	objectlist[hb_HOUSE_TEST].setproperties(Vector3(-28.8, 0, 20), Vector3(0, 0, 0), Vector3(7, 10, 7));
	objectlist[hb_HOUSE_TEST].sethitboxcollisionsize(Vector3(12, 0, 4));
	objectlist[hb_HOUSE_TEST2].setmesh(GEO_HOUSE2);
	objectlist[hb_HOUSE_TEST2].setproperties(Vector3(42.2, 0, 20), Vector3(0, 0, 0), Vector3(7, 10, 7));
	objectlist[hb_HOUSE_TEST2].sethitboxcollisionsize(Vector3(12, 0, 4));
	objectlist[hb_HOUSE_TEST3].setmesh(GEO_HOUSE2);
	objectlist[hb_HOUSE_TEST3].setproperties(Vector3(39, 0, 3.4), Vector3(0, 180, 0), Vector3(7, 10, 7));
	objectlist[hb_HOUSE_TEST3].sethitboxcollisionsize(Vector3(12, 0, 4));
	objectlist[hb_HOUSE_TEST4].setmesh(GEO_HOUSE2);
	objectlist[hb_HOUSE_TEST4].setproperties(Vector3(-34.8, 0, 3.6), Vector3(0, 180, 0), Vector3(7, 10, 7));
	objectlist[hb_HOUSE_TEST4].sethitboxcollisionsize(Vector3(12, 0, 4));


	objectlist[hb_HOUSE7].setmesh(GEO_HOUSE2);
	objectlist[hb_HOUSE7].setproperties(Vector3(-23, 0, -16), Vector3(0, 90, 0), Vector3(17, 10, 7));
	objectlist[hb_HOUSE7].sethitboxcollisionsize(Vector3(4, 0, 32));

	objectlist[hb_HOUSE12].setmesh(GEO_HOUSE2);
	objectlist[hb_HOUSE12].setproperties(Vector3(-13, 0, -27.6), Vector3(0, -90, 0), Vector3(5, 13, 7));
	objectlist[hb_HOUSE12].sethitboxcollisionsize(Vector3(4, 0, 8));

	objectlist[hb_HOUSE16].setmesh(GEO_HOUSE2);
	objectlist[hb_HOUSE16].setproperties(Vector3(22.4, 0, -32), Vector3(0, 180, 0), Vector3(7, 9, 7));
	objectlist[hb_HOUSE16].sethitboxcollisionsize(Vector3(12, 0, 4));

	objectlist[hb_HOUSE21].setmesh(GEO_HOUSE2);
	objectlist[hb_HOUSE21].setproperties(Vector3(32, 0, -7.2), Vector3(0, -90, 0), Vector3(7, 10, 7));
	objectlist[hb_HOUSE21].sethitboxcollisionsize(Vector3(4, 0, 12));

	objectlist[hb_HOUSE23].setmesh(GEO_HOUSE2);
	objectlist[hb_HOUSE23].setproperties(Vector3(32, 0, -21.8), Vector3(0, -90, 0), Vector3(8, 10, 7));
	objectlist[hb_HOUSE23].sethitboxcollisionsize(Vector3(4, 0, 14));

	objectlist[hb_HOUSE25].setmesh(GEO_HOUSE2);
	objectlist[hb_HOUSE25].setproperties(Vector3(-6, 0, 28.7), Vector3(0, 180, 0), Vector3(13, 10, 7));
	objectlist[hb_HOUSE25].sethitboxcollisionsize(Vector3(24, 0, 4));

	objectlist[hb_HOUSE26].setmesh(GEO_HOUSE2);
	objectlist[hb_HOUSE26].setproperties(Vector3(15, 0, 28.7), Vector3(0, 180, 0), Vector3(13, 10, 7));
	objectlist[hb_HOUSE26].sethitboxcollisionsize(Vector3(24, 0, 4));

	objectlist[hb_HOUSE31].setmesh(GEO_HOUSE2);
	objectlist[hb_HOUSE31].setproperties(Vector3(-13.5, 0, -44.5), Vector3(0, 0, 0), Vector3(7, 9, 7));
	objectlist[hb_HOUSE31].sethitboxcollisionsize(Vector3(12, 0, 4));

	objectlist[hb_HOUSE33].setmesh(GEO_HOUSE2);
	objectlist[hb_HOUSE33].setproperties(Vector3(-5, 0, -38), Vector3(0, -90, 0), Vector3(4, 9, 4));
	objectlist[hb_HOUSE33].sethitboxcollisionsize(Vector3(2, 0, 6));

	//skinny tall w/ stairs
	meshList[GEO_HOUSE3] = MeshBuilder::GenerateOBJMTL("modelhouse3", "OBJ//large_buildingC.obj", "OBJ//large_buildingC.mtl");
	objectlist[hb_HOUSE3].setmesh(GEO_HOUSE3);
	objectlist[hb_HOUSE3].setproperties(Vector3(5, 0, 20), Vector3(0, 0, 0), Vector3(7, 10, 7));
	objectlist[hb_HOUSE3].sethitboxcollisionsize(Vector3(4, 0, 4));

	objectlist[hb_HOUSE10].setmesh(GEO_HOUSE3);
	objectlist[hb_HOUSE10].setproperties(Vector3(-13, 0, -9), Vector3(0, -90, 0), Vector3(7, 9, 7));
	objectlist[hb_HOUSE10].sethitboxcollisionsize(Vector3(4, 0, 4));

	objectlist[hb_HOUSE19].setmesh(GEO_HOUSE3);
	objectlist[hb_HOUSE19].setproperties(Vector3(29, 0, 3), Vector3(0, 180, 0), Vector3(8, 10, 8));
	objectlist[hb_HOUSE19].sethitboxcollisionsize(Vector3(4.5, 0, 4.5));

	//skinny tall w/o stairs
	meshList[GEO_HOUSE4] = MeshBuilder::GenerateOBJMTL("modelhouse4", "OBJ//large_buildingD.obj", "OBJ//large_buildingD.mtl");
	objectlist[hb_HOUSE4].setmesh(GEO_HOUSE4);
	objectlist[hb_HOUSE4].setproperties(Vector3(10.7, 0, 20.2), Vector3(0, 0, 0), Vector3(5, 10, 5));
	objectlist[hb_HOUSE4].sethitboxcollisionsize(Vector3(4, 0, 4));

	objectlist[hb_HOUSE8].setmesh(GEO_HOUSE4);
	objectlist[hb_HOUSE8].setproperties(Vector3(-23, 0, 3.5), Vector3(0, 180, 0), Vector3(5, 10, 5));
	objectlist[hb_HOUSE8].sethitboxcollisionsize(Vector3(4.5, 0, 4.5));

	objectlist[hb_HOUSE14].setmesh(GEO_HOUSE4);
	objectlist[hb_HOUSE14].setproperties(Vector3(3.4, 0, -32.8), Vector3(0, 180, 0), Vector3(6, 9, 6));
	objectlist[hb_HOUSE14].sethitboxcollisionsize(Vector3(5.5, 0, 5.5));

	objectlist[hb_HOUSE15].setmesh(GEO_HOUSE4);
	objectlist[hb_HOUSE15].setproperties(Vector3(11.2, 0, -33.4), Vector3(0, 180, 0), Vector3(7, 9, 7));
	objectlist[hb_HOUSE15].sethitboxcollisionsize(Vector3(6.5, 0, 6.5));

	objectlist[hb_HOUSE24].setmesh(GEO_HOUSE4);
	objectlist[hb_HOUSE24].setproperties(Vector3(20.4, 0, -17.5), Vector3(0, 90, 0), Vector3(7, 10, 7));
	objectlist[hb_HOUSE24].sethitboxcollisionsize(Vector3(6.5, 0, 6.5));

	objectlist[hb_HOUSE28].setmesh(GEO_HOUSE4);
	objectlist[hb_HOUSE28].setproperties(Vector3(32, 0, 21.55), Vector3(0, 0, 0), Vector3(6, 10, 7.2));
	objectlist[hb_HOUSE28].sethitboxcollisionsize(Vector3(5.5, 0, 7));

	//short with roof
	meshList[GEO_HOUSE5] = MeshBuilder::GenerateOBJMTL("modelhouse5", "OBJ//small_buildingF.obj", "OBJ//small_buildingF.mtl");
	objectlist[hb_HOUSE5].setmesh(GEO_HOUSE5);
	objectlist[hb_HOUSE5].setproperties(Vector3(20, 0, 20), Vector3(0, 0, 0), Vector3(7, 10, 7));
	objectlist[hb_HOUSE5].sethitboxcollisionsize(Vector3(4, 0, 4));

	objectlist[hb_HOUSE9].setmesh(GEO_HOUSE5);
	objectlist[hb_HOUSE9].setproperties(Vector3(-13, 0, -3.41), Vector3(0, -90, 0), Vector3(7, 9, 7));
	objectlist[hb_HOUSE9].sethitboxcollisionsize(Vector3(4, 0, 4));

	objectlist[hb_HOUSE13].setmesh(GEO_HOUSE5);
	objectlist[hb_HOUSE13].setproperties(Vector3(-3, 0, -32), Vector3(0, 180, 0), Vector3(7, 9, 7));
	objectlist[hb_HOUSE13].sethitboxcollisionsize(Vector3(4, 0, 4));

	// Player house
	objectlist[hb_HOUSE18].setmesh(GEO_HOUSE5);
	objectlist[hb_HOUSE18].setproperties(Vector3(19, 0, 4), Vector3(0, 180, 0), Vector3(7, 9, 6));
	objectlist[hb_HOUSE18].sethitboxcollisionsize(Vector3(4, 0, 3.5));
	//---------------------------------------------------
	// Player house replica
	objectlist[hb_HOUSE34].setmesh(GEO_HOUSE5);
	objectlist[hb_HOUSE34].setproperties(Vector3(19, -20, 4), Vector3(180, 180, 0), Vector3(-7, -9, -12));
	//---------------------------------------------------
	// Player house interior collision
	objectlist[hb_Wall].setproperties(Vector3(23.3, -20, 4), Vector3(180, 180, 0), Vector3(1, 1, 1));
	objectlist[hb_Wall].sethitboxcollisionsize(Vector3(1, 0, 18));
	objectlist[hb_Wall2].setproperties(Vector3(14.7, -20, 4), Vector3(180, 180, 0), Vector3(1, 1, 1));
	objectlist[hb_Wall2].sethitboxcollisionsize(Vector3(1, 0, 18));
	objectlist[hb_Wall3].setproperties(Vector3(19, -20, 13.3), Vector3(180, 180, 0), Vector3(1, 1, 1));
	objectlist[hb_Wall3].sethitboxcollisionsize(Vector3(3, 0, 8));
	objectlist[hb_Wall4].setproperties(Vector3(19, -20, -5.7), Vector3(180, 180, 0), Vector3(1, 1, 1));
	objectlist[hb_Wall4].sethitboxcollisionsize(Vector3(3, 0, 8));
	
	// Bed collision
	objectlist[hb_WallBed].setproperties(Vector3(21.2, -20, 1), Vector3(180, 180, 0), Vector3(1, 1, 1));
	objectlist[hb_WallBed].sethitboxcollisionsize(Vector3(0.8, 0, 1.5));
	objectlist[hb_WallDesk].setproperties(Vector3(16, -20, 1), Vector3(180, 180, 0), Vector3(1, 1, 1));
	objectlist[hb_WallDesk].sethitboxcollisionsize(Vector3(0.9, 0, 1.5));
	objectlist[hb_WallTV].setproperties(Vector3(21.1, -20, 8.9), Vector3(180, 180, 0), Vector3(1, 1, 1));
	objectlist[hb_WallTV].sethitboxcollisionsize(Vector3(1, 0, 1.5));

	//house celings
	meshList[GEO_CEILING] = MeshBuilder::GenerateCube("ceilinghouse", Color(0.35, 0.36, 0.41), 2.f);
	meshList[GEO_CEILING]->material.kAmbient.Set(0.3107843, 0.3186275, 0.35);
	meshList[GEO_CEILING]->material.kDiffuse.Set(0.5607843, 0.5686275, 0.6);
	meshList[GEO_CEILING]->material.kShininess = 1.f;

	objectlist[hb_CEILINGHOUSE].setmesh(GEO_CEILING);
	objectlist[hb_CEILINGHOUSE].setproperties(Vector3(19, -16.3, 4), Vector3(0, -90, 0), Vector3(5, 0.1, 3));
	objectlist[hb_CEILINGHOUSE].sethitboxcollisionsize(Vector3(4, 0, 4));


	objectlist[hb_HOUSE27].setmesh(GEO_HOUSE5);
	objectlist[hb_HOUSE27].setproperties(Vector3(30.8, 0, 28.7), Vector3(0, 180, 0), Vector3(7, 10, 7));
	objectlist[hb_HOUSE27].sethitboxcollisionsize(Vector3(4, 0, 4));
	objectlist[hb_HOUSE32].setmesh(GEO_HOUSE5);
	objectlist[hb_HOUSE32].setproperties(Vector3(-23, 0, -41.4), Vector3(0, 90, 0), Vector3(7, 10, 7));
	objectlist[hb_HOUSE32].sethitboxcollisionsize(Vector3(4, 0, 4));




	//---------------------------------------------------
	//shop insides
	objectlist[hb_HOUSE35].setmesh(GEO_HOUSE4);
	objectlist[hb_HOUSE35].setproperties(Vector3(3.4, -20, -32.8), Vector3(180, 180, 0), Vector3(-6, -9, -10));
	//---------------------------------------------------
	objectlist[hb_WallSHOP1].setproperties(Vector3(8, -18, -30), Vector3(0, 0, 0), Vector3(0.1, 1, 6));
	objectlist[hb_WallSHOP1].sethitboxcollisionsize(Vector3(0.1, 0, 6));
	objectlist[hb_WallSHOP2].setproperties(Vector3(-1, -18, -30), Vector3(180, 180, 0), Vector3(3.7, 1, 0.1));
	objectlist[hb_WallSHOP2].sethitboxcollisionsize(Vector3(0.1, 0, 6));
	objectlist[hb_WallSHOP3].setproperties(Vector3(4, -18, -34), Vector3(180, 180, 0), Vector3(0.1, 1, 6));
	objectlist[hb_WallSHOP3].sethitboxcollisionsize(Vector3(10, 0, 0.1));
	objectlist[hb_WallSHOP4].setproperties(Vector3(2.9, -18, -26.5), Vector3(180, 180, 0), Vector3(3.7, 1, 0.1));
	objectlist[hb_WallSHOP4].sethitboxcollisionsize(Vector3(6, 0, 0.1));


	meshList[GEO_COUNTER] = MeshBuilder::GenerateOBJMTL("modelcounter", "OBJ//tableCloth.obj", "OBJ//tableCloth.mtl");
	objectlist[hb_SHOPSELLTABLE].setmesh(GEO_COUNTER);
	objectlist[hb_SHOPSELLTABLE].setproperties(Vector3(0, -20, -33), Vector3(0, 180, 0), Vector3(3, 4, 3));
	objectlist[hb_SHOPSELLTABLE].sethitboxcollisionsize(Vector3(1, 0, 1));
	meshList[GEO_TABLESHOP] = MeshBuilder::GenerateOBJMTL("modeltableshop", "OBJ//table.obj", "OBJ//table.mtl");
	objectlist[hb_SHOPTABLE].setmesh(GEO_TABLESHOP);
	objectlist[hb_SHOPTABLE].setproperties(Vector3(2.5, -20, -33), Vector3(0, 180, 0), Vector3(5.5, 4, 3));
	objectlist[hb_SHOPTABLE].sethitboxcollisionsize(Vector3(1, 0, 1));
	objectlist[hb_SHOPPROPTV].setmesh(GEO_TV);
	objectlist[hb_SHOPPROPTV].setproperties(Vector3(5.5, -18.7, -34), Vector3(0, 135, 0), Vector3(3, 3, 3));
	objectlist[hb_SHOPPROPTV].sethitboxcollisionsize(Vector3(1, 0, 1));
	meshList[GEO_SHOPPROPSHELF] = MeshBuilder::GenerateCube("shoppropshelf", Color(0.8962264, 0.6015712, 0.3931559), 2.f);
	objectlist[hb_SHOPPROPSHELF].setmesh(GEO_SHOPPROPSHELF);
	objectlist[hb_SHOPPROPSHELF].setproperties(Vector3(6.4, -18, -37), Vector3(0, 0, 0), Vector3(0.7, 0.1, 2));
	objectlist[hb_SHOPPROPSHELF].sethitboxcollisionsize(Vector3(1, 0, 1));
	meshList[GEO_RADIO] = MeshBuilder::GenerateOBJMTL("modelradio", "OBJ//radio.obj", "OBJ//radio.mtl");
	objectlist[hb_SHOPPROPRADIO].setmesh(GEO_RADIO);
	objectlist[hb_SHOPPROPRADIO].setproperties(Vector3(6, -17.9, -36), Vector3(0, 147, 0), Vector3(2, 2, 2));
	objectlist[hb_SHOPPROPRADIO].sethitboxcollisionsize(Vector3(1, 0, 1));
	meshList[GEO_WASHERDRYER] = MeshBuilder::GenerateOBJMTL("modelwasherdryer", "OBJ//washerDryerStacked.obj", "OBJ//washerDryerStacked.mtl");
	objectlist[hb_SHOPPROPWASHERDRYER].setmesh(GEO_WASHERDRYER);
	objectlist[hb_SHOPPROPWASHERDRYER].setproperties(Vector3(4, -19.9, -38), Vector3(0, 180, 0), Vector3(2, 2, 2));
	objectlist[hb_SHOPPROPWASHERDRYER].sethitboxcollisionsize(Vector3(1, 0, 1));
	objectlist[hb_CEILINGSHOP].setmesh(GEO_CEILING);
	objectlist[hb_CEILINGSHOP].setproperties(Vector3(3.3, -16.3, -33), Vector3(0, -90, 0), Vector3(7, 0.1, 4));
	objectlist[hb_CEILINGSHOP].sethitboxcollisionsize(Vector3(4, 0, 4));
	objectlist[hb_WALLSHOP].setmesh(GEO_CEILING);
	objectlist[hb_WALLSHOP].setproperties(Vector3(3.3, -16.3, -26.8), Vector3(180, 0, 0), Vector3(4, 1, 0.1));
	objectlist[hb_WALLSHOP].sethitboxcollisionsize(Vector3(4, 0, 4));




	//Bench
	meshList[GEO_BENCH] = MeshBuilder::GenerateOBJMTL("modelbench", "OBJ//Bench.obj", "OBJ//Bench.mtl");
	objectlist[hb_BENCH1].setmesh(GEO_BENCH);
	objectlist[hb_BENCH1].setproperties(Vector3(12, 0, -4), Vector3(0, -90, 0), Vector3(4.5, 4.5, 4.5));
	objectlist[hb_BENCH1].sethitboxcollisionsize(Vector3(0.1, 0, 1.1));
	objectlist[hb_BENCH2].setmesh(GEO_BENCH);
	objectlist[hb_BENCH2].setproperties(Vector3(12, 0, -12), Vector3(0, -90, 0), Vector3(4.5, 4.5, 4.5));
	objectlist[hb_BENCH2].sethitboxcollisionsize(Vector3(0.1, 0, 1.1));
	objectlist[hb_BENCH3].setmesh(GEO_BENCH);
	objectlist[hb_BENCH3].setproperties(Vector3(-6, 0, -15), Vector3(0, 90, 0), Vector3(4.5, 4.5, 4.5));
	objectlist[hb_BENCH3].sethitboxcollisionsize(Vector3(0.1, 0, 1.1));
	objectlist[hb_BENCH4].setmesh(GEO_BENCH);
	objectlist[hb_BENCH4].setproperties(Vector3(-6, 0, 0), Vector3(0, 90, 0), Vector3(4.5, 4.5, 4.5));
	objectlist[hb_BENCH4].sethitboxcollisionsize(Vector3(0.1, 0, 1.1));
	objectlist[hb_BENCH5].setmesh(GEO_BENCH);
	objectlist[hb_BENCH5].setproperties(Vector3(8, 0, -23), Vector3(0, 0, 0), Vector3(4.5, 4.5, 4.5));
	objectlist[hb_BENCH5].sethitboxcollisionsize(Vector3(1.1, 0, 0.1));

	//Trashbin
	meshList[GEO_BIN] = MeshBuilder::GenerateOBJMTL("modelBin", "OBJ//Bowl.obj", "OBJ//Bowl.mtl");
	objectlist[hb_BIN1].setmesh(GEO_BIN);
	objectlist[hb_BIN1].setproperties(Vector3(-6, 0, 2.5), Vector3(0, 0, 0), Vector3(2, 5.5, 2));
	objectlist[hb_BIN1].sethitboxcollisionsize(Vector3(0, 0, 0));
	objectlist[hb_BIN2].setmesh(GEO_BIN);
	objectlist[hb_BIN2].setproperties(Vector3(12, 0, -14), Vector3(0, 0, 0), Vector3(2, 5.5, 2));
	objectlist[hb_BIN2].sethitboxcollisionsize(Vector3(0, 0, 0));
	objectlist[hb_BIN3].setmesh(GEO_BIN);
	objectlist[hb_BIN3].setproperties(Vector3(-6, 0, -17), Vector3(0, 0, 0), Vector3(2, 5.5, 2));
	objectlist[hb_BIN3].sethitboxcollisionsize(Vector3(0, 0, 0));
	objectlist[hb_BIN4].setmesh(GEO_BIN);
	objectlist[hb_BIN4].setproperties(Vector3(15.5, 0, 3), Vector3(0, 0, 0), Vector3(2, 5.5, 2));
	objectlist[hb_BIN4].sethitboxcollisionsize(Vector3(0, 0, 0));
	objectlist[hb_BIN5].setmesh(GEO_BIN);
	objectlist[hb_BIN5].setproperties(Vector3(22.5, 0, -12.5), Vector3(0, 0, 0), Vector3(2, 5.5, 2));
	objectlist[hb_BIN5].sethitboxcollisionsize(Vector3(0, 0, 0));
	objectlist[hb_BIN6].setmesh(GEO_BIN);
	objectlist[hb_BIN6].setproperties(Vector3(-19.7, 0, 0), Vector3(0, 0, 0), Vector3(2, 5.5, 2));
	objectlist[hb_BIN6].sethitboxcollisionsize(Vector3(0, 0, 0));
	objectlist[hb_BIN7].setmesh(GEO_BIN);
	objectlist[hb_BIN7].setproperties(Vector3(16.7, 0, 18), Vector3(0, 0, 0), Vector3(2, 5.5, 2));
	objectlist[hb_BIN7].sethitboxcollisionsize(Vector3(0, 0, 0));


	//furniture
	meshList[GEO_BED] = MeshBuilder::GenerateOBJMTL("modelbed", "OBJ//BedSingle.obj", "OBJ//BedSingle.mtl");
	objectlist[hb_BED].setmesh(GEO_BED);
	objectlist[hb_BED].setproperties(Vector3(18.8, -20, 2.5), Vector3(0, 180, 0), Vector3(3, 3, 3));

	meshList[GEO_DESK] = MeshBuilder::GenerateOBJMTL("modeldesk", "OBJ//Desk.obj", "OBJ//Desk.mtl");
	objectlist[hb_DESK].setmesh(GEO_DESK);
	objectlist[hb_DESK].setproperties(Vector3(17.4, -20, 2.5), Vector3(0, -90, 0), Vector3(4, 3, 3));

	meshList[GEO_LAPTOP] = MeshBuilder::GenerateOBJMTL("modellaptop", "OBJ//Laptop.obj", "OBJ//Laptop.mtl");
	objectlist[hb_LAPTOP].setmesh(GEO_LAPTOP);
	objectlist[hb_LAPTOP].setproperties(Vector3(17.0, -18.85, 1.5), Vector3(0, -90, 0), Vector3(4, 3, 3));

	meshList[GEO_TABLE] = MeshBuilder::GenerateOBJMTL("modeltable", "OBJ//tableCoffee.obj", "OBJ//tableCoffee.mtl");
	objectlist[hb_TABLE].setmesh(GEO_TABLE);
	objectlist[hb_TABLE].setproperties(Vector3(20.1, -20, 7.5), Vector3(0, 0, 0), Vector3(3, 3, 3));

	meshList[GEO_TV] = MeshBuilder::GenerateOBJMTL("modeltv", "OBJ//televisionVintage.obj", "OBJ//televisionVintage.mtl");
	objectlist[hb_TV].setmesh(GEO_TV);
	objectlist[hb_TV].setproperties(Vector3(21.1, -19.3, 7.5), Vector3(0, 0, 0), Vector3(3, 3, 3));




	// hitbox
	meshList[GEO_HITBOX] = MeshBuilder::GenerateCube("modelhitbox", Color(0, 0, 0), 1);
	meshList[GEO_HITBOX]->textureID = LoadTGA("Image//hitbox2.tga");
	meshList[GEO_HITBOX2] = MeshBuilder::GenerateCube("modelhitbox", Color(0, 0, 0), 1);
	meshList[GEO_HITBOX2]->textureID = LoadTGA("Image//hitbox3.tga");


	//text
	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Image//DimboFont.tga");

	// Shop UI
	meshList[GEO_SHOPUI] = MeshBuilder::GenerateQuad("shop", Color(1, 1, 1), 1.f);
	meshList[GEO_SHOPUI]->textureID = LoadTGA("Image//Store.tga");

	//Inv
	meshList[GEO_HOTBAR] = MeshBuilder::GenerateQuad("hotbar", Color(1, 1, 1), 1.f);
	meshList[GEO_HOTBAR]->textureID = LoadTGA("Image//Hotbar.tga");

	//setup player's hitbox
	player.Setposition(Vector3(0, 2, 0));

	meshList[GEO_CONCRETE_PAVEMENT] = MeshBuilder::GenerateQuad("concretepavement", Color(1, 1, 1), 1);
	meshList[GEO_CONCRETE_PAVEMENT]->textureID = LoadTGA("Image//concreteSmooth.tga");

	meshList[GEO_ROAD] = MeshBuilder::GenerateQuad("road", Color(1, 1, 1), 1);
	meshList[GEO_ROAD]->textureID = LoadTGA("Image//asphalt.tga");

	meshList[GEO_CITY_CENTRE_FLOOR] = MeshBuilder::GenerateQuad("citycentrefloor", Color(1, 1, 1), 1);
	meshList[GEO_CITY_CENTRE_FLOOR]->textureID = LoadTGA("Image//Brick_03.tga");

	// Watch ring Necklace
	meshList[GEO_RING] = MeshBuilder::GenerateQuad("ring", Color(1, 1, 1), 1.f);
	meshList[GEO_RING]->textureID = LoadTGA("Image//ring.tga");
	meshList[GEO_WATCH] = MeshBuilder::GenerateQuad("watch", Color(1, 1, 1), 1.f);
	meshList[GEO_WATCH]->textureID = LoadTGA("Image//watch.tga");
	meshList[GEO_NECKLACE] = MeshBuilder::GenerateQuad("necklace", Color(1, 1, 1), 1.f);
	meshList[GEO_NECKLACE]->textureID = LoadTGA("Image//necklace.tga");


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


	meshList[GEO_PHONEBODY] = MeshBuilder::GenerateCube("phonebody", Color(0.7, 0.7, 0.7), 1.f);
	meshList[GEO_PHONESCREEN] = MeshBuilder::GenerateQuad("phonescreen", Color(1, 1, 1), 1.f);
	meshList[GEO_PHONESCREEN]->textureID = LoadTGA("Image//steal.tga");
	objectlist[hb_PHONE].setmesh(GEO_PHONESCREEN);
	objectlist[hb_PHONE].setproperties(Vector3(16.8, -18.8, 2.2), Vector3(-90, 0, 90), Vector3(0.25, 0.48, 0.01));

	meshList[GEO_CONTROLSTAB] = MeshBuilder::GenerateQuad("controltab", Color(1, 1, 1), 1.f);
	meshList[GEO_CONTROLSTAB]->textureID = LoadTGA("Image//ControlsTab.tga");

	meshList[GEO_CONTROLSCREEN] = MeshBuilder::GenerateQuad("controlscreen", Color(1, 1, 1), 1.f);
	meshList[GEO_CONTROLSCREEN]->textureID = LoadTGA("Image//ControlScreen.tga");

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
	gamestate = gs_mainmenu;
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
	navigationmesh = new Navmesh(60, 80, 1.5, -25, -47);

	//autogenerate walls for the navigation mesh
	NavmeshNode* nodemarker = navigationmesh->getzerozeronode();
	NavmeshNode* nodemarker1 = nodemarker->getrightpointer();
	for (unsigned idx = 0; idx < navigationmesh->getxcount(); idx++)
	{
		for (unsigned idx1 = 0; idx1 < navigationmesh->getzcount(); idx1++)
		{
			for (unsigned idx2 = 0; idx2 < hb_count; idx2++)
			{
				if (idx2 == hb_NPC1)
				{
					continue;
				}
				if (nodemarker->gethitbox().Checkforcollision(objectlist[idx2].gethitbox()))
				{
					nodemarker->setIsWall(true);
				}
				else if (objectlist[idx2].gethitbox().Checkforcollision(nodemarker->gethitbox()))
				{
					nodemarker->setIsWall(true);
				}
			}

			nodemarker = nodemarker->getuppointer();
			if (nodemarker == nullptr)
			{
				break;
			}
		}
		nodemarker = nodemarker1;
		nodemarker1 = nodemarker1->getrightpointer();
		if (nodemarker1 == nullptr)
		{
			break;
		}
	}

	for (unsigned idx = 0; idx < hb_total; idx++)
	{
		if (!objectlist[idx].getissetup())
		{
			continue;
		}

		objectlist[idx].sethitboxcollisionsize(Vector3(objectlist[idx].gethitboxcollisionsize().x, 2, objectlist[idx].gethitboxcollisionsize().z));
	}

	polices.push_back(new PoliceAI(90, Vector3(500, 6, 0), 40, hb_NPC1));
	objectlist[hb_POLICE1].setrenderobject(false);

	polices.push_back(new PoliceAI(90, Vector3(500, 6, 0), 40, hb_NPC1));
	objectlist[hb_POLICE2].setrenderobject(false);

	polices.push_back(new PoliceAI(90, Vector3(500, 6, 0), 40, hb_NPC1));
	objectlist[hb_POLICE3].setrenderobject(false);

	polices.push_back(new PoliceAI(90, Vector3(500, 6, 0), 40, hb_NPC1));
	objectlist[hb_POLICE4].setrenderobject(false);

	polices.push_back(new PoliceAI(90, Vector3(500, 6, 0), 40, hb_NPC1));
	objectlist[hb_POLICE5].setrenderobject(false);

}

bool SP2::Update(double dt)
{
	//Mouse Inputs
	//static bool bLButtonState = false;
	//if (!bLButtonState && Application::IsMousePressed(0))
	//{
	//	bLButtonState = true;
	//	std::cout << "LBUTTON DOWN" << std::endl;
	//	//Converting Viewport space to UI space
	//	double x, y;
	//	Application::GetCursorPos(&x, &y);
	//	unsigned w = Application::GetWindowWidth();
	//	unsigned h = Application::GetWindowHeight();
	//	float posX = x / w * 80.f; //convert (0,800) to (0,80)
	//	float posY = -(y / h * 60.f) + 60; //convert (600,0) to (0,60)
	//	std::cout << "posX:" << posX << " , posY:" << posY << std::endl;
	//	if (posX > imagepos.x - imagedimensions.x * 0.5 && posX < imagepos.x + imagedimensions.x * 0.5 && posY > imagepos.y - imagedimensions.y * 0.5 && posY < imagepos.y + imagedimensions.y * 0.5)
	//	{
	//		std::cout << "Hit!" << std::endl;
	//	}
	//	else
	//	{
	//		std::cout << "Miss!" << std::endl;
	//	}
	//}
	//else if (bLButtonState && !Application::IsMousePressed(0))
	//{
	//	bLButtonState = false;
	//	std::cout << "LBUTTON UP" << std::endl;
	//}
	//static bool RightClick = false;
	//if (!RightClick && Application::IsMousePressed(1)) {
	//	RightClick = true;
	//}
	//else if (RightClick && !Application::IsMousePressed(1)) {
	//	RightClick = false;
	//}

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

bool SP2::UpdateMainMenu()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	bool click = GetAsyncKeyState(0x01);
	timesincelastbuttonpress = 0;
	DialogueBoxOpen = false;

	if (Application::IsKeyPressed(VK_ESCAPE)) {

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


			if ((mousepos->x > 755 && mousepos->x < 805 && mousepos->y > 580 && mousepos->y < 630))
			{
				static bool bLButtonState = false;
				static bool phase2 = false;

				if (!phase2) {
					if (!bLButtonState && Application::IsMousePressed(0))
					{
						bLButtonState = true;
					}
					else if (bLButtonState && !Application::IsMousePressed(0))
					{
						bLButtonState = false;
						phase2 = true;
						Application::isMuted = true;
					}
				}

				if (phase2) {
					if (!bLButtonState && Application::IsMousePressed(0))
					{
						bLButtonState = true;
					}
					else if (bLButtonState && !Application::IsMousePressed(0))
					{
						bLButtonState = false;
						Application::isMuted = false;
						phase2 = false;
					}
				}

				if (Application::isMuted == false) {
					if (Application::ismusicPlaying == false and Stars != 0) {
						PlaySound(TEXT("GTAVParachuting.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
						Application::ismusicPlaying = true;
					}
					if (Stars == 0 and Application::ismusicPlaying == true) {
						PlaySound(NULL, 0, 0);
						Application::ismusicPlaying = false;
					}
				}
				else {
					Application::ismusicPlaying = false;
					PlaySound(NULL, 0, 0);
				}
			}
		}
	}
	return false;
}

#define LSPEED 10
void SP2::UpdateENV(double dt)
{
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
			gamestate = gs_mainmenu;
			camera.Reset();
			camera.Position_Y = -18;
			camera.position = Vector3(19.3, -18, 1.246);
			Stars = 0;
			if (Stars == 0)
			{
				for (unsigned idx = 0; idx < polices.size(); idx++)
				{
					polices[idx]->DespawnPolice();
					objectlist[hb_POLICE1 + idx].setrenderobject(false);
				}
			}
			item1Activetime = 0;
			item2Activetime = 0;
			item1_active = false;
			item2_active = false;
			item1 = 0;
			item2 = 0;
			item3 = 0;
			Dialogue = 1;
			DialogueBoxOpen = false, isShopOpen = false;
			rings = 0, watches = 0, necklace = 0, inventoryopen = false;
			ringscam = false, watchscam = false, necklacescam = false;
			SizeofStamina = 40;
			Application::Cash = 100;
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
		}
		if (GetAsyncKeyState(VK_ESCAPE))
		{
			gamestate = gs_mainmenu;
			camera.Reset();
			camera.Position_Y = -18;
			camera.position = Vector3(19.3, -18, 1.246);
			Stars = 0;
			if (Stars == 0)
			{
				for (unsigned idx = 0; idx < polices.size(); idx++)
				{
					polices[idx]->DespawnPolice();
					objectlist[hb_POLICE1 + idx].setrenderobject(false);
				}
			}
			item1Activetime = 0;
			item2Activetime = 0;
			item1_active = false;
			item2_active = false;
			item1 = 0;
			item2 = 0;
			item3 = 0;
			Dialogue = 1;
			DialogueBoxOpen = false, isShopOpen = false;
			rings = 0, watches = 0, necklace = 0, inventoryopen = false;
			ringscam = false, watchscam = false, necklacescam = false;
			SizeofStamina = 40;
			Application::Cash = 100;
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
		}
		return;
	}




	//Npc move test
	if (AllowNPC1Movement == true) {
		if (NPC1Front == true and NPC1Back == false) {
			NPCZ += (float)(2 * dt);
		}
		if (NPC1Back == true and NPC1Front == false) {
			NPCZ -= (float)(2 * dt);
		}
		if (NPCZ >= 5 and NPC1Front == true and NPC1Back == false) {
			NPC1Front = false;
			NPC1Back = true;
			PrevRotationNPC1 = 180;
		}
		if (NPCZ <= -22 and NPC1Front == false and NPC1Back == true) {
			NPC1Front = true;
			NPC1Back = false;
			PrevRotationNPC1 = 0;
		}
		objectlist[hb_NPC1].Setposition(Vector3(NPCX, 0, NPCZ));
	}
	if (AllowNPC2Movement == true) {
		if (NPC2Front == true and NPC2Back == false) {
			NPCZ2 += (float)(2 * dt);
		}
		if (NPC2Back == true and NPC2Front == false) {
			NPCZ2 -= (float)(2 * dt);
		}
		if (NPCZ2 >= 5 and NPC2Front == true and NPC2Back == false) {
			NPC2Front = false;
			NPC2Back = true;
			PrevRotationNPC2 = 180;
		}
		if (NPCZ2 <= -22 and NPC2Front == false and NPC2Back == true) {
			NPC2Front = true;
			NPC2Back = false;
			PrevRotationNPC2 = 0;
		}
		objectlist[hb_NPC2].Setposition(Vector3(NPCX2, 0, NPCZ2));
	}
	if (AllowNPC3Movement == true) {
		if (NPC3Front == true and NPC3Back == false) {
			NPCX3 += (float)(2 * dt);
		}
		if (NPC3Back == true and NPC3Front == false) {
			NPCX3 -= (float)(2 * dt);
		}
		if (NPCX3 >= 30 and NPC3Front == true and NPC3Back == false) {
			NPC3Front = false;
			NPC3Back = true;
			PrevRotationNPC3 = -90;
		}
		if (NPCX3 <= -22 and NPC3Front == false and NPC3Back == true) {
			NPC3Front = true;
			NPC3Back = false;
			PrevRotationNPC3 = 90;
		}
		objectlist[hb_NPC3].Setposition(Vector3(NPCX3, 0, NPCZ3));
	}







	if ((int)Application::Minigame_timer > 0) {
		Application::Minigame_timer -= dt;
	}


	if (bin1cooldown > 0)
	{
		bin1cooldown -= dt;
	}
	if (bin2cooldown > 0)
	{
		bin2cooldown -= dt;
	}
	if (bin3cooldown > 0)
	{
		bin3cooldown -= dt;
	}
	if (bin4cooldown > 0)
	{
		bin4cooldown -= dt;
	}
	if (bin5cooldown > 0)
	{
		bin5cooldown -= dt;
	}
	if (bin6cooldown > 0)
	{
		bin6cooldown -= dt;
	}
	if (bin7cooldown > 0)
	{
		bin7cooldown -= dt;
	}
	ArrowRotate += (float)(40 * dt);

	// Skybox
	static bool isDay = true;
	static bool isNight = false;
	static float timer_dayandNight = 20;

	if (timer_dayandNight >= 20) {
		if (isDay == true and isNight == false) {
			DayandNightSkyboxTransitioning += 0.0001;
		}
		if (isDay == false and isNight == true) {
			DayandNightSkyboxTransitioning -= 0.0001;
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




	camera.right = camera.view.Cross(camera.up);
	camera.right.y = 0;
	camera.right.Normalize();
	camera.up = camera.right.Cross(camera.view).Normalized();

	// Npc look at the camera
	Vector3 target = Vector3(0, 0, 1);
	Vector3 NPC1PositionXZ = Vector3(objectlist[hb_NPC1].getposition().x, 0, objectlist[hb_NPC1].getposition().z);
	Vector3 NPC2PositionXZ = Vector3(objectlist[hb_NPC2].getposition().x, 0, objectlist[hb_NPC2].getposition().z);
	Vector3 NPC3PositionXZ = Vector3(objectlist[hb_NPC3].getposition().x, 0, objectlist[hb_NPC3].getposition().z);

	Vector3 PlayerXZ = Vector3(camera.position.x, 0, camera.position.z);
	Vector3 NPC1toPlayerXZ = (PlayerXZ - NPC1PositionXZ).Normalized();
	Vector3 NPC2toPlayerXZ = (PlayerXZ - NPC2PositionXZ).Normalized();
	Vector3 NPC3toPlayerXZ = (PlayerXZ - NPC3PositionXZ).Normalized();

	Vector3 ShopKeeperPositionXZ = Vector3(3.5, 0, -36.5);
	Vector3 ShopKeepertoPlayerXZ = (PlayerXZ - ShopKeeperPositionXZ).Normalized();
	float angle_npc1 = (180.f / Math::PI) * (std::acos(NPC1toPlayerXZ.Dot(target) / (NPC1toPlayerXZ.Length()) * (target.Length())));
	float angle_npc2 = (180.f / Math::PI) * (std::acos(NPC2toPlayerXZ.Dot(target) / (NPC2toPlayerXZ.Length()) * (target.Length())));
	float angle_npc3 = (180.f / Math::PI) * (std::acos(NPC3toPlayerXZ.Dot(target) / (NPC3toPlayerXZ.Length()) * (target.Length())));
	float angle_shopkeeper = (180.f / Math::PI) * (std::acos(ShopKeepertoPlayerXZ.Dot(target) / (ShopKeepertoPlayerXZ.Length()) * (target.Length())));

	if (NPC1toPlayerXZ.x > 0) {
		NPC1lookAtPlayerAngle = angle_npc1;
	}
	else {
		NPC1lookAtPlayerAngle = -angle_npc1;
	}
	if (NPC2toPlayerXZ.x > 0) {
		NPC2lookAtPlayerAngle = angle_npc2;
	}
	else {
		NPC2lookAtPlayerAngle = -angle_npc2;
	}
	if (NPC3toPlayerXZ.x > 0) {
		NPC3lookAtPlayerAngle = angle_npc3;
	}
	else {
		NPC3lookAtPlayerAngle = -angle_npc3;
	}


	if (ShopKeepertoPlayerXZ.x > 0) {
		ShopKeeperlookAtPlayerAngle = angle_shopkeeper;
	}
	else {
		ShopKeeperlookAtPlayerAngle = -angle_shopkeeper;
	}

	// Player look at the npc
	Vector3 viewY = Vector3(0, camera.view.y, 0);
	Vector3 PlayertoNPC1XZ = (NPC1PositionXZ - PlayerXZ).Normalized();
	Vector3 PlayertoNPC2XZ = (NPC2PositionXZ - PlayerXZ).Normalized();
	Vector3 PlayertoNPC3XZ = (NPC3PositionXZ - PlayerXZ).Normalized();
	Vector3 viewXZ = Vector3(camera.view.x, 0, camera.view.z);
	float RightHandrule_AngleChecker_NPC1 = atan2(PlayertoNPC1XZ.z * viewXZ.x - PlayertoNPC1XZ.x * viewXZ.z, PlayertoNPC1XZ.x * viewXZ.x + PlayertoNPC1XZ.z * viewXZ.z) * (180 / Math::PI);
	float RightHandrule_AngleChecker_NPC2 = atan2(PlayertoNPC2XZ.z * viewXZ.x - PlayertoNPC2XZ.x * viewXZ.z, PlayertoNPC2XZ.x * viewXZ.x + PlayertoNPC2XZ.z * viewXZ.z) * (180 / Math::PI);
	float RightHandrule_AngleChecker_NPC3 = atan2(PlayertoNPC3XZ.z * viewXZ.x - PlayertoNPC3XZ.x * viewXZ.z, PlayertoNPC3XZ.x * viewXZ.x + PlayertoNPC3XZ.z * viewXZ.z) * (180 / Math::PI);
	float speed_yaw_NPC1 = 0, speed_pitch_NPC1 = 0;
	float speed_yaw_NPC2 = 0, speed_pitch_NPC2 = 0;
	float speed_yaw_NPC3 = 0, speed_pitch_NPC3 = 0;

	if (DialogueBoxOpen == true) {
		if (RightHandrule_AngleChecker_NPC1 > 1) {
			speed_yaw_NPC1 = -(dt * 120);
		}
		if (RightHandrule_AngleChecker_NPC1 < -1) {
			speed_yaw_NPC1 = (dt * 120);
		}
		if (RightHandrule_AngleChecker_NPC2 > 1) {
			speed_yaw_NPC2 = -(dt * 120);
		}
		if (RightHandrule_AngleChecker_NPC2 < -1) {
			speed_yaw_NPC2 = (dt * 120);
		}
		if (RightHandrule_AngleChecker_NPC3 > 1) {
			speed_yaw_NPC3 = -(dt * 120);
		}
		if (RightHandrule_AngleChecker_NPC3 < -1) {
			speed_yaw_NPC3 = (dt * 120);
		}

		if (viewY.y <= -0.1) {
			speed_pitch_NPC1 = (dt * 120);
			speed_pitch_NPC2 = (dt * 120);
			speed_pitch_NPC3 = (dt * 120);
		}
		else if (viewY.y >= 0.1) {
			speed_pitch_NPC1 = -(dt * 120);
			speed_pitch_NPC2 = -(dt * 120);
			speed_pitch_NPC3 = -(dt * 120);
		}
		else {
			speed_pitch_NPC1 = 0;
			speed_pitch_NPC2 = 0;
			speed_pitch_NPC3 = 0;
		}
	}

	// npc 1
	if (DialogueBoxOpen == true and DistanceParameter(player.getposition().x, player.getposition().z, objectlist[hb_NPC1].getposition().x, objectlist[hb_NPC1].getposition().z) <= 4) {
		if (PlayerandNpc1RotationSpeed >= NPC1lookAtPlayerAngle) {
			PlayerandNpc1RotationSpeed -= (float)(120 * dt);
		}				
		if (PlayerandNpc1RotationSpeed <= NPC1lookAtPlayerAngle) {
			PlayerandNpc1RotationSpeed += (float)(120 * dt);
		}				
		if (PlayerandNpc1RotationSpeed >= NPC1lookAtPlayerAngle) {
			PlayerandNpc1RotationSpeed -= (float)(120 * dt);
		}				
		if (PlayerandNpc1RotationSpeed <= NPC1lookAtPlayerAngle) {
			PlayerandNpc1RotationSpeed += (float)(120 * dt);
		}

		// Camera rotation
		Mtx44 rotation;
		rotation.SetToRotation(speed_yaw_NPC1, camera.up.x, camera.up.y, camera.up.z);
		camera.view = rotation * camera.view;
		camera.target = camera.position + camera.view;
		rotation.SetToRotation(speed_pitch_NPC1, camera.right.x, camera.right.y, camera.right.z);
		camera.view = rotation * camera.view;
		camera.target = camera.position + camera.view;
	}
	else {
		if (PlayerandNpc1RotationSpeed >= PrevRotationNPC1) {
			PlayerandNpc1RotationSpeed -= (float)(180 * dt);
		}
		if (PlayerandNpc1RotationSpeed <= PrevRotationNPC1) {
			PlayerandNpc1RotationSpeed += (float)(180 * dt);
		}
	}


	// npc 2
	if (DialogueBoxOpen == true and DistanceParameter(player.getposition().x, player.getposition().z, objectlist[hb_NPC2].getposition().x, objectlist[hb_NPC2].getposition().z) <= 4) {
		if (PlayerandNpc2RotationSpeed >= NPC2lookAtPlayerAngle) {
			PlayerandNpc2RotationSpeed -= (float)(120 * dt);
		}
		if (PlayerandNpc2RotationSpeed <= NPC2lookAtPlayerAngle) {
			PlayerandNpc2RotationSpeed += (float)(120 * dt);
		}
		if (PlayerandNpc2RotationSpeed >= NPC2lookAtPlayerAngle) {
			PlayerandNpc2RotationSpeed -= (float)(120 * dt);
		}
		if (PlayerandNpc2RotationSpeed <= NPC2lookAtPlayerAngle) {
			PlayerandNpc2RotationSpeed += (float)(120 * dt);
		}

		// Camera rotation
		Mtx44 rotation;
		rotation.SetToRotation(speed_yaw_NPC2, camera.up.x, camera.up.y, camera.up.z);
		camera.view = rotation * camera.view;
		camera.target = camera.position + camera.view;
		rotation.SetToRotation(speed_pitch_NPC2, camera.right.x, camera.right.y, camera.right.z);
		camera.view = rotation * camera.view;
		camera.target = camera.position + camera.view;
	}
	else {
		if (PlayerandNpc2RotationSpeed >= PrevRotationNPC2) {
			PlayerandNpc2RotationSpeed -= (float)(180 * dt);
		}
		if (PlayerandNpc2RotationSpeed <= PrevRotationNPC2) {
			PlayerandNpc2RotationSpeed += (float)(180 * dt);
		}
	}



	// npc 3
	if (DialogueBoxOpen == true and DistanceParameter(player.getposition().x, player.getposition().z, objectlist[hb_NPC3].getposition().x, objectlist[hb_NPC3].getposition().z) <= 4) {
		if (PlayerandNpc3RotationSpeed >= NPC3lookAtPlayerAngle) {
			PlayerandNpc3RotationSpeed -= (float)(120 * dt);
		}
		if (PlayerandNpc3RotationSpeed <= NPC3lookAtPlayerAngle) {
			PlayerandNpc3RotationSpeed += (float)(120 * dt);
		}
		if (PlayerandNpc3RotationSpeed >= NPC3lookAtPlayerAngle) {
			PlayerandNpc3RotationSpeed -= (float)(120 * dt);
		}
		if (PlayerandNpc3RotationSpeed <= NPC3lookAtPlayerAngle) {
			PlayerandNpc3RotationSpeed += (float)(120 * dt);
		}

		// Camera rotation
		Mtx44 rotation;
		rotation.SetToRotation(speed_yaw_NPC3, camera.up.x, camera.up.y, camera.up.z);
		camera.view = rotation * camera.view;
		camera.target = camera.position + camera.view;
		rotation.SetToRotation(speed_pitch_NPC3, camera.right.x, camera.right.y, camera.right.z);
		camera.view = rotation * camera.view;
		camera.target = camera.position + camera.view;
	}
	else {
		if (PlayerandNpc3RotationSpeed >= PrevRotationNPC3) {
			PlayerandNpc3RotationSpeed -= (float)(180 * dt);
		}
		if (PlayerandNpc3RotationSpeed <= PrevRotationNPC3) {
			PlayerandNpc3RotationSpeed += (float)(180 * dt);
		}
	}



	// Shopkeeper
	Vector3 PlayertoShopXZ = (Vector3(objectlist[hb_ShopKeeper].getposition().x, 0, objectlist[hb_ShopKeeper].getposition().z) - PlayerXZ).Normalized();
	float RightHandrule_AngleChecker_Shop = atan2(PlayertoShopXZ.z * viewXZ.x - PlayertoShopXZ.x * viewXZ.z, PlayertoShopXZ.x * viewXZ.x + PlayertoShopXZ.z * viewXZ.z) * (180 / Math::PI);
	float speed_yaw_Shop = 0;
	if (RightHandrule_AngleChecker_Shop > 1) {
		speed_yaw_Shop = -(dt * 120);
	}
	if (RightHandrule_AngleChecker_Shop < -1) {
		speed_yaw_Shop = (dt * 120);
	}

	if (PlayerandShopKeeperRotationSpeed >= ShopKeeperlookAtPlayerAngle) {
		PlayerandShopKeeperRotationSpeed -= (float)(120 * dt);
	}
	if (PlayerandShopKeeperRotationSpeed <= ShopKeeperlookAtPlayerAngle) {
		PlayerandShopKeeperRotationSpeed += (float)(120 * dt);
	}
	if (PlayerandShopKeeperRotationSpeed >= ShopKeeperlookAtPlayerAngle) {
		PlayerandShopKeeperRotationSpeed -= (float)(120 * dt);
	}
	if (PlayerandShopKeeperRotationSpeed <= ShopKeeperlookAtPlayerAngle) {
		PlayerandShopKeeperRotationSpeed += (float)(120 * dt);
	}
	if (DialogueBoxOpen == true and interactableObjectRect(player.getposition().x, player.getposition().z, objectlist[hb_SHOPSELLTABLE].getposition().x, objectlist[hb_SHOPSELLTABLE].getposition().z + 0.5, 1.5, 1) == true) {
		Mtx44 rotation;
		rotation.SetToRotation(speed_yaw_Shop, camera.up.x, camera.up.y, camera.up.z);
		camera.view = rotation * camera.view;
		camera.target = camera.position + camera.view;
	}

	objectlist[hb_NPC1].setrotationparentonly(Vector3(0, PlayerandNpc1RotationSpeed, 0));
	objectlist[hb_NPC2].setrotationparentonly(Vector3(0, PlayerandNpc2RotationSpeed, 0));
	objectlist[hb_NPC3].setrotationparentonly(Vector3(0, PlayerandNpc3RotationSpeed, 0));
	objectlist[hb_ShopKeeper].setrotationparentonly(Vector3(0, PlayerandShopKeeperRotationSpeed, 0));









	//Enemy Movement
	// will be motified
	if (Stars != 0) {
		static bool isChanged = false;
		static bool canseeplayer = true;
		if (canseeplayer)
		{
			timer_blinking = 0;
		}
		if (timer_Wanted_Chase <= 40.f and isChanged == false) {
			speed_police *= 1.2;
			isChanged = true;
		}
		if (timer_Wanted_Chase >= 40.f and timer_Wanted_Chase <= 80.f and isChanged == true) {
			Stars++;
			speed_police *= 1.2;
			isChanged = false;
		}
		if (timer_Wanted_Chase >= 80.f and timer_Wanted_Chase <= 120.f and isChanged == false) {
			Stars++;
			speed_police *= 1.2;
			isChanged = true;
		}
		if (timer_Wanted_Chase >= 120.f and timer_Wanted_Chase <= 160.f and isChanged == true) {
			Stars++;
			speed_police *= 1.2;
			isChanged = false;
		}
		if (timer_Wanted_Chase >= 160.f and isChanged == false) {
			Stars++;
			isChanged = true;
		}





		if (Stars > 0) {
			if (!polices[0]->GetSpawnStatus())
			{
				//calculate viewdirection
				Vector3 temp = (camerapos - polices[0]->GetPosition()).Normalize(); //make the police face the player during spawn
				polices[0]->SpawnPolice(Vector3(rand() % 24 - 9, 0, rand() % 8 + 7), 5, temp, navigationmesh, camerapos);
				objectlist[hb_POLICE1].setrenderobject(true);
			}
			else
			{
				polices[0]->PoliceAIUpdate(camerapos, navigationmesh, dt, objectlist);
				polices[0]->setpolicemovementspeed(4 + 0.5 * Stars);
				if ((polices[0]->GetPosition() - camerapos).Length() < 3.5)
				{
					gameover = true;
				}
				if (polices[0]->getcanseeplayer())
				{
					timer_wanted = 0;
					canseeplayer = true;
				}
				else
				{
					canseeplayer = false;
				}
			}
		}

		if (Stars > 1) {
			if (!polices[1]->GetSpawnStatus())
			{
				//calculate viewdirection
				Vector3 temp = (camerapos - polices[1]->GetPosition()).Normalize(); //make the police face the player during spawn
				polices[1]->SpawnPolice(Vector3(rand() % 24 - 9, 0, rand() % 8 + 7), 5, temp, navigationmesh, camerapos);
				objectlist[hb_POLICE2].setrenderobject(true);
			}
			else
			{
				polices[1]->PoliceAIUpdate(camerapos, navigationmesh, dt, objectlist);
				polices[1]->setpolicemovementspeed(4 + 0.5 * Stars);
				if ((polices[1]->GetPosition() - camerapos).Length() < 3.5)
				{
					gameover = true;
				}
				if (polices[1]->getcanseeplayer())
				{
					timer_wanted = 0;
					canseeplayer = true;
				}
				else
				{
					canseeplayer = false;
				}
			}
		}

		if (Stars > 2) {
			if (!polices[2]->GetSpawnStatus())
			{
				//calculate viewdirection
				Vector3 temp = (camerapos - polices[2]->GetPosition()).Normalize(); //make the police face the player during spawn
				polices[2]->SpawnPolice(Vector3(rand() % 24 - 9, 0, rand() % 8 + 7), 5, temp, navigationmesh, camerapos);
				objectlist[hb_POLICE3].setrenderobject(true);
			}
			else
			{
				polices[2]->PoliceAIUpdate(camerapos, navigationmesh, dt, objectlist);
				polices[2]->setpolicemovementspeed(4 + 0.5 * Stars);
				if ((polices[2]->GetPosition() - camerapos).Length() < 3.5)
				{
					gameover = true;
				}
				if (polices[2]->getcanseeplayer())
				{
					timer_wanted = 0;
					canseeplayer = true;
				}
				else
				{
					canseeplayer = false;
				}
			}
		}

		if (Stars > 3) {
			if (!polices[3]->GetSpawnStatus())
			{
				//calculate viewdirection
				Vector3 temp = (camerapos - polices[3]->GetPosition()).Normalize(); //make the police face the player during spawn
				polices[3]->SpawnPolice(Vector3(rand() % 24 - 9, 0, rand() % 8 + 7), 5, temp, navigationmesh, camerapos);
				objectlist[hb_POLICE4].setrenderobject(true);
			}
			else
			{
				polices[3]->PoliceAIUpdate(camerapos, navigationmesh, dt, objectlist);
				polices[3]->setpolicemovementspeed(4 + 0.5 * Stars);
				if ((polices[3]->GetPosition() - camerapos).Length() < 3.5)
				{
					gameover = true;
				}
				if (polices[3]->getcanseeplayer())
				{
					timer_wanted = 0;
					canseeplayer = true;
				}
				else
				{
					canseeplayer = false;
				}
			}
		}

		if (Stars > 4) {
			if (!polices[4]->GetSpawnStatus())
			{
				//calculate viewdirection
				Vector3 temp = (camerapos - polices[4]->GetPosition()).Normalize(); //make the police face the player during spawn
				polices[4]->SpawnPolice(Vector3(rand() % 24 - 9, 0, rand() % 8 + 7), 5, temp, navigationmesh, camerapos);
				objectlist[hb_POLICE5].setrenderobject(true);
			}
			else
			{
				polices[4]->PoliceAIUpdate(camerapos, navigationmesh, dt, objectlist);
				polices[4]->setpolicemovementspeed(4 + 0.5 * Stars);
				if ((polices[4]->GetPosition() - camerapos).Length() < 3.5)
				{
					gameover = true;
				}
				if (polices[4]->getcanseeplayer())
				{
					timer_wanted = 0;
					canseeplayer = true;
				}
				else
				{
					canseeplayer = false;
				}
			}
		}




		if (timer_wanted > 20.f)
		{
			Stars--;
			timer_wanted = 0;
		}

		for (unsigned idx = Stars; idx < polices.size(); idx++)
		{
			polices[idx]->DespawnPolice();
			objectlist[hb_POLICE1 + idx].setrenderobject(false);
		}

		timer_wanted += dt;
	}
	for (unsigned idx = 0; idx < polices.size(); idx++) //update object position to the police position
	{
		objectlist[hb_POLICE1 + idx].Setposition(polices[idx]->GetPosition());
		objectlist[hb_POLICE1 + idx].setrotationparentonly(Vector3(0, -polices[idx]->getangle(), 0));
	}




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
						Vector3 finalPos = PlayerCollision(i, camera.position.x, camera.position.z); //ignore y
						camera.position.x = finalPos.x;
						camera.position.z = finalPos.z;
					}
					camera.target = camera.position + camera.view;
				}

				// Walls
				if (camera.position.y == -18) {
					for (int i = hb_Wall; i < hb_total; ++i) {
						Vector3 finalPos = PlayerCollision(i, camera.position.x, camera.position.z); //ignore y
						camera.position.x = finalPos.x;
						camera.position.z = finalPos.z;
						camera.target = camera.position + camera.view;
					}
				}


				if (polices[0]->getcanseeplayer())
				{
					if (camera.position.y != -18) {
						for (int i = 0; i < hb_count; ++i) {
							if (i != hb_POLICE1) {
								Vector3 finalPos = PlayerCollision(i, polices[0]->GetPosition().x, polices[0]->GetPosition().z); //ignore y
								polices[0]->setposition(finalPos);
							}
						}
					}
				}





			} //else do nothing with camera
		}



		if (GetAsyncKeyState(0x01) && timesincelastpunch > 0.5)
		{
			punch = true;
			timesincelastpunch = 0;
		}
		else if (timesincelastpunch > 0.5)
		{
			punch = false;
			if (playerfist1 > 0)
				playerfist1 -= 10.f * dt;
			if (playerfist2 > 0)
				playerfist2 -= 10.f * dt;
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

		static bool tabbuttonstate = false;
		if (Application::IsKeyPressed(VK_TAB) && !tabbuttonstate) //'Tab'
		{
			inventoryopen = !inventoryopen;
			tabbuttonstate = true;
		}
		else if (!GetAsyncKeyState(VK_TAB) && tabbuttonstate)
		{
			tabbuttonstate = false;
		}

		static bool ctrlbuttonstate = false;
		if (Application::IsKeyPressed(VK_CONTROL) && !ctrlbuttonstate) //'ctrl'
		{
			controlopen = !controlopen;
			ctrlbuttonstate = true;
		}
		else if (!GetAsyncKeyState(VK_CONTROL) && ctrlbuttonstate)
		{
			ctrlbuttonstate = false;
		}


		if (GetAsyncKeyState(VK_ESCAPE) && !escbuttonstate) //to exit env
		{
			if (inshop)
			{
				SetCursorPos(camera.center.x, camera.center.y);
				GD_PrintLine1 = "";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				DialogueBoxOpen = false;
				timesincelastbuttonpress = 0;
				inshop = false;
				Dialogue = 1;
				if (isShopOpen == true) {
					ShowCursor(false);
					isShopOpen = false;
				}
				AllowNPC1Movement = true;
				AllowNPC2Movement = true;
			}
			else
			{
				resume = true;
				gamestate = gs_mainmenu;
				ShowCursor(true);
				mousehidden = false;
			}
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
		if (Application::IsKeyPressed(VK_BACK) == true)
		{
			if (InWindow == false) InWindow = true;
			else InWindow = false;
			ShowCursor(true);
			mousehidden = false;
		}
		//update the player position
		player.Setposition(camerapos);
		//punchcooldown += dt;
	}
	else
	{
		ShowCursor(true);
		mousehidden = false;
	}
	camera.center.x = (camera.windowcenter.left + camera.windowcenter.right) * 0.5;
	camera.center.y = (camera.windowcenter.top + camera.windowcenter.bottom) * 0.5;


	//Street Scam 
	if (DialogueBoxOpen == true)
	{
		if (DistanceParameter(player.getposition().x, player.getposition().z, objectlist[hb_NPC1].getposition().x, objectlist[hb_NPC1].getposition().z) <= 4 and Stars == 0 and camera.Position_Y == 2) {
			if (timesincelastbuttonpress > 0.2 and Dialogue == 1)
			{
				AllowNPC1Movement = false;
				randomgreet = rand() % 4 + 1;
				if (randomgreet == 1)
				{
					GD_PrintLine1 = "Huh? Whayya want?";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
				}
				else if (randomgreet == 2)
				{
					GD_PrintLine1 = "Hi! What do you need?";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
				}
				else if (randomgreet == 3)
				{
					GD_PrintLine1 = "I've got no time to waste for you kid, hurry up and say it!";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
				}
				else if (randomgreet == 4)
				{
					GD_PrintLine1 = "I hate wasting time, spit it out already!";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
				}
				person = "PEDESTRIAN";
				Dialogue = 2;
				timesincelastbuttonpress = 0;
				randomscam = rand() % 3 + 1;
			}
			if (randomscam == 1)
			{
				if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 2)
				{
					GD_PrintLine1 = "I have this newest limited edition game!";
					GD_PrintLine2 = "Care to buy it off my hands for $100?";
					GD_PrintLine3 = "";
					person = "       YOU";
					Dialogue = 3;
					timesincelastbuttonpress = 0;
				}
				if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 3)
				{
					GD_PrintLine1 = "What console is this from?";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRAIN";
					Dialogue = 4;
					timesincelastbuttonpress = 0;
				}
				if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 4)
				{
					randomtext = rand() % 4 + 1;
					if (randomtext == 1)
					{
						GD_PrintLine1 = "1. It is from bXob!";
						GD_PrintLine2 = "2. It is from Barney Bros!";
						GD_PrintLine3 = "3. I have no idea.";
						Dialogue = 5;
						timesincelastbuttonpress = 0;
					}
					if (randomtext == 2)
					{
						GD_PrintLine1 = "1. It is from a retail store in china.";
						GD_PrintLine2 = "2. It is from the console Albert Einstein made.";
						GD_PrintLine3 = "3. It's from PrayStation!";
						Dialogue = 6;
						timesincelastbuttonpress = 0;
					}
					if (randomtext == 3)
					{
						GD_PrintLine1 = "1. It is from Barney Bros!";
						GD_PrintLine2 = "2. It is from a retail store in china.";
						GD_PrintLine3 = "3. It is from bXob!";
						Dialogue = 7;
						timesincelastbuttonpress = 0;
					}
					if (randomtext == 4)
					{
						GD_PrintLine1 = "1. It is from the console Albert Einstein made.";
						GD_PrintLine2 = "2. I have no idea.";
						GD_PrintLine3 = "3. It is from PrayStation!";
						Dialogue = 8;
						timesincelastbuttonpress = 0;
					}
					person = "       YOU";
				}
				if (Application::IsKeyPressed('1') and timesincelastbuttonpress > 0.2 and Dialogue == 5)
				{
					GD_PrintLine1 = "Cool! Alright, I can finally try a different game now!";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
					Dialogue = 9;
					timesincelastbuttonpress = 0;
				}
				if (Application::IsKeyPressed('2') and timesincelastbuttonpress > 0.2 and Dialogue == 5)
				{
					GD_PrintLine1 = "Hah! That's a movie studio,";
					GD_PrintLine2 = "no game comes from there!";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
					Dialogue = 10;
					timesincelastbuttonpress = 0;
					failNPC++;
					if (failNPC < 2)
					{
						Dialogue = 13;
					}
					else
					{
						Dialogue = 10;
					}
				}
				if (Application::IsKeyPressed('3') and timesincelastbuttonpress > 0.2 and Dialogue == 5)
				{
					GD_PrintLine1 = "Can I even trust you into buying this game?";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
					Dialogue = 10;
					timesincelastbuttonpress = 0;
					failNPC++;
					if (failNPC < 2)
					{
						Dialogue = 13;
					}
					else
					{
						Dialogue = 10;
					}
				}
				if (Application::IsKeyPressed('1') and timesincelastbuttonpress > 0.2 and Dialogue == 6)
				{
					GD_PrintLine1 = "Are you being serious?";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
					Dialogue = 10;
					timesincelastbuttonpress = 0;
					failNPC++;
					if (failNPC < 2)
					{
						Dialogue = 13;
					}
					else
					{
						Dialogue = 10;
					}
				}
				if (Application::IsKeyPressed('2') and timesincelastbuttonpress > 0.2 and Dialogue == 6)
				{
					GD_PrintLine1 = "Do you even know who is that?";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
					Dialogue = 10;
					timesincelastbuttonpress = 0;
					failNPC++;
					if (failNPC < 2)
					{
						Dialogue = 13;
					}
					else
					{
						Dialogue = 10;
					}
				}
				if (Application::IsKeyPressed('3') and timesincelastbuttonpress > 0.2 and Dialogue == 6)
				{
					GD_PrintLine1 = "Oh! I could get this for my brother!";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
					Dialogue = 9;
					timesincelastbuttonpress = 0;
				}
				if (Application::IsKeyPressed('1') and timesincelastbuttonpress > 0.2 and Dialogue == 7)
				{
					GD_PrintLine1 = "Hah! That's a movie studio,";
					GD_PrintLine2 = "no game comes from there!";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
					Dialogue = 10;
					timesincelastbuttonpress = 0;
					failNPC++;
					if (failNPC < 2)
					{
						Dialogue = 13;
					}
					else
					{
						Dialogue = 10;
					}
				}
				if (Application::IsKeyPressed('2') and timesincelastbuttonpress > 0.2 and Dialogue == 7)
				{
					GD_PrintLine1 = "Are you being serious?";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
					Dialogue = 10;
					timesincelastbuttonpress = 0;
					failNPC++;
					if (failNPC < 2)
					{
						Dialogue = 13;
					}
					else
					{
						Dialogue = 10;
					}
				}
				if (Application::IsKeyPressed('3') and timesincelastbuttonpress > 0.2 and Dialogue == 7)
				{
					GD_PrintLine1 = "Cool! Alright, I can finally try a different game now!";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
					Dialogue = 9;
					timesincelastbuttonpress = 0;
				}
				if (Application::IsKeyPressed('1') and timesincelastbuttonpress > 0.2 and Dialogue == 8)
				{
					GD_PrintLine1 = "Do you even know who is that?";
					GD_PrintLine2 = " ";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
					Dialogue = 10;
					timesincelastbuttonpress = 0;
					failNPC++;
					if (failNPC < 2)
					{
						Dialogue = 13;
					}
					else
					{
						Dialogue = 10;
					}
				}
				if (Application::IsKeyPressed('2') and timesincelastbuttonpress > 0.2 and Dialogue == 8)
				{

					GD_PrintLine1 = "Can I even trust you into buying this game?";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
					Dialogue = 10;
					timesincelastbuttonpress = 0;
					failNPC++;
					if (failNPC < 2)
					{
						Dialogue = 13;
					}
					else
					{
						Dialogue = 10;
					}
				}
				if (Application::IsKeyPressed('3') and timesincelastbuttonpress > 0.2 and Dialogue == 8)
				{
					GD_PrintLine1 = "Oh! I could get this for my brother!";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
					Dialogue = 9;
					timesincelastbuttonpress = 0;
				}
				if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 9)
				{
					randomsuccess = rand() % 3 + 1;
					if (randomsuccess == 1)
					{
						GD_PrintLine1 = "This looks like a steal, I will take it.";
						GD_PrintLine2 = "";
						GD_PrintLine3 = "";
					}
					if (randomsuccess == 2)
					{
						GD_PrintLine1 = "You've convinced me, I'll take it.";
						GD_PrintLine2 = "";
						GD_PrintLine3 = "";
					}
					if (randomsuccess == 3)
					{
						GD_PrintLine1 = "I would use this, thanks!";
						GD_PrintLine2 = "";
						GD_PrintLine3 = "";
					}
					// testing
					person = "PEDESTRIAN";
					Dialogue = 11;
					timesincelastbuttonpress = 0;
					Application::Cash += 100;
				}
				if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 10)
				{
					randomfail = rand() % 3 + 1;
					if (randomfail == 1)
					{
						GD_PrintLine1 = "You just tried to scam me right, ";
						GD_PrintLine2 = "I am calling the police.";
						GD_PrintLine3 = "";
					}
					if (randomfail == 2)
					{
						GD_PrintLine1 = "Police! This person is a scammer!";
						GD_PrintLine2 = "";
						GD_PrintLine3 = "";
					}
					if (randomfail == 3)
					{
						GD_PrintLine1 = "I am calling the police, you are done for!";
						GD_PrintLine2 = "";
						GD_PrintLine3 = "";
					}
					person = "PEDESTRIAN";
					Dialogue = 12;
					timesincelastbuttonpress = 0;
				}
				if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 11)
				{
					SetCursorPos(camera.center.x, camera.center.y);
					Dialogue = 1;
					DialogueBoxOpen = false;
					GD_PrintLine1 = "";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					AllowNPC1Movement = true;
					person = "???";
					timesincelastbuttonpress = 0;
					inshop = false;
				}
				if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 12)
				{
					SetCursorPos(camera.center.x, camera.center.y);
					Dialogue = 1;
					DialogueBoxOpen = false;
					Stars++;
					GD_PrintLine1 = "";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					AllowNPC1Movement = true;
					person = "???";
					timesincelastbuttonpress = 0;
					inshop = false;
				}
				if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 13)
				{
					GD_PrintLine1 = "(Crap, i better choose my words";
					GD_PrintLine2 = " carefully before i get caught)";
					GD_PrintLine3 = "";
					person = "       YOU";
					Dialogue = 4;
					timesincelastbuttonpress = 0;
				}
			}
			if (randomscam == 2)
			{
				if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 2)
				{
					GD_PrintLine1 = "I am selling this wine for $100.";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "       YOU";
					Dialogue = 3;
					timesincelastbuttonpress = 0;
				}

				if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 3)
				{
					GD_PrintLine1 = "Oh! Where is this wine from?";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
					Dialogue = 4;
					timesincelastbuttonpress = 0;
				}
				if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 4)
				{
					randomtext = rand() % 4 + 1;

					if (randomtext == 1)
					{
						GD_PrintLine1 = "1. The countryside!";
						GD_PrintLine2 = "2. The dollar store! ";
						GD_PrintLine3 = "3. It is from Alsace, France!";
						Dialogue = 5;
						timesincelastbuttonpress = 0;
					}
					if (randomtext == 2)
					{
						GD_PrintLine1 = "1. I personally brewed this wine.";
						GD_PrintLine2 = "2. I got it from Italy";
						GD_PrintLine3 = "3. I got it from 711";
						Dialogue = 6;
						timesincelastbuttonpress = 0;
					}
					if (randomtext == 3)
					{
						GD_PrintLine1 = "1. It is from Alsace, France!";
						GD_PrintLine2 = "2. I personally brewed this wine. ";
						GD_PrintLine3 = "3. The countryside!";
						Dialogue = 7;
						timesincelastbuttonpress = 0;
					}
					if (randomtext == 4)
					{
						GD_PrintLine1 = "1. The dollar store!";
						GD_PrintLine2 = "2. I got it from Italy ";
						GD_PrintLine3 = "3. I got it from 711";
						Dialogue = 8;
						timesincelastbuttonpress = 0;
					}
					person = "       YOU";
				}
				if (Application::IsKeyPressed('1') and timesincelastbuttonpress > 0.2 and Dialogue == 5)
				{
					GD_PrintLine1 = "You are kidding me right?";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
					Dialogue = 10;
					timesincelastbuttonpress = 0;
					failNPC++;
					if (failNPC < 2)
					{
						Dialogue = 13;
					}
					else
					{
						Dialogue = 10;
					}
				}
				if (Application::IsKeyPressed('2') and timesincelastbuttonpress > 0.2 and Dialogue == 5)
				{
					GD_PrintLine1 = "Did you expect me to give you a good answer for that?";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
					Dialogue = 10;
					timesincelastbuttonpress = 0;
					failNPC++;
					if (failNPC < 2)
					{
						Dialogue = 13;
					}

					else
					{
						Dialogue = 10;
					}
				}
				if (Application::IsKeyPressed('3') and timesincelastbuttonpress > 0.2 and Dialogue == 5)
				{
					GD_PrintLine1 = "I love the wine there!";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
					Dialogue = 9;
					timesincelastbuttonpress = 0;
				}
				if (Application::IsKeyPressed('1') and timesincelastbuttonpress > 0.2 and Dialogue == 6)
				{
					GD_PrintLine1 = "Is your wine even old enough to drink?";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
					Dialogue = 10;
					timesincelastbuttonpress = 0;
					failNPC++;
					if (failNPC < 2)
					{
						Dialogue = 13;
					}
					else
					{
						Dialogue = 10;
					}
				}
				if (Application::IsKeyPressed('2') and timesincelastbuttonpress > 0.2 and Dialogue == 6)
				{
					GD_PrintLine1 = "The classic I see, I will take it.";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
					Dialogue = 9;
					timesincelastbuttonpress = 0;
				}
				if (Application::IsKeyPressed('3') and timesincelastbuttonpress > 0.2 and Dialogue == 6)
				{
					GD_PrintLine1 = "For a wine from 711 that is pretty expensive.";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
					Dialogue = 10;
					timesincelastbuttonpress = 0;
					failNPC++;
					if (failNPC < 2)
					{
						Dialogue = 13;
					}
					else
					{
						Dialogue = 10;
					}
				}
				if (Application::IsKeyPressed('1') and timesincelastbuttonpress > 0.2 and Dialogue == 7)
				{
					GD_PrintLine1 = "I love the wine there!";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
					Dialogue = 9;
					timesincelastbuttonpress = 0;
				}
				if (Application::IsKeyPressed('2') and timesincelastbuttonpress > 0.2 and Dialogue == 7)
				{
					GD_PrintLine1 = "Is your wine even old enough to drink?";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
					Dialogue = 10;
					timesincelastbuttonpress = 0;
					failNPC++;
					if (failNPC < 2)
					{
						Dialogue = 13;
					}
					else
					{
						Dialogue = 10;
					}
				}
				if (Application::IsKeyPressed('3') and timesincelastbuttonpress > 0.2 and Dialogue == 7)
				{
					GD_PrintLine1 = "You are kidding me right?";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
					Dialogue = 10;
					timesincelastbuttonpress = 0;
					failNPC++;
					if (failNPC < 2)
					{
						Dialogue = 13;
					}
					else
					{
						Dialogue = 10;
					}
				}
				if (Application::IsKeyPressed('1') and timesincelastbuttonpress > 0.2 and Dialogue == 8)
				{
					GD_PrintLine1 = "Did you expect me to give you a good answer for that?";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
					Dialogue = 10;
					timesincelastbuttonpress = 0;
					failNPC++;
					if (failNPC < 2)
					{
						Dialogue = 13;
					}
					else
					{
						Dialogue = 10;
					}
				}
				if (Application::IsKeyPressed('2') and timesincelastbuttonpress > 0.2 and Dialogue == 8)
				{
					GD_PrintLine1 = "The classic I see, I will take it.";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
					Dialogue = 9;
					timesincelastbuttonpress = 0;
				}
				if (Application::IsKeyPressed('3') and timesincelastbuttonpress > 0.2 and Dialogue == 8)
				{
					GD_PrintLine1 = "For a wine from 711 that is pretty expensive.";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
					Dialogue = 10;
					timesincelastbuttonpress = 0;
					failNPC++;
					if (failNPC < 2)
					{
						Dialogue = 13;
					}
					else
					{
						Dialogue = 10;
					}
				}
				if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 9)
				{
					randomsuccess = rand() % 3 + 1;
					if (randomsuccess == 1)
					{
						GD_PrintLine1 = "This looks like a steal, I will take it.";
						GD_PrintLine2 = "";
						GD_PrintLine3 = "";
					}
					if (randomsuccess == 2)
					{
						GD_PrintLine1 = "You've convinced me, I'll take it.";
						GD_PrintLine2 = "";
						GD_PrintLine3 = "";
					}
					if (randomsuccess == 3)
					{
						GD_PrintLine1 = "I would use this, thanks!";
						GD_PrintLine2 = "";
						GD_PrintLine3 = "";
					}
					person = "PEDESTRIAN";
					Dialogue = 11;
					timesincelastbuttonpress = 0;
					Application::Cash += 100;
				}
				if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 10)
				{
					randomfail = rand() % 3 + 1;
					if (randomfail == 1)
					{
						GD_PrintLine1 = "You just tried to scam me right, I am calling the police.";
						GD_PrintLine2 = "";
						GD_PrintLine3 = "";
					}
					if (randomfail == 2)
					{
						GD_PrintLine1 = "Police! This person is a scammer!";
						GD_PrintLine2 = "";
						GD_PrintLine3 = "";
					}
					if (randomfail == 3)
					{
						GD_PrintLine1 = "I am calling the police, you are done for!";
						GD_PrintLine2 = "";
						GD_PrintLine3 = "";
					}
					person = "PEDESTRIAN";
					Dialogue = 12;
					timesincelastbuttonpress = 0;
				}
				if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 11)
				{
					SetCursorPos(camera.center.x, camera.center.y);
					Dialogue = 1;
					DialogueBoxOpen = false;
					GD_PrintLine1 = "";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					AllowNPC1Movement = true;
					person = "???";
					timesincelastbuttonpress = 0;
					inshop = false;
				}
				if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 12)
				{
					SetCursorPos(camera.center.x, camera.center.y);
					Dialogue = 1;
					DialogueBoxOpen = false;
					Stars++;
					GD_PrintLine1 = "";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					AllowNPC1Movement = true;
					person = "???";
					timesincelastbuttonpress = 0;
					inshop = false;
				}
				if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 13)
				{
					GD_PrintLine1 = "(Crap, i better choose my words ";
					GD_PrintLine2 = " carefully before i get caught)";
					GD_PrintLine3 = "";
					person = "       YOU";
					Dialogue = 4;
					timesincelastbuttonpress = 0;
				}
			}
			if (randomscam == 3)
			{
				if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 2)
				{
					GD_PrintLine1 = "I see you are a religious person.";
					GD_PrintLine2 = "I have this charm just for you for $100.";
					GD_PrintLine3 = "";
					person = "       YOU";
					Dialogue = 3;
					timesincelastbuttonpress = 0;
				}
				if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 3)
				{
					GD_PrintLine1 = "Convince me to buy it.";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
					Dialogue = 4;
					timesincelastbuttonpress = 0;
				}
				if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 4)
				{
					randomtext = rand() % 3 + 1;
					if (randomtext == 1)
					{
						GD_PrintLine1 = "1. This charm has helped many people become millions!";
						GD_PrintLine2 = "2. My mom uses it everyday.";
						GD_PrintLine3 = "3. This charm came from the temple of water. ";
						Dialogue = 5;
						timesincelastbuttonpress = 0;
					}
					if (randomtext == 2)
					{
						GD_PrintLine1 = "1. It guarantees you will pass every exam you have.";
						GD_PrintLine2 = "2. This charm can scare off ghosts.";
						GD_PrintLine3 = "3. It would make your face look nicer. ";
						Dialogue = 6;
						timesincelastbuttonpress = 0;
					}
					if (randomtext == 3)
					{
						GD_PrintLine1 = "1. This charm came from the temple of water.";
						GD_PrintLine2 = "2. It guarantees you will pass every exam you have.";
						GD_PrintLine3 = "3. This charm has helped many people become millions! ";
						Dialogue = 7;
						timesincelastbuttonpress = 0;
					}
					if (randomtext == 4)
					{
						GD_PrintLine1 = "1. My mom uses it everyday.";
						GD_PrintLine2 = "2. It would make your face look nicer.";
						GD_PrintLine3 = "3. This charm can scare off ghosts. ";
						Dialogue = 8;
						timesincelastbuttonpress = 0;
					}
					person = "       YOU";
				}
				if (Application::IsKeyPressed('1') and timesincelastbuttonpress > 0.2 and Dialogue == 5)
				{
					GD_PrintLine1 = "Wow! I love to be a millionaire one day.";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
					Dialogue = 9;
					timesincelastbuttonpress = 0;
				}
				if (Application::IsKeyPressed('2') and timesincelastbuttonpress > 0.2 and Dialogue == 5)
				{
					GD_PrintLine1 = "I do not care.";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
					Dialogue = 10;
					timesincelastbuttonpress = 0;
					failNPC++;
					if (failNPC < 2)
					{
						Dialogue = 13;
					}
					else
					{
						Dialogue = 10;
					}
				}
				if (Application::IsKeyPressed('3') and timesincelastbuttonpress > 0.2 and Dialogue == 5)
				{
					GD_PrintLine1 = "That is from a game. ";
					GD_PrintLine2 = "Are you even selling a real charm?";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
					Dialogue = 10;
					timesincelastbuttonpress = 0;
					failNPC++;
					if (failNPC < 2)
					{
						Dialogue = 13;
					}
					else
					{
						Dialogue = 10;
					}
				}
				if (Application::IsKeyPressed('1') and timesincelastbuttonpress > 0.2 and Dialogue == 6)
				{
					GD_PrintLine1 = "You do know I am not a student anymore right? ";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
					Dialogue = 10;
					timesincelastbuttonpress = 0;
					failNPC++;
					if (failNPC < 2)
					{
						Dialogue = 13;
					}
					else
					{
						Dialogue = 10;
					}
				}
				if (Application::IsKeyPressed('2') and timesincelastbuttonpress > 0.2 and Dialogue == 6)
				{
					GD_PrintLine1 = "Oh this could keep me safe.";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
					Dialogue = 9;
					timesincelastbuttonpress = 0;
				}
				if (Application::IsKeyPressed('3') and timesincelastbuttonpress > 0.2 and Dialogue == 6)
				{
					GD_PrintLine1 = "Have you looked at yourself in the mirror?";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
					Dialogue = 10;
					timesincelastbuttonpress = 0;
					failNPC++;
					if (failNPC < 2)
					{
						Dialogue = 13;
					}
					else
					{
						Dialogue = 10;
					}
				}
				if (Application::IsKeyPressed('1') and timesincelastbuttonpress > 0.2 and Dialogue == 7)
				{
					GD_PrintLine1 = "That is from a game. ";
					GD_PrintLine2 = "Are you even selling a real charm?";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
					Dialogue = 10;
					timesincelastbuttonpress = 0;
					failNPC++;
					if (failNPC < 2)
					{
						Dialogue = 13;
					}
					else
					{
						Dialogue = 10;
					}
				}
				if (Application::IsKeyPressed('2') and timesincelastbuttonpress > 0.2 and Dialogue == 7)
				{
					GD_PrintLine1 = "You do know I am not a student anymore right? ";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
					Dialogue = 10;
					timesincelastbuttonpress = 0;
					failNPC++;
					if (failNPC < 2)
					{
						Dialogue = 13;
					}
					else
					{
						Dialogue = 10;
					}
				}
				if (Application::IsKeyPressed('3') and timesincelastbuttonpress > 0.2 and Dialogue == 7)
				{
					GD_PrintLine1 = "Wow! I love to be a millionaire one day.";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
					Dialogue = 9;
					timesincelastbuttonpress = 0;
				}
				if (Application::IsKeyPressed('1') and timesincelastbuttonpress > 0.2 and Dialogue == 8)
				{
					GD_PrintLine1 = "I do not care.";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
					Dialogue = 10;
					timesincelastbuttonpress = 0;
					failNPC++;
					if (failNPC < 2)
					{
						Dialogue = 13;
					}
					else
					{
						Dialogue = 10;
					}
				}
				if (Application::IsKeyPressed('2') and timesincelastbuttonpress > 0.2 and Dialogue == 8)
				{
					GD_PrintLine1 = "Have you looked at yourself in the mirror?";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
					Dialogue = 10;
					timesincelastbuttonpress = 0;
					failNPC++;
					if (failNPC < 2)
					{
						Dialogue = 13;
					}
					else
					{
						Dialogue = 10;
					}
				}
				if (Application::IsKeyPressed('3') and timesincelastbuttonpress > 0.2 and Dialogue == 8)
				{
					GD_PrintLine1 = "Oh this could keep me safe.";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
					Dialogue = 9;
					timesincelastbuttonpress = 0;
				}
				if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 9)
				{
					randomsuccess = rand() % 3 + 1;
					if (randomsuccess == 1)
					{
						GD_PrintLine1 = "This looks like a steal, I will take it.";
						GD_PrintLine2 = "";
						GD_PrintLine3 = "";
					}
					if (randomsuccess == 2)
					{
						GD_PrintLine1 = "You've convinced me, I'll take it.";
						GD_PrintLine2 = "";
						GD_PrintLine3 = "";
					}
					if (randomsuccess == 3)
					{
						GD_PrintLine1 = "I would use this, thanks!";
						GD_PrintLine2 = "";
						GD_PrintLine3 = "";
					}
					person = "PEDESTRIAN";
					Dialogue = 11;
					timesincelastbuttonpress = 0;
					Application::Cash += 100;
				}
				if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 10)
				{
					randomfail = rand() % 3 + 1;
					if (randomfail == 1)
					{
						GD_PrintLine1 = "You just tried to scam me right, I am calling the police.";
						GD_PrintLine2 = "";
						GD_PrintLine3 = "";
					}
					if (randomfail == 2)
					{
						GD_PrintLine1 = "Police! This person is a scammer!";
						GD_PrintLine2 = "";
						GD_PrintLine3 = "";
					}
					if (randomfail == 3)
					{
						GD_PrintLine1 = "I am calling the police, you are done for!";
						GD_PrintLine2 = "";
						GD_PrintLine3 = "";
					}
					person = "PEDESTRIAN";
					Dialogue = 12;
					timesincelastbuttonpress = 0;
				}
				if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 11)
				{
					SetCursorPos(camera.center.x, camera.center.y);
					Dialogue = 1;
					DialogueBoxOpen = false;
					GD_PrintLine1 = "";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					AllowNPC1Movement = true;
					person = "???";
					timesincelastbuttonpress = 0;
					inshop = false;
				}
				if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 12)
				{
					SetCursorPos(camera.center.x, camera.center.y);
					Dialogue = 1;
					DialogueBoxOpen = false;
					Stars++;
					GD_PrintLine1 = "";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					AllowNPC1Movement = true;
					person = "???";
					timesincelastbuttonpress = 0;
					inshop = false;
				}
				if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 13)
				{
					GD_PrintLine1 = "(Crap, i better choose my words ";
					GD_PrintLine2 = "carefully before i get caught)";
					GD_PrintLine3 = "";
					person = "       YOU";
					Dialogue = 4;
					timesincelastbuttonpress = 0;
				}
			}
		}

		// NPC item scam
		else if (DistanceParameter(player.getposition().x, player.getposition().z, objectlist[hb_NPC2].getposition().x, objectlist[hb_NPC2].getposition().z) <= 4 and Stars == 0 and camera.Position_Y == 2)
		{
			if (rings <= 0) {
				ringscam = false;
			}
			if (watches <= 0) {
				watchscam = false;
			}
			if (necklace <= 0) {
				necklacescam = false;
			}
			if (timesincelastbuttonpress > 0.2 and Dialogue == 1)
			{
				AllowNPC2Movement = false;
				randomgreet = rand() % 4 + 1;
				if (randomgreet == 1)
				{
					GD_PrintLine1 = "Huh? Whayya want?";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
				}
				else if (randomgreet == 2)
				{
					GD_PrintLine1 = "Hi! What do you need?";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
				}
				else if (randomgreet == 3)
				{
					GD_PrintLine1 = "I've got no time to waste for you kid, hurry up and say it!";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
				}
				else if (randomgreet == 4)
				{
					GD_PrintLine1 = "I hate wasting time, spit it out already!";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
				}
				person = "PEDESTRIAN";
				Dialogue = 3;
				timesincelastbuttonpress = 0;
				randomscam = rand() % 3 + 1;
			}
			if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 3 && rings >= 1)
			{
				GD_PrintLine1 = "I like to sell you this exquisite ring i have here.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "       YOU";
				Dialogue = 4;
				ringscam = true;
				timesincelastbuttonpress = 0;
			}
			else if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 3 && watches >= 1)
			{
				GD_PrintLine1 = "I like to sell you this exquisite watch i have here.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "       YOU";
				Dialogue = 4;
				watchscam = true;
				timesincelastbuttonpress = 0;
			}
			else if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 3 && necklace >= 1)
			{
				GD_PrintLine1 = "I like to sell you exquisite necklace i have here.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "       YOU";
				Dialogue = 4;
				necklacescam = true;
				timesincelastbuttonpress = 0;
			}
			else if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 4 && ringscam == true) {
				randomtext = rand() % 4 + 1;
				if (randomtext == 1)
				{
					GD_PrintLine1 = "1. It is made with multiple diamonds";
					GD_PrintLine2 = "2. I found it from the garbage bin down the road";
					GD_PrintLine3 = "3. It is a ring worn by the famous actor James Bonk.";
					person = "       YOU";
					Dialogue = 5;
					timesincelastbuttonpress = 0;
				}
				else if (randomtext == 2)
				{
					GD_PrintLine1 = "1. I found it from the garbage bin down the road.";
					GD_PrintLine2 = "2. It is a limited edition swalawalaski ring.";
					GD_PrintLine3 = "3. It is from a gift from the late president.";
					person = "       YOU";
					Dialogue = 6;
					timesincelastbuttonpress = 0;
				}
				else if (randomtext == 3)
				{
					GD_PrintLine1 = "1. It is from a gift from the late president.";
					GD_PrintLine2 = "2. It is a family treasure passed down from my late grandfather. ";
					GD_PrintLine3 = "3. I just bought this from the shop down the street.";
					person = "       YOU";
					Dialogue = 7;
					timesincelastbuttonpress = 0;
				}
				else if (randomtext == 4)
				{
					GD_PrintLine1 = "1. It is a limited edition swalawalaski ring.";
					GD_PrintLine2 = "2. It is a ring worn by the famous actor James Bonk.";
					GD_PrintLine3 = "3. I just bought this from the shop down the street.";
					person = "       YOU";
					Dialogue = 8;
					timesincelastbuttonpress = 0;
				}
			}
			if (Application::IsKeyPressed('1') and timesincelastbuttonpress > 0.2 and Dialogue == 5 && ringscam == true)
			{
				GD_PrintLine1 = "Nice try, that clearly is a fake ring.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			else if (Application::IsKeyPressed('2') and timesincelastbuttonpress > 0.2 and Dialogue == 5 && ringscam == true)
			{
				GD_PrintLine1 = "Why are you even trying to sell it to me? ";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			else if (Application::IsKeyPressed('3') and timesincelastbuttonpress > 0.2 and Dialogue == 5 && ringscam == true)
			{
				GD_PrintLine1 = "He is my favourite actor!,";
				GD_PrintLine2 = "I would love to get that ring.";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 11;
			}
			if (Application::IsKeyPressed('1') and timesincelastbuttonpress > 0.2 and Dialogue == 6 && ringscam == true)
			{
				GD_PrintLine1 = "Why are you even trying to sell it to me? ";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			else if (Application::IsKeyPressed('2') and timesincelastbuttonpress > 0.2 and Dialogue == 6 && ringscam == true)
			{
				GD_PrintLine1 = "Never heard of it before, ";
				GD_PrintLine2 = "probably from the pasar malam.";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			else if (Application::IsKeyPressed('3') and timesincelastbuttonpress > 0.2 and Dialogue == 6 && ringscam == true)
			{
				GD_PrintLine1 = "Oh that could be worth a lot.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 11;
			}
			if (Application::IsKeyPressed('1') and timesincelastbuttonpress > 0.2 and Dialogue == 7 && ringscam == true)
			{
				GD_PrintLine1 = "Oh that could be worth a lot.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 11;
			}
			else if (Application::IsKeyPressed('2') and timesincelastbuttonpress > 0.2 and Dialogue == 7 && ringscam == true)
			{
				GD_PrintLine1 = "So sorry to hear that but this ring ";
				GD_PrintLine2 = "is not to my liking";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			else if (Application::IsKeyPressed('3') and timesincelastbuttonpress > 0.2 and Dialogue == 7 && ringscam == true)
			{
				GD_PrintLine1 = "They do not even sell rings, what are you saying?";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			if (Application::IsKeyPressed('1') and timesincelastbuttonpress > 0.2 and Dialogue == 8 && ringscam == true)
			{
				GD_PrintLine1 = "Never heard of it before, ";
				GD_PrintLine2 = "probably from the pasar malam.";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			else if (Application::IsKeyPressed('2') and timesincelastbuttonpress > 0.2 and Dialogue == 8 && ringscam == true)
			{
				GD_PrintLine1 = "He is my favourite actor!,";
				GD_PrintLine2 = "I would love to get that ring.";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 11;
			}
			else if (Application::IsKeyPressed('3') and timesincelastbuttonpress > 0.2 and Dialogue == 8 && ringscam == true)
			{
				GD_PrintLine1 = "They do not even sell rings, what are you saying?";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			else if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 4 && watchscam == true) {
				randomtext = rand() % 4 + 1;
				if (randomtext == 1)
				{
					GD_PrintLine1 = "1. It's able to stop time!";
					GD_PrintLine2 = "2. It tells you the time.";
					GD_PrintLine3 = "3. It is the watch that was showcased in the new movie.";
					person = "       YOU";
					Dialogue = 5;
					timesincelastbuttonpress = 0;
				}
				else if (randomtext == 2)
				{
					GD_PrintLine1 = "1. You can do JoJo poses with this.";
					GD_PrintLine2 = "2. The watch is an ancient relic.";
					GD_PrintLine3 = "3. It is an Apple Watch.";
					person = "       YOU";
					Dialogue = 6;
					timesincelastbuttonpress = 0;
				}
				else if (randomtext == 3)
				{
					GD_PrintLine1 = "1. It is the watch that was showcased in the new movie.";
					GD_PrintLine2 = "2. You can do JoJo poses with this.";
					GD_PrintLine3 = "3. It tells you the time.";
					person = "       YOU";
					Dialogue = 7;
					timesincelastbuttonpress = 0;
				}
				else if (randomtext == 4)
				{
					GD_PrintLine1 = "1. It's able to stop time!";
					GD_PrintLine2 = "2. The watch is an ancient relic.";
					GD_PrintLine3 = "3. It is an Apple Watch.";
					person = "       YOU";
					Dialogue = 8;
					timesincelastbuttonpress = 0;
				}
			}
			if (Application::IsKeyPressed('1') and timesincelastbuttonpress > 0.2 and Dialogue == 5 && watchscam == true)
			{
				GD_PrintLine1 = "Ya, ya, stop dreaming about your weird fantasies.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			else if (Application::IsKeyPressed('2') and timesincelastbuttonpress > 0.2 and Dialogue == 5 && watchscam == true)
			{
				GD_PrintLine1 = "No shit sherlock.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			else if (Application::IsKeyPressed('3') and timesincelastbuttonpress > 0.2 and Dialogue == 5 && watchscam == true)
			{
				GD_PrintLine1 = "The new movie 'Royal'? I love that movie!";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 11;
			}
			if (Application::IsKeyPressed('1') and timesincelastbuttonpress > 0.2 and Dialogue == 6 && watchscam == true)
			{
				GD_PrintLine1 = "That's cringe, are you ok?";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			else if (Application::IsKeyPressed('2') and timesincelastbuttonpress > 0.2 and Dialogue == 6 && watchscam == true)
			{
				GD_PrintLine1 = "Oh! A rare find from the age of old.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 11;
			}
			else if (Application::IsKeyPressed('3') and timesincelastbuttonpress > 0.2 and Dialogue == 6 && watchscam == true)
			{
				GD_PrintLine1 = "Nice lie when your watch can't even touch screen.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			if (Application::IsKeyPressed('1') and timesincelastbuttonpress > 0.2 and Dialogue == 7 && watchscam == true)
			{
				GD_PrintLine1 = "The new movie 'Royal'? I love that movie!";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 11;
			}
			else if (Application::IsKeyPressed('2') and timesincelastbuttonpress > 0.2 and Dialogue == 7 && watchscam == true)
			{
				GD_PrintLine1 = "That's cringe, are you ok?";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			else if (Application::IsKeyPressed('3') and timesincelastbuttonpress > 0.2 and Dialogue == 7 && watchscam == true)
			{
				GD_PrintLine1 = "No shit sherlock.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			if (Application::IsKeyPressed('1') and timesincelastbuttonpress > 0.2 and Dialogue == 8 && watchscam == true)
			{
				GD_PrintLine1 = "Ya, ya, stop dreaming about your weird fantasies.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			else if (Application::IsKeyPressed('2') and timesincelastbuttonpress > 0.2 and Dialogue == 8 && watchscam == true)
			{
				GD_PrintLine1 = "Oh! A rare find from the age of old.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 11;
			}
			else if (Application::IsKeyPressed('3') and timesincelastbuttonpress > 0.2 and Dialogue == 8 && watchscam == true)
			{
				GD_PrintLine1 = "Nice lie when your watch can't even touch screen.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			else if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 4 && necklacescam == true) {
				randomtext = rand() % 4 + 1;
				if (randomtext == 1)
				{
					GD_PrintLine1 = "1. My friend bought it to me as a gift.";
					GD_PrintLine2 = "2. It is an olympic gold necklace.";
					GD_PrintLine3 = "3. It is 999 karat gold necklace.";
					person = "       YOU";
					Dialogue = 5;
					timesincelastbuttonpress = 0;
				}
				else if (randomtext == 2)
				{
					GD_PrintLine1 = "1. The necklace is worn by a soldier that died in WWII.";
					GD_PrintLine2 = "2. The necklace is nice to wear.";
					GD_PrintLine3 = "3. The necklace helps to cure cancer.";
					person = "       YOU";
					Dialogue = 6;
					timesincelastbuttonpress = 0;
				}
				else if (randomtext == 3)
				{
					GD_PrintLine1 = "1. It is 999 karat gold necklace.";
					GD_PrintLine2 = "2. It is an olympic gold necklace.";
					GD_PrintLine3 = "3. The necklace is nice to wear.";
					person = "       YOU";
					Dialogue = 7;
					timesincelastbuttonpress = 0;
				}
				else if (randomtext == 4)
				{
					GD_PrintLine1 = "1. The necklace helps to cure cancer.";
					GD_PrintLine2 = "2. My friend bought it to me as a gift.";
					GD_PrintLine3 = "3.  The necklace is worn by a soldier that died in WWII";
					person = "       YOU";
					Dialogue = 8;
					timesincelastbuttonpress = 0;
				}
			}
			if (Application::IsKeyPressed('1') and timesincelastbuttonpress > 0.2 and Dialogue == 5 && necklacescam == true)
			{
				GD_PrintLine1 = "So sorry but this necklace is a cheap one.";
				GD_PrintLine2 = "It's probably better to keep it.";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			else if (Application::IsKeyPressed('2') and timesincelastbuttonpress > 0.2 and Dialogue == 5 && necklacescam == true)
			{
				GD_PrintLine1 = "Wow you are a gold medalist? That sure would worth a lot!";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 11;
			}
			else if (Application::IsKeyPressed('3') and timesincelastbuttonpress > 0.2 and Dialogue == 5 && necklacescam == true)
			{
				GD_PrintLine1 = "This is clearly just gold plated, look at the shininess!";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			if (Application::IsKeyPressed('1') and timesincelastbuttonpress > 0.2 and Dialogue == 6 && necklacescam == true)
			{
				GD_PrintLine1 = "I am fond of history, I can add that to my collection.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 11;
			}
			else if (Application::IsKeyPressed('2') and timesincelastbuttonpress > 0.2 and Dialogue == 6 && necklacescam == true)
			{
				GD_PrintLine1 = "Ok and? All of the necklaces is nice to wear too.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			else if (Application::IsKeyPressed('3') and timesincelastbuttonpress > 0.2 and Dialogue == 6 && necklacescam == true)
			{
				GD_PrintLine1 = "This is a necklace not some medicine. Get your facts right.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			if (Application::IsKeyPressed('1') and timesincelastbuttonpress > 0.2 and Dialogue == 7 && necklacescam == true)
			{
				GD_PrintLine1 = "This is clearly just gold plated, look at the shininess!";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			else if (Application::IsKeyPressed('2') and timesincelastbuttonpress > 0.2 and Dialogue == 7 && necklacescam == true)
			{
				GD_PrintLine1 = "Wow you are a gold medalist? That sure would worth a lot!";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 11;
			}
			else if (Application::IsKeyPressed('3') and timesincelastbuttonpress > 0.2 and Dialogue == 7 && necklacescam == true)
			{
				GD_PrintLine1 = "Ok and? All of the necklaces is nice to wear too.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			if (Application::IsKeyPressed('1') and timesincelastbuttonpress > 0.2 and Dialogue == 8 && necklacescam == true)
			{
				GD_PrintLine1 = "This is a necklace not some medicine. Get your facts right.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			else if (Application::IsKeyPressed('2') and timesincelastbuttonpress > 0.2 and Dialogue == 8 && necklacescam == true)
			{
				GD_PrintLine1 = "So sorry but this necklace is a cheap one.";
				GD_PrintLine2 = "It's probably better to keep it.";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			else if (Application::IsKeyPressed('3') and timesincelastbuttonpress > 0.2 and Dialogue == 8 && necklacescam == true)
			{
				GD_PrintLine1 = "I am fond of history, I can add that to my collection.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 11;
			}
			if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 10)
			{
				randomfail = rand() % 3 + 1;
				if (randomfail == 1)
				{
					GD_PrintLine1 = "I am not buying that.";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
				}
				if (randomfail == 2)
				{
					GD_PrintLine1 = "This serves no purpose to me.";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
				}
				if (randomfail == 3)
				{
					GD_PrintLine1 = "This is not worth my money.";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
				}
				timesincelastbuttonpress = 0;
				failshop++;
				if (failshop < 2)
				{
					Dialogue = 15;
				}
				else
				{
					Dialogue = 12;
				}
			}
			if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 11)
			{
				randomsuccess = rand() % 3 + 1;
				if (randomsuccess == 1)
				{
					GD_PrintLine1 = "I will take it. This looks promising.";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
				}
				if (randomsuccess == 2)
				{
					GD_PrintLine1 = "Guess I can take this off your hands.";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
				}
				if (randomsuccess == 3)
				{
					GD_PrintLine1 = "This would be a great investment.";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
				}
				timesincelastbuttonpress = 0;
				Dialogue = 13;
			}
			if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 12)
			{
				GD_PrintLine1 = "Hey! Are you trying to scam me?";
				GD_PrintLine2 = "I am calling the police this instant!";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 14;
			}
			if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 13 && ringscam == true)
			{
				SetCursorPos(camera.center.x, camera.center.y);
				Dialogue = 1;
				DialogueBoxOpen = false;
				GD_PrintLine1 = "";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "???";
				timesincelastbuttonpress = 0;
				AllowNPC2Movement = true;
				inshop = false;
				rings--;
				Application::Cash += 100;
			}
			if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 13 && watchscam == true)
			{
				SetCursorPos(camera.center.x, camera.center.y);
				Dialogue = 1;
				DialogueBoxOpen = false;
				GD_PrintLine1 = "";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				AllowNPC2Movement = true;
				person = "???";
				timesincelastbuttonpress = 0;
				inshop = false;
				watches--;
				Application::Cash += 100;
			}
			if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 13 && necklacescam == true)
			{
				SetCursorPos(camera.center.x, camera.center.y);
				Dialogue = 1;
				DialogueBoxOpen = false;
				GD_PrintLine1 = "";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "???";
				timesincelastbuttonpress = 0;
				AllowNPC2Movement = true;
				inshop = false;
				necklace--;
				Application::Cash += 100;
			}
			if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 14)
			{
				SetCursorPos(camera.center.x, camera.center.y);
				Dialogue = 1;
				DialogueBoxOpen = false;
				Stars++;
				GD_PrintLine1 = "";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				AllowNPC2Movement = true;
				person = "???";
				timesincelastbuttonpress = 0;
				inshop = false;
			}
			if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 15)
			{
				GD_PrintLine1 = "(Crap, I better choose my words ";
				GD_PrintLine2 = "carefully before i get caught)";
				GD_PrintLine3 = "";
				person = "       YOU";
				Dialogue = 4;
				timesincelastbuttonpress = 0;
			}
		}


		// NPC3 item scam
		else if (DistanceParameter(player.getposition().x, player.getposition().z, objectlist[hb_NPC3].getposition().x, objectlist[hb_NPC3].getposition().z) <= 4 and Stars == 0 and camera.Position_Y == 2)
		{
			if (rings <= 0) {
				ringscam = false;
			}
			if (watches <= 0) {
				watchscam = false;
			}
			if (necklace <= 0) {
				necklacescam = false;
			}
			if (timesincelastbuttonpress > 0.2 and Dialogue == 1)
			{
				AllowNPC3Movement = false;
				randomgreet = rand() % 4 + 1;
				if (randomgreet == 1)
				{
					GD_PrintLine1 = "Huh? Whayya want?";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
				}
				else if (randomgreet == 2)
				{
					GD_PrintLine1 = "Hi! What do you need?";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
				}
				else if (randomgreet == 3)
				{
					GD_PrintLine1 = "I've got no time to waste for you kid, hurry up and say it!";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
				}
				else if (randomgreet == 4)
				{
					GD_PrintLine1 = "I hate wasting time, spit it out already!";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
				}
				person = "PEDESTRIAN";
				Dialogue = 3;
				timesincelastbuttonpress = 0;
				randomscam = rand() % 3 + 1;
			}
			if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 3 && rings >= 1)
			{
				GD_PrintLine1 = "I like to sell you this exquisite ring i have here.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "       YOU";
				Dialogue = 4;
				ringscam = true;
				timesincelastbuttonpress = 0;
			}
			else if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 3 && watches >= 1)
			{
				GD_PrintLine1 = "I like to sell you this exquisite watch i have here.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "       YOU";
				Dialogue = 4;
				watchscam = true;
				timesincelastbuttonpress = 0;
			}
			else if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 3 && necklace >= 1)
			{
				GD_PrintLine1 = "I like to sell you exquisite necklace i have here.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "       YOU";
				Dialogue = 4;
				necklacescam = true;
				timesincelastbuttonpress = 0;
			}
			else if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 4 && ringscam == true) {
				randomtext = rand() % 4 + 1;
				if (randomtext == 1)
				{
					GD_PrintLine1 = "1. It is made with multiple diamonds";
					GD_PrintLine2 = "2. I found it from the garbage bin down the road";
					GD_PrintLine3 = "3. It is a ring worn by the famous actor James Bonk.";
					person = "       YOU";
					Dialogue = 5;
					timesincelastbuttonpress = 0;
				}
				else if (randomtext == 2)
				{
					GD_PrintLine1 = "1. I found it from the garbage bin down the road.";
					GD_PrintLine2 = "2. It is a limited edition swalawalaski ring.";
					GD_PrintLine3 = "3. It is from a gift from the late president.";
					person = "       YOU";
					Dialogue = 6;
					timesincelastbuttonpress = 0;
				}
				else if (randomtext == 3)
				{
					GD_PrintLine1 = "1. It is from a gift from the late president.";
					GD_PrintLine2 = "2. It is a family treasure passed down from my late grandfather. ";
					GD_PrintLine3 = "3. I just bought this from the shop down the street.";
					person = "       YOU";
					Dialogue = 7;
					timesincelastbuttonpress = 0;
				}
				else if (randomtext == 4)
				{
					GD_PrintLine1 = "1. It is a limited edition swalawalaski ring.";
					GD_PrintLine2 = "2. It is a ring worn by the famous actor James Bonk.";
					GD_PrintLine3 = "3. I just bought this from the shop down the street.";
					person = "       YOU";
					Dialogue = 8;
					timesincelastbuttonpress = 0;
				}
			}
			if (Application::IsKeyPressed('1') and timesincelastbuttonpress > 0.2 and Dialogue == 5 && ringscam == true)
			{
				GD_PrintLine1 = "Nice try, that clearly is a fake ring.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			else if (Application::IsKeyPressed('2') and timesincelastbuttonpress > 0.2 and Dialogue == 5 && ringscam == true)
			{
				GD_PrintLine1 = "Why are you even trying to sell it to me? ";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			else if (Application::IsKeyPressed('3') and timesincelastbuttonpress > 0.2 and Dialogue == 5 && ringscam == true)
			{
				GD_PrintLine1 = "He is my favourite actor!,";
				GD_PrintLine2 = "I would love to get that ring.";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 11;
			}
			if (Application::IsKeyPressed('1') and timesincelastbuttonpress > 0.2 and Dialogue == 6 && ringscam == true)
			{
				GD_PrintLine1 = "Why are you even trying to sell it to me? ";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			else if (Application::IsKeyPressed('2') and timesincelastbuttonpress > 0.2 and Dialogue == 6 && ringscam == true)
			{
				GD_PrintLine1 = "Never heard of it before, ";
				GD_PrintLine2 = "probably from the pasar malam.";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			else if (Application::IsKeyPressed('3') and timesincelastbuttonpress > 0.2 and Dialogue == 6 && ringscam == true)
			{
				GD_PrintLine1 = "Oh that could be worth a lot.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 11;
			}
			if (Application::IsKeyPressed('1') and timesincelastbuttonpress > 0.2 and Dialogue == 7 && ringscam == true)
			{
				GD_PrintLine1 = "Oh that could be worth a lot.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 11;
			}
			else if (Application::IsKeyPressed('2') and timesincelastbuttonpress > 0.2 and Dialogue == 7 && ringscam == true)
			{
				GD_PrintLine1 = "So sorry to hear that but this ring ";
				GD_PrintLine2 = "is not to my liking";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			else if (Application::IsKeyPressed('3') and timesincelastbuttonpress > 0.2 and Dialogue == 7 && ringscam == true)
			{
				GD_PrintLine1 = "They do not even sell rings, what are you saying?";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			if (Application::IsKeyPressed('1') and timesincelastbuttonpress > 0.2 and Dialogue == 8 && ringscam == true)
			{
				GD_PrintLine1 = "Never heard of it before, ";
				GD_PrintLine2 = "probably from the pasar malam.";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			else if (Application::IsKeyPressed('2') and timesincelastbuttonpress > 0.2 and Dialogue == 8 && ringscam == true)
			{
				GD_PrintLine1 = "He is my favourite actor!,";
				GD_PrintLine2 = "I would love to get that ring.";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 11;
			}
			else if (Application::IsKeyPressed('3') and timesincelastbuttonpress > 0.2 and Dialogue == 8 && ringscam == true)
			{
				GD_PrintLine1 = "They do not even sell rings, what are you saying?";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			else if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 4 && watchscam == true) {
				randomtext = rand() % 4 + 1;
				if (randomtext == 1)
				{
					GD_PrintLine1 = "1. It's able to stop time!";
					GD_PrintLine2 = "2. It tells you the time.";
					GD_PrintLine3 = "3. It is the watch that was showcased in the new movie.";
					person = "       YOU";
					Dialogue = 5;
					timesincelastbuttonpress = 0;
				}
				else if (randomtext == 2)
				{
					GD_PrintLine1 = "1. You can do JoJo poses with this.";
					GD_PrintLine2 = "2. The watch is an ancient relic.";
					GD_PrintLine3 = "3. It is an Apple Watch.";
					person = "       YOU";
					Dialogue = 6;
					timesincelastbuttonpress = 0;
				}
				else if (randomtext == 3)
				{
					GD_PrintLine1 = "1. It is the watch that was showcased in the new movie.";
					GD_PrintLine2 = "2. You can do JoJo poses with this.";
					GD_PrintLine3 = "3. It tells you the time.";
					person = "       YOU";
					Dialogue = 7;
					timesincelastbuttonpress = 0;
				}
				else if (randomtext == 4)
				{
					GD_PrintLine1 = "1. It's able to stop time!";
					GD_PrintLine2 = "2. The watch is an ancient relic.";
					GD_PrintLine3 = "3. It is an Apple Watch.";
					person = "       YOU";
					Dialogue = 8;
					timesincelastbuttonpress = 0;
				}
			}
			if (Application::IsKeyPressed('1') and timesincelastbuttonpress > 0.2 and Dialogue == 5 && watchscam == true)
			{
				GD_PrintLine1 = "Ya, ya, stop dreaming about your weird fantasies.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			else if (Application::IsKeyPressed('2') and timesincelastbuttonpress > 0.2 and Dialogue == 5 && watchscam == true)
			{
				GD_PrintLine1 = "No shit sherlock.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			else if (Application::IsKeyPressed('3') and timesincelastbuttonpress > 0.2 and Dialogue == 5 && watchscam == true)
			{
				GD_PrintLine1 = "The new movie 'Royal'? I love that movie!";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 11;
			}
			if (Application::IsKeyPressed('1') and timesincelastbuttonpress > 0.2 and Dialogue == 6 && watchscam == true)
			{
				GD_PrintLine1 = "That's cringe, are you ok?";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			else if (Application::IsKeyPressed('2') and timesincelastbuttonpress > 0.2 and Dialogue == 6 && watchscam == true)
			{
				GD_PrintLine1 = "Oh! A rare find from the age of old.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 11;
			}
			else if (Application::IsKeyPressed('3') and timesincelastbuttonpress > 0.2 and Dialogue == 6 && watchscam == true)
			{
				GD_PrintLine1 = "Nice lie when your watch can't even touch screen.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			if (Application::IsKeyPressed('1') and timesincelastbuttonpress > 0.2 and Dialogue == 7 && watchscam == true)
			{
				GD_PrintLine1 = "The new movie 'Royal'? I love that movie!";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 11;
			}
			else if (Application::IsKeyPressed('2') and timesincelastbuttonpress > 0.2 and Dialogue == 7 && watchscam == true)
			{
				GD_PrintLine1 = "That's cringe, are you ok?";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			else if (Application::IsKeyPressed('3') and timesincelastbuttonpress > 0.2 and Dialogue == 7 && watchscam == true)
			{
				GD_PrintLine1 = "No shit sherlock.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			if (Application::IsKeyPressed('1') and timesincelastbuttonpress > 0.2 and Dialogue == 8 && watchscam == true)
			{
				GD_PrintLine1 = "Ya, ya, stop dreaming about your weird fantasies.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			else if (Application::IsKeyPressed('2') and timesincelastbuttonpress > 0.2 and Dialogue == 8 && watchscam == true)
			{
				GD_PrintLine1 = "Oh! A rare find from the age of old.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 11;
			}
			else if (Application::IsKeyPressed('3') and timesincelastbuttonpress > 0.2 and Dialogue == 8 && watchscam == true)
			{
				GD_PrintLine1 = "Nice lie when your watch can't even touch screen.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			else if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 4 && necklacescam == true) {
				randomtext = rand() % 4 + 1;
				if (randomtext == 1)
				{
					GD_PrintLine1 = "1. My friend bought it to me as a gift.";
					GD_PrintLine2 = "2. It is an olympic gold necklace.";
					GD_PrintLine3 = "3. It is 999 karat gold necklace.";
					person = "       YOU";
					Dialogue = 5;
					timesincelastbuttonpress = 0;
				}
				else if (randomtext == 2)
				{
					GD_PrintLine1 = "1. The necklace is worn by a soldier that died in WWII.";
					GD_PrintLine2 = "2. The necklace is nice to wear.";
					GD_PrintLine3 = "3. The necklace helps to cure cancer.";
					person = "       YOU";
					Dialogue = 6;
					timesincelastbuttonpress = 0;
				}
				else if (randomtext == 3)
				{
					GD_PrintLine1 = "1. It is 999 karat gold necklace.";
					GD_PrintLine2 = "2. It is an olympic gold necklace.";
					GD_PrintLine3 = "3. The necklace is nice to wear.";
					person = "       YOU";
					Dialogue = 7;
					timesincelastbuttonpress = 0;
				}
				else if (randomtext == 4)
				{
					GD_PrintLine1 = "1. The necklace helps to cure cancer.";
					GD_PrintLine2 = "2. My friend bought it to me as a gift.";
					GD_PrintLine3 = "3.  The necklace is worn by a soldier that died in WWII";
					person = "       YOU";
					Dialogue = 8;
					timesincelastbuttonpress = 0;
				}
			}
			if (Application::IsKeyPressed('1') and timesincelastbuttonpress > 0.2 and Dialogue == 5 && necklacescam == true)
			{
				GD_PrintLine1 = "So sorry but this necklace is a cheap one.";
				GD_PrintLine2 = "It's probably better to keep it.";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			else if (Application::IsKeyPressed('2') and timesincelastbuttonpress > 0.2 and Dialogue == 5 && necklacescam == true)
			{
				GD_PrintLine1 = "Wow you are a gold medalist? That sure would worth a lot!";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 11;
			}
			else if (Application::IsKeyPressed('3') and timesincelastbuttonpress > 0.2 and Dialogue == 5 && necklacescam == true)
			{
				GD_PrintLine1 = "This is clearly just gold plated, look at the shininess!";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			if (Application::IsKeyPressed('1') and timesincelastbuttonpress > 0.2 and Dialogue == 6 && necklacescam == true)
			{
				GD_PrintLine1 = "I am fond of history, I can add that to my collection.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 11;
			}
			else if (Application::IsKeyPressed('2') and timesincelastbuttonpress > 0.2 and Dialogue == 6 && necklacescam == true)
			{
				GD_PrintLine1 = "Ok and? All of the necklaces is nice to wear too.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			else if (Application::IsKeyPressed('3') and timesincelastbuttonpress > 0.2 and Dialogue == 6 && necklacescam == true)
			{
				GD_PrintLine1 = "This is a necklace not some medicine. Get your facts right.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			if (Application::IsKeyPressed('1') and timesincelastbuttonpress > 0.2 and Dialogue == 7 && necklacescam == true)
			{
				GD_PrintLine1 = "This is clearly just gold plated, look at the shininess!";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			else if (Application::IsKeyPressed('2') and timesincelastbuttonpress > 0.2 and Dialogue == 7 && necklacescam == true)
			{
				GD_PrintLine1 = "Wow you are a gold medalist? That sure would worth a lot!";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 11;
			}
			else if (Application::IsKeyPressed('3') and timesincelastbuttonpress > 0.2 and Dialogue == 7 && necklacescam == true)
			{
				GD_PrintLine1 = "Ok and? All of the necklaces is nice to wear too.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			if (Application::IsKeyPressed('1') and timesincelastbuttonpress > 0.2 and Dialogue == 8 && necklacescam == true)
			{
				GD_PrintLine1 = "This is a necklace not some medicine. Get your facts right.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			else if (Application::IsKeyPressed('2') and timesincelastbuttonpress > 0.2 and Dialogue == 8 && necklacescam == true)
			{
				GD_PrintLine1 = "So sorry but this necklace is a cheap one.";
				GD_PrintLine2 = "It's probably better to keep it.";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 10;
			}
			else if (Application::IsKeyPressed('3') and timesincelastbuttonpress > 0.2 and Dialogue == 8 && necklacescam == true)
			{
				GD_PrintLine1 = "I am fond of history, I can add that to my collection.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 11;
			}
			if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 10)
			{
				randomfail = rand() % 3 + 1;
				if (randomfail == 1)
				{
					GD_PrintLine1 = "I am not buying that.";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
				}
				if (randomfail == 2)
				{
					GD_PrintLine1 = "This serves no purpose to me.";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
				}
				if (randomfail == 3)
				{
					GD_PrintLine1 = "This is not worth my money.";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
				}
				timesincelastbuttonpress = 0;
				failshop++;
				if (failshop < 2)
				{
					Dialogue = 15;
				}
				else
				{
					Dialogue = 12;
				}
			}
			if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 11)
			{
				randomsuccess = rand() % 3 + 1;
				if (randomsuccess == 1)
				{
					GD_PrintLine1 = "I will take it. This looks promising.";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
				}
				if (randomsuccess == 2)
				{
					GD_PrintLine1 = "Guess I can take this off your hands.";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
				}
				if (randomsuccess == 3)
				{
					GD_PrintLine1 = "This would be a great investment.";
					GD_PrintLine2 = "";
					GD_PrintLine3 = "";
					person = "PEDESTRIAN";
				}
				timesincelastbuttonpress = 0;
				Dialogue = 13;
			}
			if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 12)
			{
				GD_PrintLine1 = "Hey! Are you trying to scam me?";
				GD_PrintLine2 = "I am calling the police this instant!";
				GD_PrintLine3 = "";
				person = "PEDESTRIAN";
				timesincelastbuttonpress = 0;
				Dialogue = 14;
			}
			if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 13 && ringscam == true)
			{
				SetCursorPos(camera.center.x, camera.center.y);
				Dialogue = 1;
				DialogueBoxOpen = false;
				GD_PrintLine1 = "";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "???";
				timesincelastbuttonpress = 0;
				AllowNPC3Movement = true;
				inshop = false;
				rings--;
				Application::Cash += 100;
			}
			if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 13 && watchscam == true)
			{
				SetCursorPos(camera.center.x, camera.center.y);
				Dialogue = 1;
				DialogueBoxOpen = false;
				GD_PrintLine1 = "";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				AllowNPC3Movement = true;
				person = "???";
				timesincelastbuttonpress = 0;
				inshop = false;
				watches--;
				Application::Cash += 100;
			}
			if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 13 && necklacescam == true)
			{
				SetCursorPos(camera.center.x, camera.center.y);
				Dialogue = 1;
				DialogueBoxOpen = false;
				GD_PrintLine1 = "";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "???";
				timesincelastbuttonpress = 0;
				AllowNPC3Movement = true;
				inshop = false;
				necklace--;
				Application::Cash += 100;
			}
			if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 14)
			{
				SetCursorPos(camera.center.x, camera.center.y);
				Dialogue = 1;
				DialogueBoxOpen = false;
				Stars++;
				GD_PrintLine1 = "";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				AllowNPC3Movement = true;
				person = "???";
				timesincelastbuttonpress = 0;
				inshop = false;
			}
			if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 15)
			{
				GD_PrintLine1 = "(Crap, I better choose my words ";
				GD_PrintLine2 = "carefully before i get caught)";
				GD_PrintLine3 = "";
				person = "       YOU";
				Dialogue = 4;
				timesincelastbuttonpress = 0;
			}
		}




		//CALL SCAM
		else if (DistanceParameter(player.getposition().x, player.getposition().z, objectlist[hb_PHONE].getposition().x, objectlist[hb_PHONE].getposition().z) <= 1)
		{
		if (timesincelastbuttonpress > 0.2 and Dialogue == 1)
		{
			GD_PrintLine1 = "Hello? To whom am I speaking to?";
			GD_PrintLine2 = "";
			GD_PrintLine3 = "";
			Dialogue = 2;
			person = "    CALLER";
			timesincelastbuttonpress = 0;
		}
		if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 2)
		{
			randomtext = rand() % 4 + 1;

			if (randomtext == 1)
			{
				GD_PrintLine1 = "1. NFT scam.";
				GD_PrintLine2 = "2. Kidnapping scam.";
				GD_PrintLine3 = "3. Bank scam.";
				Dialogue = 3;
				timesincelastbuttonpress = 0;
			}
			if (randomtext == 2)
			{
				GD_PrintLine1 = "1. Job scam";
				GD_PrintLine2 = "2. Overdue fine scam.";
				GD_PrintLine3 = "3. Donation scam.";
				Dialogue = 7;
				timesincelastbuttonpress = 0;
			}
			if (randomtext == 3)
			{
				GD_PrintLine1 = "1. Insurance scam";
				GD_PrintLine2 = "2. Bank scam.";
				GD_PrintLine3 = "3. Kidnapping scam.";
				Dialogue = 11;
				timesincelastbuttonpress = 0;
			}
			if (randomtext == 4)
			{
				GD_PrintLine1 = "1. Donation scam";
				GD_PrintLine2 = "2. Insurance scam.";
				GD_PrintLine3 = "3. NFT scam.";
				Dialogue = 15;
				timesincelastbuttonpress = 0;
			}
			person = "       YOU";
		}
		if (Application::IsKeyPressed('1') and timesincelastbuttonpress > 0.2 and Dialogue == 3)
		{
			GD_PrintLine1 = "I have some exclusive NFTs for sale at $100.";
			GD_PrintLine2 = "Would you like to buy them?";
			GD_PrintLine3 = "";
			timesincelastbuttonpress = 0;
			Dialogue = 4;
			person = "       YOU";
		}
		if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 4) {

			randomtext = rand() % 2 + 1;

			if (randomtext == 1)
			{
				GD_PrintLine1 = "No thanks, I have no interest in NFTs.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				Dialogue = 20;
				timesincelastbuttonpress = 0;
			}
			if (randomtext == 2)
			{
				GD_PrintLine1 = "Sure! I will take it.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				Dialogue = 19;
				timesincelastbuttonpress = 0;
			}
			person = "    CALLER";
		}
		if (Application::IsKeyPressed('2') and timesincelastbuttonpress > 0.2 and Dialogue == 3)
		{
			GD_PrintLine1 = "I have your child with me!";
			GD_PrintLine2 = "Give me $100 or you will never see your child ever again! ";
			GD_PrintLine3 = "";
			person = "       YOU";
			timesincelastbuttonpress = 0;
			Dialogue = 5;
		}
		if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 5)
		{
			randomtext = rand() % 2 + 1;

			if (randomtext == 1)
			{
				GD_PrintLine1 = "What are you even talking about!";
				GD_PrintLine2 = "I do not even have a child!";
				GD_PrintLine3 = "";
				Dialogue = 20;
				timesincelastbuttonpress = 0;
			}
			if (randomtext == 2)
			{
				GD_PrintLine1 = "Please spare my child!";
				GD_PrintLine2 = "I will pay you the ransom money!";
				GD_PrintLine3 = "";
				Dialogue = 19;
				timesincelastbuttonpress = 0;
			}
			person = "    CALLER";
		}
		if (Application::IsKeyPressed('3') and timesincelastbuttonpress > 0.2 and Dialogue == 3)
		{
			GD_PrintLine1 = "I am from the ABC bank, your card has been declined. ";
			GD_PrintLine2 = "You need to give me $100 or else your account is forever blocked! ";
			GD_PrintLine3 = "";
			person = "       YOU";
			timesincelastbuttonpress = 0;
			Dialogue = 6;
		}
		if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 6)
		{
			randomtext = rand() % 2 + 1;

			if (randomtext == 1)
			{
				GD_PrintLine1 = "I do not even have a bank.";
				GD_PrintLine2 = "Don't you dare try to scam me! ";
				GD_PrintLine3 = "";
				Dialogue = 20;
				timesincelastbuttonpress = 0;

			}

			if (randomtext == 2)
			{
				GD_PrintLine1 = "Oh my god! Alright alright I will give it to you.";
				GD_PrintLine2 = "Please recover my account back! ";
				GD_PrintLine3 = "";
				Dialogue = 19;
				timesincelastbuttonpress = 0;
			}
			person = "    CALLER";
		}
		if (Application::IsKeyPressed('1') and timesincelastbuttonpress > 0.2 and Dialogue == 7)
		{
			GD_PrintLine1 = "I have a job application for you! ";
			GD_PrintLine2 = "Simply pay a $100 registration fee, and you can apply for this   ";
			GD_PrintLine3 = "job that earns $8000 a month just by doing surveys.";
			Dialogue = 8;
			person = "       YOU";
			timesincelastbuttonpress = 0;
		}
		if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 8)
		{
			randomtext = rand() % 2 + 1;

			if (randomtext == 1)
			{
				GD_PrintLine1 = "Life doesn't come easy.";
				GD_PrintLine2 = "Do not try to cheat me with your obvious fake job application! ";
				GD_PrintLine3 = "";
				Dialogue = 20;
				timesincelastbuttonpress = 0;

			}
			if (randomtext == 2)
			{
				GD_PrintLine1 = "Oh! I will take it.";
				GD_PrintLine2 = "I can finallly quit my boring job. ";
				GD_PrintLine3 = "";
				Dialogue = 19;
				timesincelastbuttonpress = 0;
			}
			person = "    CALLER";
		}
		if (Application::IsKeyPressed('2') and timesincelastbuttonpress > 0.2 and Dialogue == 7)
		{
			GD_PrintLine1 = "Your house is seized for not paying your overdue fine of $100,";
			GD_PrintLine2 = "pay it and we will return your house to you.";
			GD_PrintLine3 = "";
			Dialogue = 9;
			person = "       YOU";
			timesincelastbuttonpress = 0;
		}
		if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 9)
		{
			randomtext = rand() % 2 + 1;

			if (randomtext == 1)
			{
				GD_PrintLine1 = "My house is rented and I did not even have a fine.";
				GD_PrintLine2 = "What are you talking about?";
				GD_PrintLine3 = "";
				Dialogue = 20;
				timesincelastbuttonpress = 0;
			}
			if (randomtext == 2)
			{
				GD_PrintLine1 = "I have an overdue fine?.";
				GD_PrintLine2 = "So sorry, I will pay it immediately.";
				GD_PrintLine3 = "";
				Dialogue = 19;
				timesincelastbuttonpress = 0;
			}
			person = "    CALLER";
		}
		if (Application::IsKeyPressed('3') and timesincelastbuttonpress > 0.2 and Dialogue == 7)
		{
			GD_PrintLine1 = "I am from the organization that donates ";
			GD_PrintLine2 = "food and money to african children,";
			GD_PrintLine3 = "would you like to donate $100 to them?";
			person = "       YOU";
			Dialogue = 10;
			timesincelastbuttonpress = 0;
		}
		if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 10)
		{
			randomtext = rand() % 2 + 1;

			if (randomtext == 1)
			{
				GD_PrintLine1 = "I have no interest in helping them.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				Dialogue = 20;
				timesincelastbuttonpress = 0;
			}
			if (randomtext == 2)
			{
				GD_PrintLine1 = "Sure, sure, anything to become a better person. ";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				Dialogue = 19;
				timesincelastbuttonpress = 0;
			}
			person = "    CALLER";
		}
		if (Application::IsKeyPressed('1') and timesincelastbuttonpress > 0.2 and Dialogue == 11)
		{
			GD_PrintLine1 = "I have an insurance plan for you! For only $100,";
			GD_PrintLine2 = "you can get health insurance for a lifetime no matter";
			GD_PrintLine3 = "the problem that you have!";
			person = "       YOU";
			Dialogue = 12;
			timesincelastbuttonpress = 0;
		}
		if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 12)
		{
			randomtext = rand() % 2 + 1;

			if (randomtext == 1)
			{
				GD_PrintLine1 = "Sorry but I already have insurance.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				Dialogue = 20;
				timesincelastbuttonpress = 0;
			}
			if (randomtext == 2)
			{
				GD_PrintLine1 = "That sounds amazing!.";
				GD_PrintLine2 = "I will buy it.";
				GD_PrintLine3 = "";
				Dialogue = 19;
				timesincelastbuttonpress = 0;
			}
			person = "    CALLER";
		}
		if (Application::IsKeyPressed('2') and timesincelastbuttonpress > 0.2 and Dialogue == 11)
		{
			GD_PrintLine1 = "I am from the ABC bank, your card has been declined. ";
			GD_PrintLine2 = "You need to give me $100 or else your account is forever blocked! ";
			GD_PrintLine3 = "";
			person = "       YOU";
			Dialogue = 13;
			timesincelastbuttonpress = 0;
		}
		if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 13)
		{
			randomtext = rand() % 2 + 1;

			if (randomtext == 1)
			{
				GD_PrintLine1 = "I do not even have a bank.";
				GD_PrintLine2 = "Don't you dare try to scam me! ";
				GD_PrintLine3 = "";
				Dialogue = 20;
				timesincelastbuttonpress = 0;

			}

			if (randomtext == 2)
			{
				GD_PrintLine1 = "Oh my god! Alright alright I will give it to you.";
				GD_PrintLine2 = "Please recover my account back! ";
				GD_PrintLine3 = "";
				Dialogue = 19;
				timesincelastbuttonpress = 0;
			}
			person = "    CALLER";
		}
		if (Application::IsKeyPressed('3') and timesincelastbuttonpress > 0.2 and Dialogue == 11)
		{
			GD_PrintLine1 = "I have your child with me!";
			GD_PrintLine2 = "Give me $100 or you will never see your child ever again! ";
			GD_PrintLine3 = "";
			person = "       YOU";
			Dialogue = 14;
			timesincelastbuttonpress = 0;
		}
		if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 14)
		{
			randomtext = rand() % 2 + 1;

			if (randomtext == 1)
			{
				GD_PrintLine1 = "What are you even talking about!";
				GD_PrintLine2 = "I do not even have a child!";
				GD_PrintLine3 = "";
				Dialogue = 20;
				timesincelastbuttonpress = 0;
			}
			if (randomtext == 2)
			{
				GD_PrintLine1 = "Please spare my child!";
				GD_PrintLine2 = "I will pay you the ransom money!";
				GD_PrintLine3 = "";
				Dialogue = 19;
				timesincelastbuttonpress = 0;
			}
			person = "    CALLER";
		}
		if (Application::IsKeyPressed('1') and timesincelastbuttonpress > 0.2 and Dialogue == 15)
		{
			GD_PrintLine1 = "I am from the organization that donates ";
			GD_PrintLine2 = "food and money to african children,";
			GD_PrintLine3 = "would you like to donate $100 to them?";
			person = "       YOU";
			Dialogue = 16;
			timesincelastbuttonpress = 0;
		}
		if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 16)
		{
			randomtext = rand() % 2 + 1;

			if (randomtext == 1)
			{
				GD_PrintLine1 = "I have no interest in helping them.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				Dialogue = 20;
				timesincelastbuttonpress = 0;
			}
			if (randomtext == 2)
			{
				GD_PrintLine1 = "Sure, sure, anything to become a better person. ";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				Dialogue = 19;
				timesincelastbuttonpress = 0;
			}
			person = "    CALLER";
		}
		if (Application::IsKeyPressed('2') and timesincelastbuttonpress > 0.2 and Dialogue == 15)
		{
			GD_PrintLine1 = "I have an insurance plan for you! For only $100,";
			GD_PrintLine2 = "you can get health insurance for a lifetime no matter";
			GD_PrintLine3 = "the problem that you have!";
			person = "       YOU";
			Dialogue = 17;
			timesincelastbuttonpress = 0;
		}
		if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 17)
		{
			randomtext = rand() % 2 + 1;

			if (randomtext == 1)
			{
				GD_PrintLine1 = "Sorry but I already have insurance.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				Dialogue = 20;
				timesincelastbuttonpress = 0;
			}
			if (randomtext == 2)
			{
				GD_PrintLine1 = "That sounds amazing!.";
				GD_PrintLine2 = "I will buy it.";
				GD_PrintLine3 = "";
				Dialogue = 19;
				timesincelastbuttonpress = 0;
			}
			person = "    CALLER";
		}
		if (Application::IsKeyPressed('3') and timesincelastbuttonpress > 0.2 and Dialogue == 15)
		{
			GD_PrintLine1 = "I have some exclusive NFTs for sale at $100.";
			GD_PrintLine2 = "Would you like to buy them?";
			GD_PrintLine3 = "";
			person = "       YOU";
			Dialogue = 18;
			timesincelastbuttonpress = 0;
		}
		if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 18) {
			randomtext = rand() % 2 + 1;

			if (randomtext == 1)
			{
				GD_PrintLine1 = "No thanks, I have no interest in NFTs.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				Dialogue = 20;
				timesincelastbuttonpress = 0;
			}
			if (randomtext == 2)
			{
				GD_PrintLine1 = "Sure! I will take it.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				Dialogue = 19;
				timesincelastbuttonpress = 0;
			}
			person = "    CALLER";
		}
		if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 19)
		{
			SetCursorPos(camera.center.x, camera.center.y);
			Dialogue = 1;
			DialogueBoxOpen = false;
			GD_PrintLine1 = "";
			GD_PrintLine2 = "";
			GD_PrintLine3 = "";
			person = "???";
			timesincelastbuttonpress = 0;
			inshop = false;
			Application::Cash += 100;
		}
		if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 20)
		{
			SetCursorPos(camera.center.x, camera.center.y);
			Dialogue = 1;
			DialogueBoxOpen = false;
			GD_PrintLine1 = "";
			GD_PrintLine2 = "";
			GD_PrintLine3 = "";
			person = "???";
			timesincelastbuttonpress = 0;
			inshop = false;
		}
		}


		
		//tv dialogue
		else if (interactableObjectRect(player.getposition().x, player.getposition().z, objectlist[hb_TV].getposition().x - 0.11, objectlist[hb_TV].getposition().z - 0.5, 1.9, 1) == true and Stars == 0 and camera.Position_Y == -18) 
		{
			if (timesincelastbuttonpress > 0.2 and Dialogue == 1)
			{
				GD_PrintLine1 = "BREAKING NEWS! ABC Bank has just been robbed off $1M!";
				GD_PrintLine2 = "Police investigations are currently ongoing.";
				GD_PrintLine3 = "Stay tuned for more upcoming updates!";
				Dialogue = 2;
				timesincelastbuttonpress = 0;
			}
			if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 2)
			{
				GD_PrintLine1 = "I hope I ever managed to successfully pull out a money heist.";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				Dialogue = 3;
				timesincelastbuttonpress = 0;
			}
			if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 3)
			{
				SetCursorPos(camera.center.x, camera.center.y);
				GD_PrintLine1 = "";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				Dialogue = 1;
				timesincelastbuttonpress = 0;
				DialogueBoxOpen = false;
				inshop = false;
			}
		}
		
		//Pick rings
		//Bin 1
		else if (DistanceParameter(player.getposition().x, player.getposition().z, objectlist[hb_BIN1].getposition().x, objectlist[hb_BIN1].getposition().z) <= 3)
		{
		if (finditemchance <= 4 and timesincelastbuttonpress > 0.2 and Dialogue == 1 and bin1cooldown <= 0)
		{
			if (finditemchance == 1)
			{
				GD_PrintLine1 = "Oh a ring!";
				GD_PrintLine2 = "I can try to convince someone to buy this from me.";
				GD_PrintLine3 = "";
				Dialogue = 2;
				timesincelastbuttonpress = 0;
				rings++;
			}
			if (finditemchance == 2)
			{
				GD_PrintLine1 = "Oh an old watch!";
				GD_PrintLine2 = "It's still ticking...";
				GD_PrintLine3 = "I can try to convince someone to buy this from me.";
				Dialogue = 2;
				timesincelastbuttonpress = 0;
				watches++;
			}
			if (finditemchance >= 3)
			{
				GD_PrintLine1 = "A beautiful necklace!";
				GD_PrintLine2 = "I can try to convince someone to buy this from me.";
				GD_PrintLine3 = "";
				Dialogue = 2;
				timesincelastbuttonpress = 0;
				necklace++;
			}
		}
		else if (finditemchance > 4 and timesincelastbuttonpress > 0.2 and Dialogue == 1 and bin1cooldown <= 0)
		{
			GD_PrintLine1 = "Darn, nothing useful here.";
			GD_PrintLine2 = "Better luck next time.";
			GD_PrintLine3 = "";
			Dialogue = 2;
			timesincelastbuttonpress = 0;
		}
		else if (bin1cooldown > 0 and timesincelastbuttonpress > 0.2 and Dialogue == 1)
		{
			GD_PrintLine1 = "I've just searched this bin.";
			GD_PrintLine2 = "No use going through it again.";
			GD_PrintLine3 = "";
			Dialogue = 2;
			timesincelastbuttonpress = 0;
		}
		if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 2)
		{
			SetCursorPos(camera.center.x, camera.center.y);
			GD_PrintLine1 = "";
			GD_PrintLine2 = "";
			GD_PrintLine3 = "";
			Dialogue = 1;
			timesincelastbuttonpress = 0;
			DialogueBoxOpen = false;
			inshop = false;
			bin1cooldown = 20;
		}
		}

		//Bin 2
		else if (DistanceParameter(player.getposition().x, player.getposition().z, objectlist[hb_BIN2].getposition().x, objectlist[hb_BIN2].getposition().z) <= 3)
		{
		if (finditemchance <= 4 and timesincelastbuttonpress > 0.2 and Dialogue == 1 and bin2cooldown <= 0)
		{
			if (finditemchance == 1)
			{
				GD_PrintLine1 = "Oh a ring!";
				GD_PrintLine2 = "I can try to convince someone to buy this from me.";
				GD_PrintLine3 = "";
				Dialogue = 2;
				timesincelastbuttonpress = 0;
				rings++;
			}
			if (finditemchance == 2)
			{
				GD_PrintLine1 = "Oh an old watch!";
				GD_PrintLine2 = "It's still ticking...";
				GD_PrintLine3 = "I can try to convince someone to buy this from me.";
				Dialogue = 2;
				timesincelastbuttonpress = 0;
				watches++;
			}
			if (finditemchance >= 3)
			{
				GD_PrintLine1 = "A beautiful necklace!";
				GD_PrintLine2 = "I can try to convince someone to buy this from me.";
				GD_PrintLine3 = "";
				Dialogue = 2;
				timesincelastbuttonpress = 0;
				necklace++;
			}
		}
		else if (finditemchance > 4 and timesincelastbuttonpress > 0.2 and Dialogue == 1 and bin2cooldown <= 0)
		{
			GD_PrintLine1 = "Darn, nothing useful here.";
			GD_PrintLine2 = "Better luck next time.";
			GD_PrintLine3 = "";
			Dialogue = 2;
			timesincelastbuttonpress = 0;
		}
		else if (bin2cooldown > 0 and timesincelastbuttonpress > 0.2 and Dialogue == 1)
		{
			GD_PrintLine1 = "I've just searched this bin.";
			GD_PrintLine2 = "No use going through it again.";
			GD_PrintLine3 = "";
			Dialogue = 2;
			timesincelastbuttonpress = 0;
		}
		if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 2)
		{
			SetCursorPos(camera.center.x, camera.center.y);
			GD_PrintLine1 = "";
			GD_PrintLine2 = "";
			GD_PrintLine3 = "";
			Dialogue = 1;
			timesincelastbuttonpress = 0;
			DialogueBoxOpen = false;
			inshop = false;
			bin2cooldown = 20;
		}
		}

		//Bin 3
		else if (DistanceParameter(player.getposition().x, player.getposition().z, objectlist[hb_BIN3].getposition().x, objectlist[hb_BIN3].getposition().z) <= 3)
		{
		if (finditemchance <= 4 and timesincelastbuttonpress > 0.2 and Dialogue == 1 and bin3cooldown <= 0)
		{
			if (finditemchance == 1)
			{
				GD_PrintLine1 = "Oh a ring!";
				GD_PrintLine2 = "I can try to convince someone to buy this from me.";
				GD_PrintLine3 = "";
				Dialogue = 2;
				timesincelastbuttonpress = 0;
				rings++;
			}
			if (finditemchance == 2)
			{
				GD_PrintLine1 = "Oh an old watch!";
				GD_PrintLine2 = "It's still ticking...";
				GD_PrintLine3 = "I can try to convince someone to buy this from me.";
				Dialogue = 2;
				timesincelastbuttonpress = 0;
				watches++;
			}
			if (finditemchance >= 3)
			{
				GD_PrintLine1 = "A beautiful necklace!";
				GD_PrintLine2 = "I can try to convince someone to buy this from me.";
				GD_PrintLine3 = "";
				Dialogue = 2;
				timesincelastbuttonpress = 0;
				necklace++;
			}
		}
		else if (finditemchance > 4 and timesincelastbuttonpress > 0.2 and Dialogue == 1 and bin3cooldown <= 0)
		{
			GD_PrintLine1 = "Darn, nothing useful here.";
			GD_PrintLine2 = "Better luck next time.";
			GD_PrintLine3 = "";
			Dialogue = 2;
			timesincelastbuttonpress = 0;
		}
		else if (bin3cooldown > 0 and timesincelastbuttonpress > 0.2 and Dialogue == 1)
		{
			GD_PrintLine1 = "I've just searched this bin.";
			GD_PrintLine2 = "No use going through it again.";
			GD_PrintLine3 = "";
			Dialogue = 2;
			timesincelastbuttonpress = 0;
		}
		if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 2)
		{
			SetCursorPos(camera.center.x, camera.center.y);
			GD_PrintLine1 = "";
			GD_PrintLine2 = "";
			GD_PrintLine3 = "";
			Dialogue = 1;
			timesincelastbuttonpress = 0;
			DialogueBoxOpen = false;
			inshop = false;
			bin3cooldown = 20;
		}
		}

		//Bin 4
		else if (DistanceParameter(player.getposition().x, player.getposition().z, objectlist[hb_BIN4].getposition().x, objectlist[hb_BIN4].getposition().z) <= 3)
		{
		if (finditemchance <= 4 and timesincelastbuttonpress > 0.2 and Dialogue == 1 and bin4cooldown <= 0)
		{
			if (finditemchance == 1)
			{
				GD_PrintLine1 = "Oh a ring!";
				GD_PrintLine2 = "I can try to convince someone to buy this from me.";
				GD_PrintLine3 = "";
				Dialogue = 2;
				timesincelastbuttonpress = 0;
				rings++;
			}
			if (finditemchance == 2)
			{
				GD_PrintLine1 = "Oh an old watch!";
				GD_PrintLine2 = "It's still ticking...";
				GD_PrintLine3 = "I can try to convince someone to buy this from me.";
				Dialogue = 2;
				timesincelastbuttonpress = 0;
				watches++;
			}
			if (finditemchance >= 3)
			{
				GD_PrintLine1 = "A beautiful necklace!";
				GD_PrintLine2 = "I can try to convince someone to buy this from me.";
				GD_PrintLine3 = "";
				Dialogue = 2;
				timesincelastbuttonpress = 0;
				necklace++;
			}
		}
		else if (finditemchance > 4 and timesincelastbuttonpress > 0.2 and Dialogue == 1 and bin4cooldown <= 0)
		{
			GD_PrintLine1 = "Darn, nothing useful here.";
			GD_PrintLine2 = "Better luck next time.";
			GD_PrintLine3 = "";
			Dialogue = 2;
			timesincelastbuttonpress = 0;
		}
		else if (bin4cooldown > 0 and timesincelastbuttonpress > 0.2 and Dialogue == 1)
		{
			GD_PrintLine1 = "I've just searched this bin.";
			GD_PrintLine2 = "No use going through it again.";
			GD_PrintLine3 = "";
			Dialogue = 2;
			timesincelastbuttonpress = 0;
		}
		if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 2)
		{
			SetCursorPos(camera.center.x, camera.center.y);
			GD_PrintLine1 = "";
			GD_PrintLine2 = "";
			GD_PrintLine3 = "";
			Dialogue = 1;
			timesincelastbuttonpress = 0;
			DialogueBoxOpen = false;
			inshop = false;
			bin4cooldown = 20;
		}
		}

		//Bin 5
		else if (DistanceParameter(player.getposition().x, player.getposition().z, objectlist[hb_BIN5].getposition().x, objectlist[hb_BIN5].getposition().z) <= 3)
		{
		if (finditemchance <= 4 and timesincelastbuttonpress > 0.2 and Dialogue == 1 and bin5cooldown <= 0)
		{
			if (finditemchance == 1)
			{
				GD_PrintLine1 = " Oh a ring!";
				GD_PrintLine2 = " I can try to convince someone to buy this from me.";
				GD_PrintLine3 = "";
				Dialogue = 2;
				timesincelastbuttonpress = 0;
				rings++;
			}
			if (finditemchance == 2)
			{
				GD_PrintLine1 = " Oh an old watch!";
				GD_PrintLine2 = " It's still ticking...";
				GD_PrintLine3 = " I can try to convince someone to buy this from me.";
				Dialogue = 2;
				timesincelastbuttonpress = 0;
				watches++;
			}
			if (finditemchance >= 3)
			{
				GD_PrintLine1 = " A beautiful necklace!";
				GD_PrintLine2 = " I can try to convince someone to buy this from me.";
				GD_PrintLine3 = "";
				Dialogue = 2;
				timesincelastbuttonpress = 0;
				necklace++;
			}
		}
		else if (finditemchance > 4 and timesincelastbuttonpress > 0.2 and Dialogue == 1 and bin5cooldown <= 0)
		{
			GD_PrintLine1 = "Darn, nothing useful here.";
			GD_PrintLine2 = "Better luck next time.";
			GD_PrintLine3 = "";
			Dialogue = 2;
			timesincelastbuttonpress = 0;
		}
		else if (bin5cooldown > 0 and timesincelastbuttonpress > 0.2 and Dialogue == 1)
		{
			GD_PrintLine1 = "I've just searched this bin.";
			GD_PrintLine2 = "No use going through it again.";
			GD_PrintLine3 = "";
			Dialogue = 2;
			timesincelastbuttonpress = 0;
		}
		if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 2)
		{
			SetCursorPos(camera.center.x, camera.center.y);
			GD_PrintLine1 = "";
			GD_PrintLine2 = "";
			GD_PrintLine3 = "";
			Dialogue = 1;
			timesincelastbuttonpress = 0;
			DialogueBoxOpen = false;
			inshop = false;
			bin5cooldown = 20;
		}

		}

		//Bin 6
		else if (DistanceParameter(player.getposition().x, player.getposition().z, objectlist[hb_BIN6].getposition().x, objectlist[hb_BIN6].getposition().z) <= 3)
		{
		if (finditemchance <= 4 and timesincelastbuttonpress > 0.2 and Dialogue == 1 and bin6cooldown <= 0)
		{
			if (finditemchance == 1)
			{
				GD_PrintLine1 = " Oh a ring!";
				GD_PrintLine2 = " I can try to convince someone to buy this from me.";
				GD_PrintLine3 = "";
				Dialogue = 2;
				timesincelastbuttonpress = 0;
				rings++;
			}
			if (finditemchance == 2)
			{
				GD_PrintLine1 = " Oh an old watch!";
				GD_PrintLine2 = " It's still ticking...";
				GD_PrintLine3 = " I can try to convince someone to buy this from me.";
				Dialogue = 2;
				timesincelastbuttonpress = 0;
				watches++;
			}
			if (finditemchance >= 3)
			{
				GD_PrintLine1 = " A beautiful necklace!";
				GD_PrintLine2 = " I can try to convince someone to buy this from me.";
				GD_PrintLine3 = "";
				Dialogue = 2;
				timesincelastbuttonpress = 0;
				necklace++;
			}
		}
		else if (finditemchance > 4 and timesincelastbuttonpress > 0.2 and Dialogue == 1 and bin6cooldown <= 0)
		{
			GD_PrintLine1 = "Darn, nothing useful here.";
			GD_PrintLine2 = "Better luck next time.";
			GD_PrintLine3 = "";
			Dialogue = 2;
			timesincelastbuttonpress = 0;
		}
		else if (bin6cooldown > 0 and timesincelastbuttonpress > 0.2 and Dialogue == 1)
		{
			GD_PrintLine1 = "I've just searched this bin.";
			GD_PrintLine2 = "No use going through it again.";
			GD_PrintLine3 = "";
			Dialogue = 2;
			timesincelastbuttonpress = 0;
		}
		if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 2)
		{
			SetCursorPos(camera.center.x, camera.center.y);
			GD_PrintLine1 = "";
			GD_PrintLine2 = "";
			GD_PrintLine3 = "";
			Dialogue = 1;
			timesincelastbuttonpress = 0;
			DialogueBoxOpen = false;
			inshop = false;
			bin6cooldown = 20;
		}

		}

		//Bin 7
		else if (DistanceParameter(player.getposition().x, player.getposition().z, objectlist[hb_BIN7].getposition().x, objectlist[hb_BIN7].getposition().z) <= 3)
		{
		if (finditemchance <= 4 and timesincelastbuttonpress > 0.2 and Dialogue == 1 and bin7cooldown <= 0)
		{
			if (finditemchance == 1)
			{
				GD_PrintLine1 = " Oh a ring!";
				GD_PrintLine2 = " I can try to convince someone to buy this from me.";
				GD_PrintLine3 = "";
				Dialogue = 2;
				timesincelastbuttonpress = 0;
				rings++;
			}
			if (finditemchance == 2)
			{
				GD_PrintLine1 = " Oh an old watch!";
				GD_PrintLine2 = " It's still ticking...";
				GD_PrintLine3 = " I can try to convince someone to buy this from me.";
				Dialogue = 2;
				timesincelastbuttonpress = 0;
				watches++;
			}
			if (finditemchance >= 3)
			{
				GD_PrintLine1 = " A beautiful necklace!";
				GD_PrintLine2 = " I can try to convince someone to buy this from me.";
				GD_PrintLine3 = "";
				Dialogue = 2;
				timesincelastbuttonpress = 0;
				necklace++;
			}
		}
		else if (finditemchance > 4 and timesincelastbuttonpress > 0.2 and Dialogue == 1 and bin7cooldown <= 0)
		{
			GD_PrintLine1 = "Darn, nothing useful here.";
			GD_PrintLine2 = "Better luck next time.";
			GD_PrintLine3 = "";
			Dialogue = 2;
			timesincelastbuttonpress = 0;
		}
		else if (bin7cooldown > 0 and timesincelastbuttonpress > 0.2 and Dialogue == 1)
		{
			GD_PrintLine1 = "I've just searched this bin.";
			GD_PrintLine2 = "No use going through it again.";
			GD_PrintLine3 = "";
			Dialogue = 2;
			timesincelastbuttonpress = 0;
		}
		if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 2)
		{
			SetCursorPos(camera.center.x, camera.center.y);
			GD_PrintLine1 = "";
			GD_PrintLine2 = "";
			GD_PrintLine3 = "";
			Dialogue = 1;
			timesincelastbuttonpress = 0;
			DialogueBoxOpen = false;
			inshop = false;
			bin7cooldown = 20;
		}
	}


		// SHOPKEEPER
		else if (interactableObjectRect(player.getposition().x, player.getposition().z, objectlist[hb_SHOPSELLTABLE].getposition().x, objectlist[hb_SHOPSELLTABLE].getposition().z + 0.5, 1.5, 1) == true and camera.position.y == -18)
		{
			if (timesincelastbuttonpress > 0.2 and Dialogue == 1)
			{
				GD_PrintLine1 = "Welcome to the pawn shop!";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "SHOPKEEPER";
				Dialogue = 2;
				timesincelastbuttonpress = 0;
			}
			else if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 2) {
				GD_PrintLine1 = "What would you like to do today?";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "SHOPKEEPER";
				Dialogue = 3;
				timesincelastbuttonpress = 0;
			}
			if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Dialogue == 3)
			{
				ShowCursor(true);
				Dialogue = 1;
				DialogueBoxOpen = false;
				GD_PrintLine1 = "";
				GD_PrintLine2 = "";
				GD_PrintLine3 = "";
				person = "???";
				timesincelastbuttonpress = 0;
				isShopOpen = true;
			}

		}
	}

	if (isShopOpen == true) {
		POINT initmousepos;
		mousepos = &initmousepos;//init mousepos
		GetCursorPos(mousepos); //get mouse position
		mousepos->x -= rect.left;
		mousepos->y -= rect.top;
		static bool bLButtonState = false;
		if (!bLButtonState && Application::IsMousePressed(0))
		{
			bLButtonState = true;
		}
		else if (bLButtonState && !Application::IsMousePressed(0))
		{
			bLButtonState = false;
			if (mousepos->x > 105 && mousepos->x < 142 && mousepos->y > 190 && mousepos->y < 228) {
				SetCursorPos(camera.center.x, camera.center.y);
				inshop = false;
				isShopOpen = false;
				DialogueBoxOpen = false;
				ShowCursor(false);
				timesincelastbuttonpress = 0;
				ShopUI_Status = false;
			}

			if (mousepos->x > 138 && mousepos->x < 261 && mousepos->y > 456 && mousepos->y < 498) {
				if (Application::Cash >= 500) {
					Application::Cash -= 500;
					isSufficient = true;
					item1++;
				}
				else {
					isSufficient = false;
				}
				ShopUI_Status = true;
				timesincelastbuttonpress = 0;
			}
			if (mousepos->x > 341 && mousepos->x < 464 && mousepos->y > 456 && mousepos->y < 498) {
				if (Application::Cash >= 500) {
					Application::Cash -= 500;
					isSufficient = true;
					item2++;
				}
				else {
					isSufficient = false;
				}
				ShopUI_Status = true;
				timesincelastbuttonpress = 0;
			}
			if (mousepos->x > 544 && mousepos->x < 666 && mousepos->y > 456 && mousepos->y < 498) {
				if (Application::Cash >= 800) {
					Application::Cash -= 800;
					item3++;
					isSufficient = true;
				}
				else {
					isSufficient = false;
				}
				ShopUI_Status = true;
				timesincelastbuttonpress = 0;
			}

		}
	}


	//Usage of Power ups
	static bool bLButtonState_item1 = false;
	static bool bLButtonState_item2 = false;
	static bool bLButtonState_item3 = false;
	if (!bLButtonState_item1 && Application::IsKeyPressed('1'))
	{
		bLButtonState_item1 = true;
	}
	else if (bLButtonState_item1 && !Application::IsKeyPressed('1'))
	{
		bLButtonState_item1 = false;
		if (DialogueBoxOpen == false and isShopOpen == false and item1 > 0) {
			item1--;
			item1_active = true;
		}
	}
	if (item1_active == true) {
		if (item1Activetime <= 3) {
			SizeofStamina = 40;
		}
		if (item1Activetime >= 3) {
			item1_active = false;
			item1Activetime = 0;
		}
		item1Activetime += dt;
	}




	if (!bLButtonState_item2 && Application::IsKeyPressed('2'))
	{
		bLButtonState_item2 = true;
	}
	else if (bLButtonState_item2 && !Application::IsKeyPressed('2'))
	{
		bLButtonState_item2 = false;
		if (DialogueBoxOpen == false and isShopOpen == false and item2 > 0) {
			item2--;
			item2_active = true;
		}
	}
	if (item2_active == true) {
		if (item2Activetime >= 3) {
			item2_active = false;
			item2Activetime = 0;
		}
		item2Activetime += dt;
	}





	if (!bLButtonState_item3 && Application::IsKeyPressed('3'))
	{
		bLButtonState_item3 = true;
	}
	else if (bLButtonState_item3 && !Application::IsKeyPressed('3'))
	{
		bLButtonState_item3 = false;
		if (DialogueBoxOpen == false and isShopOpen == false and item3 > 0 and Stars != 0) {
			Stars = 0;
			item3--;
			for (unsigned idx = 0; idx < polices.size(); idx++)
			{
				polices[idx]->DespawnPolice();
				objectlist[hb_POLICE1 + idx].setrenderobject(false);
			}
		}
	}
	//-----------------------------

	if (Application::Cash <= 0) {
		Application::Cash = 0;
	}

	if (DistanceParameter(player.getposition().x, player.getposition().z, objectlist[hb_NPC1].getposition().x, objectlist[hb_NPC1].getposition().z) <= 4 and Stars == 0) {
		timesincelastbuttonpress += dt;
	}
	else if (DistanceParameter(player.getposition().x, player.getposition().z, objectlist[hb_PHONE].getposition().x, objectlist[hb_PHONE].getposition().z) <= 1 and Stars == 0) {
		timesincelastbuttonpress += dt;
	}
	else if (DistanceParameter(player.getposition().x, player.getposition().z, objectlist[hb_NPC2].getposition().x, objectlist[hb_NPC2].getposition().z) <= 4 and Stars == 0 and (rings >= 1 or watches >= 1 or necklace >= 1)) {
		timesincelastbuttonpress += dt;
	}
	else if (DistanceParameter(player.getposition().x, player.getposition().z, objectlist[hb_NPC3].getposition().x, objectlist[hb_NPC3].getposition().z) <= 4 and Stars == 0 and (rings >= 1 or watches >= 1 or necklace >= 1)) {
		timesincelastbuttonpress += dt;
	}
	else if (DistanceParameter(player.getposition().x, player.getposition().z, objectlist[hb_BIN1].getposition().x, objectlist[hb_BIN1].getposition().z) <= 3 and Stars == 0)
	{
		timesincelastbuttonpress += dt;
	}
	else if (DistanceParameter(player.getposition().x, player.getposition().z, objectlist[hb_BIN2].getposition().x, objectlist[hb_BIN2].getposition().z) <= 3 and Stars == 0)
	{
		timesincelastbuttonpress += dt;
	}
	else if (DistanceParameter(player.getposition().x, player.getposition().z, objectlist[hb_BIN3].getposition().x, objectlist[hb_BIN3].getposition().z) <= 3 and Stars == 0)
	{
		timesincelastbuttonpress += dt;
	}
	else if (DistanceParameter(player.getposition().x, player.getposition().z, objectlist[hb_BIN4].getposition().x, objectlist[hb_BIN4].getposition().z) <= 3 and Stars == 0)
	{
		timesincelastbuttonpress += dt;
	}
	else if (DistanceParameter(player.getposition().x, player.getposition().z, objectlist[hb_BIN5].getposition().x, objectlist[hb_BIN5].getposition().z) <= 3 and Stars == 0)
	{
		timesincelastbuttonpress += dt;
	}
	else if (DistanceParameter(player.getposition().x, player.getposition().z, objectlist[hb_BIN6].getposition().x, objectlist[hb_BIN6].getposition().z) <= 3 and Stars == 0)
	{
		timesincelastbuttonpress += dt;
	}
	else if (DistanceParameter(player.getposition().x, player.getposition().z, objectlist[hb_BIN7].getposition().x, objectlist[hb_BIN7].getposition().z) <= 3 and Stars == 0)
	{
		timesincelastbuttonpress += dt;
	}
	else if (interactableObjectRect(player.getposition().x, player.getposition().z, objectlist[hb_TV].getposition().x - 0.11, objectlist[hb_TV].getposition().z - 0.5, 1.9, 1) == true and Stars == 0) {
		timesincelastbuttonpress += dt;
	}
	else if (interactableObjectRect(player.getposition().x, player.getposition().z, objectlist[hb_HOUSE18].getposition().x - 1.8, objectlist[hb_HOUSE18].getposition().z + 3, 1.5, 1) == true) {
		timesincelastbuttonpress += dt;
	}
	else if (interactableObjectRect(player.getposition().x, player.getposition().z, objectlist[hb_HOUSE18].getposition().x - 1.8, objectlist[hb_HOUSE18].getposition().z + 2, 1.5, 1) == true and camera.position.y == -18) {
		timesincelastbuttonpress += dt;
	}
	else if (interactableObjectRect(player.getposition().x, player.getposition().z + 0.4, objectlist[hb_WallDesk].getposition().x, objectlist[hb_WallDesk].getposition().z, 1, 0.6) == true and camera.position.y == -18) {
		timesincelastbuttonpress += dt;
	}
	else if (interactableObjectRect(player.getposition().x, player.getposition().z, objectlist[hb_HOUSE14].getposition().x - 0, objectlist[hb_HOUSE14].getposition().z + 5, 1.5, 1) == true and camera.position.y != -18)
	{
		timesincelastbuttonpress += dt;
	}
	else if (interactableObjectRect(player.getposition().x, player.getposition().z, objectlist[hb_HOUSE14].getposition().x + 0, objectlist[hb_HOUSE14].getposition().z + 4.5, 1.5, 1.5) == true and camera.position.y == -18)
	{
		timesincelastbuttonpress += dt;
	}
	else if (interactableObjectRect(player.getposition().x, player.getposition().z, objectlist[hb_SHOPSELLTABLE].getposition().x, objectlist[hb_SHOPSELLTABLE].getposition().z + 0.5, 1.5, 1) == true and camera.position.y == -18) {
		timesincelastbuttonpress += dt;
	}
	else if (interactableObjectRect(player.getposition().x, player.getposition().z, objectlist[hb_SHOPPROPRADIO].getposition().x, objectlist[hb_SHOPPROPRADIO].getposition().z + 3, 1, 1) == true and camera.position.y == -18) {
		timesincelastbuttonpress += dt;
	}
	else {
		timesincelastbuttonpress = 0;
	}





	// Stamina Consumption
	static float timer_regen = 0;
	if (Application::IsKeyPressed(VK_SHIFT) and (Application::IsKeyPressed('W') or Application::IsKeyPressed('A') or Application::IsKeyPressed('S') or Application::IsKeyPressed('D')) and SizeofStamina > 0 and DialogueBoxOpen == false) {
		if (not(Application::IsKeyPressed('W') and Application::IsKeyPressed('S')) and not(Application::IsKeyPressed('A') and Application::IsKeyPressed('D'))) {
			if (item2_active == true) {
				camera.ZOOM_SPEED = 10.f;
			}
			else {
				camera.ZOOM_SPEED = 8.f;
			}
			SizeofStamina -= (float)(6 * dt);
			timer_regen = 0;
		}
	}
	else {
		if (item2_active == true) {
			if (SizeofStamina > 0) {
				camera.ZOOM_SPEED = 8.f;
			}
			else {
				camera.ZOOM_SPEED = 6.f;
			}
		}
		else {
			if (SizeofStamina > 0) {
				camera.ZOOM_SPEED = 6.f;
			}
			else {
				camera.ZOOM_SPEED = 4.f;
			}
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

	// Wanted timer


	if (Stars != 0) {
		timer_blinking += dt;
		timer_wanted += dt;
		timer_Wanted_Chase += dt;
	}
	if (Stars >= 5) {
		Stars = 5;
	}


	timesincelastpunch += dt;
	rotateSkybox += dt;
	rotateSunandMoon += dt;
}

void SP2::UpdateCredits(double dt)
{
	rotateAngle += 10 * dt;
	if (rotateAngle > 225 || GetAsyncKeyState(0x1B))
	{
		rotateAngle = 0;
		gamestate = gs_mainmenu;
	}
}

void SP2::Render()
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

void SP2::RenderMainmenu()
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


	if (Application::isMuted == false) {
		RenderMeshOnScreen(meshList[GEO_SOUND_UNMUTE], Vector3(5, 5, 5), 0, 77, 3);
	}
	else {
		RenderMeshOnScreen(meshList[GEO_SOUND_MUTE], Vector3(5, 5, 5), 0, 77, 3);
	}

	//Render mouse pos
	//UIstringoutput.str("");
	//UIstringoutput.precision(4);
	//UIstringoutput << "Mousepos X:" << mousepos->x - rect.left << "Y:" << mousepos->y - rect.top;
	//RenderTextOnScreen(meshList[GEO_TEXT], UIstringoutput.str(), Color(0, 1, 0), 2, 0, 0);
	//UIstringoutput.str("");

	//RenderMeshOnScreen(meshList[GEO_UIBACKGROUND], imagedimensions, Vector3(0, 0, 0), imagepos.x, imagepos.y);
}

void SP2::RenderENV()
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

	if (light[2].type == Light::LIGHT_DIRECTIONAL)
	{
		Vector3 lightDir(light[2].position.x, light[2].position.y, light[2].position.z);
		Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
		glUniform3fv(m_parameters[U_LIGHT2_POSITION], 1, &lightDirection_cameraspace.x);
	}
	else if (light[2].type == Light::LIGHT_SPOT)
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[2].position;
		glUniform3fv(m_parameters[U_LIGHT2_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * light[2].spotDirection;
		glUniform3fv(m_parameters[U_LIGHT2_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[2].position;
		glUniform3fv(m_parameters[U_LIGHT2_POSITION], 1, &lightPosition_cameraspace.x);
	}

	//light 4
	if (light[3].type == Light::LIGHT_DIRECTIONAL)
	{
		Vector3 lightDir(light[3].position.x, light[3].position.y, light[3].position.z);
		Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
		glUniform3fv(m_parameters[U_LIGHT3_POSITION], 1, &lightDirection_cameraspace.x);
	}
	else if (light[3].type == Light::LIGHT_SPOT)
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[3].position;
		glUniform3fv(m_parameters[U_LIGHT3_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * light[3].spotDirection;
		glUniform3fv(m_parameters[U_LIGHT3_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[3].position;
		glUniform3fv(m_parameters[U_LIGHT3_POSITION], 1, &lightPosition_cameraspace.x);
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

	modelStack.PushMatrix();
	modelStack.Rotate(-90, 1, 0, 0);
	modelStack.Scale(100, 100, 100);
	RenderMesh(meshList[GEO_QUAD], true);
	modelStack.PopMatrix();

	//House marker
	{
		modelStack.PushMatrix();
		modelStack.Translate(light[2].position.x, light[2].position.y + 2.5, light[2].position.z);
		modelStack.Rotate(ArrowRotate - 90, 0, 1, 0);
		{
			modelStack.PushMatrix();
			modelStack.Translate(0, -0.5, -0.15);
			modelStack.Rotate(-45, 1, 0, 0);
			modelStack.Scale(0.2, 0.4, 0.2);
			RenderMesh(meshList[GEO_CUBE1], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(0, -0.5, 0.15);
			modelStack.Rotate(45, 1, 0, 0);
			modelStack.Scale(0.2, 0.4, 0.2);
			RenderMesh(meshList[GEO_CUBE1], true);
			modelStack.PopMatrix();
		}
		modelStack.Scale(0.2, 0.7, 0.2);
		RenderMesh(meshList[GEO_CUBE1], true);
		modelStack.PopMatrix();
	}

	//shop marker
	{
		modelStack.PushMatrix();
		modelStack.Translate(light[3].position.x, light[3].position.y + 2.5, light[3].position.z);
		modelStack.Rotate(ArrowRotate - 90, 0, 1, 0);
		{
			modelStack.PushMatrix();
			modelStack.Translate(0, -0.5, -0.15);
			modelStack.Rotate(-45, 1, 0, 0);
			modelStack.Scale(0.2, 0.4, 0.2);
			RenderMesh(meshList[GEO_CUBE1], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(0, -0.5, 0.15);
			modelStack.Rotate(45, 1, 0, 0);
			modelStack.Scale(0.2, 0.4, 0.2);
			RenderMesh(meshList[GEO_CUBE1], true);
			modelStack.PopMatrix();
		}
		modelStack.Scale(0.2, 0.7, 0.2);
		RenderMesh(meshList[GEO_CUBE1], true);
		modelStack.PopMatrix();
	}

	{
		modelStack.PushMatrix();
		modelStack.Translate(-6, 1, 2.5);
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(0.7, 0.7, 0.7);
		RenderMesh(meshList[GEO_GARBAGE], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(12, 1, -14);
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(0.7, 0.7, 0.7);
		RenderMesh(meshList[GEO_GARBAGE], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-6, 1, -17);
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(0.7, 0.7, 0.7);
		RenderMesh(meshList[GEO_GARBAGE], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(15.5, 1, 3);
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(0.7, 0.7, 0.7);
		RenderMesh(meshList[GEO_GARBAGE], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(22.5, 1, -12.5);
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(0.7, 0.7, 0.7);
		RenderMesh(meshList[GEO_GARBAGE], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-19.7, 1, 0);
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(0.7, 0.7, 0.7);
		RenderMesh(meshList[GEO_GARBAGE], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(16.7, 1, 18);
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(0.7, 0.7, 0.7);
		RenderMesh(meshList[GEO_GARBAGE], true);
		modelStack.PopMatrix();

	}
	

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

	//render roads + pavement
	{
		for (int idx = 0; idx < 50; idx++)
		{
			modelStack.PushMatrix();
			modelStack.Translate(49 - (idx * 2), 0.001, 25);
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(49 - (idx * 2), 0.001, 23);
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(49 - (idx * 2), 0.001, 21);
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(49 - (idx * 2), 0.001, 19);
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();




			modelStack.PushMatrix();
			modelStack.Translate(49 - (idx * 2), 0.001, 17);
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(49 - (idx * 2), 0.001, 15);
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(49 - (idx * 2), 0.001, 13);
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_ROAD], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(49 - (idx * 2), 0.001, 11);
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_ROAD], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(49 - (idx * 2), 0.001, 9);
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_ROAD], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(49 - (idx * 2), 0.001, 7);
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();
		}
		for (int idx = 0; idx < 25; idx++)
		{
			modelStack.PushMatrix();
			modelStack.Translate(-49, 0.0009, 6 - (idx * 2));
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(-47, 0.0009, 6 - (idx * 2));
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(-45, 0.0009, 6 - (idx * 2));
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(-43, 0.0009, 6 - (idx * 2));
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(-41, 0.0009, 6 - (idx * 2));
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(-39, 0.0009, 6 - (idx * 2));
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();


			modelStack.PushMatrix();
			modelStack.Translate(-37, 0.0009, 6 - (idx * 2));
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(-35, 0.0009, 6 - (idx * 2));
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(-33, 0.0009, 6 - (idx * 2));
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(-31, 0.0009, 6 - (idx * 2));
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(-29, 0.0009, 6 - (idx * 2));
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(-27, 0.0009, 6 - (idx * 2));
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(-25, 0.0009, 6 - (idx * 2));
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(-23, 0.0009, 6 - (idx * 2));
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(-21, 0.0009, 6 - (idx * 2));
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(-19, 0.0009, 6 - (idx * 2));
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(-17, 0.0009, 6 - (idx * 2));
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(-15, 0.0009, 6 - (idx * 2));
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(-13, 0.0009, 6 - (idx * 2));
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(-11, 0.0009, 6 - (idx * 2));
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(17, 0.0009, 6 - (idx * 2));
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(19, 0.0009, 6 - (idx * 2));
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(19, 0.0009, 6 - (idx * 2));
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(21, 0.0009, 6 - (idx * 2));
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(23, 0.0009, 6 - (idx * 2));
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(25, 0.0009, 6 - (idx * 2));
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(27, 0.0009, 6 - (idx * 2));
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(29, 0.0009, 6 - (idx * 2));
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(31, 0.0009, 6 - (idx * 2));
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(33, 0.0009, 6 - (idx * 2));
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(35, 0.0009, 6 - (idx * 2));
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(37, 0.0009, 6 - (idx * 2));
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(39, 0.0009, 6 - (idx * 2));
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(41, 0.0009, 6 - (idx * 2));
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(43, 0.0009, 6 - (idx * 2));
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(45, 0.0009, 6 - (idx * 2));
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(47, 0.0009, 6 - (idx * 2));
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(49, 0.0009, 6 - (idx * 2));
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();
		}
		for (int idx = 0; idx < 3; idx++)
		{
			modelStack.PushMatrix();
			modelStack.Translate((idx * 2) - 9, 0.009, -42);
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate((idx * 2) - 9, 0.009, -40);
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate((idx * 2) - 9, 0.009, -38);
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate((idx * 2) - 9, 0.009, -36);
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate((idx * 2) - 9, 0.009, -34);
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate((idx * 2) - 9, 0.009, -32);
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();

		}
		for (int idx = 0; idx < 13; idx++)
		{
			modelStack.PushMatrix();
			modelStack.Translate((idx * 2) - 9, 0.009, -30);
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(2, 2, 2);
			RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
			modelStack.PopMatrix();
		}

		for (int idx = 0; idx < 25; idx++)
		{
			for (int i = 0; i < 4; i++)
			{
				modelStack.PushMatrix();
				modelStack.Translate(31 - (idx * 2), 0.0009, 25 - (i * 2));
				modelStack.Rotate(-90, 1, 0, 0);
				modelStack.Scale(2, 2, 2);
				RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
				modelStack.PopMatrix();
			}
		}

	}

	//render city centre
	for (int idx = 0; idx < 18; idx++)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-9, 0.0009, 6 - (idx * 2.001));
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CITY_CENTRE_FLOOR], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-7, 0.0009, 6 - (idx * 2.001));
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CITY_CENTRE_FLOOR], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-5, 0.0009, 6 - (idx * 2.001));
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CITY_CENTRE_FLOOR], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-3, 0.0009, 6 - (idx * 2.001));
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CITY_CENTRE_FLOOR], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-1, 0.0009, 6 - (idx * 2.001));
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CITY_CENTRE_FLOOR], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(1, 0.0009, 6 - (idx * 2.001));
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CITY_CENTRE_FLOOR], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(3, 0.0009, 6 - (idx * 2.001));
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CITY_CENTRE_FLOOR], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(5, 0.0009, 6 - (idx * 2.001));
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CITY_CENTRE_FLOOR], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(7, 0.0009, 6 - (idx * 2.001));
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CITY_CENTRE_FLOOR], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(9, 0.0009, 6 - (idx * 2.001));
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CITY_CENTRE_FLOOR], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(11, 0.0009, 6 - (idx * 2.001));
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CITY_CENTRE_FLOOR], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(13, 0.0009, 6 - (idx * 2.001));
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CITY_CENTRE_FLOOR], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(15, 0.0009, 6 - (idx * 2.001));
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CITY_CENTRE_FLOOR], true);
		modelStack.PopMatrix();
	}

	//Interaction
	if ((DistanceParameter(player.getposition().x, player.getposition().z, objectlist[hb_NPC1].getposition().x, objectlist[hb_NPC1].getposition().z) <= 4 or (DistanceParameter(player.getposition().x, player.getposition().z, objectlist[hb_NPC2].getposition().x, objectlist[hb_NPC2].getposition().z) <= 4 or DistanceParameter(player.getposition().x, player.getposition().z, objectlist[hb_NPC3].getposition().x, objectlist[hb_NPC3].getposition().z) <= 4) and (rings >= 1 or watches >= 1 or necklace >= 1)) and Stars == 0) {
		if (DialogueBoxOpen == false) {
			RenderTextOnScreen(meshList[GEO_TEXT], "Press 'E' to interact", Color(1, 1, 1), 4, 28, 14);
		}
		if (Application::IsKeyPressed('E') and DialogueBoxOpen == false and timesincelastbuttonpress > 0.2) {
			DialogueBoxOpen = true;
			timesincelastbuttonpress = 0;
			inshop = true;
		}
		if (DialogueBoxOpen == true) {
			RenderMeshOnScreen(meshList[GEO_DIALOGUEUI], Vector3(60, 15, 1), 0, 40, 10);
			RenderTextOnScreen(meshList[GEO_TEXT], person, Color(1, 1, 1), 2.7, 23.5, 13.5);
			RenderTextOnScreen(meshList[GEO_TEXT], GD_PrintLine1, Color(0, 0, 0), 3, 14, 10);
			RenderTextOnScreen(meshList[GEO_TEXT], GD_PrintLine2, Color(0, 0, 0), 3, 14, 7.5);
			RenderTextOnScreen(meshList[GEO_TEXT], GD_PrintLine3, Color(0, 0, 0), 3, 14, 5);
			RenderTextOnScreen(meshList[GEO_TEXT], "Press 'E' to speed up", Color(1, 1, 1), 3, 30.2, 0.8);
		}
	}

	if (interactableObjectRect(player.getposition().x, player.getposition().z, objectlist[hb_TV].getposition().x - 0.11, objectlist[hb_TV].getposition().z - 0.5, 1.9, 1) == true and Stars == 0 and camera.position.y == -18)
	{
		if (DialogueBoxOpen == false)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "Press 'E' to listen", Color(1, 1, 1), 4, 30, 14);
		}
		if (Application::IsKeyPressed('E') and DialogueBoxOpen == false and timesincelastbuttonpress > 0.2)
		{
			DialogueBoxOpen = true;
			timesincelastbuttonpress = 0;
			inshop = true;
		}
		if (DialogueBoxOpen == true)
		{
			RenderMeshOnScreen(meshList[GEO_DIALOGUEUI], Vector3(60, 15, 1), 0, 40, 10);
			RenderTextOnScreen(meshList[GEO_TEXT], "TELEVISION", Color(1, 1, 1), 2.7, 23.5, 13.5);
			RenderTextOnScreen(meshList[GEO_TEXT], GD_PrintLine1, Color(0, 0, 0), 3, 14, 10);
			RenderTextOnScreen(meshList[GEO_TEXT], GD_PrintLine2, Color(0, 0, 0), 3, 14, 7.5);
			RenderTextOnScreen(meshList[GEO_TEXT], GD_PrintLine3, Color(0, 0, 0), 3, 14, 5);
			RenderTextOnScreen(meshList[GEO_TEXT], "Press 'E' to speed up", Color(1, 1, 1), 3, 30.2, 0.8);
		}

	}

	if (DistanceParameter(player.getposition().x, player.getposition().z, objectlist[hb_PHONE].getposition().x, objectlist[hb_PHONE].getposition().z) <= 1)
	{
		if (DialogueBoxOpen == false)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "Press 'E' to initiate call scam", Color(1, 1, 1), 4, 26, 14);
		}
		if (Application::IsKeyPressed('E') and DialogueBoxOpen == false and timesincelastbuttonpress > 0.2) {
			DialogueBoxOpen = true;
			timesincelastbuttonpress = 0;
			inshop = true;
		}
		if (DialogueBoxOpen == true) {
			RenderMeshOnScreen(meshList[GEO_DIALOGUEUI], Vector3(60, 15, 1), 0, 40, 10);
			RenderTextOnScreen(meshList[GEO_TEXT], person, Color(1, 1, 1), 2.7, 23.5, 13.5);
			RenderTextOnScreen(meshList[GEO_TEXT], GD_PrintLine1, Color(0, 0, 0), 3, 14, 10);
			RenderTextOnScreen(meshList[GEO_TEXT], GD_PrintLine2, Color(0, 0, 0), 3, 14, 7.5);
			RenderTextOnScreen(meshList[GEO_TEXT], GD_PrintLine3, Color(0, 0, 0), 3, 14, 5);
		}
	}


	static bool isClick_Wanted = false;
	static float timer_click_Wanted = 0;
	if (interactableObjectRect(player.getposition().x, player.getposition().z, objectlist[hb_HOUSE18].getposition().x - 1.8, objectlist[hb_HOUSE18].getposition().z + 3, 1.5, 1) == true and camera.position.y != -18) {
		if (isClick_Wanted == false) {
			RenderTextOnScreen(meshList[GEO_TEXT], "Press 'E' to enter house", Color(1, 1, 1), 4, 26, 14);
		}
		if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Stars == 0) {
			camera.Position_Y = -18;
			camera.setposition(Vector3(17.5, -18, 7));
			timesincelastbuttonpress = 0;
		}
		if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Stars != 0) {
			isClick_Wanted = true;
			timesincelastbuttonpress = 0;
		}

		if (isClick_Wanted == true) {
			RenderTextOnScreen(meshList[GEO_TEXT], "You are WANTED! You cannot enter.", Color(1, 1, 1), 4, 22, 14);
			if (timer_click_Wanted >= 1) {
				timer_click_Wanted = 0;
				isClick_Wanted = false;
			}
			timer_click_Wanted += 0.01;
		}
	}
	else {
		if (Stars != 0) {
			timer_click_Wanted = 0;
			isClick_Wanted = false;
		}
	}


	if (interactableObjectRect(player.getposition().x, player.getposition().z, objectlist[hb_HOUSE18].getposition().x - 1.8, objectlist[hb_HOUSE18].getposition().z + 3, 1.5, 1) == true and camera.position.y == -18) {
		RenderTextOnScreen(meshList[GEO_TEXT], "Press 'E' to leave house", Color(1, 1, 1), 4, 26, 14);
		if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2) {
			camera.Position_Y = 2;
			camera.setposition(Vector3(17.5, camera.Position_Y, 8));
			timesincelastbuttonpress = 0;
		}
	}

	static bool isClick_Minigame = false;
	static float timer_click_Minigame = 0;
	// Laptop
	if (interactableObjectRect(player.getposition().x, player.getposition().z + 0.4, objectlist[hb_WallDesk].getposition().x, objectlist[hb_WallDesk].getposition().z, 1, 0.6) == true and camera.position.y == -18) {
		if (isClick_Minigame == false) {
			RenderTextOnScreen(meshList[GEO_TEXT], "Entry Cost: $100", Color(1, 1, 1), 4, 31, 18);
			RenderTextOnScreen(meshList[GEO_TEXT], "Press 'E' to play minigame", Color(1, 1, 1), 4, 26, 14);
		}
		if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and (int)Application::Minigame_timer == 0 and Application::Cash >= 100) {
			Application::Cash -= 100;
			Application::ChangeScene(Application::gs_jigglypuffgame);
			ShowCursor(true);
			timesincelastbuttonpress = 0;
		}
		if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and ((int)Application::Minigame_timer != 0 or Application::Cash < 100)) {
			isClick_Minigame = true;
			timesincelastbuttonpress = 0;
		}

		if (isClick_Minigame == true) {
			if ((int)Application::Minigame_timer != 0) {
				RenderTextOnScreen(meshList[GEO_TEXT], "Minigame Cooldown in: " + to_string((int)Application::Minigame_timer) + "s", Color(1, 1, 1), 4, 26, 14);
			}
			if (Application::Cash < 100 and (int)Application::Minigame_timer == 0) {
				RenderTextOnScreen(meshList[GEO_TEXT], "Sorry, insufficient cash! Go get a Job" , Color(1, 1, 1), 4, 20, 14);
			}
			if (timer_click_Minigame >= 1) {
				timer_click_Minigame = 0;
				isClick_Minigame = false;
			}
			timer_click_Minigame += 0.01;
		}
	}
	else {
		timer_click_Minigame = 0;
		isClick_Minigame = false;
	}




	static bool isClick_Minigame_Shooting = false;
	static float timer_click_Minigame_Shooting = 0;
	// Shop Minigame
	if (interactableObjectRect(player.getposition().x, player.getposition().z, objectlist[hb_SHOPPROPRADIO].getposition().x, objectlist[hb_SHOPPROPRADIO].getposition().z + 3, 1, 1) == true and camera.position.y == -18) {
		if (isClick_Minigame_Shooting == false) {
			RenderTextOnScreen(meshList[GEO_TEXT], "Entry Cost: $50", Color(1, 1, 1), 4, 31, 18);
			RenderTextOnScreen(meshList[GEO_TEXT], "Press 'E' to play minigame", Color(1, 1, 1), 4, 26, 14);
		}
		if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Application::Cash >= 50) {
			Application::Cash -= 50;
			Application::ChangeScene(Application::gs_shootingminigame);
			ShowCursor(true);
			timesincelastbuttonpress = 0;
		}
		if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Application::Cash < 50) {
			isClick_Minigame_Shooting = true;
			timesincelastbuttonpress = 0;
		}

		if (isClick_Minigame_Shooting == true) {
			if (Application::Cash < 50) {
				RenderTextOnScreen(meshList[GEO_TEXT], "Sorry, insufficient cash! Go get a Job", Color(1, 1, 1), 4, 20, 14);
			}
			if (timer_click_Minigame_Shooting >= 1) {
				timer_click_Minigame_Shooting = 0;
				isClick_Minigame_Shooting = false;
			}
			timer_click_Minigame_Shooting += 0.01;
		}
	}
	else {
		timer_click_Minigame_Shooting = 0;
		isClick_Minigame_Shooting = false;
	}


	//pick junk
	if (DistanceParameter(player.getposition().x, player.getposition().z, objectlist[hb_BIN1].getposition().x, objectlist[hb_BIN1].getposition().z) <= 3 && Stars == 0 and camera.position.y != -18)
	{
		if (DialogueBoxOpen == false)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "Press 'E' to pick junk", Color(1, 1, 1), 4, 28, 14);
		}
		if (Application::IsKeyPressed('E') and DialogueBoxOpen == false and timesincelastbuttonpress > 0.2) {
			DialogueBoxOpen = true;
			timesincelastbuttonpress = 0;
			inshop = true;
			finditemchance = rand() % 10 + 1;
		}
		if (DialogueBoxOpen == true) {
			RenderMeshOnScreen(meshList[GEO_DIALOGUEUI], Vector3(60, 15, 1), 0, 40, 10);
			RenderTextOnScreen(meshList[GEO_TEXT], "       YOU", Color(1, 1, 1), 2.7, 23.5, 13.5);
			RenderTextOnScreen(meshList[GEO_TEXT], GD_PrintLine1, Color(0, 0, 0), 3, 14, 10);
			RenderTextOnScreen(meshList[GEO_TEXT], GD_PrintLine2, Color(0, 0, 0), 3, 14, 7.5);
			RenderTextOnScreen(meshList[GEO_TEXT], GD_PrintLine3, Color(0, 0, 0), 3, 14, 5);
		}
	}
	if (DistanceParameter(player.getposition().x, player.getposition().z, objectlist[hb_BIN2].getposition().x, objectlist[hb_BIN2].getposition().z) <= 3 && Stars == 0 and camera.position.y != -18)
	{
		if (DialogueBoxOpen == false)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "Press 'E' to pick junk", Color(1, 1, 1), 4, 28, 14);
		}
		if (Application::IsKeyPressed('E') and DialogueBoxOpen == false and timesincelastbuttonpress > 0.2) {
			DialogueBoxOpen = true;
			timesincelastbuttonpress = 0;
			inshop = true;
			finditemchance = rand() % 10 + 1;
		}
		if (DialogueBoxOpen == true) {
			RenderMeshOnScreen(meshList[GEO_DIALOGUEUI], Vector3(60, 15, 1), 0, 40, 10);
			RenderTextOnScreen(meshList[GEO_TEXT], "       YOU", Color(1, 1, 1), 2.7, 23.5, 13.5);
			RenderTextOnScreen(meshList[GEO_TEXT], GD_PrintLine1, Color(0, 0, 0), 3, 14, 10);
			RenderTextOnScreen(meshList[GEO_TEXT], GD_PrintLine2, Color(0, 0, 0), 3, 14, 7.5);
			RenderTextOnScreen(meshList[GEO_TEXT], GD_PrintLine3, Color(0, 0, 0), 3, 14, 5);
		}
	}
	if (DistanceParameter(player.getposition().x, player.getposition().z, objectlist[hb_BIN3].getposition().x, objectlist[hb_BIN3].getposition().z) <= 3 && Stars == 0 and camera.position.y != -18)
	{
		if (DialogueBoxOpen == false)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "Press 'E' to pick junk", Color(1, 1, 1), 4, 28, 14);
		}
		if (Application::IsKeyPressed('E') and DialogueBoxOpen == false and timesincelastbuttonpress > 0.2) {
			DialogueBoxOpen = true;
			timesincelastbuttonpress = 0;
			inshop = true;
			finditemchance = rand() % 10 + 1;
		}
		if (DialogueBoxOpen == true) {
			RenderMeshOnScreen(meshList[GEO_DIALOGUEUI], Vector3(60, 15, 1), 0, 40, 10);
			RenderTextOnScreen(meshList[GEO_TEXT], "       YOU", Color(1, 1, 1), 2.7, 23.5, 13.5);
			RenderTextOnScreen(meshList[GEO_TEXT], GD_PrintLine1, Color(0, 0, 0), 3, 14, 10);
			RenderTextOnScreen(meshList[GEO_TEXT], GD_PrintLine2, Color(0, 0, 0), 3, 14, 7.5);
			RenderTextOnScreen(meshList[GEO_TEXT], GD_PrintLine3, Color(0, 0, 0), 3, 14, 5);
		}
	}
	if (DistanceParameter(player.getposition().x, player.getposition().z, objectlist[hb_BIN4].getposition().x, objectlist[hb_BIN4].getposition().z) <= 3 && Stars == 0 and camera.position.y != -18)
	{
		if (DialogueBoxOpen == false)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "Press 'E' to pick junk", Color(1, 1, 1), 4, 28, 14);
		}
		if (Application::IsKeyPressed('E') and DialogueBoxOpen == false and timesincelastbuttonpress > 0.2) {
			DialogueBoxOpen = true;
			timesincelastbuttonpress = 0;
			inshop = true;
			finditemchance = rand() % 10 + 1;
		}
		if (DialogueBoxOpen == true)
		{
			RenderMeshOnScreen(meshList[GEO_DIALOGUEUI], Vector3(60, 15, 1), 0, 40, 10);
			RenderTextOnScreen(meshList[GEO_TEXT], "       YOU", Color(1, 1, 1), 2.7, 23.5, 13.5);
			RenderTextOnScreen(meshList[GEO_TEXT], GD_PrintLine1, Color(0, 0, 0), 3, 14, 10);
			RenderTextOnScreen(meshList[GEO_TEXT], GD_PrintLine2, Color(0, 0, 0), 3, 14, 7.5);
			RenderTextOnScreen(meshList[GEO_TEXT], GD_PrintLine3, Color(0, 0, 0), 3, 14, 5);
		}

	}
	if (DistanceParameter(player.getposition().x, player.getposition().z, objectlist[hb_BIN5].getposition().x, objectlist[hb_BIN5].getposition().z) <= 3 && Stars == 0 and camera.position.y != -18)
	{
		if (DialogueBoxOpen == false)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "Press 'E' to pick junk", Color(1, 1, 1), 4, 28, 14);
		}
		if (Application::IsKeyPressed('E') and DialogueBoxOpen == false and timesincelastbuttonpress > 0.2) {
			DialogueBoxOpen = true;
			timesincelastbuttonpress = 0;
			inshop = true;
			finditemchance = rand() % 10 + 1;
		}
		if (DialogueBoxOpen == true) {
			RenderMeshOnScreen(meshList[GEO_DIALOGUEUI], Vector3(60, 15, 1), 0, 40, 10);
			RenderTextOnScreen(meshList[GEO_TEXT], "       YOU", Color(1, 1, 1), 2.7, 23.5, 13.5);
			RenderTextOnScreen(meshList[GEO_TEXT], GD_PrintLine1, Color(0, 0, 0), 3, 14, 10);
			RenderTextOnScreen(meshList[GEO_TEXT], GD_PrintLine2, Color(0, 0, 0), 3, 14, 7.5);
			RenderTextOnScreen(meshList[GEO_TEXT], GD_PrintLine3, Color(0, 0, 0), 3, 14, 5);
		}
	}
	if (DistanceParameter(player.getposition().x, player.getposition().z, objectlist[hb_BIN6].getposition().x, objectlist[hb_BIN6].getposition().z) <= 3 && Stars == 0 and camera.position.y != -18)
	{
		if (DialogueBoxOpen == false)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "Press 'E' to pick junk", Color(1, 1, 1), 4, 28, 14);
		}
		if (Application::IsKeyPressed('E') and DialogueBoxOpen == false and timesincelastbuttonpress > 0.2) {
			DialogueBoxOpen = true;
			timesincelastbuttonpress = 0;
			inshop = true;
			finditemchance = rand() % 10 + 1;
		}
		if (DialogueBoxOpen == true) {
			RenderMeshOnScreen(meshList[GEO_DIALOGUEUI], Vector3(60, 15, 1), 0, 40, 10);
			RenderTextOnScreen(meshList[GEO_TEXT], "       YOU", Color(1, 1, 1), 2.7, 23.5, 13.5);
			RenderTextOnScreen(meshList[GEO_TEXT], GD_PrintLine1, Color(0, 0, 0), 3, 14, 10);
			RenderTextOnScreen(meshList[GEO_TEXT], GD_PrintLine2, Color(0, 0, 0), 3, 14, 7.5);
			RenderTextOnScreen(meshList[GEO_TEXT], GD_PrintLine3, Color(0, 0, 0), 3, 14, 5);
		}
	}
	if (DistanceParameter(player.getposition().x, player.getposition().z, objectlist[hb_BIN7].getposition().x, objectlist[hb_BIN7].getposition().z) <= 3 && Stars == 0 and camera.position.y != -18)
	{
		if (DialogueBoxOpen == false)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "Press 'E' to pick junk", Color(1, 1, 1), 4, 28, 14);
		}
		if (Application::IsKeyPressed('E') and DialogueBoxOpen == false and timesincelastbuttonpress > 0.2) {
			DialogueBoxOpen = true;
			timesincelastbuttonpress = 0;
			inshop = true;
			finditemchance = rand() % 10 + 1;
		}
		if (DialogueBoxOpen == true) {
			RenderMeshOnScreen(meshList[GEO_DIALOGUEUI], Vector3(60, 15, 1), 0, 40, 10);
			RenderTextOnScreen(meshList[GEO_TEXT], "     YOU", Color(1, 1, 1), 2.7, 23.5, 13.5);
			RenderTextOnScreen(meshList[GEO_TEXT], GD_PrintLine1, Color(0, 0, 0), 3, 14, 10);
			RenderTextOnScreen(meshList[GEO_TEXT], GD_PrintLine2, Color(0, 0, 0), 3, 14, 7.5);
			RenderTextOnScreen(meshList[GEO_TEXT], GD_PrintLine3, Color(0, 0, 0), 3, 14, 5);
		}
	}


	// Shop
	static bool isClick_Wanted_Shop = false;
	static float timer_click_Wanted_Shop = 0;
	if (interactableObjectRect(player.getposition().x, player.getposition().z, objectlist[hb_HOUSE14].getposition().x, objectlist[hb_HOUSE14].getposition().z + 5, 1.5, 1) == true and camera.position.y != -18) {
		if (isClick_Wanted_Shop == false) {
			RenderTextOnScreen(meshList[GEO_TEXT], "Press 'E' to enter shop", Color(1, 1, 1), 4, 28, 14);
		}
		if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Stars == 0) {
			camera.Position_Y = -18;
			camera.setposition(Vector3(3.5, camera.Position_Y, -28.7));
			timesincelastbuttonpress = 0;
		}
		if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2 and Stars != 0) {
			isClick_Wanted_Shop = true;
			timesincelastbuttonpress = 0;
		}

		if (isClick_Wanted_Shop == true) {
			RenderTextOnScreen(meshList[GEO_TEXT], "You are WANTED! You cannot enter.", Color(1, 1, 1), 4, 22, 14);
			if (timer_click_Wanted_Shop >= 1) {
				timer_click_Wanted_Shop = 0;
				isClick_Wanted_Shop = false;
			}
			timer_click_Wanted += 0.01;
		}
	}
	else {
		if (Stars != 0) {
			timer_click_Wanted_Shop = 0;
			isClick_Wanted_Shop = false;
		}
	}


	if (interactableObjectRect(player.getposition().x, player.getposition().z, objectlist[hb_HOUSE14].getposition().x, objectlist[hb_HOUSE14].getposition().z + 4.5, 1.5, 1) == true and camera.position.y == -18)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Press 'E' to leave shop", Color(1, 1, 1), 4, 28, 14);
		if (Application::IsKeyPressed('E') and timesincelastbuttonpress > 0.2)
		{
			camera.Position_Y = 2;
			camera.setposition(Vector3(3.5, camera.Position_Y, -27));
			timesincelastbuttonpress = 0;
		}
	}



	//buy items 
	if (interactableObjectRect(player.getposition().x, player.getposition().z, objectlist[hb_SHOPSELLTABLE].getposition().x, objectlist[hb_SHOPSELLTABLE].getposition().z + 0.5, 1.5, 1) == true and camera.position.y == -18)
	{
		if (DialogueBoxOpen == false and isShopOpen == false)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "Press 'E' to buy items", Color(1, 1, 1), 4, 28, 14);
		}
		if (Application::IsKeyPressed('E') and DialogueBoxOpen == false and timesincelastbuttonpress > 0.2)
		{
			DialogueBoxOpen = true;
			timesincelastbuttonpress = 0;
			inshop = true;
		}
		if (isShopOpen) {
			RenderMeshOnScreen(meshList[GEO_SHOPUI], Vector3(70, 55, 1), 0, 40, 25);
			RenderMeshOnScreen(meshList[GEO_CASH], Vector3(5, 5, 5), 0, 54, 42.8);
			RenderTextOnScreen(meshList[GEO_TEXT], "$" + to_string(Application::Cash), Color(0, 0.9, 0), 4, 57, 41);

			if (ShopUI_Status == true and timesincelastbuttonpress < 3) {
				if (isSufficient == false) {
					RenderTextOnScreen(meshList[GEO_TEXT], "Insufficient Cash", Color(1, 0, 0), 3, 32, 8.5);
				}
				else {
					RenderTextOnScreen(meshList[GEO_TEXT], "Purchased Successfully", Color(0, 0.7, 0), 3, 29, 8.5);
				}
			}

			if (ShopUI_Status == true and timesincelastbuttonpress >= 2) {
				ShopUI_Status = false;
			}

		}
		if (DialogueBoxOpen == true and isShopOpen == false)
		{
			RenderMeshOnScreen(meshList[GEO_DIALOGUEUI], Vector3(60, 15, 1), 0, 40, 10);
			RenderTextOnScreen(meshList[GEO_TEXT], person, Color(1, 1, 1), 2.7, 23.5, 13.5);
			RenderTextOnScreen(meshList[GEO_TEXT], GD_PrintLine1, Color(0, 0, 0), 3, 14, 10);
			RenderTextOnScreen(meshList[GEO_TEXT], GD_PrintLine2, Color(0, 0, 0), 3, 14, 7.5);
			RenderTextOnScreen(meshList[GEO_TEXT], GD_PrintLine3, Color(0, 0, 0), 3, 14, 5);
			RenderTextOnScreen(meshList[GEO_TEXT], "Try to scam people.", Color(1, 1, 1), 3, 30.2, 0.8);
		}
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



	//render UI

	//Stamina bar
	if (!gameover) {
		if (SizeofStamina < 40 and !inshop) {
			RenderMeshOnScreen(meshList[GEO_STAMINA_BLACK], Vector3(40, 3, 1), 0, 40, 10);
			RenderMeshOnScreen(meshList[GEO_STAMINA_BAR], Vector3(SizeofStamina, 3, 1), 0, 40, 10);
		}
		if (SizeofStamina <= 0 and !inshop) {
			RenderTextOnScreen(meshList[GEO_TEXT], "YOU NEED REST!", Color(1, 1, 1), 3, 34, 8.5);
		}
	}


	//Stars
	if (Stars == 1) {
		RenderMeshOnScreen(meshList[GEO_STAR1], Vector3(18, 3.5, 1), 0, 67, 56);
		if (timer_blinking >= 0.5 and timer_blinking <= 1) {
			RenderMeshOnScreen(meshList[GEO_STAR1_Grey], Vector3(18, 3.5, 1), 0, 67, 56);
		}
		if (timer_blinking >= 1) {
			timer_blinking = 0;
		}
	}
	else if (Stars == 2) {
		RenderMeshOnScreen(meshList[GEO_STAR2], Vector3(18, 3.5, 1), 0, 67, 56);
		if (timer_blinking >= 0.5 and timer_blinking <= 1) {
			RenderMeshOnScreen(meshList[GEO_STAR2_Grey], Vector3(18, 3.5, 1), 0, 67, 56);
		}
		if (timer_blinking >= 1) {
			timer_blinking = 0;
		}
	}
	else if (Stars == 3) {
		RenderMeshOnScreen(meshList[GEO_STAR3], Vector3(18, 3.5, 1), 0, 67, 56);
		if (timer_blinking >= 0.5 and timer_blinking <= 1) {
			RenderMeshOnScreen(meshList[GEO_STAR3_Grey], Vector3(18, 3.5, 1), 0, 67, 56);
		}
		if (timer_blinking >= 1) {
			timer_blinking = 0;
		}
	}
	else if (Stars == 4) {
		RenderMeshOnScreen(meshList[GEO_STAR4], Vector3(18, 3.5, 1), 0, 67, 56);
		if (timer_blinking >= 0.5 and timer_blinking <= 1) {
			RenderMeshOnScreen(meshList[GEO_STAR4_Grey], Vector3(18, 3.5, 1), 0, 67, 56);
		}
		if (timer_blinking >= 1) {
			timer_blinking = 0;
		}
	}
	else if (Stars == 5) {
		RenderMeshOnScreen(meshList[GEO_STAR5], Vector3(18, 3.5, 1), 0, 67, 56);
		if (timer_blinking >= 0.5 and timer_blinking <= 1) {
			RenderMeshOnScreen(meshList[GEO_STAR5_Grey], Vector3(18, 3.5, 1), 0, 67, 56);
		}
		if (timer_blinking >= 1) {
			timer_blinking = 0;
		}
	}

	if (timer_wanted >= 50 and Stars != 0) {
		timer_wanted = 0;
		timer_blinking = 0;
		Stars = 0;
	}
	else {
		if (Application::isMuted == false) {
			if (Application::ismusicPlaying == false and Stars != 0) {
				PlaySound(TEXT("GTAVParachuting.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
				Application::ismusicPlaying = true;
			}
			if (Stars == 0 and Application::ismusicPlaying == true) {
				PlaySound(NULL, 0, 0);
				Application::ismusicPlaying = false;
			}
		}
		else {
			Application::ismusicPlaying = false;
			PlaySound(NULL, 0, 0);
		}
	}



	//Inv
	if (!inshop or isShopOpen == true and !gameover)
	{
		RenderMeshOnScreen(meshList[GEO_HOTBAR], Vector3(30, 20, 30), 0, 39.3, 4);
		RenderTextOnScreen(meshList[GEO_TEXT], "1", Color(1, 1, 1), 2, 34.7, 4.3);
		RenderTextOnScreen(meshList[GEO_TEXT], "2", Color(1, 1, 1), 2, 41.8, 4.3);
		RenderTextOnScreen(meshList[GEO_TEXT], "3", Color(1, 1, 1), 2, 48.8, 4.3);

		RenderTextOnScreen(meshList[GEO_TEXT], "x" + to_string(item1), Color(1, 1, 1), 2, 34.3, 0.7);
		RenderTextOnScreen(meshList[GEO_TEXT], "x" + to_string(item2), Color(1, 1, 1), 2, 41.3, 0.7);
		RenderTextOnScreen(meshList[GEO_TEXT], "x" + to_string(item3), Color(1, 1, 1), 2, 48.3, 0.7);
	}


	// Show money rings necklace and watches
	if (inventoryopen and !gameover)
	{
		if (!inshop)
		{
			//Cash 
			RenderMeshOnScreen(meshList[GEO_CASH], Vector3(5, 5, 5), 0, 3, 55);
			RenderTextOnScreen(meshList[GEO_TEXT], "$" + to_string(Application::Cash), Color(0, 0.9, 0), 4, 6, 53);

			//Junk
			RenderMeshOnScreen(meshList[GEO_RING], Vector3(5, 5, 5), 0, 3, 48);
			RenderTextOnScreen(meshList[GEO_TEXT], "Rings: " + to_string(rings), Color(1, 1, 1), 4, 6, 46);
			RenderMeshOnScreen(meshList[GEO_WATCH], Vector3(5, 5, 5), 0, 3, 43);
			RenderTextOnScreen(meshList[GEO_TEXT], "Watches: " + to_string(watches), Color(1, 1, 1), 4, 6, 41);
			RenderMeshOnScreen(meshList[GEO_NECKLACE], Vector3(5, 5, 5), 0, 3, 38);
			RenderTextOnScreen(meshList[GEO_TEXT], "Necklaces: " + to_string(necklace), Color(1, 1, 1), 4, 6, 36);
		}
	}
	else
	{
		if (!inshop and !gameover)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "Press Tab to check inventory", Color(0.8, 0.8, 0.8), 3, 2, 34);
		}

	}

	if (controlopen and !gameover)
	{
		if (!inshop)
		{
			RenderMeshOnScreen(meshList[GEO_CONTROLSCREEN], Vector3(60, 60, 60), 0, 40, 28.5);
		}
	}
	else
	{
		if (!inshop and !gameover)
		{
			RenderMeshOnScreen(meshList[GEO_CONTROLSTAB], Vector3(23, 11, 23), 0, 9, 0);
		}
	}

	// Gameover
	if (gameover)
	{
		RenderMeshOnScreen(meshList[GEO_GAMEOVERBACKGROUND], Vector3(100, 100, 1), 0, 40, 30);
		RenderMeshOnScreen(meshList[GEO_GAMEOVER], Vector3(60, 10, 0), 0, 40, 50);
		RenderMeshOnScreen(meshList[GEO_GAMEOVERBUTTON], Vector3(21, 9, 0), 0, 40, 10);
		RenderTextOnScreen(meshList[GEO_TEXT], "Total earnings: $" + to_string(Application::Cash), Color(1, 1, 1), 5, 25, 39);
	}
	//modelStack.PushMatrix();
	//modelStack.Translate(light[0].position.x, light[0].position.y, light[0].position.z);
	//RenderMesh(meshList[GEO_LIGHTBALL], false);
	//modelStack.PopMatrix();
}


void SP2::RenderCredits()
{
	//roll credits
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	RenderTextOnScreen(meshList[GEO_TEXT], "Scam Life", Color(1, 1, 1), 4, 0, 0 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT], "Developed", Color(1, 1, 1), 4, 0, -10 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT], "by:", Color(1, 1, 1), 4, 0, -15 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT], "Randall Soh", Color(1, 1, 1), 4, 0, -20 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT], "Khor Ming Jie", Color(1, 1, 1), 4, 0, -25 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT], "Luke Tan", Color(1, 1, 1), 4, 0, -30 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT], "Sebastian Tan", Color(1, 1, 1), 4, 0, -35 + rotateAngle);
	//RenderTextOnScreen(meshList[GEO_TEXT], "ground grass texture", Color(1, 1, 1), 4, 0, -45 + rotateAngle);
	//RenderTextOnScreen(meshList[GEO_TEXT], "by: Simoon Murray", Color(1, 1, 1), 3, 0, -50 + rotateAngle);
	//RenderTextOnScreen(meshList[GEO_TEXT], "https://www.deviantart.com", Color(1, 1, 1), 3, 0, -55 + rotateAngle);
	//RenderTextOnScreen(meshList[GEO_TEXT], "/simoonmurray/art", Color(1, 1, 1), 3, 0, -60 + rotateAngle);
	//RenderTextOnScreen(meshList[GEO_TEXT], "/Green-Grass-Texture-01-155704377", Color(1, 1, 1), 3, 0, -65 + rotateAngle);
	//RenderTextOnScreen(meshList[GEO_TEXT], "Skybox By:", Color(1, 1, 1), 4, 0, -75 + rotateAngle);
	//RenderTextOnScreen(meshList[GEO_TEXT], "Emil Persson, aka Humus.", Color(1, 1, 1), 4, 0, -80 + rotateAngle);
	//RenderTextOnScreen(meshList[GEO_TEXT], "http://www.humus.name", Color(1, 1, 1), 4, 0, -85 + rotateAngle);
	//RenderTextOnScreen(meshList[GEO_TEXT], "mudkip avatar from:", Color(1, 1, 1), 4, 0, -95 + rotateAngle);
	//RenderTextOnScreen(meshList[GEO_TEXT], "https://your-guide-to-pokemon", Color(1, 1, 1), 4, 0, -100 + rotateAngle);
	//RenderTextOnScreen(meshList[GEO_TEXT], ".fandom.com/wiki/Chris%27s_Mudkip", Color(1, 1, 1), 4, 0, -105 + rotateAngle);
								
	//RenderTextOnScreen(meshList[GEO_TEXT], "Mudkip model by:", Color(1, 1, 1), 4, 0, -115 + rotateAngle);
	//RenderTextOnScreen(meshList[GEO_TEXT], "Ming Jie", Color(1, 1, 1), 4, 0, -120 + rotateAngle);
								
	//RenderTextOnScreen(meshList[GEO_TEXT], "Water drop in UI from:", Color(1, 1, 1), 4, 0, -130 + rotateAngle);
	//RenderTextOnScreen(meshList[GEO_TEXT], "https://uxwing.com/water-drop-icon/", Color(1, 1, 1), 4, 0, -135 + rotateAngle);
								
	RenderTextOnScreen(meshList[GEO_TEXT], "other assets are from", Color(1, 1, 1), 4, 0, -45 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT], "Kenny", Color(1, 1, 1), 4, 0, -50 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT], "www.kenney.nl", Color(1, 1, 1), 4, 0, -55 + rotateAngle);
}

void SP2::Exit()
{
	// Cleanup VBO here
	delete navigationmesh;
	for (unsigned idx = 0; idx < polices.size(); idx++)
	{
		delete polices[idx];
	}
	polices.clear();

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

void SP2::RenderMesh(Mesh* mesh, bool enableLight)
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

void SP2::RenderSkybox(Vector3 skyboxoffset)
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

void SP2::RenderText(Mesh* mesh, std::string text, Color color)
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

void SP2::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
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


void SP2::RenderMeshOnScreen(Mesh* mesh, Vector3 size, float rotate, float x, float y)
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
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

SP2::SP2()
{
}

SP2::~SP2()
{
}

float DistanceParameter(float EnemyXPos, float EnemyZPos, float OriginalPosX, float OriginalPosZ) {
	float squareofxz = (pow(EnemyXPos - OriginalPosX, 2) + pow(EnemyZPos - OriginalPosZ, 2));
	float magnitude = sqrt(squareofxz);

	return magnitude;
}

Vector3 SP2::CollisionCircleRect(float cx, float cy, float radius, float rx, float ry, float rw, float rh) {
	//assume all y = 0 since y not needed
	Vector3 endPos = Vector3(cx, 0, cy);
	//Work out nearest point to future player position, around perimeter of cell rectangle. 
	//We can test the distance to this point to see if we have collided. 
	Vector3 nearestPoint;
	nearestPoint.x = Math::Clamp(cx, rx - 0.5f * rw, rx + 0.5f * rw);
	nearestPoint.y = 0;
	nearestPoint.z = Math::Clamp(cy, ry - 0.5f * rh, ry + 0.5f * rh);
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
		// Statically resolve the collision
		endPos = endPos - rayToNearest.Normalized() * overlap;
	}

	return endPos;
}


Vector3 SP2::PlayerCollision(unsigned count, CameraTest camera) {
	if (count == count) {
		return CollisionCircleRect(camera.position.x, camera.position.z, 1.5, objectlist[count].getposition().x, objectlist[count].getposition().z, objectlist[count].gethitboxcollisionsize().x, objectlist[count].gethitboxcollisionsize().z);
	}
	else
	{
		return Vector3(camera.position.x, 0, camera.position.z); //wont trigger
	}
}

void SP2::Init()
{
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
		// Set background color to black
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

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

		//light 5
		m_parameters[U_LIGHT4_POSITION] = glGetUniformLocation(m_programID, "lights[4].position_cameraspace");
		m_parameters[U_LIGHT4_COLOR] = glGetUniformLocation(m_programID, "lights[4].color");
		m_parameters[U_LIGHT4_POWER] = glGetUniformLocation(m_programID, "lights[4].power");
		m_parameters[U_LIGHT4_KC] = glGetUniformLocation(m_programID, "lights[4].kC");
		m_parameters[U_LIGHT4_KL] = glGetUniformLocation(m_programID, "lights[4].kL");
		m_parameters[U_LIGHT4_KQ] = glGetUniformLocation(m_programID, "lights[4].kQ");
		m_parameters[U_LIGHT4_TYPE] = glGetUniformLocation(m_programID, "lights[4].type");
		m_parameters[U_LIGHT4_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[4].spotDirection");
		m_parameters[U_LIGHT4_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[4].cosCutoff");
		m_parameters[U_LIGHT4_COSINNER] = glGetUniformLocation(m_programID, "lights[4].cosInner");
		m_parameters[U_LIGHT4_EXPONENT] = glGetUniformLocation(m_programID, "lights[4].exponent");

		//light 5
		m_parameters[U_LIGHT5_POSITION] = glGetUniformLocation(m_programID, "lights[5].position_cameraspace");
		m_parameters[U_LIGHT5_COLOR] = glGetUniformLocation(m_programID, "lights[5].color");
		m_parameters[U_LIGHT5_POWER] = glGetUniformLocation(m_programID, "lights[5].power");
		m_parameters[U_LIGHT5_KC] = glGetUniformLocation(m_programID, "lights[5].kC");
		m_parameters[U_LIGHT5_KL] = glGetUniformLocation(m_programID, "lights[5].kL");
		m_parameters[U_LIGHT5_KQ] = glGetUniformLocation(m_programID, "lights[5].kQ");
		m_parameters[U_LIGHT5_TYPE] = glGetUniformLocation(m_programID, "lights[5].type");
		m_parameters[U_LIGHT5_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[5].spotDirection");
		m_parameters[U_LIGHT5_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[5].cosCutoff");
		m_parameters[U_LIGHT5_COSINNER] = glGetUniformLocation(m_programID, "lights[5].cosInner");
		m_parameters[U_LIGHT5_EXPONENT] = glGetUniformLocation(m_programID, "lights[5].exponent");

		//light 6
		m_parameters[U_LIGHT5_POSITION] = glGetUniformLocation(m_programID, "lights[5].position_cameraspace");
		m_parameters[U_LIGHT5_COLOR] = glGetUniformLocation(m_programID, "lights[5].color");
		m_parameters[U_LIGHT5_POWER] = glGetUniformLocation(m_programID, "lights[5].power");
		m_parameters[U_LIGHT5_KC] = glGetUniformLocation(m_programID, "lights[5].kC");
		m_parameters[U_LIGHT5_KL] = glGetUniformLocation(m_programID, "lights[5].kL");
		m_parameters[U_LIGHT5_KQ] = glGetUniformLocation(m_programID, "lights[5].kQ");
		m_parameters[U_LIGHT5_TYPE] = glGetUniformLocation(m_programID, "lights[5].type");
		m_parameters[U_LIGHT5_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[5].spotDirection");
		m_parameters[U_LIGHT5_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[5].cosCutoff");
		m_parameters[U_LIGHT5_COSINNER] = glGetUniformLocation(m_programID, "lights[5].cosInner");
		m_parameters[U_LIGHT5_EXPONENT] = glGetUniformLocation(m_programID, "lights[5].exponent");

		//light 7
		m_parameters[U_LIGHT6_POSITION] = glGetUniformLocation(m_programID, "lights[6].position_cameraspace");
		m_parameters[U_LIGHT6_COLOR] = glGetUniformLocation(m_programID, "lights[6].color");
		m_parameters[U_LIGHT6_POWER] = glGetUniformLocation(m_programID, "lights[6].power");
		m_parameters[U_LIGHT6_KC] = glGetUniformLocation(m_programID, "lights[6].kC");
		m_parameters[U_LIGHT6_KL] = glGetUniformLocation(m_programID, "lights[6].kL");
		m_parameters[U_LIGHT6_KQ] = glGetUniformLocation(m_programID, "lights[6].kQ");
		m_parameters[U_LIGHT6_TYPE] = glGetUniformLocation(m_programID, "lights[6].type");
		m_parameters[U_LIGHT6_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[6].spotDirection");
		m_parameters[U_LIGHT6_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[6].cosCutoff");
		m_parameters[U_LIGHT6_COSINNER] = glGetUniformLocation(m_programID, "lights[6].cosInner");
		m_parameters[U_LIGHT6_EXPONENT] = glGetUniformLocation(m_programID, "lights[6].exponent");

		//light 8
		m_parameters[U_LIGHT7_POSITION] = glGetUniformLocation(m_programID, "lights[7].position_cameraspace");
		m_parameters[U_LIGHT7_COLOR] = glGetUniformLocation(m_programID, "lights[7].color");
		m_parameters[U_LIGHT7_POWER] = glGetUniformLocation(m_programID, "lights[7].power");
		m_parameters[U_LIGHT7_KC] = glGetUniformLocation(m_programID, "lights[7].kC");
		m_parameters[U_LIGHT7_KL] = glGetUniformLocation(m_programID, "lights[7].kL");
		m_parameters[U_LIGHT7_KQ] = glGetUniformLocation(m_programID, "lights[7].kQ");
		m_parameters[U_LIGHT7_TYPE] = glGetUniformLocation(m_programID, "lights[7].type");
		m_parameters[U_LIGHT7_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[7].spotDirection");
		m_parameters[U_LIGHT7_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[7].cosCutoff");
		m_parameters[U_LIGHT7_COSINNER] = glGetUniformLocation(m_programID, "lights[7].cosInner");
		m_parameters[U_LIGHT7_EXPONENT] = glGetUniformLocation(m_programID, "lights[7].exponent");

		m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
		m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");
		// Get a handle for our "colorTexture" uniform
		m_parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_programID, "colorTextureEnabled");
		m_parameters[U_COLOR_TEXTURE] = glGetUniformLocation(m_programID, "colorTexture");

		// Get a handle for our "textColor" uniform
		m_parameters[U_TEXT_ENABLED] = glGetUniformLocation(m_programID, "textEnabled");
		m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID, "textColor");

		glUseProgram(m_programID);

		//light 1
		light[0].type = Light::LIGHT_POINT;
		light[0].position.Set(0, 0, 0);
		light[0].color.Set(1, 1, 1);
		light[0].power = 0.6f;
		light[0].kC = 1.f;
		light[0].kL = 0.1f;
		light[0].kQ = 0.01f;
		light[0].cosCutoff = cos(Math::DegreeToRadian(45));
		light[0].cosInner = cos(Math::DegreeToRadian(30));
		light[0].exponent = 3.f;
		light[0].spotDirection.Set(0.f, 1.f, 0.f);
		// Make sure you pass uniform parameters after glUseProgram()
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


		//light 2
		light[1].type = Light::LIGHT_DIRECTIONAL;
		light[1].position.Set(-100, 100, -100);
		light[1].color.Set(253.0f / 255.0f, 251.0f / 255.0f, 241.0f / 255.0f);
		light[1].power = 0.6f;
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
		glUniform1f(m_parameters[U_LIGHT1_POWER], light[1].power);
		glUniform1f(m_parameters[U_LIGHT1_KC], light[1].kC);
		glUniform1f(m_parameters[U_LIGHT1_KL], light[1].kL);
		glUniform1f(m_parameters[U_LIGHT1_KQ], light[1].kQ);
		glUniform1f(m_parameters[U_LIGHT1_COSCUTOFF], light[1].cosCutoff);
		glUniform1f(m_parameters[U_LIGHT1_COSINNER], light[1].cosInner);
		glUniform1f(m_parameters[U_LIGHT1_EXPONENT], light[1].exponent);

		//light 3
		light[2].type = Light::LIGHT_DIRECTIONAL;
		light[2].position.Set(100, 100, 100);
		light[2].color.Set(253.0f / 255.0f, 251.0f / 255.0f, 241.0f / 255.0f);
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
		light[3].type = Light::LIGHT_POINT;
		light[3].position.Set(-19, -4.5, 22.8);
		light[3].color.Set(1, 1, 1);
		light[3].power = 0.8f;
		light[3].kC = 1.f;
		light[3].kL = 0.1f;
		light[3].kQ = 0.01f;
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

		//light 5
		light[4].type = Light::LIGHT_POINT;
		light[4].position.Set(-59, -4.5, 22.8);
		light[4].color.Set(1, 1, 1);
		light[4].power = 0.8f;
		light[4].kC = 1.f;
		light[4].kL = 0.1f;
		light[4].kQ = 0.01f;
		light[4].cosCutoff = cos(Math::DegreeToRadian(45));
		light[4].cosInner = cos(Math::DegreeToRadian(30));
		light[4].exponent = 3.f;
		light[4].spotDirection.Set(0.f, 1.f, 0.f);
		glUniform3fv(m_parameters[U_LIGHT4_COLOR], 1, &light[4].color.r);
		glUniform1f(m_parameters[U_LIGHT4_POWER], light[4].power);
		glUniform1f(m_parameters[U_LIGHT4_KC], light[4].kC);
		glUniform1f(m_parameters[U_LIGHT4_KL], light[4].kL);
		glUniform1f(m_parameters[U_LIGHT4_KQ], light[4].kQ);
		glUniform1i(m_parameters[U_LIGHT4_TYPE], light[4].type);
		glUniform3fv(m_parameters[U_LIGHT4_COLOR], 1, &light[4].color.r);
		glUniform1f(m_parameters[U_LIGHT4_KC], light[4].kC);
		glUniform1f(m_parameters[U_LIGHT4_POWER], light[4].power);
		glUniform1f(m_parameters[U_LIGHT4_KL], light[4].kL);
		glUniform1f(m_parameters[U_LIGHT4_KQ], light[4].kQ);
		glUniform1f(m_parameters[U_LIGHT4_COSCUTOFF], light[4].cosCutoff);
		glUniform1f(m_parameters[U_LIGHT4_COSINNER], light[4].cosInner);
		glUniform1f(m_parameters[U_LIGHT4_EXPONENT], light[4].exponent);

		//light 6
		light[5].type = Light::LIGHT_POINT;
		light[5].position.Set(0, 0, 0);
		light[5].color.Set(1, 1, 1);
		light[5].power = 0.6f;
		light[5].kC = 1.f;
		light[5].kL = 0.1f;
		light[5].kQ = 0.01f;
		light[5].cosCutoff = cos(Math::DegreeToRadian(45));
		light[5].cosInner = cos(Math::DegreeToRadian(30));
		light[5].exponent = 3.f;
		light[5].spotDirection.Set(0.f, 1.f, 0.f);
		glUniform3fv(m_parameters[U_LIGHT5_COLOR], 1, &light[5].color.r);
		glUniform1f(m_parameters[U_LIGHT5_POWER], light[5].power);
		glUniform1f(m_parameters[U_LIGHT5_KC], light[5].kC);
		glUniform1f(m_parameters[U_LIGHT5_KL], light[5].kL);
		glUniform1f(m_parameters[U_LIGHT5_KQ], light[5].kQ);
		glUniform1i(m_parameters[U_LIGHT5_TYPE], light[5].type);
		glUniform3fv(m_parameters[U_LIGHT5_COLOR], 1, &light[5].color.r);
		glUniform1f(m_parameters[U_LIGHT5_KC], light[5].kC);
		glUniform1f(m_parameters[U_LIGHT5_POWER], light[5].power);
		glUniform1f(m_parameters[U_LIGHT5_KL], light[5].kL);
		glUniform1f(m_parameters[U_LIGHT5_KQ], light[5].kQ);
		glUniform1f(m_parameters[U_LIGHT5_COSCUTOFF], light[5].cosCutoff);
		glUniform1f(m_parameters[U_LIGHT5_COSINNER], light[5].cosInner);
		glUniform1f(m_parameters[U_LIGHT5_EXPONENT], light[5].exponent);

		//light 7
		light[6].type = Light::LIGHT_POINT;
		light[6].position.Set(0, 0, 0);
		light[6].color.Set(1, 1, 1);
		light[6].power = 0.6f;
		light[6].kC = 1.f;
		light[6].kL = 0.1f;
		light[6].kQ = 0.01f;
		light[6].cosCutoff = cos(Math::DegreeToRadian(45));
		light[6].cosInner = cos(Math::DegreeToRadian(30));
		light[6].exponent = 3.f;
		light[6].spotDirection.Set(0.f, 1.f, 0.f);
		glUniform3fv(m_parameters[U_LIGHT6_COLOR], 1, &light[6].color.r);
		glUniform1f(m_parameters[U_LIGHT6_POWER], light[6].power);
		glUniform1f(m_parameters[U_LIGHT6_KC], light[6].kC);
		glUniform1f(m_parameters[U_LIGHT6_KL], light[6].kL);
		glUniform1f(m_parameters[U_LIGHT6_KQ], light[6].kQ);
		glUniform1i(m_parameters[U_LIGHT6_TYPE], light[6].type);
		glUniform3fv(m_parameters[U_LIGHT6_COLOR], 1, &light[6].color.r);
		glUniform1f(m_parameters[U_LIGHT6_KC], light[6].kC);
		glUniform1f(m_parameters[U_LIGHT6_POWER], light[6].power);
		glUniform1f(m_parameters[U_LIGHT6_KL], light[6].kL);
		glUniform1f(m_parameters[U_LIGHT6_KQ], light[6].kQ);
		glUniform1f(m_parameters[U_LIGHT6_COSCUTOFF], light[6].cosCutoff);
		glUniform1f(m_parameters[U_LIGHT6_COSINNER], light[6].cosInner);
		glUniform1f(m_parameters[U_LIGHT6_EXPONENT], light[6].exponent);

		//light 8
		light[7].type = Light::LIGHT_POINT;
		light[7].position.Set(0, 0, 0);
		light[7].color.Set(1, 1, 1);
		light[7].power = 0.6f;
		light[7].kC = 1.f;
		light[7].kL = 0.1f;
		light[7].kQ = 0.01f;
		light[7].cosCutoff = cos(Math::DegreeToRadian(45));
		light[7].cosInner = cos(Math::DegreeToRadian(30));
		light[7].exponent = 3.f;
		light[7].spotDirection.Set(0.f, 1.f, 0.f);
		glUniform3fv(m_parameters[U_LIGHT7_COLOR], 1, &light[7].color.r);
		glUniform1f(m_parameters[U_LIGHT7_POWER], light[7].power);
		glUniform1f(m_parameters[U_LIGHT7_KC], light[7].kC);
		glUniform1f(m_parameters[U_LIGHT7_KL], light[7].kL);
		glUniform1f(m_parameters[U_LIGHT7_KQ], light[7].kQ);
		glUniform1i(m_parameters[U_LIGHT7_TYPE], light[7].type);
		glUniform3fv(m_parameters[U_LIGHT7_COLOR], 1, &light[7].color.r);
		glUniform1f(m_parameters[U_LIGHT7_KC], light[7].kC);
		glUniform1f(m_parameters[U_LIGHT7_POWER], light[7].power);
		glUniform1f(m_parameters[U_LIGHT7_KL], light[7].kL);
		glUniform1f(m_parameters[U_LIGHT7_KQ], light[7].kQ);
		glUniform1f(m_parameters[U_LIGHT7_COSCUTOFF], light[7].cosCutoff);
		glUniform1f(m_parameters[U_LIGHT7_COSINNER], light[7].cosInner);
		glUniform1f(m_parameters[U_LIGHT7_EXPONENT], light[7].exponent);

		glUniform1i(m_parameters[U_NUMLIGHTS], 8);
	}

	//variable to rotate geometry
	rotateAngle = 0;

	//Initialize camera settings
	camera.Init(Vector3(-18, 2, 15), Vector3(-18, 2, 14), Vector3(0, 1, 0));

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

	//sky box
	meshList[GEO_FRONT] = MeshBuilder::GenerateQuad("front", Color(0, 1, 1), 1.f);
	meshList[GEO_FRONT]->textureID = LoadTGA("Image//front.tga");
	meshList[GEO_BACK] = MeshBuilder::GenerateQuad("back", Color(0, 1, 1), 1.f);
	meshList[GEO_BACK]->textureID = LoadTGA("Image//back.tga");
	meshList[GEO_LEFT] = MeshBuilder::GenerateQuad("left", Color(0, 1, 1), 1.f);
	meshList[GEO_LEFT]->textureID = LoadTGA("Image//left.tga");
	meshList[GEO_RIGHT] = MeshBuilder::GenerateQuad("right", Color(0, 1, 1), 1.f);
	meshList[GEO_RIGHT]->textureID = LoadTGA("Image//right.tga");
	meshList[GEO_TOP] = MeshBuilder::GenerateQuad("top", Color(0, 1, 1), 1.f);
	meshList[GEO_TOP]->textureID = LoadTGA("Image//top.tga");
	meshList[GEO_BOTTOM] = MeshBuilder::GenerateQuad("bottom", Color(0, 1, 1), 1.f);
	meshList[GEO_BOTTOM]->textureID = LoadTGA("Image//bottom.tga");

	//Npc
	meshList[GEO_NPC1] = MeshBuilder::GenerateOBJMTL("npc1", "OBJ//advancedCharacter.obj", "OBJ//advancedCharacter.obj.mtl");
	meshList[GEO_NPC1]->textureID = LoadTGA("Image//skin_adventurer.tga");
	objectlist[hb_NPC1].setmesh(GEO_NPC1);
	objectlist[hb_NPC1].setproperties(Vector3(0, 0, -10), Vector3(0, 0, 0), Vector3(0.16, 0.16, 0.16));
	objectlist[hb_NPC1].Setuphitbox(Vector3(0.8, 2, 0.8), Vector3(0, 0, 0));
	objectlist[hb_NPC1].sethitboxcollisionsize(Vector3(1, 0, 1));

	meshList[GEO_DIALOGUEUI] = MeshBuilder::GenerateQuad("dialoguebox", Color(1, 1, 1), 1.f);
	meshList[GEO_DIALOGUEUI]->textureID = LoadTGA("Image//DialogueBox.tga");


	//houses
	//fat tall
	meshList[GEO_HOUSE1] = MeshBuilder::GenerateOBJMTL("modelhouse1", "OBJ//large_buildingB.obj", "OBJ//large_buildingB.mtl");
	objectlist[hb_HOUSE1].setmesh(GEO_HOUSE1);
	objectlist[hb_HOUSE1].setproperties(Vector3(-19, 0, 21.6), Vector3(0, 0, 0), Vector3(7, 10, 11));
	objectlist[hb_HOUSE1].Setuphitbox(Vector3(4, 1, 5), Vector3(0, 1, 0));
	objectlist[hb_HOUSE1].sethitboxcollisionsize(Vector3(4, 0, 6.7));

	objectlist[hb_HOUSE6].setmesh(GEO_HOUSE1);
	objectlist[hb_HOUSE6].setproperties(Vector3(-13, 0, 3), Vector3(0, 180, 0), Vector3(7, 10, 9));
	objectlist[hb_HOUSE6].Setuphitbox(Vector3(1, 1, 1), Vector3(0, 3, 0));
	objectlist[hb_HOUSE6].sethitboxcollisionsize(Vector3(4, 0, 6.7));

	objectlist[hb_HOUSE11].setmesh(GEO_HOUSE1);
	objectlist[hb_HOUSE11].setproperties(Vector3(-13, 0, -19), Vector3(0, -90, 0), Vector3(9, 13, 7));
	objectlist[hb_HOUSE11].Setuphitbox(Vector3(1, 1, 1), Vector3(0, 3, 0));
	objectlist[hb_HOUSE11].sethitboxcollisionsize(Vector3(4, 0, 6.7));

	objectlist[hb_HOUSE17].setmesh(GEO_HOUSE1);
	objectlist[hb_HOUSE17].setproperties(Vector3(25.6, 0, 20), Vector3(0, 0, 0), Vector3(7, 10, 7));
	objectlist[hb_HOUSE17].Setuphitbox(Vector3(1, 1, 1), Vector3(0, 3, 0));
	objectlist[hb_HOUSE17].sethitboxcollisionsize(Vector3(4, 0, 4));

	objectlist[hb_HOUSE20].setmesh(GEO_HOUSE1);
	objectlist[hb_HOUSE20].setproperties(Vector3(19, 0, -5), Vector3(0, 90, 0), Vector3(7, 10, 7));
	objectlist[hb_HOUSE20].Setuphitbox(Vector3(1, 1, 1), Vector3(0, 3, 0));
	objectlist[hb_HOUSE20].sethitboxcollisionsize(Vector3(4, 0, 4));

	objectlist[hb_HOUSE22].setmesh(GEO_HOUSE1);
	objectlist[hb_HOUSE22].setproperties(Vector3(19, 0, -10.5), Vector3(0, 90, 0), Vector3(7, 10, 7));
	objectlist[hb_HOUSE22].Setuphitbox(Vector3(1, 1, 1), Vector3(0, 3, 0));
	objectlist[hb_HOUSE22].sethitboxcollisionsize(Vector3(4, 0, 6.7));

	objectlist[hb_HOUSE29].setmesh(GEO_HOUSE1);
	objectlist[hb_HOUSE29].setproperties(Vector3(-23, 0, -35.8), Vector3(0, 90, 0), Vector3(7, 10, 7));
	objectlist[hb_HOUSE29].Setuphitbox(Vector3(1, 1, 1), Vector3(0, 3, 0));
	objectlist[hb_HOUSE29].sethitboxcollisionsize(Vector3(4, 0, 4));

	objectlist[hb_HOUSE30].setmesh(GEO_HOUSE1);
	objectlist[hb_HOUSE30].setproperties(Vector3(-13, 0, -35.4), Vector3(0, -90, 0), Vector3(7, 13, 7));
	objectlist[hb_HOUSE30].Setuphitbox(Vector3(1, 1, 1), Vector3(0, 3, 0));
	objectlist[hb_HOUSE30].sethitboxcollisionsize(Vector3(4, 0, 4));

	//long
	meshList[GEO_HOUSE2] = MeshBuilder::GenerateOBJMTL("modelhouse2", "OBJ//large_buildingF.obj", "OBJ//large_buildingF.mtl");
	objectlist[hb_HOUSE2].setmesh(GEO_HOUSE2);
	objectlist[hb_HOUSE2].setproperties(Vector3(-9.2, 0, 20), Vector3(0, 0, 0), Vector3(7, 10, 7));
	objectlist[hb_HOUSE2].Setuphitbox(Vector3(1, 1, 1), Vector3(0, 3, 0));
	objectlist[hb_HOUSE2].sethitboxcollisionsize(Vector3(12, 0, 4));

	objectlist[hb_HOUSE7].setmesh(GEO_HOUSE2);
	objectlist[hb_HOUSE7].setproperties(Vector3(-23, 0, -16), Vector3(0, 90, 0), Vector3(17, 10, 7));
	objectlist[hb_HOUSE7].Setuphitbox(Vector3(1, 1, 1), Vector3(0, 3, 0));
	objectlist[hb_HOUSE7].sethitboxcollisionsize(Vector3(4, 0, 32));

	objectlist[hb_HOUSE12].setmesh(GEO_HOUSE2);
	objectlist[hb_HOUSE12].setproperties(Vector3(-13, 0, -27.6), Vector3(0, -90, 0), Vector3(5, 13, 7));
	objectlist[hb_HOUSE12].Setuphitbox(Vector3(1, 1, 1), Vector3(0, 3, 0));
	objectlist[hb_HOUSE12].sethitboxcollisionsize(Vector3(4, 0, 8));

	objectlist[hb_HOUSE16].setmesh(GEO_HOUSE2);
	objectlist[hb_HOUSE16].setproperties(Vector3(22.4, 0, -32), Vector3(0, 180, 0), Vector3(7, 9, 7));
	objectlist[hb_HOUSE16].Setuphitbox(Vector3(1, 1, 1), Vector3(0, 3, 0));
	objectlist[hb_HOUSE16].sethitboxcollisionsize(Vector3(12, 0, 4));

	objectlist[hb_HOUSE21].setmesh(GEO_HOUSE2);
	objectlist[hb_HOUSE21].setproperties(Vector3(32, 0, -7.2), Vector3(0, -90, 0), Vector3(7, 10, 7));
	objectlist[hb_HOUSE21].Setuphitbox(Vector3(1, 1, 1), Vector3(0, 3, 0));
	objectlist[hb_HOUSE21].sethitboxcollisionsize(Vector3(4, 0, 12));

	objectlist[hb_HOUSE23].setmesh(GEO_HOUSE2);
	objectlist[hb_HOUSE23].setproperties(Vector3(32, 0, -21.8), Vector3(0, -90, 0), Vector3(8, 10, 7));
	objectlist[hb_HOUSE23].Setuphitbox(Vector3(1, 1, 1), Vector3(0, 3, 0));
	objectlist[hb_HOUSE23].sethitboxcollisionsize(Vector3(4, 0, 14));

	objectlist[hb_HOUSE25].setmesh(GEO_HOUSE2);
	objectlist[hb_HOUSE25].setproperties(Vector3(-6, 0, 28.7), Vector3(0, 180, 0), Vector3(13, 10, 7));
	objectlist[hb_HOUSE25].Setuphitbox(Vector3(1, 1, 1), Vector3(0, 3, 0));
	objectlist[hb_HOUSE25].sethitboxcollisionsize(Vector3(24, 0, 4));

	objectlist[hb_HOUSE26].setmesh(GEO_HOUSE2);
	objectlist[hb_HOUSE26].setproperties(Vector3(15, 0, 28.7), Vector3(0, 180, 0), Vector3(13, 10, 7));
	objectlist[hb_HOUSE26].Setuphitbox(Vector3(1, 1, 1), Vector3(0, 3, 0));
	objectlist[hb_HOUSE26].sethitboxcollisionsize(Vector3(24, 0, 4));

	objectlist[hb_HOUSE31].setmesh(GEO_HOUSE2);
	objectlist[hb_HOUSE31].setproperties(Vector3(-13.5, 0, -44.5), Vector3(0, 0, 0), Vector3(7, 9, 7));
	objectlist[hb_HOUSE31].Setuphitbox(Vector3(1, 1, 1), Vector3(0, 3, 0));
	objectlist[hb_HOUSE31].sethitboxcollisionsize(Vector3(12, 0, 4));

	objectlist[hb_HOUSE33].setmesh(GEO_HOUSE2);
	objectlist[hb_HOUSE33].setproperties(Vector3(-5, 0, -38), Vector3(0, -90, 0), Vector3(4, 9, 4));
	objectlist[hb_HOUSE33].Setuphitbox(Vector3(1, 1, 1), Vector3(0, 3, 0));
	objectlist[hb_HOUSE33].sethitboxcollisionsize(Vector3(2, 0, 6));

	//skinny tall w/ stairs
	meshList[GEO_HOUSE3] = MeshBuilder::GenerateOBJMTL("modelhouse3", "OBJ//large_buildingC.obj", "OBJ//large_buildingC.mtl");
	objectlist[hb_HOUSE3].setmesh(GEO_HOUSE3);
	objectlist[hb_HOUSE3].setproperties(Vector3(5, 0, 20), Vector3(0, 0, 0), Vector3(7, 10, 7));
	objectlist[hb_HOUSE3].Setuphitbox(Vector3(1, 1, 1), Vector3(0, 3, 0));
	objectlist[hb_HOUSE3].sethitboxcollisionsize(Vector3(4, 0, 4));

	objectlist[hb_HOUSE10].setmesh(GEO_HOUSE3);
	objectlist[hb_HOUSE10].setproperties(Vector3(-13, 0, -9), Vector3(0, -90, 0), Vector3(7, 9, 7));
	objectlist[hb_HOUSE10].Setuphitbox(Vector3(1, 1, 1), Vector3(0, 3, 0));
	objectlist[hb_HOUSE10].sethitboxcollisionsize(Vector3(4, 0, 4));

	objectlist[hb_HOUSE19].setmesh(GEO_HOUSE3);
	objectlist[hb_HOUSE19].setproperties(Vector3(29, 0, 3), Vector3(0, 180, 0), Vector3(8, 10, 8));
	objectlist[hb_HOUSE19].Setuphitbox(Vector3(1, 1, 1), Vector3(0, 3, 0));
	objectlist[hb_HOUSE19].sethitboxcollisionsize(Vector3(4.5, 0, 4.5));

	//skinny tall w/o stairs
	meshList[GEO_HOUSE4] = MeshBuilder::GenerateOBJMTL("modelhouse4", "OBJ//large_buildingD.obj", "OBJ//large_buildingD.mtl");
	objectlist[hb_HOUSE4].setmesh(GEO_HOUSE4);
	objectlist[hb_HOUSE4].setproperties(Vector3(10.7, 0, 20.2), Vector3(0, 0, 0), Vector3(5, 10, 5));
	objectlist[hb_HOUSE4].Setuphitbox(Vector3(1, 1, 1), Vector3(0, 3, 0));
	objectlist[hb_HOUSE4].sethitboxcollisionsize(Vector3(4, 0, 4));

	objectlist[hb_HOUSE8].setmesh(GEO_HOUSE4);
	objectlist[hb_HOUSE8].setproperties(Vector3(-23, 0, 3.5), Vector3(0, 180, 0), Vector3(5, 10, 5));
	objectlist[hb_HOUSE8].Setuphitbox(Vector3(1, 1, 1), Vector3(0, 3, 0));
	objectlist[hb_HOUSE8].sethitboxcollisionsize(Vector3(4.5, 0, 4.5));

	objectlist[hb_HOUSE14].setmesh(GEO_HOUSE4);
	objectlist[hb_HOUSE14].setproperties(Vector3(3.4, 0, -32.8), Vector3(0, 180, 0), Vector3(6, 9, 6));
	objectlist[hb_HOUSE14].Setuphitbox(Vector3(1, 1, 1), Vector3(0, 3, 0));
	objectlist[hb_HOUSE14].sethitboxcollisionsize(Vector3(5.5, 0, 5.5));

	objectlist[hb_HOUSE15].setmesh(GEO_HOUSE4);
	objectlist[hb_HOUSE15].setproperties(Vector3(11.2, 0, -33.4), Vector3(0, 180, 0), Vector3(7, 9, 7));
	objectlist[hb_HOUSE15].Setuphitbox(Vector3(1, 1, 1), Vector3(0, 3, 0));
	objectlist[hb_HOUSE15].sethitboxcollisionsize(Vector3(6.5, 0, 6.5));

	objectlist[hb_HOUSE24].setmesh(GEO_HOUSE4);
	objectlist[hb_HOUSE24].setproperties(Vector3(20.4, 0, -17.5), Vector3(0, 90, 0), Vector3(7, 10, 7));
	objectlist[hb_HOUSE24].Setuphitbox(Vector3(1, 1, 1), Vector3(0, 3, 0));
	objectlist[hb_HOUSE24].sethitboxcollisionsize(Vector3(6.5, 0, 6.5));

	objectlist[hb_HOUSE28].setmesh(GEO_HOUSE4);
	objectlist[hb_HOUSE28].setproperties(Vector3(32, 0, 21.55), Vector3(0, 0, 0), Vector3(6, 10, 7.2));
	objectlist[hb_HOUSE28].Setuphitbox(Vector3(1, 1, 1), Vector3(0, 3, 0));
	objectlist[hb_HOUSE28].sethitboxcollisionsize(Vector3(5.5, 0, 7));

	//short with roof
	meshList[GEO_HOUSE5] = MeshBuilder::GenerateOBJMTL("modelhouse5", "OBJ//small_buildingF.obj", "OBJ//small_buildingF.mtl");
	objectlist[hb_HOUSE5].setmesh(GEO_HOUSE5);
	objectlist[hb_HOUSE5].setproperties(Vector3(20, 0, 20), Vector3(0, 0, 0), Vector3(7, 10, 7));
	objectlist[hb_HOUSE5].Setuphitbox(Vector3(1, 1, 1), Vector3(0, 3, 0));
	objectlist[hb_HOUSE5].sethitboxcollisionsize(Vector3(4, 0, 4));

	objectlist[hb_HOUSE9].setmesh(GEO_HOUSE5);
	objectlist[hb_HOUSE9].setproperties(Vector3(-13, 0, -3.41), Vector3(0, -90, 0), Vector3(7, 9, 7));
	objectlist[hb_HOUSE9].Setuphitbox(Vector3(1, 1, 1), Vector3(0, 3, 0));
	objectlist[hb_HOUSE9].sethitboxcollisionsize(Vector3(4, 0, 4));

	objectlist[hb_HOUSE13].setmesh(GEO_HOUSE5);
	objectlist[hb_HOUSE13].setproperties(Vector3(-3, 0, -32), Vector3(0, 180, 0), Vector3(7, 9, 7));
	objectlist[hb_HOUSE13].Setuphitbox(Vector3(1, 1, 1), Vector3(0, 3, 0));
	objectlist[hb_HOUSE13].sethitboxcollisionsize(Vector3(4, 0, 4));

	objectlist[hb_HOUSE18].setmesh(GEO_HOUSE5);
	objectlist[hb_HOUSE18].setproperties(Vector3(19, 0, 4), Vector3(0, 180, 0), Vector3(7, 9, 6));
	objectlist[hb_HOUSE18].Setuphitbox(Vector3(1, 1, 1), Vector3(0, 3, 0));
	objectlist[hb_HOUSE18].sethitboxcollisionsize(Vector3(4, 0, 3.5));

	objectlist[hb_HOUSE27].setmesh(GEO_HOUSE5);
	objectlist[hb_HOUSE27].setproperties(Vector3(30.8, 0, 28.7), Vector3(0, 180, 0), Vector3(7, 10, 7));
	objectlist[hb_HOUSE27].Setuphitbox(Vector3(1, 1, 1), Vector3(0, 3, 0));
	objectlist[hb_HOUSE27].sethitboxcollisionsize(Vector3(4, 0, 4));

	objectlist[hb_HOUSE32].setmesh(GEO_HOUSE5);
	objectlist[hb_HOUSE32].setproperties(Vector3(-23, 0, -41.4), Vector3(0, 90, 0), Vector3(7, 10, 7));
	objectlist[hb_HOUSE32].Setuphitbox(Vector3(1, 1, 1), Vector3(0, 3, 0));
	objectlist[hb_HOUSE32].sethitboxcollisionsize(Vector3(4, 0, 4));

	//walls
	//meshList[GEO_OUTERWALLS] = MeshBuilder::GenerateOBJMTL("model outerwall", "OBJ//wall_half.obj", "OBJ//wall_half.mtl");
	//meshList[GEO_OUTERWALLS]->textureID = LoadTGA("Image//bricks.tga");

	meshList[GEO_HITBOX] = MeshBuilder::GenerateCube("modelhitbox", Color(0, 0, 0), 1);
	meshList[GEO_HITBOX]->textureID = LoadTGA("Image//hitbox2.tga");
	meshList[GEO_HITBOX2] = MeshBuilder::GenerateCube("modelhitbox", Color(0, 0, 0), 1);
	meshList[GEO_HITBOX2]->textureID = LoadTGA("Image//hitbox3.tga");

	//text
	meshList[GEO_TEXT_CALIBRI] = MeshBuilder::GenerateText("textcalibri", 16, 16);
	meshList[GEO_TEXT_CALIBRI]->textureID = LoadTGA("Image//calibri.tga");
	meshList[GEO_TEXT_COMICSANS] = MeshBuilder::GenerateText("textcomicsans", 16, 16);
	meshList[GEO_TEXT_COMICSANS]->textureID = LoadTGA("Image//calibri.tga");
	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Image//DimboFont.tga");

	//setup player's hitbox
	player.Setposition(Vector3(0, 2, 0));
	player.Setuphitbox(Vector3(1, 1.5, 1), Vector3(0, 0.5, 0));

	meshList[GEO_CONCRETE_PAVEMENT] = MeshBuilder::GenerateQuad("concretepavement", Color(1, 1, 1), 1);
	meshList[GEO_CONCRETE_PAVEMENT]->textureID = LoadTGA("Image//concreteSmooth.tga");

	meshList[GEO_ROAD] = MeshBuilder::GenerateQuad("road", Color(1, 1, 1), 1);
	meshList[GEO_ROAD]->textureID = LoadTGA("Image//asphalt.tga");

	meshList[GEO_CITY_CENTRE_FLOOR] = MeshBuilder::GenerateQuad("citycentrefloor", Color(1, 1, 1), 1);
	meshList[GEO_CITY_CENTRE_FLOOR]->textureID = LoadTGA("Image//Brick_03.tga");

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
	playerfist1extend = true;
	playerfist2extend = false;
	treegrowthstage = 0;
	treeplanted = false; lackofwater = false;
	imagepos = Vector3(40, 30, 0);
	imagedimensions = Vector3(20, 20, 1);
}

bool SP2::Update(double dt)
{
	//Mouse Inputs
	static bool bLButtonState = false;
	if (!bLButtonState && Application::IsMousePressed(0))
	{
		bLButtonState = true;
		std::cout << "LBUTTON DOWN" << std::endl;
		//Converting Viewport space to UI space
		double x, y;
		Application::GetCursorPos(&x, &y);
		unsigned w = Application::GetWindowWidth();
		unsigned h = Application::GetWindowHeight();
		float posX = x / w * 80.f; //convert (0,800) to (0,80)
		float posY = -(y / h * 60.f) + 60; //convert (600,0) to (0,60)
		std::cout << "posX:" << posX << " , posY:" << posY << std::endl;
		if (posX > imagepos.x - imagedimensions.x * 0.5 && posX < imagepos.x + imagedimensions.x * 0.5 && posY > imagepos.y - imagedimensions.y * 0.5 && posY < imagepos.y + imagedimensions.y * 0.5)
		{
			std::cout << "Hit!" << std::endl;
		}
		else
		{
			std::cout << "Miss!" << std::endl;
		}
	}
	else if (bLButtonState && !Application::IsMousePressed(0))
	{
		bLButtonState = false;
		std::cout << "LBUTTON UP" << std::endl;
	}
	static bool RightClick = false;
	if (!RightClick && Application::IsMousePressed(1))
		RightClick = true;
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

bool SP2::UpdateMainMenu()
{
	bool click = GetAsyncKeyState(0x01);
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

		if (GetAsyncKeyState(0x52))
		{
			//reset all the variables
			ShowCursor(true);
			mousehidden = false;
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
			playerfist1extend = true;
			playerfist2extend = false;
			treegrowthstage = 0;
			treeplanted = false; lackofwater = false;
			camera.Reset();
		}
	}
	return false;
}

#define LSPEED 10
void SP2::UpdateENV(double dt)
{
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
				static Hitbox tempplayerhitbox;
				tempplayerhitbox = player.getobjecthitbox();
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

				Vector3 view = (camera.target - camera.position).Normalized();
				for (int i = 0; i < hb_count; ++i) {
				    Vector3 finalPos = PlayerCollision(i, camera); //ignore y
					camera.position.x = finalPos.x;
					camera.position.z = finalPos.z;
				}
				camera.target = camera.position + view;


			} //else do nothing with camera
		}



		//check for interactions hitboxes
		//for (unsigned idx = 0; idx < i_count; idx++)
		//{
		//	if (!interactionhitboxes[idx].getissetup())
		//		continue;
		//	else if (interactionhitboxes[idx].getobjecthitbox().Checkforcollision(player.getobjecthitbox()))
		//		interact = idx;
		//	else if (player.getobjecthitbox().Checkforcollision(interactionhitboxes[idx].getobjecthitbox()))
		//		interact = idx;
		//}

		//static bool leftpunch = false;
		//Vector3 fistvector;
		//fistvector = camera.position + camera.view * (2 + playerfist2) + camera.right * 0.8 + camera.view.Cross(camera.right) * 0.7;
		//fist1.sethitboxcenterdimensions(Vector3(0.3, 0.3, 0.3), fistvector);
		//fistvector = camera.position + camera.view * (2 + playerfist1) - camera.right * 0.8 + camera.view.Cross(camera.right) * 0.7;
		//fist2.sethitboxcenterdimensions(Vector3(0.3, 0.3, 0.3), fistvector);

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

		//static float punchcooldown = 1;
		//if (punch)
		//{
		//	if (leftpunch)
		//	{
		//		if (playerfist1extend)
		//		{
		//			playerfist1 += 10.f * dt;
		//			if (playerfist1 > 1)
		//			{
		//				playerfist1extend = false;
		//			}
		//		}
		//		else
		//		{
		//			playerfist1 -= 10.f * dt;
		//			if (playerfist1 <= 0)
		//			{
		//				playerfist1extend = true;
		//				leftpunch = false;
		//			}
		//		}
		//	}
		//	else
		//	{
		//		if (playerfist2extend)
		//		{
		//			playerfist2 += 10.f * dt;
		//			if (playerfist2 >= 1)
		//			{
		//				playerfist2extend = false;
		//			}
		//		}
		//		else
		//		{
		//			playerfist2 -= 10.f * dt;
		//			if (playerfist2 <= 0)
		//			{
		//				playerfist2extend = true;
		//				leftpunch = true;
		//			}
		//		}
		//	}
		//}


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

		if (GetAsyncKeyState(VK_ESCAPE) && !escbuttonstate) //to exit env
		{
			if (ingame)
			{
				ingame = false;
				inshop = false;
				//camera.MoveCam(savedposition);
				//camera.Settarget(savedtarget);
			}
			else if (inshop)
			{
				inshop = false;
				shopselect = 0;
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
		//punchcooldown += dt;
	}
	else
	{
		ShowCursor(true);
		mousehidden = false;
	}

	camera.center.x = (camera.windowcenter.left + camera.windowcenter.right) * 0.5;
	camera.center.y = (camera.windowcenter.top + camera.windowcenter.bottom) * 0.5;

	//Street Scam______________________________________________________________________________________________________________________

	if (DialogueBoxOpen == true)
	{
		if (Application::IsKeyPressed('E') and isRead == false and timesincelastbuttonpress > 0.2 and Dialogue == 1) 
		{
			GD_PrintLine1 = "Huh? Whayya want?";
			Dialogue = 2;
			timesincelastbuttonpress = 0;
		}
		if (Application::IsKeyPressed('E') and isRead == false and timesincelastbuttonpress > 0.2 and Dialogue == 2)
		{
			GD_PrintLine1 = " I have this newest limited edition game!";
			GD_PrintLine2 = " Care to buy it off my hands for $100?";
			GD_PrintLine3 = "";
			Dialogue = 3;
			timesincelastbuttonpress = 0;
		}
		if (Application::IsKeyPressed('E') and isRead == false and timesincelastbuttonpress > 0.2 and Dialogue == 3)
		{
			GD_PrintLine1 = " What console is this from?";
			GD_PrintLine2 = "";
			GD_PrintLine3 = "";
			Dialogue = 4;
			timesincelastbuttonpress = 0;
		}
		if (Application::IsKeyPressed('E') and isRead == false and timesincelastbuttonpress > 0.2 and Dialogue == 4)
		{
			GD_PrintLine1 = "1. It is from bXob!";
			GD_PrintLine2 = " 2. It is from Barney Bros!";
			GD_PrintLine3 = "3.  I have no idea.";
			Dialogue = 5;
			timesincelastbuttonpress = 0;
		}
		if (Application::IsKeyPressed('1') and isRead == false and timesincelastbuttonpress > 0.2 and Dialogue == 5)
		{
			GD_PrintLine1 = " Cool! Alright, I can finally try a different game now!";
			GD_PrintLine2 = "";
			GD_PrintLine3 = "";
			Dialogue = 6;
			timesincelastbuttonpress = 0;
		}
		if (Application::IsKeyPressed('2') and isRead == false and timesincelastbuttonpress > 0.2 and Dialogue == 5)
		{
			GD_PrintLine1 = " Hah! That's a movie studio,";
			GD_PrintLine2 = " no game comes from there!";
			GD_PrintLine3 = "";
			Dialogue = 7;
			timesincelastbuttonpress = 0;
		}
		if (Application::IsKeyPressed('3') and isRead == false and timesincelastbuttonpress > 0.2 and Dialogue == 5)
		{
			GD_PrintLine1 = " Can I even trust you into buying this game?";
			GD_PrintLine2 = "";
			GD_PrintLine3 = "";
			Dialogue = 7;
			timesincelastbuttonpress = 0;
		}
		if (Application::IsKeyPressed('E') and isRead == false and timesincelastbuttonpress > 0.2 and Dialogue == 6)
		{
			GD_PrintLine1 = " This looks like a steal, I will take it.";
			GD_PrintLine2 = "";
			GD_PrintLine3 = "";
			Dialogue = 4;
			timesincelastbuttonpress = 0;
		}
		if (Application::IsKeyPressed('E') and isRead == false and timesincelastbuttonpress > 0.2 and Dialogue == 7)
		{
			GD_PrintLine1 = " You just tried to scam me right, I am calling the police.";
			GD_PrintLine2 = "";
			GD_PrintLine3 = "";
			Dialogue = 4;
			timesincelastbuttonpress = 0;
		}
	}

	// SHOPKEEPER---------------------------------------------------------------------------------------------------------------
	//if (DialogueBoxOpen == true)
	//{
	//	if (Application::IsKeyPressed('E') and isRead == false and timesincelastbuttonpress > 0.2 and Dialogue == 1) {
	//		GD_PrintLine1 = "Welcome to the pawn shop!";
	//		Dialogue = 2;
	//		timesincelastbuttonpress = 0;
	//	}
	//	if (Application::IsKeyPressed('E') and isRead == false and timesincelastbuttonpress > 0.2 and Dialogue == 2) {
	//		GD_PrintLine1 = "What would you like to do today?";
	//		Dialogue = 3;
	//		timesincelastbuttonpress = 0;
	//	}
	//	if (Application::IsKeyPressed('E') and isRead == false and timesincelastbuttonpress > 0.2 and Dialogue == 3) {
	//		GD_PrintLine1 = "I like to pawn this exquisite ring i have here.";
	//		Dialogue = 4;
	//		timesincelastbuttonpress = 0;
	//	}
	//	if (Application::IsKeyPressed('E') and isRead == false and timesincelastbuttonpress > 0.2 and Dialogue == 4) {
	//		GD_PrintLine1 = "1. It is made with multiple diamonds";
	//		GD_PrintLine2 = "2. I found it from the garbage bin down the road";
	//		GD_PrintLine3 = "3. It is a ring worn by the famous actor James Bonk.";
	//		Dialogue = 5;
	//		timesincelastbuttonpress = 0;
	//	}
	//	if (Application::IsKeyPressed('1') and isRead == false and timesincelastbuttonpress > 0.2 and Dialogue == 5)
	//	{
	//		GD_PrintLine1 = "Nice try, but i have checked it and its a fake ring.";
	//		GD_PrintLine2 = "";
	//		GD_PrintLine3 = "";
	//		timesincelastbuttonpress = 0;
	//		Dialogue = 6;
	//	}
	//	if (Application::IsKeyPressed('2') and isRead == false and timesincelastbuttonpress > 0.2 and Dialogue == 5)
	//	{
	//		GD_PrintLine1 = " Why are you even here? ";
	//		GD_PrintLine2 = "";
	//		GD_PrintLine3 = "";
	//		timesincelastbuttonpress = 0;
	//		Dialogue = 6;
	//	}
	//	if (Application::IsKeyPressed('3') and isRead == false and timesincelastbuttonpress > 0.2 and Dialogue == 5)
	//	{
	//		GD_PrintLine1 = "I do not know much about him,";
	//		GD_PrintLine2 = "but I think I did see him wear this ring.";
	//		GD_PrintLine3 = "";
	//		timesincelastbuttonpress = 0;
	//		Dialogue = 7;
	//	}
	//	if (Application::IsKeyPressed('E') and isRead == false and timesincelastbuttonpress > 0.2 and Dialogue == 6)
	//	{
	//		GD_PrintLine1 = " I am not buying that.";
	//		GD_PrintLine2 = "";
	//		GD_PrintLine3 = "";
	//		timesincelastbuttonpress = 0;
	//		Dialogue = 4;
	//	}
	//	if (Application::IsKeyPressed('E') and isRead == false and timesincelastbuttonpress > 0.2 and Dialogue == 7)
	//	{
	//		GD_PrintLine1 = " I could make a profit from this. Thank you for coming!";
	//		GD_PrintLine2 = "";
	//		GD_PrintLine3 = "";
	//		timesincelastbuttonpress = 0;
	//		Dialogue = 4;
	//	}
	//}

	//CALL SCAM_____________________________________________________________________________________________________________________
	/*if (DialogueBoxOpen == true)
	{
		if (Application::IsKeyPressed('E') and isRead == false and timesincelastbuttonpress > 0.2 and Dialogue == 1) {
			GD_PrintLine1 = " Hello? To whom am I speaking to?";
			Dialogue = 2;
			timesincelastbuttonpress = 0;
		}
		if (Application::IsKeyPressed('E') and isRead == false and timesincelastbuttonpress > 0.2 and Dialogue == 2) {
			GD_PrintLine1 = "1. NFT scam.";
			GD_PrintLine2 = "2. Kidnapping scam.";
			GD_PrintLine3 = "3. Bank scam.";
			Dialogue = 3;
			timesincelastbuttonpress = 0;
		}
		if (Application::IsKeyPressed('1') and isRead == false and timesincelastbuttonpress > 0.2 and Dialogue == 3)
		{
			GD_PrintLine1 = " I have some exclusive NFTs for sale at $100.";
			GD_PrintLine2 = " Would you like to buy them?";
			GD_PrintLine3 = "";
			timesincelastbuttonpress = 0;
			Dialogue = 4;
		}
		if (Application::IsKeyPressed('E') and isRead == false and timesincelastbuttonpress > 0.2 and Dialogue == 4) {
			GD_PrintLine1 = " No thanks, I have no interest in NFTs.";
			GD_PrintLine2 = "";
			GD_PrintLine3 = "";
			Dialogue = 2;
			timesincelastbuttonpress = 0;
		}
		if (Application::IsKeyPressed('2') and isRead == false and timesincelastbuttonpress > 0.2 and Dialogue == 3)
		{
			GD_PrintLine1 = " I have your child with me!";
			GD_PrintLine2 = " Give me $100 or you won’t see your child ever again! ";
			GD_PrintLine3 = "";
			timesincelastbuttonpress = 0;
			Dialogue = 5;
		}
		if (Application::IsKeyPressed('E') and isRead == false and timesincelastbuttonpress > 0.2 and Dialogue == 5) {
			GD_PrintLine1 = " What are you even talking about!";
			GD_PrintLine2 = " I do not even have a child!";
			GD_PrintLine3 = "";
			Dialogue = 2;
			timesincelastbuttonpress = 0;
		}
		if (Application::IsKeyPressed('3') and isRead == false and timesincelastbuttonpress > 0.2 and Dialogue == 3)
		{
			GD_PrintLine1 = " I am from the ABC bank, your card has been declined. ";
			GD_PrintLine2 = " You need to give me $100 or else your account is forever blocked! ";
			GD_PrintLine3 = "";
			timesincelastbuttonpress = 0;
			Dialogue = 6;
		}
		if (Application::IsKeyPressed('E') and isRead == false and timesincelastbuttonpress > 0.2 and Dialogue == 6) {
			GD_PrintLine1 = " Oh my god! Alright alright I will give it to you.";
			GD_PrintLine2 = " Please recover my account back! ";
			GD_PrintLine3 = "";
			Dialogue = 2;
			timesincelastbuttonpress = 0;
		}
	}*/


	if (DistanceParameter(player.getposition().x, player.getposition().z, objectlist[hb_NPC1].getposition().x, objectlist[hb_NPC1].getposition().z) <= 8) {
		timesincelastbuttonpress += dt;
	}
	else {
		timesincelastbuttonpress = 0;
	}
	timesincelastpunch += dt;
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
	RenderTextOnScreen(meshList[GEO_TEXT], "Scam life", Color(1, 1, 1), 10, 0, 45);

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

	//light 3
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

	//light 5
	if (light[4].type == Light::LIGHT_DIRECTIONAL)
	{
		Vector3 lightDir(light[4].position.x, light[4].position.y, light[4].position.z);
		Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
		glUniform3fv(m_parameters[U_LIGHT4_POSITION], 1, &lightDirection_cameraspace.x);
	}
	else if (light[4].type == Light::LIGHT_SPOT)
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[4].position;
		glUniform3fv(m_parameters[U_LIGHT4_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * light[4].spotDirection;
		glUniform3fv(m_parameters[U_LIGHT4_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[4].position;
		glUniform3fv(m_parameters[U_LIGHT4_POSITION], 1, &lightPosition_cameraspace.x);
	}

	//light 6
	if (light[5].type == Light::LIGHT_DIRECTIONAL)
	{
		Vector3 lightDir(light[5].position.x, light[5].position.y, light[5].position.z);
		Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
		glUniform3fv(m_parameters[U_LIGHT5_POSITION], 1, &lightDirection_cameraspace.x);
	}
	else if (light[5].type == Light::LIGHT_SPOT)
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[5].position;
		glUniform3fv(m_parameters[U_LIGHT5_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * light[5].spotDirection;
		glUniform3fv(m_parameters[U_LIGHT5_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[5].position;
		glUniform3fv(m_parameters[U_LIGHT5_POSITION], 1, &lightPosition_cameraspace.x);
	}

	//light 7
	if (light[6].type == Light::LIGHT_DIRECTIONAL)
	{
		Vector3 lightDir(light[6].position.x, light[6].position.y, light[6].position.z);
		Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
		glUniform3fv(m_parameters[U_LIGHT6_POSITION], 1, &lightDirection_cameraspace.x);
	}
	else if (light[6].type == Light::LIGHT_SPOT)
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[6].position;
		glUniform3fv(m_parameters[U_LIGHT6_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * light[6].spotDirection;
		glUniform3fv(m_parameters[U_LIGHT6_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[6].position;
		glUniform3fv(m_parameters[U_LIGHT6_POSITION], 1, &lightPosition_cameraspace.x);
	}

	//light 8
	if (light[7].type == Light::LIGHT_DIRECTIONAL)
	{
		Vector3 lightDir(light[7].position.x, light[7].position.y, light[7].position.z);
		Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
		glUniform3fv(m_parameters[U_LIGHT7_POSITION], 1, &lightDirection_cameraspace.x);
	}
	else if (light[7].type == Light::LIGHT_SPOT)
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[7].position;
		glUniform3fv(m_parameters[U_LIGHT7_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * light[7].spotDirection;
		glUniform3fv(m_parameters[U_LIGHT7_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[7].position;
		glUniform3fv(m_parameters[U_LIGHT7_POSITION], 1, &lightPosition_cameraspace.x);
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
	for (unsigned idx = 0; idx < hb_count; idx++) //for objects
	{
		if (!objectlist[idx].getissetup() || objectlist[idx].getmodel() == 0)
			continue;
		modelStack.PushMatrix();
		modelStack.Translate(objectlist[idx].getposition().x, objectlist[idx].getposition().y, objectlist[idx].getposition().z);
		modelStack.Rotate(objectlist[idx].getrotation().x, 1, 0, 0);
		modelStack.Rotate(objectlist[idx].getrotation().y, 0, 1, 0);
		modelStack.Rotate(objectlist[idx].getrotation().z, 0, 0, 1);
		modelStack.Scale(objectlist[idx].getscale().x, objectlist[idx].getscale().y, objectlist[idx].getscale().z);
	
		if (idx != hb_NPC1) {
			RenderMesh(meshList[objectlist[idx].getmodel()], true);
		}
		else {
			RenderMesh(meshList[objectlist[idx].getmodel()], false);
		}
		modelStack.PopMatrix();
	}


	//render roads + pavement
	{
		for (int idx = 0; idx < 50; idx++)
		{
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
		modelStack.Translate(-9, 0.0009, 6 - (idx * 2));
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CITY_CENTRE_FLOOR], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-7, 0.0009, 6 - (idx * 2));
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CITY_CENTRE_FLOOR], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-5, 0.0009, 6 - (idx * 2));
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CITY_CENTRE_FLOOR], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-3, 0.0009, 6 - (idx * 2));
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CITY_CENTRE_FLOOR], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-1, 0.0009, 6 - (idx * 2));
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CITY_CENTRE_FLOOR], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(1, 0.0009, 6 - (idx * 2));
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CITY_CENTRE_FLOOR], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(3, 0.0009, 6 - (idx * 2));
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CITY_CENTRE_FLOOR], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(5, 0.0009, 6 - (idx * 2));
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CITY_CENTRE_FLOOR], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(7, 0.0009, 6 - (idx * 2));
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CITY_CENTRE_FLOOR], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(9, 0.0009, 6 - (idx * 2));
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CITY_CENTRE_FLOOR], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(11, 0.0009, 6 - (idx * 2));
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CITY_CENTRE_FLOOR], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(13, 0.0009, 6 - (idx * 2));
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CITY_CENTRE_FLOOR], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(15, 0.0009, 6 - (idx * 2));
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CITY_CENTRE_FLOOR], true);
		modelStack.PopMatrix();
	}
	//Interaction
	if (DistanceParameter(player.getposition().x, player.getposition().z, objectlist[hb_NPC1].getposition().x, objectlist[hb_NPC1].getposition().z) <= 8) {
		if (DialogueBoxOpen == false ) {
			RenderTextOnScreen(meshList[GEO_TEXT], "Press 'E' to interact", Color(1, 1, 1), 5, 27, 5);
		}
		if (Application::IsKeyPressed('E') and DialogueBoxOpen == false and timesincelastbuttonpress > 0.2) {
			DialogueBoxOpen = true;
			timesincelastbuttonpress = 0;
			inshop = true;
		}
		if (DialogueBoxOpen == true) {
			RenderMeshOnScreen(meshList[GEO_DIALOGUEUI], Vector3(60, 15, 1), Vector3(0, 0, 0), 40, 10);
			RenderTextOnScreen(meshList[GEO_TEXT], "SHOPKEEPER", Color(1, 1, 1), 2.7, 23.5, 13.5);
			RenderTextOnScreen(meshList[GEO_TEXT], GD_PrintLine1, Color(0, 0, 0), 3, 13.5, 10);
			RenderTextOnScreen(meshList[GEO_TEXT], GD_PrintLine2, Color(0, 0, 0), 3, 13, 7.5);
			RenderTextOnScreen(meshList[GEO_TEXT], GD_PrintLine3, Color(0, 0, 0), 3, 13.5, 5);
			RenderTextOnScreen(meshList[GEO_TEXT], "Press 'E' to speed up", Color(1, 1, 1), 3, 30.2, 0.8);
		}
		
	}




	if (renderhitboxes)
	{
		for (unsigned idx = 0; idx < hb_count; idx++) //render other hitboxes
		{
			if (!objectlist[idx].getissetup())
				continue;
			modelStack.PushMatrix();
			modelStack.Translate(objectlist[idx].getposition().x + objectlist[idx].getobjecthitbox().gethboffset().x, objectlist[idx].getposition().y + objectlist[idx].getobjecthitbox().gethboffset().y, objectlist[idx].getposition().z + objectlist[idx].getobjecthitbox().gethboffset().z);
			modelStack.Scale(objectlist[idx].getobjecthitbox().gethitboxdimensions().x * 2, objectlist[idx].getobjecthitbox().gethitboxdimensions().y * 2, objectlist[idx].getobjecthitbox().gethitboxdimensions().z * 2);
			RenderMesh(meshList[GEO_HITBOX], false);
			modelStack.PopMatrix();
		}
		for (unsigned idx = 0; idx < i_count; idx++) //render interaction hitboxes
		{
			if (!interactionhitboxes[idx].getissetup())
				continue;
			modelStack.PushMatrix();
			modelStack.Translate(interactionhitboxes[idx].getposition().x + interactionhitboxes[idx].getobjecthitbox().gethboffset().x, interactionhitboxes[idx].getposition().y + interactionhitboxes[idx].getobjecthitbox().gethboffset().y, interactionhitboxes[idx].getposition().z + interactionhitboxes[idx].getobjecthitbox().gethboffset().z);
			modelStack.Scale(interactionhitboxes[idx].getobjecthitbox().gethitboxdimensions().x * 2, interactionhitboxes[idx].getobjecthitbox().gethitboxdimensions().y * 2, interactionhitboxes[idx].getobjecthitbox().gethitboxdimensions().z * 2);
			RenderMesh(meshList[GEO_HITBOX2], false);
			modelStack.PopMatrix();
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
	//modelStack.PushMatrix();
	//modelStack.Translate(light[0].position.x, light[0].position.y, light[0].position.z);
	//RenderMesh(meshList[GEO_LIGHTBALL], false);
	//modelStack.PopMatrix();
}


void SP2::RenderCredits()
{
	//roll credits
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	RenderTextOnScreen(meshList[GEO_TEXT], "Name??", Color(1, 1, 1), 4, 0, 0 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT], "Player's starting house", Color(1, 1, 1), 4, 0, -10 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT], "by:", Color(1, 1, 1), 4, 0, -15 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT], "Vinrax", Color(1, 1, 1), 4, 0, -20 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT], "vinrax@gmail.com", Color(1, 1, 1), 4, 0, -25 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT], "opengameart.org/content", Color(1, 1, 1), 3, 0, -30 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT], "/small-old-house", Color(1, 1, 1), 3, 0, -35 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT], "ground grass texture", Color(1, 1, 1), 4, 0, -45 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT], "by: Simoon Murray", Color(1, 1, 1), 3, 0, -50 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT], "https://www.deviantart.com", Color(1, 1, 1), 3, 0, -55 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT], "/simoonmurray/art", Color(1, 1, 1), 3, 0, -60 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT], "/Green-Grass-Texture-01-155704377", Color(1, 1, 1), 3, 0, -65 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT], "Skybox By:", Color(1, 1, 1), 4, 0, -75 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT], "Emil Persson, aka Humus.", Color(1, 1, 1), 4, 0, -80 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT], "http://www.humus.name", Color(1, 1, 1), 4, 0, -85 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT], "mudkip avatar from:", Color(1, 1, 1), 4, 0, -95 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT], "https://your-guide-to-pokemon", Color(1, 1, 1), 4, 0, -100 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT], ".fandom.com/wiki/Chris%27s_Mudkip", Color(1, 1, 1), 4, 0, -105 + rotateAngle);
								
	RenderTextOnScreen(meshList[GEO_TEXT], "Mudkip model by:", Color(1, 1, 1), 4, 0, -115 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT], "Ming Jie", Color(1, 1, 1), 4, 0, -120 + rotateAngle);
								
	RenderTextOnScreen(meshList[GEO_TEXT], "Water drop in UI from:", Color(1, 1, 1), 4, 0, -130 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT], "https://uxwing.com/water-drop-icon/", Color(1, 1, 1), 4, 0, -135 + rotateAngle);
								
	RenderTextOnScreen(meshList[GEO_TEXT], "other assets are from", Color(1, 1, 1), 4, 0, -145 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT], "Kenny", Color(1, 1, 1), 4, 0, -150 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT], "www.kenney.nl", Color(1, 1, 1), 4, 0, -155 + rotateAngle);
}

void SP2::Exit()
{
	// Cleanup VBO here
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
	modelStack.Translate(0 + skyboxoffset.x, skyboxoffset.y, -500 + skyboxoffset.z);
	modelStack.Scale(1004, 1004, 1004);
	RenderMesh(meshList[GEO_FRONT], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0 + skyboxoffset.x, skyboxoffset.y, 500 + skyboxoffset.z);
	modelStack.Rotate(180, 0, 1, 0);
	modelStack.Scale(1004, 1004, 1004);
	RenderMesh(meshList[GEO_BACK], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-500 + skyboxoffset.x, skyboxoffset.y, 0 + skyboxoffset.z);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(1004, 1004, 1004);
	RenderMesh(meshList[GEO_LEFT], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(500 + skyboxoffset.x, skyboxoffset.y, 0 + skyboxoffset.z);
	modelStack.Rotate(-90, 0, 1, 0);
	modelStack.Scale(1004, 1004, 1004);
	RenderMesh(meshList[GEO_RIGHT], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0 + skyboxoffset.x, 500 + skyboxoffset.y, 0 + skyboxoffset.z);
	//modelStack.Rotate(0, 0, 1, 0);
	modelStack.Rotate(90, 1, 0, 0);
	modelStack.Scale(1004, 1004, 1004);
	RenderMesh(meshList[GEO_TOP], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0 + skyboxoffset.x, -500 + skyboxoffset.y, 0 + skyboxoffset.z);
	//modelStack.Rotate(-90, 0, 1, 0);
	modelStack.Rotate(-90, 1, 0, 0);
	modelStack.Scale(1004, 1004, 1004);
	RenderMesh(meshList[GEO_BOTTOM], false);
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


void SP2::RenderMeshOnScreen(Mesh* mesh, Vector3 size, Vector3 rotate, float x, float y)
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
	modelStack.Rotate(rotate.x, 1, 0, 0);
	modelStack.Rotate(rotate.y, 0, 1, 0);
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

#include "SceneAssignment2.h"
#include "GL\glew.h"

#include "shader.hpp"
#include "Mtx44.h"

#include "Application.h"
#include "MeshBuilder.h"

#include "Utility.h"
#include "LoadTGA.h"


SceneAssignment2::SceneAssignment2()
{

fsdaerv avsgrreargverwr thrtwtrhetfw4vr 
}

SceneAssignment2::~SceneAssignment2()
{
}

void SceneAssignment2::Init()
{
	{// Set background color to black
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

	meshList[GEO_FIST] = MeshBuilder::GenerateSphere("sphere", Color(205.f / 255.f, 173.f / 255.f, 134.f / 255.f), 10, 10, 1);
	meshList[GEO_FIST]->material.kAmbient.Set(155.f / 255.f, 123.f / 255.f, 84.f / 255.f);
	meshList[GEO_FIST]->material.kDiffuse.Set(205.f / 255.f, 173.f / 255.f, 134.f / 255.f);
	meshList[GEO_FIST]->material.kSpecular.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_FIST]->material.kShininess = .5f;

	//ground
	meshList[GEO_QUAD] = MeshBuilder::GenerateQuad("quad", Color(1, 1, 1), 1.f, 8);
	meshList[GEO_QUAD]->textureID = LoadTGA("Image//grass.tga");

	//sky box
	meshList[GEO_FRONT] = MeshBuilder::GenerateQuad("front", Color(1, 1, 1), 1.f);
	meshList[GEO_FRONT]->textureID = LoadTGA("Image//front.tga");
	meshList[GEO_BACK] = MeshBuilder::GenerateQuad("back", Color(1, 1, 1), 1.f);
	meshList[GEO_BACK]->textureID = LoadTGA("Image//back.tga");
	meshList[GEO_LEFT] = MeshBuilder::GenerateQuad("left", Color(1, 1, 1), 1.f);
	meshList[GEO_LEFT]->textureID = LoadTGA("Image//left.tga");
	meshList[GEO_RIGHT] = MeshBuilder::GenerateQuad("right", Color(1, 1, 1), 1.f);
	meshList[GEO_RIGHT]->textureID = LoadTGA("Image//right.tga");
	meshList[GEO_TOP] = MeshBuilder::GenerateQuad("top", Color(1, 1, 1), 1.f);
	meshList[GEO_TOP]->textureID = LoadTGA("Image//top.tga");
	meshList[GEO_BOTTOM] = MeshBuilder::GenerateQuad("bottom", Color(1, 1, 1), 1.f);
	meshList[GEO_BOTTOM]->textureID = LoadTGA("Image//bottom.tga");


	//mudkip
	meshList[GEO_MUDKIP_SPHERE] = MeshBuilder::GenerateSphere("Mudkip Sphere", Color(0.f, 158.0 / 255.0f, 197.0 / 255.0f), 20, 20, 3);
	meshList[GEO_MUDKIP_SPHERE]->material.kAmbient.Set(0.f, 108.0 / 255.0f, 147.0 / 255.0f);
	meshList[GEO_MUDKIP_SPHERE]->material.kDiffuse.Set(0.f, 168.0 / 255.0f, 207.0 / 255.0f);
	meshList[GEO_MUDKIP_SPHERE]->material.kSpecular.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_MUDKIP_SPHERE]->material.kShininess = .5f;

	meshList[GEO_MUDKIP_CHEEKS] = MeshBuilder::GenerateSphere("Mudkip Cheeks", Color(205.0 / 255.0f, 111.0 / 255.0f, 0.0 / 255.0f), 20, 20, 3);
	meshList[GEO_MUDKIP_CHEEKS]->material.kAmbient.Set(205.0 / 255.0f, 111.0 / 255.0f, 0.0 / 255.0f);
	meshList[GEO_MUDKIP_CHEEKS]->material.kDiffuse.Set(205.0 / 255.0f, 111.0 / 255.0f, 0.0 / 255.0f);
	meshList[GEO_MUDKIP_CHEEKS]->material.kSpecular.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_MUDKIP_CHEEKS]->material.kShininess = .5f;

	meshList[GEO_MUDKIP_TAIL] = MeshBuilder::GenerateCone("Mudkip tail", Color(166.0 / 255.0f, 243.0 / 255.0f, 255.0 / 255.0f), 20, 15, 20);
	meshList[GEO_MUDKIP_TAIL]->material.kAmbient.Set(106.0 / 255.0f, 183.0 / 255.0f, 195.0 / 255.0f);
	meshList[GEO_MUDKIP_TAIL]->material.kDiffuse.Set(106.0 / 255.0f, 183.0 / 255.0f, 195.0 / 255.0f);
	meshList[GEO_MUDKIP_TAIL]->material.kSpecular.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_MUDKIP_TAIL]->material.kShininess = .5f;

	meshList[GEO_MUDKIP_CONE] = MeshBuilder::GenerateCone("mudkip cone", Color(205.0 / 255.0f, 111.0 / 255.0f, 0.0 / 255.0f), 10, 3, 8);
	meshList[GEO_MUDKIP_CONE]->material.kAmbient.Set(205.0 / 255.0f, 111.0 / 255.0f, 0.0 / 255.0f);
	meshList[GEO_MUDKIP_CONE]->material.kDiffuse.Set(205.0 / 255.0f, 111.0 / 255.0f, 0.0 / 255.0f);
	meshList[GEO_MUDKIP_CONE]->material.kSpecular.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_MUDKIP_CONE]->material.kShininess = 0.5f;

	meshList[GEO_MUDKIP_WHITE_SPOT] = MeshBuilder::GenerateSphere("mudkip white spot", Color(1.0f, 1.0f, 1.0f), 20, 20, 10);
	meshList[GEO_MUDKIP_WHITE_SPOT]->material.kAmbient.Set(0.8f, 0.8f, 0.8f);
	meshList[GEO_MUDKIP_WHITE_SPOT]->material.kDiffuse.Set(0.8f, 0.8f, 0.8f);
	meshList[GEO_MUDKIP_WHITE_SPOT]->material.kSpecular.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_MUDKIP_WHITE_SPOT]->material.kShininess = 0.5f;

	meshList[GEO_MUDKIP_LIGHTER_SPHERE] = MeshBuilder::GenerateSphere("mudkip lighter sphere", Color(194.0 / 255.0f, 244.0 / 255.0f, 255.0 / 255.0f), 20, 20, 10);
	meshList[GEO_MUDKIP_LIGHTER_SPHERE]->material.kAmbient.Set(104.0 / 255.0f, 154.0 / 255.0f, 165.0 / 255.0f);
	meshList[GEO_MUDKIP_LIGHTER_SPHERE]->material.kDiffuse.Set(194.0 / 255.0f, 244.0 / 255.0f, 255.0 / 255.0f);
	meshList[GEO_MUDKIP_LIGHTER_SPHERE]->material.kSpecular.Set(0.1f, 0.1f, 0.1f);
	meshList[GEO_MUDKIP_LIGHTER_SPHERE]->material.kShininess = 0.5f;

	meshList[GEO_BLACKBALL] = MeshBuilder::GenerateSphere("black ball", Color(1.f, 1.f, 1.f), 20, 20, 10);
	meshList[GEO_MUDKIP_EYES] = MeshBuilder::GenerateQuad("Mudkipeye", Color(0, 0, 0), 1, 1);
	meshList[GEO_MUDKIP_EYES]->textureID = LoadTGA("Image//mudkipeye.tga");


	//houses
	meshList[GEO_HOUSE1] = MeshBuilder::GenerateOBJMTL("modelhouse1", "OBJ//house_rundown.obj", "OBJ//house_rundown.mtl");
	meshList[GEO_HOUSE1]->textureID = LoadTGA("Image//house_rundown.tga");
	objectlist[GEO_HOUSE1].setproperties(Vector3(-19, 0, 20), Vector3(0, -90, 0), Vector3(1, 1, 1));
	objectlist[GEO_HOUSE1].Setuphitbox(Vector3(5.5, 3, 2.5), Vector3(0, 3, 0));
	meshList[GEO_HOUSE2] = MeshBuilder::GenerateOBJMTL("modelhouse2", "OBJ//house_type07.obj", "OBJ//house_type07.mtl");
	objectlist[GEO_HOUSE2].setproperties(Vector3(2, 0, 22), Vector3(0, 0, 0), Vector3(10, 10, 10));
	objectlist[GEO_HOUSE2].Setuphitbox(Vector3(6, 3, 4), Vector3(0, 3, 0));
	meshList[GEO_HOUSE3] = MeshBuilder::GenerateOBJMTL("modelhouse3", "OBJ//house_type02.obj", "OBJ//house_type02.mtl");
	objectlist[GEO_HOUSE3].setproperties(Vector3(2, 0, -2), Vector3(0, 0, 0), Vector3(10, 10, 10));
	objectlist[GEO_HOUSE3].Setuphitbox(Vector3(6, 6, 4.1), Vector3(-0.1, 6, 2.5));
	otherhitboxes[hb_house31].setproperties(Vector3(2, 0, -2), Vector3(0, 0, 0), Vector3(10, 10, 10));
	otherhitboxes[hb_house31].Setuphitbox(Vector3(4, 6, 6.2), Vector3(2, 6, -0.2));
	meshList[GEO_SHOP_STRUCTURE] = MeshBuilder::GenerateOBJMTL("modelshopstruct", "OBJ//scaffolding_structure.obj", "OBJ//scaffolding_structure.mtl");
	meshList[GEO_SHOP_STRUCTURE]->textureID = LoadTGA("Image//metal.tga");
	objectlist[GEO_SHOP_STRUCTURE].setproperties(Vector3(22, 0, -4), Vector3(0, 0, 0), Vector3(4, 4, 8));
	objectlist[GEO_SHOP_STRUCTURE].Setuphitbox(Vector3(2, 2, 4), Vector3(0, 2, 0));
	meshList[GEO_SHOP_ROOF] = MeshBuilder::GenerateOBJMTL("modelshoproof", "OBJ//roofMetal_typeA.obj", "OBJ//roofMetal_typeA.mtl");
	meshList[GEO_SHOP_ROOF]->textureID = LoadTGA("Image//roof_plates.tga");
	objectlist[GEO_SHOP_ROOF].setproperties(Vector3(22, 3.7, -4), Vector3(0, 90, 0), Vector3(9, 4, 5));
	objectlist[GEO_SHOP_ROOF].Setuphitbox(Vector3(2, 2, 4), Vector3(0, 2, 0));
	meshList[GEO_HOUSE4] = MeshBuilder::GenerateOBJMTL("modelhouse4", "OBJ//house_type15.obj", "OBJ//house_type15.mtl");
	objectlist[GEO_HOUSE4].setproperties(Vector3(-20, 0, -3), Vector3(0, 0, 0), Vector3(13, 13, 13));
	objectlist[GEO_HOUSE4].Setuphitbox(Vector3(5, 6, 5), Vector3(0, 6, 0));


	//walls
	meshList[GEO_OUTERWALLS] = MeshBuilder::GenerateOBJMTL("model outerwall", "OBJ//wall_half.obj", "OBJ//wall_half.mtl");
	meshList[GEO_OUTERWALLS]->textureID = LoadTGA("Image//bricks.tga");


	//others
	meshList[GEO_BARREL] = MeshBuilder::GenerateOBJMTL("modelbarrel", "OBJ//barrel.obj", "OBJ//barrel.mtl");
	meshList[GEO_BARREL]->textureID = LoadTGA("Image//barrel.tga");
	objectlist[GEO_BARREL].setproperties(Vector3(22.5, 0, 1), Vector3(0, 0, 0), Vector3(5, 5, 5));
	objectlist[GEO_BARREL].Setuphitbox(Vector3(0.65, 0.75, 0.65), Vector3(0, 0.75, 0));
	otherhitboxes[hb_barrel1].setmesh(GEO_BARREL);
	otherhitboxes[hb_barrel1].setproperties(Vector3(22.5, 0, -9), Vector3(0, 0, 0), Vector3(5, 5, 5));
	otherhitboxes[hb_barrel1].Setuphitbox(Vector3(0.65, 0.75, 0.65), Vector3(0, 0.75, 0));

	meshList[GEO_CRATE] = MeshBuilder::GenerateOBJMTL("modelcrate", "OBJ//crate.obj", "OBJ//crate.mtl");
	meshList[GEO_CRATE]->textureID = LoadTGA("Image//barrel.tga");
	objectlist[GEO_CRATE].setproperties(Vector3(24, 0, 1), Vector3(0, 0, 0), Vector3(5, 5, 5));
	objectlist[GEO_CRATE].Setuphitbox(Vector3(0.75, 0.75, 0.75), Vector3(0, 0.75, 0));
	otherhitboxes[hb_crate1].setmesh(GEO_CRATE);
	otherhitboxes[hb_crate1].setproperties(Vector3(24, 0, -9), Vector3(0, 0, 0), Vector3(5, 5, 5));
	otherhitboxes[hb_crate1].Setuphitbox(Vector3(0.75, 0.75, 0.75), Vector3(0, 0.75, 0));
	otherhitboxes[hb_crate2].setmesh(GEO_CRATE);
	otherhitboxes[hb_crate2].setproperties(Vector3(24, 0, 2.6), Vector3(0, 0, 0), Vector3(5, 5, 5));
	otherhitboxes[hb_crate2].Setuphitbox(Vector3(0.75, 0.75, 0.75), Vector3(0, 0.75, 0));
	otherhitboxes[hb_crate3].setmesh(GEO_CRATE);
	otherhitboxes[hb_crate3].setproperties(Vector3(24, 0, -10.6), Vector3(0, 0, 0), Vector3(5, 5, 5));
	otherhitboxes[hb_crate3].Setuphitbox(Vector3(0.75, 0.75, 0.75), Vector3(0, 0.75, 0));

	meshList[GEO_LAMPPOST] = MeshBuilder::GenerateOBJMTL("modellamppost", "OBJ//lightpost.obj", "OBJ//lightpost.mtl");
	objectlist[GEO_LAMPPOST].setproperties(Vector3(-12, 0, 16), Vector3(0, 0, 0), Vector3(5, 5, 5));
	objectlist[GEO_LAMPPOST].Setuphitbox(Vector3(0.5, 2, 0.5), Vector3(0, 2, 0));
	//set light pos to the lamp post
	light[0].position.Set(objectlist[GEO_LAMPPOST].getposition().x, objectlist[GEO_LAMPPOST].getposition().y + 6, objectlist[GEO_LAMPPOST].getposition().z);
	otherhitboxes[hb_lamppost2].setmesh(GEO_LAMPPOST);
	otherhitboxes[hb_lamppost2].setproperties(Vector3(19, 0, 16), Vector3(0, 0, 0), Vector3(5, 5, 5));
	otherhitboxes[hb_lamppost2].Setuphitbox(Vector3(0.5, 2, 0.5), Vector3(0, 2, 0));
	light[5].position.Set(otherhitboxes[hb_lamppost2].getposition().x, otherhitboxes[hb_lamppost2].getposition().y + 6, otherhitboxes[hb_lamppost2].getposition().z);
	otherhitboxes[hb_lamppost3].setmesh(GEO_LAMPPOST);
	otherhitboxes[hb_lamppost3].setproperties(Vector3(-12, 0, -11), Vector3(0, 0, 0), Vector3(5, 5, 5));
	otherhitboxes[hb_lamppost3].Setuphitbox(Vector3(0.5, 2, 0.5), Vector3(0, 2, 0));
	light[6].position.Set(otherhitboxes[hb_lamppost3].getposition().x, otherhitboxes[hb_lamppost3].getposition().y + 6, otherhitboxes[hb_lamppost3].getposition().z);
	otherhitboxes[hb_lamppost4].setmesh(GEO_LAMPPOST);
	otherhitboxes[hb_lamppost4].setproperties(Vector3(19, 0, -14), Vector3(0, 0, 0), Vector3(5, 5, 5));
	otherhitboxes[hb_lamppost4].Setuphitbox(Vector3(0.5, 2, 0.5), Vector3(0, 2, 0));
	light[7].position.Set(otherhitboxes[hb_lamppost4].getposition().x, otherhitboxes[hb_lamppost4].getposition().y + 6, otherhitboxes[hb_lamppost4].getposition().z);
	meshList[GEO_BED] = MeshBuilder::GenerateOBJMTL("modelbed", "OBJ//bedSingle.obj", "OBJ//bedSingle.mtl");
	objectlist[GEO_BED].setproperties(Vector3(-22, -7, 19.5), Vector3(0, 0, 0), Vector3(3, 3, 3));
	objectlist[GEO_BED].Setuphitbox(Vector3(1, 0.5, 2), Vector3(-2, 0.5, 2));
	otherhitboxes[hb_bed].setmesh(GEO_BED);
	otherhitboxes[hb_bed].setproperties(Vector3(-62, -7, 19.5), Vector3(0, 0, 0), Vector3(3, 3, 3));
	otherhitboxes[hb_bed].Setuphitbox(Vector3(1, 0.5, 2), Vector3(-2, 0.5, 2));
	meshList[GEO_DESK] = MeshBuilder::GenerateOBJMTL("model desk", "OBJ//desk.obj", "OBJ//desk.mtl");
	objectlist[GEO_DESK].setproperties(Vector3(-15, -7, 18.5), Vector3(0, 90, 0), Vector3(3, 3, 3));
	objectlist[GEO_DESK].Setuphitbox(Vector3(0.4, 0.5, 1), Vector3(0.7, 0.5, 1));
	otherhitboxes[hb_table].setmesh(GEO_DESK);
	otherhitboxes[hb_table].setproperties(Vector3(-55, -7, 18.5), Vector3(0, 90, 0), Vector3(3, 3, 3));
	otherhitboxes[hb_table].Setuphitbox(Vector3(0.4, 0.5, 1), Vector3(0.7, 0.5, 1));
	meshList[GEO_LAPTOP] = MeshBuilder::GenerateOBJMTL("model laptop", "OBJ//laptop.obj", "OBJ//laptop.mtl");
	objectlist[GEO_LAPTOP].setproperties(Vector3(-15, -5.8, 19.5), Vector3(0, 90, 0), Vector3(3, 3, 3));
	objectlist[GEO_LAPTOP].Setuphitbox(Vector3(0, 0, 0), Vector3(0, 0, 0));
	meshList[GEO_HOMELAMP] = MeshBuilder::GenerateOBJMTL("model lamp", "OBJ//lampWall.obj", "OBJ//lampWall.mtl");
	objectlist[GEO_HOMELAMP].setproperties(Vector3(-19, -4.5, 22.8), Vector3(0, 0, 0), Vector3(3, 3, 3));
	objectlist[GEO_HOMELAMP].Setuphitbox(Vector3(0, 0, 0), Vector3(0, -50, 0));
	otherhitboxes[hb_lamp].setmesh(GEO_HOMELAMP);
	otherhitboxes[hb_lamp].setproperties(Vector3(-59, -4.5, 22.8), Vector3(0, 0, 0), Vector3(3, 3, 3));
	otherhitboxes[hb_lamp].Setuphitbox(Vector3(0, 0, 0), Vector3(0, -50, 0));
	meshList[GEO_WATER] = MeshBuilder::GenerateQuad("modelwater", Color(0.6901961f, 0.9568627f, 1), 1);
	meshList[GEO_WATER]->material.kAmbient.Set(0.3901961f, 0.6568627f, 0.7);
	meshList[GEO_WATER]->material.kDiffuse.Set(0.6901961f, 0.9568627f, 1);
	meshList[GEO_WATER]->material.kSpecular.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_WATER]->material.kShininess = .5f;
	otherhitboxes[hb_water].setmesh(GEO_WATER);
	otherhitboxes[hb_water].setproperties(Vector3(10, -20, 0), Vector3(0, -90, 0), Vector3(20, 20, 20));
	otherhitboxes[hb_water].Setuphitbox(Vector3(0, 0, 0), Vector3(0, 0, 0));
	meshList[GEO_BOAT] = MeshBuilder::GenerateOBJMTL("modelboat", "OBJ//canoe.obj", "OBJ//canoe.mtl");
	otherhitboxes[hb_BOAT1].setmesh(GEO_BOAT);
	otherhitboxes[hb_BOAT1].setproperties(Vector3(10, -22.5, 0), Vector3(90, 180, -90), Vector3(1, 1, 1));
	otherhitboxes[hb_BOAT1].Setuphitbox(Vector3(0.2, 0.6, 0.2), Vector3(0, 0, 0));
	meshList[GEO_GROUND] = MeshBuilder::GenerateOBJMTL("modeground", "OBJ//cliff_block_rock.obj", "OBJ//cliff_block_rock.mtl");
	otherhitboxes[hb_side1].setmesh(GEO_GROUND);
	otherhitboxes[hb_side1].setproperties(Vector3(10, -20, 5), Vector3(90, 180, -90), Vector3(5.5, 1, 15));
	otherhitboxes[hb_side1].Setuphitbox(Vector3(0, 0, 0), Vector3(0, 0, 0));
	otherhitboxes[hb_side2].setmesh(GEO_GROUND);
	otherhitboxes[hb_side2].setproperties(Vector3(10, -20, -5), Vector3(90, 180, -90), Vector3(5.5, 1, 15));
	otherhitboxes[hb_side2].Setuphitbox(Vector3(0, 0, 0), Vector3(0, 0, 0));
	meshList[GEO_ROCK] = MeshBuilder::GenerateOBJMTL("modelrock", "OBJ//rock_smallE.obj", "OBJ//rock_smallE.mtl");
	otherhitboxes[hb_ROCK1].setmesh(GEO_ROCK);
	otherhitboxes[hb_ROCK1].setproperties(Vector3(10, -15, -2 + (float)(rand() % 16) * 0.25), Vector3(90, 180, -90), Vector3(1.5, 1.5, 1.5));
	otherhitboxes[hb_ROCK1].Setuphitbox(Vector3(0.2, 0.2, 0.2), Vector3(0, 0, 0));
	otherhitboxes[hb_ROCK2].setmesh(GEO_ROCK);
	otherhitboxes[hb_ROCK2].setproperties(Vector3(10, -12.5, -2 + (float)(rand() % 16) * 0.25), Vector3(90, 180, -90), Vector3(1.5, 1.5, 1.5));
	otherhitboxes[hb_ROCK2].Setuphitbox(Vector3(0.2, 0.2, 0.2), Vector3(0, 0, 0));
	otherhitboxes[hb_ROCK3].setmesh(GEO_ROCK);
	otherhitboxes[hb_ROCK3].setproperties(Vector3(10, -10, -2 + (float)(rand() % 16) * 0.25), Vector3(90, 180, -90), Vector3(1.5, 1.5, 1.5));
	otherhitboxes[hb_ROCK3].Setuphitbox(Vector3(0.2, 0.2, 0.2), Vector3(0, 0, 0));
	otherhitboxes[hb_ROCK4].setmesh(GEO_ROCK);
	otherhitboxes[hb_ROCK4].setproperties(Vector3(10, -7.5, -2 + (float)(rand() % 16) * 0.25), Vector3(90, 180, -90), Vector3(1.5, 1.5, 1.5));
	otherhitboxes[hb_ROCK4].Setuphitbox(Vector3(0.2, 0.2, 0.2), Vector3(0, 0, 0));
	meshList[GEO_TREE] = MeshBuilder::GenerateOBJMTL("modeltree", "OBJ//tree_default_dark.obj", "OBJ//tree_default_dark.mtl");
	otherhitboxes[hb_tree1].setmesh(GEO_TREE);
	otherhitboxes[hb_tree1].setproperties(Vector3(-20, 0, -23), Vector3(0, 0, 0), Vector3(10, 7, 10));
	otherhitboxes[hb_tree1].Setuphitbox(Vector3(1, 6, 1), Vector3(0, 4, 0));
	otherhitboxes[hb_tree2].setmesh(GEO_TREE);
	otherhitboxes[hb_tree2].setproperties(Vector3(-10, 0, -23), Vector3(0, 0, 0), Vector3(10, 7, 10));
	otherhitboxes[hb_tree2].Setuphitbox(Vector3(1, 6, 1), Vector3(0, 4, 0));
	otherhitboxes[hb_tree3].setmesh(GEO_TREE);
	otherhitboxes[hb_tree3].setproperties(Vector3(0, 0, -23), Vector3(0, 0, 0), Vector3(10, 7, 10));
	otherhitboxes[hb_tree3].Setuphitbox(Vector3(1, 6, 1), Vector3(0, 4, 0));
	otherhitboxes[hb_tree4].setmesh(GEO_TREE);
	otherhitboxes[hb_tree4].setproperties(Vector3(10, 0, -23), Vector3(0, 0, 0), Vector3(10, 7, 10));
	otherhitboxes[hb_tree4].Setuphitbox(Vector3(1, 6, 1), Vector3(0, 4, 0));
	otherhitboxes[hb_newtree5].setmesh(GEO_TREE);
	otherhitboxes[hb_newtree5].setproperties(Vector3(20, 0, -23), Vector3(0, 0, 0), Vector3(10, 7, 10));
	otherhitboxes[hb_newtree5].Setuphitbox(Vector3(0, 0, 0), Vector3(0, 0, 0));
	meshList[GEO_TREERING] = MeshBuilder::GenerateOBJMTL("modeltreering", "OBJ//campfire_stones.obj", "OBJ//campfire_stones.mtl");
	objectlist[GEO_TREERING].setproperties(Vector3(20, 0, -23), Vector3(0, 0, 0), Vector3(7, 7, 7));
	objectlist[GEO_TREERING].Setuphitbox(Vector3(0, 0, 0), Vector3(0, -50, 0));

	meshList[GEO_HITBOX] = MeshBuilder::GenerateCube("modelhitbox", Color(0, 0, 0), 1);
	meshList[GEO_HITBOX]->textureID = LoadTGA("Image//hitbox2.tga");
	meshList[GEO_HITBOX2] = MeshBuilder::GenerateCube("modelhitbox", Color(0, 0, 0), 1);
	meshList[GEO_HITBOX2]->textureID = LoadTGA("Image//hitbox3.tga");

	//text
	meshList[GEO_TEXT_CALIBRI] = MeshBuilder::GenerateText("textcalibri", 16, 16);
	meshList[GEO_TEXT_CALIBRI]->textureID = LoadTGA("Image//calibri.tga");
	meshList[GEO_TEXT_COMICSANS] = MeshBuilder::GenerateText("textcomicsans", 16, 16);
	meshList[GEO_TEXT_COMICSANS]->textureID = LoadTGA("Image//calibri.tga");

	//objects without hitboxes
	meshList[GEO_CONCRETE_PAVEMENT] = MeshBuilder::GenerateQuad("concretepavement", Color(1, 1, 1), 1);
	meshList[GEO_CONCRETE_PAVEMENT]->textureID = LoadTGA("Image//concreteSmooth.tga");
	meshList[GEO_ROAD] = MeshBuilder::GenerateQuad("road", Color(1, 1, 1), 1);
	meshList[GEO_ROAD]->textureID = LoadTGA("Image//asphalt.tga");
	meshList[GEO_TEXTBACKGROUND] = MeshBuilder::GenerateQuad("txtbg", Color(1, 1, 1), 1);
	meshList[GEO_TEXTBACKGROUND]->textureID = LoadTGA("Image//textbox.tga");
	meshList[GEO_MUDKIP_AVATAR] = MeshBuilder::GenerateQuad("mudkip avatar", Color(1, 1, 1), 1);
	meshList[GEO_MUDKIP_AVATAR]->textureID = LoadTGA("Image//mudkipa.tga");
	meshList[GEO_UIBACKGROUND] = MeshBuilder::GenerateQuad("uibg", Color(1, 1, 1), 1);
	meshList[GEO_UIBACKGROUND]->textureID = LoadTGA("Image//UIBOX.tga");
	meshList[GEO_LOG] = MeshBuilder::GenerateQuad("uilog", Color(1, 1, 1), 1);
	meshList[GEO_LOG]->textureID = LoadTGA("Image//log_stack.tga");
	meshList[GEO_WATERDROP] = MeshBuilder::GenerateQuad("uiwaterdrop", Color(1, 1, 1), 1);
	meshList[GEO_WATERDROP]->textureID = LoadTGA("Image//water-drop.tga");
	meshList[GEO_DOORWAY] = MeshBuilder::GenerateOBJMTL("modeldoorway", "OBJ//doorway.obj", "OBJ//doorway.mtl");
	otherhitboxes[hb_house1_door].setmesh(GEO_DOORWAY);
	otherhitboxes[hb_house1_door].setproperties(Vector3(-20, -7, 17.3), Vector3(0, 180, 0), Vector3(3, 3, 3));
	otherhitboxes[hb_house1_door].Setuphitbox(Vector3(0, 0, 0), Vector3(0, 0, 0));
	otherhitboxes[hb_house2_door].setmesh(GEO_DOORWAY);
	otherhitboxes[hb_house2_door].setproperties(Vector3(-60, -7, 17.3), Vector3(0, 180, 0), Vector3(3, 3, 3));
	otherhitboxes[hb_house2_door].Setuphitbox(Vector3(0, 0, 0), Vector3(0, 0, 0));
	meshList[GEO_HOUSE_WALL] = MeshBuilder::GenerateOBJMTL("modelhousewall", "OBJ//wall.obj", "OBJ//wall.mtl");
	otherhitboxes[hb_house1_wall_back].setmesh(GEO_HOUSE_WALL);
	otherhitboxes[hb_house1_wall_back].setproperties(Vector3(-25, -7, 23), Vector3(0, 180, 0), Vector3(11, 3, 5));
	otherhitboxes[hb_house1_wall_back].Setuphitbox(Vector3(5.5, 2, 1), Vector3(5.5, 2, 0.5));
	otherhitboxes[hb_house1_wall_right].setmesh(GEO_HOUSE_WALL);
	otherhitboxes[hb_house1_wall_right].setproperties(Vector3(-25, -7, 17), Vector3(0, 90, 0), Vector3(6, 3, 5));
	otherhitboxes[hb_house1_wall_right].Setuphitbox(Vector3(1, 2, 5.5), Vector3(-0.5, 2, 5.5));
	otherhitboxes[hb_house1_roof].setmesh(GEO_HOUSE_WALL);
	otherhitboxes[hb_house1_roof].setproperties(Vector3(-14, -3, 17), Vector3(90, 0, 0), Vector3(11, 6, 3));
	otherhitboxes[hb_house1_roof].Setuphitbox(Vector3(0, 0, 0), Vector3(0, 0, 50));
	otherhitboxes[hb_house2_wall_back].setmesh(GEO_HOUSE_WALL);
	otherhitboxes[hb_house2_wall_back].setproperties(Vector3(-65, -7, 23), Vector3(0, 180, 0), Vector3(11, 3, 5));
	otherhitboxes[hb_house2_wall_back].Setuphitbox(Vector3(5.5, 2, 1), Vector3(5.5, 2, 0.5));
	otherhitboxes[hb_house2_wall_right].setmesh(GEO_HOUSE_WALL);
	otherhitboxes[hb_house2_wall_right].setproperties(Vector3(-65, -7, 17), Vector3(0, 90, 0), Vector3(6, 3, 5));
	otherhitboxes[hb_house2_wall_right].Setuphitbox(Vector3(1, 2, 5.5), Vector3(-0.5, 2, 5.5));
	otherhitboxes[hb_house2_roof].setmesh(GEO_HOUSE_WALL);
	otherhitboxes[hb_house2_roof].setproperties(Vector3(-54, -3, 17), Vector3(90, 0, 0), Vector3(11, 6, 3));
	otherhitboxes[hb_house2_roof].Setuphitbox(Vector3(0, 0, 0), Vector3(0, 0, 50));
	meshList[GEO_HOUSE_WALL_WINDOW] = MeshBuilder::GenerateOBJMTL("modelhousewallwindow", "OBJ//wallWindow.obj", "OBJ//wallWindow.mtl");
	otherhitboxes[hb_house1_wall_front_window1].setmesh(GEO_HOUSE_WALL_WINDOW);
	otherhitboxes[hb_house1_wall_front_window1].setproperties(Vector3(-14, -7, 17), Vector3(0, 0, 0), Vector3(5.5, 3, 3));
	otherhitboxes[hb_house1_wall_front_window1].Setuphitbox(Vector3(2.75, 2, 1), Vector3(0, 2, -1));
	otherhitboxes[hb_house1_wall_front_window2].setmesh(GEO_HOUSE_WALL_WINDOW);
	otherhitboxes[hb_house1_wall_front_window2].setproperties(Vector3(-19.5, -7, 17), Vector3(0, 0, 0), Vector3(5.5, 3, 3));
	otherhitboxes[hb_house1_wall_front_window2].Setuphitbox(Vector3(2.75, 2, 1), Vector3(0, 2, -1));
	otherhitboxes[hb_house1_wall_right_window].setmesh(GEO_HOUSE_WALL_WINDOW);
	otherhitboxes[hb_house1_wall_right_window].setproperties(Vector3(-14, -7, 23), Vector3(0, -90, 0), Vector3(6, 3, 3));
	otherhitboxes[hb_house1_wall_right_window].Setuphitbox(Vector3(1, 2, 3), Vector3(1, 2, -3));
	otherhitboxes[hb_house2_wall_front_window1].setmesh(GEO_HOUSE_WALL_WINDOW);
	otherhitboxes[hb_house2_wall_front_window1].setproperties(Vector3(-54, -7, 17), Vector3(0, 0, 0), Vector3(5.5, 3, 3));
	otherhitboxes[hb_house2_wall_front_window1].Setuphitbox(Vector3(2.75, 2, 1), Vector3(0, 2, -1));
	otherhitboxes[hb_house2_wall_front_window2].setmesh(GEO_HOUSE_WALL_WINDOW);
	otherhitboxes[hb_house2_wall_front_window2].setproperties(Vector3(-59.5, -7, 17), Vector3(0, 0, 0), Vector3(5.5, 3, 3));
	otherhitboxes[hb_house2_wall_front_window2].Setuphitbox(Vector3(2.75, 2, 1), Vector3(0, 2, -1));
	otherhitboxes[hb_house2_wall_right_window].setmesh(GEO_HOUSE_WALL_WINDOW);
	otherhitboxes[hb_house2_wall_right_window].setproperties(Vector3(-54, -7, 23), Vector3(0, -90, 0), Vector3(6, 3, 3));
	otherhitboxes[hb_house2_wall_right_window].Setuphitbox(Vector3(1, 2, 3), Vector3(1, 2, -3));
	meshList[GEO_HOUSE_FLOOR] = MeshBuilder::GenerateOBJMTL("modelhousefloor", "OBJ//floorFull.obj", "OBJ//floorFull.mtl");
	otherhitboxes[hb_house1_floor].setmesh(GEO_HOUSE_FLOOR);
	otherhitboxes[hb_house1_floor].setproperties(Vector3(-14, -7, 17), Vector3(0, 0, 0), Vector3(11, 3, 6));
	otherhitboxes[hb_house1_floor].Setuphitbox(Vector3(0, 0, 0), Vector3(0, 0, 0));
	otherhitboxes[hb_house2_floor].setmesh(GEO_HOUSE_FLOOR);
	otherhitboxes[hb_house2_floor].setproperties(Vector3(-54, -7, 17), Vector3(0, 0, 0), Vector3(11, 3, 6));
	otherhitboxes[hb_house2_floor].Setuphitbox(Vector3(0, 0, 0), Vector3(0, 0, 0));


	//setup player's hitbox
	player.Setposition(Vector3(0, 2, 0));
	player.Setuphitbox(Vector3(1, 1.5, 1), Vector3(0, 0.5, 0));

	//setup interaction boxes.
	interactionhitboxes[i_light1].setproperties(objectlist[GEO_LAMPPOST].getposition(), Vector3(0, 0, 0), Vector3(1, 1, 1));
	interactionhitboxes[i_light1].Setuphitbox(Vector3(1.3, 2, 1.3), Vector3(0, 2, 0));
	interactionhitboxes[i_light2].setproperties(otherhitboxes[hb_lamppost2].getposition(), Vector3(0, 0, 0), Vector3(1, 1, 1));
	interactionhitboxes[i_light2].Setuphitbox(Vector3(1.3, 2, 1.3), Vector3(0, 2, 0));
	interactionhitboxes[i_light3].setproperties(otherhitboxes[hb_lamppost3].getposition(), Vector3(0, 0, 0), Vector3(1, 1, 1));
	interactionhitboxes[i_light3].Setuphitbox(Vector3(1.3, 2, 1.3), Vector3(0, 2, 0));
	interactionhitboxes[i_light4].setproperties(otherhitboxes[hb_lamppost4].getposition(), Vector3(0, 0, 0), Vector3(1, 1, 1));
	interactionhitboxes[i_light4].Setuphitbox(Vector3(1.3, 2, 1.3), Vector3(0, 2, 0));
	interactionhitboxes[i_shop].setproperties(objectlist[GEO_SHOP_STRUCTURE].getposition(), Vector3(0, 0, 0), Vector3(1, 1, 1));
	interactionhitboxes[i_shop].Setuphitbox(Vector3(3, 2, 5), Vector3(-1, 2, 0));
	interactionhitboxes[i_plant_tree].setproperties(objectlist[GEO_TREERING].getposition(), Vector3(0, 0, 0), Vector3(1, 1, 1));
	interactionhitboxes[i_plant_tree].Setuphitbox(Vector3(3, 2, 3), Vector3(0, 2, 0.5));
	interactionhitboxes[i_home1_frontdoor].setproperties(objectlist[GEO_HOUSE1].getposition(), Vector3(0, 0, 0), Vector3(1, 1, 1));
	interactionhitboxes[i_home1_frontdoor].Setuphitbox(Vector3(1, 2, 1), Vector3(0, 2, -2));
	interactionhitboxes[i_home2_frontdoor].setproperties(objectlist[GEO_HOUSE2].getposition(), Vector3(0, 0, 0), Vector3(1, 1, 1));
	interactionhitboxes[i_home2_frontdoor].Setuphitbox(Vector3(1, 2, 1), Vector3(0, 2, -4));
	interactionhitboxes[i_laptop].setproperties(objectlist[GEO_LAPTOP].getposition(), Vector3(0, 0, 0), Vector3(1, 1, 1));
	interactionhitboxes[i_laptop].Setuphitbox(Vector3(1, 1, 1), Vector3(-0.5, 0, 0.5));
	interactionhitboxes[i_home1_frontdoor_inside].setproperties(objectlist[GEO_HOUSE1].getposition(), Vector3(0, 0, 0), Vector3(1, 1, 1));
	interactionhitboxes[i_home1_frontdoor_inside].Setuphitbox(Vector3(1, 2, 1), Vector3(0, -5, -3.5));
	interactionhitboxes[i_home3_back].setproperties(objectlist[GEO_HOUSE3].getposition(), Vector3(0, 0, 0), Vector3(1, 1, 1));
	interactionhitboxes[i_home3_back].Setuphitbox(Vector3(1, 2, 1), Vector3(0, 2, 7.5));
	interactionhitboxes[i_home3_garage].setproperties(objectlist[GEO_HOUSE3].getposition(), Vector3(0, 0, 0), Vector3(1, 1, 1));
	interactionhitboxes[i_home3_garage].Setuphitbox(Vector3(2, 2, 1), Vector3(-4, 2, -2));
	interactionhitboxes[i_home3_frontdoor].setproperties(objectlist[GEO_HOUSE3].getposition(), Vector3(0, 0, 0), Vector3(1, 1, 1));
	interactionhitboxes[i_home3_frontdoor].Setuphitbox(Vector3(1, 2, 1), Vector3(0, 2, -6));
	interactionhitboxes[i_home4_frontdoor].setproperties(objectlist[GEO_HOUSE4].getposition(), Vector3(0, 0, 0), Vector3(1, 1, 1));
	interactionhitboxes[i_home4_frontdoor].Setuphitbox(Vector3(1, 2, 1), Vector3(-2.5, 2, -5));
	interactionhitboxes[i_home4_backdoor].setproperties(objectlist[GEO_HOUSE4].getposition(), Vector3(0, 0, 0), Vector3(1, 1, 1));
	interactionhitboxes[i_home4_backdoor].Setuphitbox(Vector3(1, 2, 1), Vector3(2.5, 2, 5));


	interactionhitboxes[i_exit].setproperties(Vector3(14.5, 0, 25.5), Vector3(0, 0, 0), Vector3(1, 1, 1));
	interactionhitboxes[i_exit].Setuphitbox(Vector3(1, 2, 1), Vector3(0, 2, 0));

	//other hitboxes
	otherhitboxes[hb_border_front].setproperties(Vector3(0, 0, -27), Vector3(0, 0, 0), Vector3(1, 1, 1));
	otherhitboxes[hb_border_front].Setuphitbox(Vector3(27, 5, 1), Vector3(0, 5, 0));
	otherhitboxes[hb_border_back].setproperties(Vector3(0, 0, 27), Vector3(0, 0, 0), Vector3(1, 1, 1));
	otherhitboxes[hb_border_back].Setuphitbox(Vector3(27, 5, 1), Vector3(0, 5, 0));
	otherhitboxes[hb_border_left].setproperties(Vector3(-27, 0, 0), Vector3(0, 0, 0), Vector3(1, 1, 1));
	otherhitboxes[hb_border_left].Setuphitbox(Vector3(1, 5, 27), Vector3(0, 5, 0));
	otherhitboxes[hb_border_right].setproperties(Vector3(27, 0, 0), Vector3(0, 0, 0), Vector3(1, 1, 1));
	otherhitboxes[hb_border_right].Setuphitbox(Vector3(1, 5, 27), Vector3(0, 5, 0));

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
}

bool SceneAssignment2::Update(double dt)
{
	bool quit = false;
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

bool SceneAssignment2::UpdateMainMenu()
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
void SceneAssignment2::UpdateENV(double dt)
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
				//mouse inside window
				if (!mousehidden)
				{
					ShowCursor(false);
					mousehidden = true;
				}
				if (!ingame || chatting)
				{
					camerapos = camera.Update(dt, initmousepos, !inshop, debug);
				}
				//check for player collision
				if (checkcollision)
				{
					for (unsigned obj = 9; obj < NUM_GEOMETRY; obj++) //check hitboxes in objectlist
					{
						if (!objectlist[obj].getissetup())
							continue;
						else if (objectlist[obj].getobjecthitbox().Checkforcollision(player.getobjecthitbox())) //if the player's hitbox is smaller than the object's hitbox
							camera.gotoprevpos(x, y, z);
						else if (player.getobjecthitbox().Checkforcollision(objectlist[obj].getobjecthitbox()))
							camera.gotoprevpos(x, y, z);
					}
					for (unsigned idx = 0; idx < hb_count; idx++) //check against other hitboxes
					{
						if (!otherhitboxes[idx].getissetup())
							continue;
						else if (otherhitboxes[idx].getobjecthitbox().Checkforcollision(player.getobjecthitbox())) //if the player's hitbox is smaller than the object's hitbox
							camera.gotoprevpos(x, y, z);
						else if (player.getobjecthitbox().Checkforcollision(otherhitboxes[idx].getobjecthitbox()))
							camera.gotoprevpos(x, y, z);
					}
				}
			} //else do nothing with camera
		}

		//check for interactions hitboxes
		for (unsigned idx = 0; idx < i_count; idx++)
		{
			if (!interactionhitboxes[idx].getissetup())
				continue;
			else if (interactionhitboxes[idx].getobjecthitbox().Checkforcollision(player.getobjecthitbox()))
				interact = idx;
			else if (player.getobjecthitbox().Checkforcollision(interactionhitboxes[idx].getobjecthitbox()))
				interact = idx;
		}

		static bool leftpunch = false;
		Vector3 fistvector;
		fistvector = camera.position + camera.view * (2 + playerfist2) + camera.right * 0.8 + camera.view.Cross(camera.right) * 0.7;
		fist1.sethitboxcenterdimensions(Vector3(0.3, 0.3, 0.3), fistvector);
		fistvector = camera.position + camera.view * (2 + playerfist1) - camera.right * 0.8 + camera.view.Cross(camera.right) * 0.7;
		fist2.sethitboxcenterdimensions(Vector3(0.3, 0.3, 0.3), fistvector);
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

		static float punchcooldown = 1;
		if (punch)
		{
			if (leftpunch)
			{
				if (playerfist1extend)
				{
					playerfist1 += 10.f * dt;
					if (playerfist1 > 1)
					{
						playerfist1extend = false;
					}
				}
				else
				{
					playerfist1 -= 10.f * dt;
					if (playerfist1 <= 0)
					{
						playerfist1extend = true;
						leftpunch = false;
					}
				}
			}
			else
			{
				if (playerfist2extend)
				{
					playerfist2 += 10.f * dt;
					if (playerfist2 >= 1)
					{
						playerfist2extend = false;
					}
				}
				else
				{
					playerfist2 -= 10.f * dt;
					if (playerfist2 <= 0)
					{
						playerfist2extend = true;
						leftpunch = true;
					}
				}
			}

			if (punchcooldown >= 0.5)
			{
				//check fist collisions
				for (unsigned idx = hb_tree1; idx <= hb_newtree5; idx++)
				{
					if (otherhitboxes[idx].getobjecthitbox().Checkforcollision(fist1))
					{
						playerwood += playerpunchpower;
						punchcooldown = 0;
						break;
					}
					else if (fist1.Checkforcollision(otherhitboxes[idx].getobjecthitbox()))
					{
						playerwood += playerpunchpower;
						punchcooldown = 0;
						break;
					}
					else if (otherhitboxes[idx].getobjecthitbox().Checkforcollision(fist2))
					{
						playerwood += playerpunchpower;
						punchcooldown = 0;
						break;
					}
					else if (fist2.Checkforcollision(otherhitboxes[idx].getobjecthitbox()))
					{
						playerwood += playerpunchpower;
						punchcooldown = 0;
						break;
					}
				}

				if (objectlist[GEO_SHOP_STRUCTURE].getobjecthitbox().Checkforcollision(fist1))
				{
					pricemultiplier++;
					punchcooldown = 0;
				}
				else if (objectlist[GEO_SHOP_STRUCTURE].getobjecthitbox().Checkforcollision(fist2))
				{
					pricemultiplier++;
					punchcooldown = 0;
				}
			}
		}

		static bool ebuttonstate = false;
		if (GetAsyncKeyState(0x45) && interact == i_light1 && !ebuttonstate)
		{
			if (light[0].power == 0)
			{
				light[0].power = 0.6;
				glUniform1f(m_parameters[U_LIGHT0_POWER], light[0].power);
			}
			else
			{
				light[0].power = 0;
				glUniform1f(m_parameters[U_LIGHT0_POWER], light[0].power);
			}
			ebuttonstate = true;
		}
		else if (GetAsyncKeyState(0x45) && interact == i_light2 && !ebuttonstate)
		{
			if (light[5].power == 0)
			{
				light[5].power = 0.6;
				glUniform1f(m_parameters[U_LIGHT5_POWER], light[5].power);
			}
			else
			{
				light[5].power = 0;
				glUniform1f(m_parameters[U_LIGHT5_POWER], light[5].power);
			}
			ebuttonstate = true;
		}
		else if (GetAsyncKeyState(0x45) && interact == i_light3 && !ebuttonstate)
		{
			if (light[6].power == 0)
			{
				light[6].power = 0.6;
				glUniform1f(m_parameters[U_LIGHT6_POWER], light[6].power);
			}
			else
			{
				light[6].power = 0;
				glUniform1f(m_parameters[U_LIGHT6_POWER], light[6].power);
			}
			ebuttonstate = true;
		}
		else if (GetAsyncKeyState(0x45) && interact == i_light4 && !ebuttonstate)
		{
			if (light[7].power == 0)
			{
				light[7].power = 0.6;
				glUniform1f(m_parameters[U_LIGHT7_POWER], light[7].power);
			}
			else
			{
				light[7].power = 0;
				glUniform1f(m_parameters[U_LIGHT7_POWER], light[7].power);
			}
			ebuttonstate = true;
		}
		else if (GetAsyncKeyState(0x45) && !ebuttonstate && chatting)
		{
			textprogress++;
			if (textprogress == 4)
			{
				camera.MoveCam(Vector3(2, 2, 16.5));
				camera.Settarget(Vector3(2, 2, 15.5));
				chatting = false;
				house2locked = true;
			}
			ebuttonstate = true;
		}
		else if (GetAsyncKeyState(0x45) && interact == i_shop && !ebuttonstate && inshop == false)
		{
			inshop = true;
			shopselect = 0;
			interact = i_count;
			timesincelastbuttonpress = 0;
			ebuttonstate = true;
		}
		else if (interact == i_shop && !ebuttonstate && inshop == true && pricemultiplier < 5)
		{
			static bool wbuttonstate = false, sbuttonstate = false;
			if (GetAsyncKeyState(0x45) && !ebuttonstate && shopselect == 0)
			{
				shopselect = 1;
				ebuttonstate = true;
			}
			if (GetAsyncKeyState(0x45) && !ebuttonstate && shopselect != 0)
			{
				if (shopselect == 1) //for tree seed
				{
					if (playergold >= 100 * pricemultiplier)
					{
						playerhasseed = true;
						playergold = playergold - 100 * pricemultiplier;
						playerboughtitem = true;
						lackofmoney = false;
					}
					else
					{
						lackofmoney = true;
						playerboughtitem = false;
					}
					lackofwood = false;
				}
				else if (shopselect == 2)
				{
					if (playergold >= 10 * pricemultiplier)
					{
						playerwater += 100;
						playergold = playergold - 10 * pricemultiplier;
						playerboughtitem = true;
						lackofmoney = false;
					}
					else
					{
						lackofmoney = true;
						playerboughtitem = false;
					}
					lackofwood = false;
				}
				else if (shopselect == 3)
				{
					if (playergold >= 50 * pricemultiplier)
					{
						playerpunchpower++;
						playergold = playergold - 50 * pricemultiplier;
						playerboughtitem = true;
						lackofmoney = false;
					}
					else
					{
						lackofmoney = true;
						playerboughtitem = false;
					}
					lackofwood = false;
				}
				else if (shopselect == 4)
				{
					if (playerwood > 0)
					{
						playergold = playergold + (5 * playerwood) / pricemultiplier;
						playerwood = 0;
						playerboughtitem = true;
						lackofmoney = false;
						lackofwood = false;
					}
					else
					{
						lackofmoney = false;
						lackofwood = true;
						playerboughtitem = false;
					}
				}
				else if (shopselect == 5)
				{
					if (playergold > 5000 * pricemultiplier)
					{
						playergold = playergold - 5000 * pricemultiplier;
						playerhaskey = true;
						playerboughtitem = true;
						lackofmoney = false;
						lackofwood = false;
					}
					else
					{
						playerboughtitem = false;
						lackofmoney = true;
						lackofwood = false;
					}
				}
				ebuttonstate = true;
			}

			if (wbuttonstate == false && GetAsyncKeyState(0x57) && shopselect != 0) //'W'
			{
				if (shopselect == 1)
					shopselect = 5;
				else
					shopselect--;
				wbuttonstate = true;
			}
			else if (wbuttonstate == true && !GetAsyncKeyState(0x57))
			{
				wbuttonstate = false;
			}

			if (sbuttonstate == false && GetAsyncKeyState(0x53) && shopselect != 0) //'S'
			{
				if (shopselect == 5)
					shopselect = 1;
				else
					shopselect++;
				sbuttonstate = true;
			}
			else if (sbuttonstate == true && !GetAsyncKeyState(0x53))
			{
				sbuttonstate = false;
			}
		}
		else if (GetAsyncKeyState(0x45) && interact == i_shop && !ebuttonstate && inshop && pricemultiplier >= 5)
		{
			inshop = false;
			ebuttonstate = true;
		}
		else if (interact == i_plant_tree && GetAsyncKeyState(0x45) && !ebuttonstate)
		{
			if (!treeplanted && playerhasseed)
			{
				treeplanted = true;
				treegrowthstage = 1;
				playerhasseed = false;
			}
			else if (treeplanted)
			{
				if (playerwater >= 1000 && treegrowthstage <= 5)
				{
					treegrowthstage += 1;
					playerwater -= 1000;
				}
				else
				{
					lackofwater = true;
				}
			}
			else
			{
				lackofwater = true;
			}

			if (treeplanted)
			{
				//update tree 5
				otherhitboxes[hb_newtree5].setproperties(Vector3(20, 0, -23), Vector3(0, 0, 0), Vector3(1.667f * treegrowthstage, 1.1667f * treegrowthstage, 1.667f * treegrowthstage));
				otherhitboxes[hb_newtree5].Setuphitbox(Vector3(1 * treeplanted, 6 * treeplanted, 1 * treeplanted), Vector3(0, 4 * treeplanted, 0));
			}
			ebuttonstate = true;
		}
		else if (interact == i_laptop && !ebuttonstate && GetAsyncKeyState(0x45))
		{
			savedposition = camera.position;
			savedtarget = camera.target;
			ingame = true;
			inshop = true;
			playerlost = false;
			boatalive = true;
			camera.MoveCam(Vector3(0, -20, 0));
			camera.Settarget(Vector3(1, -20, 0));
			camerapos = camera.position;
			playerscore = 0;
			otherhitboxes[hb_ROCK1].Setposition(Vector3(10, -15, -2 + (float)(rand() % 16) * 0.25));
			otherhitboxes[hb_ROCK2].Setposition(Vector3(10, -12.5, -2 + (float)(rand() % 16) * 0.25));
			otherhitboxes[hb_ROCK3].Setposition(Vector3(10, -10, -2 + (float)(rand() % 16) * 0.25));
			otherhitboxes[hb_ROCK4].Setposition(Vector3(10, -7.5, -2 + (float)(rand() % 16) * 0.25));
			gamemovementspeed = 0.5;
			otherhitboxes[hb_BOAT1].Setposition(Vector3(10, -22.5, 0));
		}
		else if (interact == i_home1_frontdoor_inside && !ebuttonstate && GetAsyncKeyState(0x45))
		{
			camera.MoveCam(Vector3(-18, 2, 16));
			ebuttonstate = true;
		}
		else if (interact == i_home1_frontdoor && !ebuttonstate && GetAsyncKeyState(0x45))
		{
			camera.MoveCam(Vector3(-19, -5, 20));
			ebuttonstate = true;
		}
		else if (interact == i_home2_frontdoor && !ebuttonstate && GetAsyncKeyState(0x45))
		{
			if (!house2locked)
			{
				camera.MoveCam(Vector3(-59, -5, 20));
				chatting = true;
				textprogress = 1;
			}
			else
				triedtoopendoor = true;
			ebuttonstate = true;
		}
		else if (interact == i_home3_back && !ebuttonstate && GetAsyncKeyState(0x45))
		{
			triedtoopendoor = true;
			ebuttonstate = true;
		}
		else if (interact == i_home3_garage && !ebuttonstate && GetAsyncKeyState(0x45))
		{
			triedtoopendoor = true;
			ebuttonstate = true;
		}
		else if (interact == i_home3_frontdoor && !ebuttonstate && GetAsyncKeyState(0x45))
		{
			triedtoopendoor = true;
			ebuttonstate = true;
		}
		else if (interact == i_home4_frontdoor && !ebuttonstate && GetAsyncKeyState(0x45))
		{
			triedtoopendoor = true;
			ebuttonstate = true;
		}
		else if (interact == i_home4_backdoor && !ebuttonstate && GetAsyncKeyState(0x45))
		{
			triedtoopendoor = true;
			ebuttonstate = true;
		}
		else if (interact == i_exit && !ebuttonstate && GetAsyncKeyState(0x45) && inshop == false)
		{
			if (playerhaskey)
				inshop = true;
			else
				triedtoopendoor = true;
			ebuttonstate = true;
		}
		else if (interact == i_exit && !ebuttonstate && GetAsyncKeyState(0x45) && inshop == true)
		{
			//reset all the variables
			gamestate = gs_credits;
			ShowCursor(true);
			mousehidden = false;
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
			camera.Reset();
		}
		else if (!GetAsyncKeyState(0x45) && ebuttonstate)
		{
			ebuttonstate = false;
		}

		if (ingame && !playerlost)
		{
			if (GetAsyncKeyState(0x41))
			{
				otherhitboxes[hb_BOAT1].moveposition(Vector3(0, 0, -5 * dt));
				if (otherhitboxes[hb_BOAT1].getposition().z < -2)
				{
					otherhitboxes[hb_BOAT1].Setposition(Vector3(otherhitboxes[hb_BOAT1].getposition().x, otherhitboxes[hb_BOAT1].getposition().y, -2));
				}
				inshop = false;
			}
			if (GetAsyncKeyState(0x44))
			{
				otherhitboxes[hb_BOAT1].moveposition(Vector3(0, 0, 5 * dt));
				if (otherhitboxes[hb_BOAT1].getposition().z > 2)
				{
					otherhitboxes[hb_BOAT1].Setposition(Vector3(otherhitboxes[hb_BOAT1].getposition().x, otherhitboxes[hb_BOAT1].getposition().y, 2));
				}
				inshop = false;
			}
			if (GetAsyncKeyState(0x57))
			{
				otherhitboxes[hb_BOAT1].moveposition(Vector3(0, 5 * dt, 0));
				if (otherhitboxes[hb_BOAT1].getposition().y > -16.5)
				{
					otherhitboxes[hb_BOAT1].Setposition(Vector3(otherhitboxes[hb_BOAT1].getposition().x, -16.5, otherhitboxes[hb_BOAT1].getposition().z));
				}
				inshop = false;
			}
			if (GetAsyncKeyState(0x53))
			{
				otherhitboxes[hb_BOAT1].moveposition(Vector3(0, -5 * dt, 0));
				if (otherhitboxes[hb_BOAT1].getposition().y < -24)
				{
					otherhitboxes[hb_BOAT1].Setposition(Vector3(otherhitboxes[hb_BOAT1].getposition().x, -24, otherhitboxes[hb_BOAT1].getposition().z));
				}
				inshop = false;
			}
			otherhitboxes[hb_ROCK1].moveposition(Vector3(0, -gamemovementspeed * dt, 0));
			otherhitboxes[hb_ROCK2].moveposition(Vector3(0, -gamemovementspeed * dt, 0));
			otherhitboxes[hb_ROCK3].moveposition(Vector3(0, -gamemovementspeed * dt, 0));
			otherhitboxes[hb_ROCK4].moveposition(Vector3(0, -gamemovementspeed * dt, 0));
			gamemovementspeed += 0.1 * dt;;
			playerscore += gamemovementspeed * 0.25;

			//check collisions with boat
			for (unsigned idx = hb_ROCK1; idx <= hb_ROCK4; idx++)
			{
				if (!otherhitboxes[idx].getissetup())
					continue;
				else if (otherhitboxes[idx].getobjecthitbox().Checkforcollision(otherhitboxes[hb_BOAT1].getobjecthitbox()))
					boatalive = false;
				else if (otherhitboxes[hb_BOAT1].getobjecthitbox().Checkforcollision(otherhitboxes[idx].getobjecthitbox()))
					boatalive = false;
			}
			if (boatalive == false)
			{
				playerlost = true;
			}

			//move the blocks back up
			if (otherhitboxes[hb_ROCK1].getposition().y < -24)
			{
				otherhitboxes[hb_ROCK1].Setposition(Vector3(10, -15, -2 + (float)(rand() % 16) * 0.25));
			}
			if (otherhitboxes[hb_ROCK2].getposition().y < -24)
			{
				otherhitboxes[hb_ROCK2].Setposition(Vector3(10, -15, -2 + (float)(rand() % 16) * 0.25));
			}
			if (otherhitboxes[hb_ROCK3].getposition().y < -24)
			{
				otherhitboxes[hb_ROCK3].Setposition(Vector3(10, -15, -2 + (float)(rand() % 16) * 0.25));
			}
			if (otherhitboxes[hb_ROCK4].getposition().y < -24)
			{
				otherhitboxes[hb_ROCK4].Setposition(Vector3(10, -15, -2 + (float)(rand() % 16) * 0.25));
			}
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

		if (GetAsyncKeyState(VK_ESCAPE) && !escbuttonstate) //to exit env
		{
			if (ingame)
			{
				ingame = false;
				inshop = false;
				camera.MoveCam(savedposition);
				camera.Settarget(savedtarget);
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
		punchcooldown += dt;
	}
	else
	{
		ShowCursor(true);
		mousehidden = false;
	}
	timesincelastbuttonpress += dt;
	timesincelastpunch += dt;
}

void SceneAssignment2::UpdateCredits(double dt)
{
	rotateAngle += 10 * dt;
	if (rotateAngle > 225 || GetAsyncKeyState(0x1B))
	{
		rotateAngle = 0;
		gamestate = gs_mainmenu;
	}
}

void SceneAssignment2::Render()
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

void SceneAssignment2::RenderMainmenu()
{
	Color color = Color(0, 0, 0);
	GetWindowRect(window, &rect);
	POINT initmousepos;
	mousepos = &initmousepos;//init mousepos
	GetCursorPos(mousepos); //get mouse position
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Render title
	RenderTextOnScreen(meshList[GEO_TEXT_CALIBRI], "Name??", Color(1, 1, 1), 15, 0, 45);

	//Render buttons
	if (buttonhover == mb_start)
		color = Color(0, 1, 0);
	else
		color = Color(1, 1, 1);
	if (resume)
		RenderTextOnScreen(meshList[GEO_TEXT_CALIBRI], "Resume", color, 5, 0, 40);
	else
		RenderTextOnScreen(meshList[GEO_TEXT_CALIBRI], "Start", color, 5, 0, 40);
	if (buttonhover == mb_credits)
		color = Color(0, 1, 0);
	else
		color = Color(1, 1, 1);
	RenderTextOnScreen(meshList[GEO_TEXT_CALIBRI], "Credits", color, 5, 0, 35);
	if (buttonhover == mb_quit)
		color = Color(0, 1, 0);
	else
		color = Color(1, 1, 1);
	RenderTextOnScreen(meshList[GEO_TEXT_CALIBRI], "Quit", color, 5, 0, 30);

	//Render mouse pos
	UIstringoutput.str("");
	UIstringoutput.precision(4);
	UIstringoutput << "Mousepos X:" << mousepos->x - rect.left << "Y:" << mousepos->y - rect.top;
	RenderTextOnScreen(meshList[GEO_TEXT_CALIBRI], UIstringoutput.str(), Color(0, 1, 0), 2, 0, 0);
	UIstringoutput.str("");
}

void SceneAssignment2::RenderENV()
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
	modelStack.Scale(52, 52, 52);
	RenderMesh(meshList[GEO_QUAD], true);
	modelStack.PopMatrix();

	if (!ingame)
	{
		//render the fists
		modelStack.PushMatrix();
		modelStack.Translate(fist1.gethitboxcenterposition().x, fist1.gethitboxcenterposition().y, fist1.gethitboxcenterposition().z);
		modelStack.Scale(0.3, 0.3, 0.3);
		RenderMesh(meshList[GEO_FIST], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(fist2.gethitboxcenterposition().x, fist2.gethitboxcenterposition().y, fist2.gethitboxcenterposition().z);
		modelStack.Scale(0.3, 0.3, 0.3);
		RenderMesh(meshList[GEO_FIST], true);
		modelStack.PopMatrix();
	}

	//render objects
	for (unsigned idx = 9; idx < GEO_HITBOX; idx++) //render all objects, starts from 9 so that it renders after the skybox
	{
		if (!objectlist[idx].getissetup())
			continue;
		modelStack.PushMatrix();
		modelStack.Translate(objectlist[idx].getposition().x, objectlist[idx].getposition().y, objectlist[idx].getposition().z);
		modelStack.Rotate(objectlist[idx].getrotation().x, 1, 0, 0);
		modelStack.Rotate(objectlist[idx].getrotation().y, 0, 1, 0);
		modelStack.Rotate(objectlist[idx].getrotation().z, 0, 0, 1);
		modelStack.Scale(objectlist[idx].getscale().x, objectlist[idx].getscale().y, objectlist[idx].getscale().z);
		RenderMesh(meshList[idx], true);
		modelStack.PopMatrix();
	}
	for (unsigned idx = 0; idx < hb_border_front; idx++) //for objects
	{
		if (!otherhitboxes[idx].getissetup() || otherhitboxes[idx].getmodel() == 0 || idx == hb_newtree5)
			continue;
		modelStack.PushMatrix();
		modelStack.Translate(otherhitboxes[idx].getposition().x, otherhitboxes[idx].getposition().y, otherhitboxes[idx].getposition().z);
		modelStack.Rotate(otherhitboxes[idx].getrotation().x, 1, 0, 0);
		modelStack.Rotate(otherhitboxes[idx].getrotation().y, 0, 1, 0);
		modelStack.Rotate(otherhitboxes[idx].getrotation().z, 0, 0, 1);
		modelStack.Scale(otherhitboxes[idx].getscale().x, otherhitboxes[idx].getscale().y, otherhitboxes[idx].getscale().z);
		RenderMesh(meshList[otherhitboxes[idx].getmodel()], true);
		modelStack.PopMatrix();
	}

	if (treeplanted)
	{
		modelStack.PushMatrix();
		modelStack.Translate(otherhitboxes[hb_newtree5].getposition().x, otherhitboxes[hb_newtree5].getposition().y, otherhitboxes[hb_newtree5].getposition().z);
		modelStack.Rotate(otherhitboxes[hb_newtree5].getrotation().x, 1, 0, 0);
		modelStack.Rotate(otherhitboxes[hb_newtree5].getrotation().y, 0, 1, 0);
		modelStack.Rotate(otherhitboxes[hb_newtree5].getrotation().z, 0, 0, 1);
		modelStack.Scale(otherhitboxes[hb_newtree5].getscale().x, otherhitboxes[hb_newtree5].getscale().y, otherhitboxes[hb_newtree5].getscale().z);
		RenderMesh(meshList[otherhitboxes[hb_newtree5].getmodel()], true);
		modelStack.PopMatrix();
	}

	//render outerwalls
	for (int idx = 0; idx < 11; idx++)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-28.5, 0, 25 - (idx * 5));
		modelStack.Scale(5, 5, 5);
		RenderMesh(meshList[GEO_OUTERWALLS], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(26, 0, 25 - (idx * 5));
		modelStack.Scale(5, 5, 5);
		RenderMesh(meshList[GEO_OUTERWALLS], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(25 - (idx * 5), 0, 28.5);
		modelStack.Rotate(90, 0, 1, 0);
		modelStack.Scale(5, 5, 5);
		RenderMesh(meshList[GEO_OUTERWALLS], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(25 - (idx * 5), 0, -26);
		modelStack.Rotate(90, 0, 1, 0);
		modelStack.Scale(5, 5, 5);
		RenderMesh(meshList[GEO_OUTERWALLS], true);
		modelStack.PopMatrix();
	}


	//render roads + pavement
	{
		modelStack.PushMatrix();
		modelStack.Translate(-21, 0.001, 17);
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		modelStack.Translate(1, 0.001, 17);
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		modelStack.Translate(3, 0.001, 17);
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		modelStack.Translate(1, 0.001, 19);
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		modelStack.Translate(3, 0.001, 19);
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		modelStack.Translate(22, 0, -4);
		modelStack.Scale(12, 3, 24);
		RenderMesh(meshList[GEO_CRATE], true);
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		modelStack.Translate(-19, 0.001, 17);
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		modelStack.Translate(11, 0.001, -19);
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		modelStack.Translate(13, 0.001, -19);
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		modelStack.Translate(15, 0.001, -19);
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		modelStack.Translate(17, 0.001, -19);
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		modelStack.Translate(-17, 0.001, 17);
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		modelStack.Translate(-25, 0.001, -11);
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		modelStack.Translate(-25, 0.001, -13);
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		modelStack.Translate(-25, 0.001, -15);
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		modelStack.Translate(-25, 0.001, -17);
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		modelStack.Translate(-25, 0.001, -19);
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
		modelStack.PopMatrix();
	}
	for (int idx = 0; idx < 17; idx++)
	{
		modelStack.PushMatrix();
		modelStack.Translate(9 - (idx * 2), 0.001, 15);
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(9 - (idx * 2), 0.001, 13);
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_ROAD], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(9 - (idx * 2), 0.001, 11);
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_ROAD], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(9 - (idx * 2), 0.001, 9);
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_ROAD], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(9 - (idx * 2), 0.001, 7);
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
		modelStack.PopMatrix();
	}
	for (int idx = 0; idx < 22; idx++)
	{
		modelStack.PushMatrix();
		modelStack.Translate(9, 0.0009, 25 - (idx * 2));
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(11, 0.0009, 25 - (idx * 2));
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_ROAD], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(13, 0.0009, 25 - (idx * 2));
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_ROAD], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(15, 0.0009, 25 - (idx * 2));
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_ROAD], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(17, 0.0009, 25 - (idx * 2));
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
		modelStack.PopMatrix();
	}
	for (int idx = 0; idx < 17; idx++)
	{
		modelStack.PushMatrix();
		modelStack.Translate(9 - (idx * 2), 0.002, -11);
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(9 - (idx * 2), 0.002, -13);
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_ROAD], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(9 - (idx * 2), 0.002, -15);
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_ROAD], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(9 - (idx * 2), 0.002, -17);
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_ROAD], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(9 - (idx * 2), 0.002, -19);
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
		modelStack.PopMatrix();
	}
	for (int idx = 0; idx < 5; idx++)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-25, 0.001, 7 + idx * 2);
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_CONCRETE_PAVEMENT], true);
		modelStack.PopMatrix();

	}

	modelStack.PushMatrix();
	modelStack.Translate(15, 0, 25.8);
	modelStack.Scale(3, 3, 3);
	RenderMesh(meshList[GEO_DOORWAY], true);
	modelStack.PopMatrix();

	RenderMudkip(Vector3(22, 1.5, -4));

	RenderMudkip(Vector3(-64, -5.6, 21), Vector3(0, 180, 0));
	modelStack.PushMatrix();
	modelStack.Translate(-62.92, -5, 21);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(1, 1, 1);
	RenderMesh(meshList[GEO_MUDKIP_EYES], false);
	modelStack.PopMatrix();

	//render shop text
	modelStack.PushMatrix();
	modelStack.Translate(19.7, 1, -6.5);
	modelStack.Rotate(-90, 0, 1, 0);
	modelStack.Scale(2, 2, 2);
	RenderText(meshList[GEO_TEXT_COMICSANS], "SHOP", Color(0, 0, 1));
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(25.9, 3, 2);
	modelStack.Rotate(-90, 0, 1, 0);
	modelStack.Scale(3, 3, 3);
	RenderText(meshList[GEO_TEXT_COMICSANS], "SALE", Color(0, 0, 1));
	modelStack.PopMatrix();

	if (renderhitboxes)
	{
		for (unsigned idx = 9; idx < GEO_HITBOX; idx++) //render hitboxes
		{
			if (!objectlist[idx].getissetup())
				continue;
			modelStack.PushMatrix();
			modelStack.Translate(objectlist[idx].getposition().x + objectlist[idx].getobjecthitbox().gethboffset().x, objectlist[idx].getposition().y + objectlist[idx].getobjecthitbox().gethboffset().y, objectlist[idx].getposition().z + objectlist[idx].getobjecthitbox().gethboffset().z);
			modelStack.Scale(objectlist[idx].getobjecthitbox().gethitboxdimensions().x * 2, objectlist[idx].getobjecthitbox().gethitboxdimensions().y * 2, objectlist[idx].getobjecthitbox().gethitboxdimensions().z * 2);
			RenderMesh(meshList[GEO_HITBOX], false);
			modelStack.PopMatrix();
		}
		for (unsigned idx = 0; idx < hb_count; idx++) //render other hitboxes
		{
			if (!otherhitboxes[idx].getissetup())
				continue;
			modelStack.PushMatrix();
			modelStack.Translate(otherhitboxes[idx].getposition().x + otherhitboxes[idx].getobjecthitbox().gethboffset().x, otherhitboxes[idx].getposition().y + otherhitboxes[idx].getobjecthitbox().gethboffset().y, otherhitboxes[idx].getposition().z + otherhitboxes[idx].getobjecthitbox().gethboffset().z);
			modelStack.Scale(otherhitboxes[idx].getobjecthitbox().gethitboxdimensions().x * 2, otherhitboxes[idx].getobjecthitbox().gethitboxdimensions().y * 2, otherhitboxes[idx].getobjecthitbox().gethitboxdimensions().z * 2);
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
		RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(0, 1, 0), 4, 0, 0);
		UIstringoutput.str("");

		UIstringoutput << "Pos x:" << player.getposition().x << " y:" << player.getposition().y << " z:" << player.getposition().z;
		UIstringoutput.precision(3);
		RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 4, 0, 50);
		UIstringoutput.str("");
	}

	//render ui elements during interactions
	if (interact == i_light1)
	{
		UIstringoutput << "Press 'E' to ";
		if (light[0].power == 0)
			UIstringoutput << "turn on";
		else
			UIstringoutput << "turn off";
		UIstringoutput << " light";
		RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 3, 40 - UIstringoutput.str().size() * 1.15, 15);
		UIstringoutput.str("");
	}
	else if (interact == i_light2)
	{
		UIstringoutput << "Press 'E' to ";
		if (light[5].power == 0)
			UIstringoutput << "turn on";
		else
			UIstringoutput << "turn off";
		UIstringoutput << " light";
		RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 3, 40 - UIstringoutput.str().size() * 1.15, 15);
		UIstringoutput.str("");
	}
	else if (interact == i_light3)
	{
		UIstringoutput << "Press 'E' to ";
		if (light[6].power == 0)
			UIstringoutput << "turn on";
		else
			UIstringoutput << "turn off";
		UIstringoutput << " light";
		RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 3, 40 - UIstringoutput.str().size() * 1.15, 15);
		UIstringoutput.str("");
	}
	else if (interact == i_light4)
	{
		UIstringoutput << "Press 'E' to ";
		if (light[7].power == 0)
			UIstringoutput << "turn on";
		else
			UIstringoutput << "turn off";
		UIstringoutput << " light";
		RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 3, 40 - UIstringoutput.str().size() * 1.15, 15);
		UIstringoutput.str("");
	}
	else if (interact == i_shop)
	{
		if (!inshop)
		{
			UIstringoutput << "Press 'E' to browse shop";
			RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 3, 40 - UIstringoutput.str().size() * 1.15, 15);
		}
		else
		{
			if (shopselect == 0 && pricemultiplier < 5)
			{
				//render text background
				RenderMeshOnScreen(meshList[GEO_TEXTBACKGROUND], Vector3(100, 30, 1), Vector3(0, 0, 0), 50, 5);
				RenderMeshOnScreen(meshList[GEO_MUDKIP_AVATAR], Vector3(20, 20, 1), Vector3(0, 0, 0), 10, 10);
				UIstringoutput << "Mudkip:";
				RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 6, 20, 14);
				UIstringoutput.str("");
				UIstringoutput << "Welcome, browse freely!";
				RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 3, 20, 9);
				UIstringoutput.str("");
				UIstringoutput << "Press 'E' to continue...";
				RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 3, 20, 0);
			}
			else if (shopselect > 0 && pricemultiplier < 5)
			{
				//top ui part
				RenderMeshOnScreen(meshList[GEO_UIBACKGROUND], Vector3(80, 40, 1), Vector3(0, 0, 0), 40, 40);
				RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], "Shop:", Color(1, 1, 1), 8, 1, 53);
				static Color printcolor = Color(1, 1, 1);
				if (shopselect == 1)
					printcolor = Color(0, 1, 0);
				else
					printcolor = Color(1, 1, 1);
				UIstringoutput << "Tree Seed:$" << 100 * pricemultiplier;
				RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), printcolor, 4, 1, 48);
				UIstringoutput.str("");
				if (shopselect == 2)
					printcolor = Color(0, 1, 0);
				else
					printcolor = Color(1, 1, 1);
				UIstringoutput << "100 Water:$" << 10 * pricemultiplier;
				RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), printcolor, 4, 1, 44);
				UIstringoutput.str("");

				if (shopselect == 3)
					printcolor = Color(0, 1, 0);
				else
					printcolor = Color(1, 1, 1);
				UIstringoutput << "Increase Punch";
				RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), printcolor, 4, 1, 40);
				UIstringoutput.str("");
				UIstringoutput << "Power:$" << 50 * pricemultiplier;
				RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), printcolor, 4, 1, 37);
				UIstringoutput.str("");

				if (shopselect == 4)
					printcolor = Color(0, 1, 0);
				else
					printcolor = Color(1, 1, 1);
				UIstringoutput << "Sell Wood:";
				RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), printcolor, 4, 1, 33);
				UIstringoutput.str("");
				UIstringoutput << "1 Wood=$5";
				RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), printcolor, 4, 1, 30);
				UIstringoutput.str("");

				if (shopselect == 5)
					printcolor = Color(0, 1, 0);
				else
					printcolor = Color(1, 1, 1);
				UIstringoutput << "The Key out:";
				RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), printcolor, 4, 1, 26);
				UIstringoutput.str("");
				UIstringoutput << "$" << 5000 * pricemultiplier;
				RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), printcolor, 4, 1, 23);
				UIstringoutput.str("");


				//bottom part
				RenderMeshOnScreen(meshList[GEO_TEXTBACKGROUND], Vector3(100, 30, 1), Vector3(0, 0, 0), 50, 5);
				if (lackofmoney)
					RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], "Not enough money", Color(1, 1, 1), 5, 2, 14);
				else if (playerboughtitem)
					RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], "Thank you!", Color(1, 1, 1), 5, 2, 14);
				else if (lackofwood)
					RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], "Not enough wood", Color(1, 1, 1), 5, 2, 14);
				UIstringoutput << "Press 'W/S' to navigate";
				RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 3, 2, 9);
				UIstringoutput.str("");
				UIstringoutput << "Press 'E' to buy";
				RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 3, 2, 6);
				UIstringoutput.str("");
				UIstringoutput << "Press Esc to exit";
				RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 3, 2, 3);
			}
			else
			{
				//render text background
				RenderMeshOnScreen(meshList[GEO_TEXTBACKGROUND], Vector3(100, 30, 1), Vector3(0, 0, 0), 50, 5);
				RenderMeshOnScreen(meshList[GEO_MUDKIP_AVATAR], Vector3(20, 20, 1), Vector3(0, 0, 0), 10, 10);
				UIstringoutput << "Mudkip:";
				RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 6, 20, 14);
				UIstringoutput.str("");
				UIstringoutput << "I find you annoying, so";
				RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 3, 20, 9);
				UIstringoutput.str("");
				UIstringoutput << "im not selling you";
				RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 3, 20, 7);
				UIstringoutput.str("");
				UIstringoutput << "anything";
				RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 3, 20, 5);
				UIstringoutput.str("");
				UIstringoutput << "Press 'E' to continue...";
				RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 3, 20, 0);
			}
		}
		UIstringoutput.str("");
	}
	else if (interact == i_plant_tree)
	{
		if (!treeplanted)
		{
			if (!lackofwater)
			{
				UIstringoutput.str("Press 'E' to plant tree");
				RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 3, 40 - UIstringoutput.str().size() * 1.15, 15);
				UIstringoutput.str("");
			}
			else
			{
				UIstringoutput.str("You lack the seed");
				RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 3, 40 - UIstringoutput.str().size() * 1.15, 15);
				UIstringoutput.str("");
			}
		}
		else
		{
			if (lackofwater)
			{
				UIstringoutput.str("Not enough water");
				RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 3, 40 - UIstringoutput.str().size() * 1.15, 15);
				UIstringoutput.str("");
			}
			else
			{
				if (treegrowthstage > 5)
				{
					UIstringoutput.str("Tree's fully grown");
					RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 3, 40 - UIstringoutput.str().size() * 1.15, 15);
					UIstringoutput.str("");
				}
				if (treegrowthstage <= 5)
				{
					UIstringoutput.str("Press 'E' to water");
					RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 3, 40 - UIstringoutput.str().size() * 1.15, 15);
					UIstringoutput.str("");
				}
			}
		}
	}
	else if (interact == i_laptop)
	{
		UIstringoutput.str("Press 'E' to game");
		RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 3, 40 - UIstringoutput.str().size() * 1.15, 15);
		UIstringoutput.str("");
	}
	else if (interact == i_home1_frontdoor)
	{
		UIstringoutput.str("Press 'E' to open door");
		RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 3, 40 - UIstringoutput.str().size() * 1.15, 15);
		UIstringoutput.str("");
	}
	else if (interact == i_home1_frontdoor_inside)
	{
		UIstringoutput.str("Press 'E' to exit");
		RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 3, 40 - UIstringoutput.str().size() * 1.15, 15);
		UIstringoutput.str("");
	}
	else if (interact == i_home2_frontdoor)
	{
		if (triedtoopendoor)
			UIstringoutput.str("Its locked");
		else
			UIstringoutput.str("Press 'E' to open door");
		RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 3, 40 - UIstringoutput.str().size() * 1.15, 15);
		UIstringoutput.str("");
	}
	else if (interact == i_home3_back)
	{
		if (triedtoopendoor)
		{
			UIstringoutput.str("It's locked");
			RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 3, 40 - UIstringoutput.str().size() * 1.15, 15);
		}
		else
		{
			UIstringoutput.str("Press 'E' to open door");
			RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 3, 40 - UIstringoutput.str().size() * 1.15, 15);
		}
		UIstringoutput.str("");
	}
	else if (interact == i_home3_garage)
	{
		if (triedtoopendoor)
		{
			UIstringoutput.str("It's locked");
			RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 3, 40 - UIstringoutput.str().size() * 1.15, 15);
		}
		else
		{
			UIstringoutput.str("Press 'E' to open door");
			RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 3, 40 - UIstringoutput.str().size() * 1.15, 15);
		}
		UIstringoutput.str("");
	}
	else if (interact == i_home3_frontdoor)
	{
		if (triedtoopendoor)
		{
			UIstringoutput.str("It's locked");
			RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 3, 40 - UIstringoutput.str().size() * 1.15, 15);
		}
		else
		{
			UIstringoutput.str("Press 'E' to open door");
			RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 3, 40 - UIstringoutput.str().size() * 1.15, 15);
		}
		UIstringoutput.str("");
	}
	else if (interact == i_home4_frontdoor)
	{
		if (triedtoopendoor)
		{
			UIstringoutput.str("It's locked");
			RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 3, 40 - UIstringoutput.str().size() * 1.15, 15);
		}
		else
		{
			UIstringoutput.str("Press 'E' to open door");
			RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 3, 40 - UIstringoutput.str().size() * 1.15, 15);
		}
		UIstringoutput.str("");
	}
	else if (interact == i_home4_backdoor)
	{
		if (triedtoopendoor)
		{
			UIstringoutput.str("It's locked");
			RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 3, 40 - UIstringoutput.str().size() * 1.15, 15);
		}
		else
		{
			UIstringoutput.str("Press 'E' to open door");
			RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 3, 40 - UIstringoutput.str().size() * 1.15, 15);
		}
		UIstringoutput.str("");
	}
	else if (interact == i_exit)
	{
		if (inshop)
		{
			UIstringoutput.str("You've escaped!! THE END");
			RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 3, 40 - UIstringoutput.str().size() * 1.15, 15);
		}
		else if (triedtoopendoor)
		{
			UIstringoutput.str("There is no escape from here");
			RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 3, 40 - UIstringoutput.str().size() * 1.15, 15);
		}
		else
		{
			UIstringoutput.str("Press 'E' to open door");
			RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 3, 40 - UIstringoutput.str().size() * 1.15, 15);
		}
		UIstringoutput.str("");
	}
	else if (ingame && inshop)
	{
		UIstringoutput.str("Use W/A/S/D to move");
		RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(0, 0, 0), 3, 40 - UIstringoutput.str().size() * 1.15, 15);
		UIstringoutput.str("Press esc to quit");
		RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(0, 0, 0), 3, 40 - UIstringoutput.str().size() * 1.15, 12);
	}
	else if (ingame && playerlost)
	{
		UIstringoutput.str("You died");
		RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(0, 0, 0), 3, 40 - UIstringoutput.str().size() * 1.15, 15);
		UIstringoutput.str("Press esc to exit");
		RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(0, 0, 0), 3, 40 - UIstringoutput.str().size() * 1.15, 12);
	}

	if (chatting)
	{
		//render text background
		RenderMeshOnScreen(meshList[GEO_TEXTBACKGROUND], Vector3(100, 30, 1), Vector3(0, 0, 0), 50, 5);
		RenderMeshOnScreen(meshList[GEO_MUDKIP_AVATAR], Vector3(20, 20, 1), Vector3(0, 0, 0), 10, 10);
		UIstringoutput << "Mudkip:";
		RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 6, 20, 14);
		UIstringoutput.str("");

		if (textprogress == 1)
			UIstringoutput << "Who are you??";
		else if (textprogress == 2)
			UIstringoutput << "Why are you here??";
		else if (textprogress == 3)
			UIstringoutput << "Get out of my house";
		RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 3, 20, 9);
		UIstringoutput.str("");
		UIstringoutput << "Press 'E' to continue...";
		RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 3, 20, 0);
	}
	//render UI
	if (!ingame)
	{
		unsigned uioffset;
		UIstringoutput.str("");
		UIstringoutput << "$" << playergold;
		uioffset = UIstringoutput.str().size();
		UIstringoutput.str("");
		UIstringoutput << playerwood;
		if (uioffset < UIstringoutput.str().size())
			uioffset = UIstringoutput.str().size();
		UIstringoutput.str("");
		UIstringoutput << playerwater;
		if (uioffset < UIstringoutput.str().size())
			uioffset = UIstringoutput.str().size();
		UIstringoutput.str("");
		UIstringoutput << "$" << playergold;
		if (shopselect == 0)
			RenderMeshOnScreen(meshList[GEO_UIBACKGROUND], Vector3(50, 20, 1), Vector3(0, 0, 0), 93 - uioffset * 1.75, 55);
		RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 4, 75 - uioffset * 2, 56);
		UIstringoutput.str("");
		UIstringoutput << playerwood;
		RenderMeshOnScreen(meshList[GEO_LOG], Vector3(3, 3, 3), Vector3(0, 0, 0), 70 - uioffset * 2, 54);
		RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 4, 75 - uioffset * 2, 52);
		UIstringoutput.str("");
		UIstringoutput << playerwater;
		RenderMeshOnScreen(meshList[GEO_WATERDROP], Vector3(3, 3, 3), Vector3(0, 0, 0), 70 - uioffset * 2, 50);
		RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(1, 1, 1), 4, 75 - uioffset * 2, 48);
		UIstringoutput.str("");
	}
	else if (ingame)
	{
		UIstringoutput.str("");
		UIstringoutput << "Score:";
		RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(0, 0, 0), 3, 1, 3);
		UIstringoutput.str("");
		UIstringoutput << (int)playerscore;
		RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], UIstringoutput.str(), Color(0, 0, 0), 3, 1, 0);
		UIstringoutput.str("");
	}

	//modelStack.PushMatrix();
	//modelStack.Translate(light[0].position.x, light[0].position.y, light[0].position.z);
	//RenderMesh(meshList[GEO_LIGHTBALL], false);
	//modelStack.PopMatrix();
}


void SceneAssignment2::RenderCredits()
{
	//roll credits
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], "Name??", Color(1, 1, 1), 4, 0, 0 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], "Player's starting house", Color(1, 1, 1), 4, 0, -10 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], "by:", Color(1, 1, 1), 4, 0, -15 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], "Vinrax", Color(1, 1, 1), 4, 0, -20 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], "vinrax@gmail.com", Color(1, 1, 1), 4, 0, -25 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], "opengameart.org/content", Color(1, 1, 1), 3, 0, -30 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], "/small-old-house", Color(1, 1, 1), 3, 0, -35 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], "ground grass texture", Color(1, 1, 1), 4, 0, -45 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], "by: Simoon Murray", Color(1, 1, 1), 3, 0, -50 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], "https://www.deviantart.com", Color(1, 1, 1), 3, 0, -55 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], "/simoonmurray/art", Color(1, 1, 1), 3, 0, -60 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], "/Green-Grass-Texture-01-155704377", Color(1, 1, 1), 3, 0, -65 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], "Skybox By:", Color(1, 1, 1), 4, 0, -75 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], "Emil Persson, aka Humus.", Color(1, 1, 1), 4, 0, -80 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], "http://www.humus.name", Color(1, 1, 1), 4, 0, -85 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], "mudkip avatar from:", Color(1, 1, 1), 4, 0, -95 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], "https://your-guide-to-pokemon", Color(1, 1, 1), 4, 0, -100 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], ".fandom.com/wiki/Chris%27s_Mudkip", Color(1, 1, 1), 4, 0, -105 + rotateAngle);

	RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], "Mudkip model by:", Color(1, 1, 1), 4, 0, -115 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], "Ming Jie", Color(1, 1, 1), 4, 0, -120 + rotateAngle);

	RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], "Water drop in UI from:", Color(1, 1, 1), 4, 0, -130 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], "https://uxwing.com/water-drop-icon/", Color(1, 1, 1), 4, 0, -135 + rotateAngle);

	RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], "other assets are from", Color(1, 1, 1), 4, 0, -145 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], "Kenny", Color(1, 1, 1), 4, 0, -150 + rotateAngle);
	RenderTextOnScreen(meshList[GEO_TEXT_COMICSANS], "www.kenney.nl", Color(1, 1, 1), 4, 0, -155 + rotateAngle);
}

void SceneAssignment2::Exit()
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

void SceneAssignment2::RenderMesh(Mesh* mesh, bool enableLight)
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

void SceneAssignment2::RenderSkybox(Vector3 skyboxoffset)
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

void SceneAssignment2::RenderText(Mesh* mesh, std::string text, Color color)
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
	for (unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(i * 1.0f, 0, 0); //1.0f is the spacing of each character, you may change this value
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
}

void SceneAssignment2::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
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
	for (unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(0.5f + i * 0.8f, 0.5f, 0);
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

void SceneAssignment2::RenderMudkip(Vector3 position, Vector3 rotation)
{
	//body
	modelStack.PushMatrix();
	modelStack.Translate(position.x, position.y, position.z);
	modelStack.Rotate(rotation.x, 1, 0, 0);
	modelStack.Rotate(rotation.y, 0, 1, 0);
	modelStack.Rotate(rotation.z, 0, 0, 1);
	modelStack.Rotate(-90, 0, 1, 0);
	modelStack.Scale(0.08, 0.08, 0.08);
	//modelStack.Scale(0.5, 0.5, 0.5);

	//head
	modelStack.PushMatrix();
	modelStack.Scale(2.5, 2.5, 2.5);
	modelStack.Translate(0, 0, 2.5);
	modelStack.Translate(0, 3, 0);

	//cheeks of mudkip
	modelStack.PushMatrix();
	modelStack.Translate(-1.1, -0.6, 0.8);

	//cone 1 left
	modelStack.PushMatrix();
	modelStack.Translate(-1.75, -0.9, 1.3);
	modelStack.Scale(0.25, 0.25, 0.25);
	modelStack.Rotate(-10, 0, 1, 0);
	modelStack.Rotate(135, 1, 0, 1);
	RenderMesh(meshList[GEO_MUDKIP_CONE], true);
	modelStack.PopMatrix();

	//cone 2 left
	modelStack.PushMatrix();
	modelStack.Translate(-1.85, -0.4, 1.3);
	modelStack.Scale(0.25, 0.25, 0.25);
	modelStack.Rotate(-10, 0, 1, 0);
	modelStack.Rotate(90, 1, 0, 1);
	RenderMesh(meshList[GEO_MUDKIP_CONE], true);
	modelStack.PopMatrix();

	//cone 3 left
	modelStack.PushMatrix();
	modelStack.Translate(-1.75, 0.3, 1.3);
	modelStack.Scale(0.25, 0.25, 0.25);
	modelStack.Rotate(-10, 0, 1, 0);
	modelStack.Rotate(45, 1, 0, 1);
	RenderMesh(meshList[GEO_MUDKIP_CONE], true);
	modelStack.PopMatrix();

	modelStack.Scale(0.6, 0.6, 0.6);
	RenderMesh(meshList[GEO_MUDKIP_CHEEKS], true);
	modelStack.PopMatrix();

	//cheek right
	modelStack.PushMatrix();
	modelStack.Translate(1.1, -0.6, 0.8);

	//cone 1 right
	modelStack.PushMatrix();
	modelStack.Translate(1.75, -0.9, 1.3);
	modelStack.Scale(0.25, 0.25, 0.25);
	modelStack.Rotate(105, 0, 1, 0);
	modelStack.Rotate(135, 1, 0, 1);
	RenderMesh(meshList[GEO_MUDKIP_CONE], true);
	modelStack.PopMatrix();

	//cone 2 right
	modelStack.PushMatrix();
	modelStack.Translate(1.85, -0.4, 1.3);
	modelStack.Scale(0.25, 0.25, 0.25);
	modelStack.Rotate(105, 0, 1, 0);
	modelStack.Rotate(90, 1, 0, 1);
	RenderMesh(meshList[GEO_MUDKIP_CONE], true);
	modelStack.PopMatrix();

	//cone 3 right
	modelStack.PushMatrix();
	modelStack.Translate(1.75, 0.3, 1.3);
	modelStack.Scale(0.25, 0.25, 0.25);
	modelStack.Rotate(105, 0, 1, 0);
	modelStack.Rotate(45, 1, 0, 1);
	RenderMesh(meshList[GEO_MUDKIP_CONE], true);
	modelStack.PopMatrix();

	modelStack.Scale(0.6, 0.6, 0.6);
	RenderMesh(meshList[GEO_MUDKIP_CHEEKS], true);
	modelStack.PopMatrix();;

	//fin on the head
	modelStack.PushMatrix();
	modelStack.Translate(0, 3, 0);
	modelStack.Scale(0.2, 1, 1);
	meshList[GEO_MUDKIP_SPHERE]->material.kAmbient.Set(0.f, 108.0 / 255.0f, 147.0 / 255.0f);
	meshList[GEO_MUDKIP_SPHERE]->material.kDiffuse.Set(0.f, 108.0 / 255.0f, 147.0 / 255.0f);
	meshList[GEO_MUDKIP_SPHERE]->material.kSpecular.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_MUDKIP_SPHERE]->material.kShininess = .5f;
	RenderMesh(meshList[GEO_MUDKIP_SPHERE], true);
	modelStack.PopMatrix();

	//lighter part of head
	modelStack.PushMatrix();
	modelStack.Rotate(-32, 1, 0, 0);
	modelStack.Translate(0, -2.4, -0.1);
	modelStack.Scale(0.16, 0.06, 0.21);
	RenderMesh(meshList[GEO_MUDKIP_LIGHTER_SPHERE], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Rotate(-32, 1, 0, 0);
	modelStack.Translate(0, -2.2, -0.17);
	modelStack.Scale(0.17, 0.11, 0.17);
	RenderMesh(meshList[GEO_MUDKIP_LIGHTER_SPHERE], true);
	modelStack.PopMatrix();

	//head
	meshList[GEO_MUDKIP_SPHERE]->material.kAmbient.Set(0.f, 108.0 / 255.0f, 147.0 / 255.0f);
	meshList[GEO_MUDKIP_SPHERE]->material.kDiffuse.Set(0.f, 108.0 / 255.0f, 147.0 / 255.0f);
	meshList[GEO_MUDKIP_SPHERE]->material.kSpecular.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_MUDKIP_SPHERE]->material.kShininess = .5f;
	RenderMesh(meshList[GEO_MUDKIP_SPHERE], true);
	modelStack.PopMatrix();

	//tail
	modelStack.PushMatrix();
	modelStack.Translate(0, 0, -5);
	modelStack.Translate(0, 1, -5);
	modelStack.Rotate(90, 0, 0, 1);
	modelStack.Rotate(90, 1, 0, 0);
	modelStack.Rotate(30, 0, 0, 1);

	//tip of the tail
	modelStack.PushMatrix();
	modelStack.Translate(0, -7.8, 0);
	modelStack.Rotate(90, 0, 0, 1);
	modelStack.Scale(0.17, 1.2, 0.17);

	meshList[GEO_MUDKIP_LIGHTER_SPHERE]->material.kAmbient.Set(106.0 / 255.0f, 183.0 / 255.0f, 195.0 / 255.0f);
	meshList[GEO_MUDKIP_LIGHTER_SPHERE]->material.kDiffuse.Set(106.0 / 255.0f, 183.0 / 255.0f, 195.0 / 255.0f);
	meshList[GEO_MUDKIP_LIGHTER_SPHERE]->material.kSpecular.Set(0.1f, 0.1f, 0.1f);
	meshList[GEO_MUDKIP_LIGHTER_SPHERE]->material.kShininess = 0.5f;

	RenderMesh(meshList[GEO_MUDKIP_LIGHTER_SPHERE], true);
	modelStack.PopMatrix();

	modelStack.Scale(0.8, 0.75, 0.1);
	RenderMesh(meshList[GEO_MUDKIP_TAIL], true);
	modelStack.PopMatrix();

	//leg 1
	modelStack.PushMatrix();
	modelStack.Translate(4, 0, 4);
	modelStack.Translate(0, -3, 0);
	modelStack.Scale(0.8, 1.7, 0.8);
	meshList[GEO_MUDKIP_SPHERE]->material.kAmbient.Set(0.f, 108.0 / 255.0f, 147.0 / 255.0f);
	meshList[GEO_MUDKIP_SPHERE]->material.kDiffuse.Set(0.f, 168.0 / 255.0f, 207.0 / 255.0f);
	meshList[GEO_MUDKIP_SPHERE]->material.kSpecular.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_MUDKIP_SPHERE]->material.kShininess = .5f;
	RenderMesh(meshList[GEO_MUDKIP_SPHERE], true);
	modelStack.PopMatrix();

	//leg 2
	modelStack.PushMatrix();
	modelStack.Translate(-4, 0, 4);
	modelStack.Translate(0, -3, 0);
	modelStack.Scale(0.8, 1.7, 0.8);
	meshList[GEO_MUDKIP_SPHERE]->material.kAmbient.Set(0.f, 108.0 / 255.0f, 147.0 / 255.0f);
	meshList[GEO_MUDKIP_SPHERE]->material.kDiffuse.Set(0.f, 168.0 / 255.0f, 207.0 / 255.0f);
	meshList[GEO_MUDKIP_SPHERE]->material.kSpecular.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_MUDKIP_SPHERE]->material.kShininess = .5f;
	RenderMesh(meshList[GEO_MUDKIP_SPHERE], true);
	modelStack.PopMatrix();

	//leg 3
	modelStack.PushMatrix();
	modelStack.Translate(-4, 0, -4);
	modelStack.Translate(0, -3, 0);
	modelStack.Scale(0.8, 1.7, 0.8);
	meshList[GEO_MUDKIP_SPHERE]->material.kAmbient.Set(0.f, 108.0 / 255.0f, 147.0 / 255.0f);
	meshList[GEO_MUDKIP_SPHERE]->material.kDiffuse.Set(0.f, 168.0 / 255.0f, 207.0 / 255.0f);
	meshList[GEO_MUDKIP_SPHERE]->material.kSpecular.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_MUDKIP_SPHERE]->material.kShininess = .5f;
	RenderMesh(meshList[GEO_MUDKIP_SPHERE], true);
	modelStack.PopMatrix();

	//leg 4
	modelStack.PushMatrix();
	modelStack.Translate(4, 0, -4);
	modelStack.Translate(0, -3, 0);
	modelStack.Scale(0.8, 1.7, 0.8);
	meshList[GEO_MUDKIP_SPHERE]->material.kAmbient.Set(0.f, 108.0 / 255.0f, 147.0 / 255.0f);
	meshList[GEO_MUDKIP_SPHERE]->material.kDiffuse.Set(0.f, 168.0 / 255.0f, 207.0 / 255.0f);
	meshList[GEO_MUDKIP_SPHERE]->material.kSpecular.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_MUDKIP_SPHERE]->material.kShininess = .5f;
	RenderMesh(meshList[GEO_MUDKIP_SPHERE], true);
	modelStack.PopMatrix();

	//under body lighter color
	modelStack.PushMatrix();
	modelStack.Translate(0, -1.2, 0);
	modelStack.Scale(0.5, 0.5, 0.8);

	meshList[GEO_MUDKIP_LIGHTER_SPHERE]->material.kAmbient.Set(106.0 / 255.0f, 183.0 / 255.0f, 195.0 / 255.0f);
	meshList[GEO_MUDKIP_LIGHTER_SPHERE]->material.kDiffuse.Set(106.0 / 255.0f, 183.0 / 255.0f, 195.0 / 255.0f);
	meshList[GEO_MUDKIP_LIGHTER_SPHERE]->material.kSpecular.Set(0.1f, 0.1f, 0.1f);
	meshList[GEO_MUDKIP_LIGHTER_SPHERE]->material.kShininess = 0.5f;

	RenderMesh(meshList[GEO_MUDKIP_LIGHTER_SPHERE], true);
	modelStack.PopMatrix();

	//body
	modelStack.Scale(2, 2, 2.5);
	meshList[GEO_MUDKIP_SPHERE]->material.kAmbient.Set(0.f, 108.0 / 255.0f, 147.0 / 255.0f);
	meshList[GEO_MUDKIP_SPHERE]->material.kDiffuse.Set(0.f, 168.0 / 255.0f, 207.0 / 255.0f);
	meshList[GEO_MUDKIP_SPHERE]->material.kSpecular.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_MUDKIP_SPHERE]->material.kShininess = .5f;
	RenderMesh(meshList[GEO_MUDKIP_SPHERE], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(position.x - 1.1, position.y + 0.5, position.z);
	modelStack.Rotate(-90, 0, 1, 0);
	modelStack.Scale(1, 1, 1);
	RenderMesh(meshList[GEO_MUDKIP_EYES], false);
	modelStack.PopMatrix();
}

void SceneAssignment2::RenderMeshOnScreen(Mesh* mesh, Vector3 size, Vector3 rotate, float x, float y)
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

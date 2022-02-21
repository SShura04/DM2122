#include "Jigglypuff_Minigame.h"
#include "GL\glew.h"

#include "shader.hpp"
#include "Mtx44.h"

#include "Application.h"
#include "MeshBuilder.h"
#include "LoadTGA.h"
#include "Utility.h"
#include <sstream>
#include <fstream>
#include <cmath>

using namespace std;

Sp2_Minigame::Sp2_Minigame()
{
}

Sp2_Minigame::~Sp2_Minigame()
{
}

void Sp2_Minigame::UseScene()
{
	glBindVertexArray(m_vertexArrayID);
	glUseProgram(m_programID);

	Mesh::SetMaterialLoc(m_parameters[U_MATERIAL_AMBIENT],
		m_parameters[U_MATERIAL_DIFFUSE],
		m_parameters[U_MATERIAL_SPECULAR],
		m_parameters[U_MATERIAL_SHININESS]);
}

//float DistanceParameter(float EnemyXPos, float EnemyZPos, float OriginalPosX, float OriginalPosZ) {
//	float squareofxz = (pow(EnemyXPos - OriginalPosX, 2) + pow(EnemyZPos - OriginalPosZ, 2));
//	float magnitude = sqrt(squareofxz);
//
//	return magnitude;
//}

void Sp2_Minigame::RenderMesh(Mesh* mesh, bool enableLight)
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

void Sp2_Minigame::Init()
{
	srand(time(NULL));
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


	// Set background color to dark blue
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//Enable depth buffer and depth testing
	glEnable(GL_DEPTH_TEST);

	//Enable back face culling
	glEnable(GL_CULL_FACE);

	//Default to fill mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Generate a default VAO for now
	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);

	m_programID = LoadShaders("Shader//Texture.vertexshader", "Shader//Text.fragmentshader");

	m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");
	m_parameters[U_MODELVIEW] = glGetUniformLocation(m_programID, "MV");
	m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(m_programID, "MV_inverse_transpose");
	m_parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(m_programID, "material.kAmbient");
	m_parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(m_programID, "material.kDiffuse");
	m_parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(m_programID, "material.kSpecular");
	m_parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(m_programID, "material.kShininess");
	m_parameters[U_LIGHT0_POSITION] = glGetUniformLocation(m_programID, "lights[0].position_cameraspace");
	m_parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_programID, "colorTextureEnabled");
	m_parameters[U_COLOR_TEXTURE] = glGetUniformLocation(m_programID, "colorTexture");
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
	m_parameters[U_TEXT_ENABLED] = glGetUniformLocation(m_programID, "textEnabled");
	m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID, "textColor");
	m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");
	glUseProgram(m_programID);

	glUniform1i(m_parameters[U_NUMLIGHTS], 2);


	light[0].type = Light::LIGHT_POINT;
	light[0].position.Set(-60, 35, 35);
	light[0].color.Set(1, 1, 1);
	light[0].power = 1;
	light[0].kC = 1.f;
	light[0].kL = 0.01f;
	light[0].kQ = 0.001f;
	light[0].cosCutoff = cos(Math::DegreeToRadian(45));
	light[0].cosInner = cos(Math::DegreeToRadian(30));
	light[0].exponent = 3.f;
	light[0].spotDirection.Set(0.f, 1.f, 0.f);

	light[1].type = Light::LIGHT_POINT;
	light[1].position.Set(0, 600, 0);
	light[1].color.Set(1, 1, 1);
	light[1].power = 100;
	light[1].kC = 1.f;
	light[1].kL = 0.01f;
	light[1].kQ = 0.001f;
	light[1].cosCutoff = cos(Math::DegreeToRadian(45));
	light[1].cosInner = cos(Math::DegreeToRadian(30));
	light[1].exponent = 3.f;
	light[1].spotDirection.Set(0.f, 1.f, 0.f);

	// Make sure you pass uniform parameters after glUseProgram()
	glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
	glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &light[0].color.r);
	glUniform1f(m_parameters[U_LIGHT0_POWER], light[0].power);
	glUniform1f(m_parameters[U_LIGHT0_KC], light[0].kC);
	glUniform1f(m_parameters[U_LIGHT0_KL], light[0].kL);
	glUniform1f(m_parameters[U_LIGHT0_KQ], light[0].kQ);
	glUniform1f(m_parameters[U_LIGHT0_COSCUTOFF], light[0].cosCutoff);
	glUniform1f(m_parameters[U_LIGHT0_COSINNER], light[0].cosInner);
	glUniform1f(m_parameters[U_LIGHT0_EXPONENT], light[0].exponent);

	glUniform1i(m_parameters[U_LIGHT1_TYPE], light[1].type);
	glUniform3fv(m_parameters[U_LIGHT1_COLOR], 1, &light[1].color.r);
	glUniform1f(m_parameters[U_LIGHT1_POWER], light[1].power);
	glUniform1f(m_parameters[U_LIGHT1_KC], light[1].kC);
	glUniform1f(m_parameters[U_LIGHT1_KL], light[1].kL);
	glUniform1f(m_parameters[U_LIGHT1_KQ], light[1].kQ);
	glUniform1f(m_parameters[U_LIGHT1_COSCUTOFF], light[1].cosCutoff);
	glUniform1f(m_parameters[U_LIGHT1_COSINNER], light[1].cosInner);
	glUniform1f(m_parameters[U_LIGHT1_EXPONENT], light[1].exponent);


	Mesh::SetMaterialLoc(m_parameters[U_MATERIAL_AMBIENT], m_parameters[U_MATERIAL_DIFFUSE], m_parameters[U_MATERIAL_SPECULAR], m_parameters[U_MATERIAL_SHININESS]);

	//variable to rotate geometry

	for (int i = 0; i < 10; i++) {
		if (Diamonds[i] == nullptr) {
			Diamonds[i] = new CollectObjects;
			Diamonds[i]->SetXYZ(rand() % 360 - 180, -3, rand() % 360 - 180);
		}
	}



	//Direction
	rotateAngle = 0;
	rotateBody = 0;
	translateX = 0.f;
	translateZ = 0.f;

	// Fire Up and Down
	ScaleUpandDown = 4;
	HopUp_FIRE = true;
	HopDown_FIRE = false;

	// Hop Up and Down
	TranslateUpandDown = 0;
	HopUp = true;
	HopDown = false;
	EnemyHopUp = true;
	EnemyHopDown = false;
	EnemyUpandDown_Translation = 0;

	EnemyHopUp2 = true;
	EnemyHopDown2 = false;
	EnemyUpandDown_Translation2 = 0;

	EnemyHopUp3 = true;
	EnemyHopDown3 = false;
	EnemyUpandDown_Translation3 = 0;

	//Body color
	BodyR = 0.95;
	BodyG = 0.85;
	BodyB = 0.89;

	//Eyelid timer
	open = true;
	close = false;
	rotate_Eyelid = -300;

	//Movement/Jump checker
	allowMovement = false;
	allowJump = false;
	air = false;
	ground = true;
	swingbackward = true;
	swingforward = false;


	SkillsTimer = 0;

	// Demon form
	isDemon = true;
	tail_shakeleft = false;
	tail_shakeright = true;
	rotateDemonTail = 0;
	ScaleFireEffect1 = 1;
	ScaleFireEffect2 = 1;
	ScaleFireEffect3 = 1;
	ScaleFireEffect4 = 1;
	ScaleFireEffect5 = 1;
	ScaleFireEffect6 = 1;
	buttonsincelastpress = 0.f;

	//Moving/Jump Animation
	rotatelegs_jump = 0;
	RotateLeftArm_Movement = 0;
	RotateRightArm_Movement = 0;
	RotateLeftLeg_Movement = 0;
	RotateRightLeg_Movement = 0;
	translatebodyY_Jump = 0.0f;
	allowAttack = false;

	SwordSwingForward = false;
	SwordSwingBackward = true;
	allowSwordSwing = false;
	RotateRightArm_SwingSword = 0;

	allowSpinattack = false;
	allowUltimateAttack = false;
	rotateBodySpinAttack = 0;
	SpinStarts = true;
	SpinEnds = false;
	rotateSwordSpinAttack = 0;
	Temp = 0;

	translateFusionWaveX = 0;
	translateFusionWaveZ = 10;
	rotateFusionWave = -90;
	allowReleaseofFusionWave = false;
	allowFusionWaveTransverse = false;
	skill = false;
	i = false;
	j = 0;
	ShockwaveScale = 0.1;
	ShockwavedirectionX = 0;
	ShockwavedirectionZ = 0;
	DisplayShockwave = false;

	allowJumpAttack = false;
	jumpAttack_rotation = 0;


	testcollect = true;
	rotateSkyBox = 0;
	isCollided = false;

	EnemyPosX = 100;
	EnemyPosZ = -140;
	EnemyPosX2 = 100;
	EnemyPosZ2 = 140;
	EnemyPosX3 = -100;
	EnemyPosZ3 = -140;

	allowEnemyMovement = true;
	allowEnemyMovement2 = true;
	allowEnemyMovement3 = true;

	swingbackwardEnemy = true;
	swingforwardEnemy = false;
	swingbackwardEnemy2 = true;
	swingforwardEnemy2 = false;
	swingbackwardEnemy3 = true;
	swingforwardEnemy3 = false;

	isAlive = true;
	RotateEnemyLeftArm_Movement = RotateEnemyRightArm_Movement = RotateEnemyRightLeg_Movement = RotateEnemyLeftLeg_Movement = 0;
	RotateEnemyLeftArm_Movement2 = RotateEnemyRightArm_Movement2 = RotateEnemyRightLeg_Movement2 = RotateEnemyLeftLeg_Movement2 = 0;
	RotateEnemyLeftArm_Movement3 = RotateEnemyRightArm_Movement3 = RotateEnemyRightLeg_Movement3 = RotateEnemyLeftLeg_Movement3 = 0;

	EnemyXSpeed = EnemyZSpeed = 0;
	EnemyXSpeed2 = EnemyZSpeed2 = 0;
	EnemyXSpeed3 = EnemyZSpeed3 = 0;

	Health = 100;
	EnemyHealth = 100;
	EnemyHealth2 = 100;
	EnemyHealth3 = 100;

	isRead = false;
	DialogueBoxOpen = false;
	x = 0;

	it = it2 = it3 = "";
	ScrollingText = 0;
	EnemyDieRotation = 0;
	EnemyDieRotation2 = 0;
	EnemyDieRotation3 = 0;

	EarnedCoins = true;

	rotateEnemyBody = 0;
	translateY_EnemyJump = 0;
	EnemyShockwaveScale = 0.1;
	ShockWaveEnemyAttack = false;
	AllowEnemyAttack = false;

	translateY_EnemyJump2 = 0;
	EnemyShockwaveScale2 = 0.1;
	ShockWaveEnemyAttack2 = false;
	AllowEnemyAttack2 = false;
	rotateEnemyBody2 = 180;

	translateY_EnemyJump3 = 0;
	EnemyShockwaveScale3 = 0.1;
	ShockWaveEnemyAttack3 = false;
	AllowEnemyAttack3 = false;
	rotateEnemyBody3 = 180;

	test = 0.7f;
	jump = true;
	Ground = false;
	test2 = 0.7f;
	jump2 = true;
	Ground2 = false;
	test3 = 0.7f;
	jump3 = true;
	Ground3 = false;
	timer_gameover = 0.f;
	isShopUIOpen = false;
	Diamond = 0;
	isSufficient = true;
	EnemyRightArm_JumpAttackRotation = 0;
	EnemyRightArm_JumpAttackRotation2 = 0;
	EnemyRightArm_JumpAttackRotation3 = 0;

	//Initialize camera settings

	camera2.Init(Vector3(0, 60, 60), Vector3(0, 0, 0), Vector3(0, 1, 0));

	// Init VBO
	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		meshList[i] = nullptr;
	}
	meshList[GEO_AXES] = MeshBuilder::GenerateAxes("reference", 1000, 1000, 1000);

	// Normal Jigglypuff
	meshList[GEO_SPHERE] = MeshBuilder::GenerateSphere("sphere", Color(BodyR, BodyG, BodyB), 80, 80, 1.f);
	meshList[GEO_SPHERE]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_SPHERE]->material.kDiffuse.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_SPHERE]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_SPHERE]->material.kShininess = 1.f;
	meshList[GEO_MOUTH] = MeshBuilder::GenerateHemiSphere("sphere", Color(0.89, 0.67, 0.71), 80, 80, 1.f);
	meshList[GEO_MOUTH]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_MOUTH]->material.kDiffuse.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_MOUTH]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_MOUTH]->material.kShininess = 1.f;
	meshList[GEO_EYELID] = MeshBuilder::GenerateEyeLid("hemisphere", Color(BodyR, BodyG, BodyB), 60, 60, 1.f);
	meshList[GEO_EYELID]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_EYELID]->material.kDiffuse.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_EYELID]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_EYELID]->material.kShininess = 1.f;
	meshList[GEO_CONE] = MeshBuilder::GenerateCone("cone", Color(0.9, 0.9, 0.9), 30, 1, 1.f);
	meshList[GEO_CONE]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_CONE]->material.kDiffuse.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_CONE]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_CONE]->material.kShininess = 1.f;
	meshList[GEO_CUBE] = MeshBuilder::GenerateCube("cube", Color(BodyR, BodyG, BodyB), 1.f);
	meshList[GEO_CUBE]->material.kAmbient.Set(0.5f, 0.5f, 0.5f);
	meshList[GEO_CUBE]->material.kDiffuse.Set(0.5f, 0.5f, 0.5f);
	meshList[GEO_CUBE]->material.kSpecular.Set(0.5f, 0.5f, 0.5f);
	meshList[GEO_CUBE]->material.kShininess = 0.7f;
	meshList[GEO_PYRAMID] = MeshBuilder::GeneratePyramid("pyramid", Color(BodyR, BodyG, BodyB), 1.f);
	meshList[GEO_PYRAMID]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_PYRAMID]->material.kDiffuse.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_PYRAMID]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_PYRAMID]->material.kShininess = 1.f;
	meshList[GEO_CYLINDER] = MeshBuilder::GenerateCylinder("cylinder", Color(BodyR, BodyG, BodyB), 60, 1.f, 1.f);
	meshList[GEO_CYLINDER]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_CYLINDER]->material.kDiffuse.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_CYLINDER]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_CYLINDER]->material.kShininess = 1.f;
	meshList[GEO_TORUS] = MeshBuilder::GenerateTorus("torus", Color(BodyR, BodyG, BodyB), 60, 60, 3.0f, 1.f);
	meshList[GEO_TORUS]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_TORUS]->material.kDiffuse.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_TORUS]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_TORUS]->material.kShininess = 1.f;
	meshList[GEO_HALFTORUS] = MeshBuilder::GenerateHalfTorus("halftorus", Color(BodyR, BodyG, BodyB), 60, 60, 3.0f, 1.f);
	meshList[GEO_HALFTORUS]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_HALFTORUS]->material.kDiffuse.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_HALFTORUS]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_HALFTORUS]->material.kShininess = 1.f;
	meshList[GEO_TAIL_CONNECTOR] = MeshBuilder::GenerateHalfTorus("torus", Color(1, 0, 0), 60, 60, 3.0f, 1.f);
	meshList[GEO_TAIL_CONNECTOR]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_TAIL_CONNECTOR]->material.kDiffuse.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_TAIL_CONNECTOR]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_TAIL_CONNECTOR]->material.kShininess = 1.f;
	meshList[GEO_TAIL_CYLINDER] = MeshBuilder::GenerateCylinder("cylinder", Color(1, 0, 0), 60, 1.f, 1.f);
	meshList[GEO_TAIL_CYLINDER]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_TAIL_CYLINDER]->material.kDiffuse.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_TAIL_CYLINDER]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_TAIL_CYLINDER]->material.kShininess = 1.f;
	meshList[GEO_FIRE] = MeshBuilder::GenerateCone("cone", Color(1, 1, 0), 30, 1, 1.f);
	meshList[GEO_FIRE]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_FIRE]->material.kDiffuse.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_FIRE]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_FIRE]->material.kShininess = 1.f;

	//SKybox
	meshList[GEO_FRONT] = MeshBuilder::GenerateQuad("front", Color(1, 1, 1), 1.f);
	meshList[GEO_FRONT]->textureID = LoadTGA("Image//DaylightBox_Front.tga");
	meshList[GEO_BACK] = MeshBuilder::GenerateQuad("back", Color(1, 1, 1), 1.f);
	meshList[GEO_BACK]->textureID = LoadTGA("Image//DaylightBox_Back.tga");
	meshList[GEO_LEFT] = MeshBuilder::GenerateQuad("left", Color(1, 1, 1), 1.f);
	meshList[GEO_LEFT]->textureID = LoadTGA("Image//DaylightBox_Left.tga");
	meshList[GEO_RIGHT] = MeshBuilder::GenerateQuad("right", Color(1, 1, 1), 1.f);
	meshList[GEO_RIGHT]->textureID = LoadTGA("Image//DaylightBox_Right.tga");
	meshList[GEO_TOP] = MeshBuilder::GenerateQuad("top", Color(1, 1, 1), 1.f);
	meshList[GEO_TOP]->textureID = LoadTGA("Image//DaylightBox_Top.tga");
	meshList[GEO_BOTTOM] = MeshBuilder::GenerateQuad("bottom", Color(1, 1, 1), 1.f);
	meshList[GEO_BOTTOM]->textureID = LoadTGA("Image//DaylightBox_Bottom.tga");

	//land
	meshList[GEO_GRASS] = MeshBuilder::GenerateQuad("grass", Color(1, 1, 1), 1.f);
	meshList[GEO_GRASS]->material.kAmbient.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_GRASS]->material.kDiffuse.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_GRASS]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_GRASS]->material.kShininess = 1.f;
	meshList[GEO_GRASS]->textureID = LoadTGA("Image//Dirt.tga");
	meshList[GEO_GAMEOVER] = MeshBuilder::GenerateQuad("gameover", Color(1, 1, 1), 1.f);
	meshList[GEO_GAMEOVER]->material.kAmbient.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_GAMEOVER]->material.kDiffuse.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_GAMEOVER]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_GAMEOVER]->material.kShininess = 1.f;
	meshList[GEO_GAMEOVER]->textureID = LoadTGA("Image//GameOverRedScreen.tga");
	meshList[GEO_RETRY] = MeshBuilder::GenerateQuad("Retry", Color(1, 1, 1), 1.f);
	meshList[GEO_RETRY]->material.kAmbient.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_RETRY]->material.kDiffuse.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_RETRY]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_RETRY]->material.kShininess = 1.f;
	meshList[GEO_RETRY]->textureID = LoadTGA("Image//Retry.tga");
	meshList[GEO_GAMEOVERTEXT] = MeshBuilder::GenerateQuad("gameovertext", Color(1, 1, 1), 1.f);
	meshList[GEO_GAMEOVERTEXT]->material.kAmbient.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_GAMEOVERTEXT]->material.kDiffuse.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_GAMEOVERTEXT]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_GAMEOVERTEXT]->material.kShininess = 1.f;
	meshList[GEO_GAMEOVERTEXT]->textureID = LoadTGA("Image//GameOver.tga");


	meshList[GEO_ROCKS_SMALL] = MeshBuilder::GenerateOBJMTL("rocks_small", "OBJ//stone_largeA.obj", "OBJ//stone_largeA.mtl");
	meshList[GEO_LAMPPOST] = MeshBuilder::GenerateOBJMTL("lightpost", "OBJ//lightpost.obj", "OBJ//lightpost.mtl");


	meshList[GEO_REGULARTREE] = MeshBuilder::GenerateOBJMTL("trees", "OBJ//tree_simple.obj", "OBJ//tree_simple.mtl");
	meshList[GEO_LARGETREES] = MeshBuilder::GenerateOBJMTL("trees", "OBJ//tree_large.obj", "OBJ//tree_large.mtl");
	meshList[GEO_PALMTREES] = MeshBuilder::GenerateOBJMTL("trees", "OBJ//tree_palmShort.obj", "OBJ//tree_palmShort.mtl");

	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Image//DimboFont.tga");

	meshList[GEO_HEALTHBAR] = MeshBuilder::GenerateQuad("healthbar", Color(1, 1, 1), 1.f);
	meshList[GEO_HEALTHBAR]->textureID = LoadTGA("Image//Health100.tga");
	meshList[GEO_HEALTHBACKGROUND] = MeshBuilder::GenerateQuad("healthbackground", Color(1, 1, 1), 1.f);
	meshList[GEO_HEALTHBACKGROUND]->textureID = LoadTGA("Image//Health0.tga");

	meshList[GEO_DIALOGUEUI] = MeshBuilder::GenerateQuad("dialoguebox", Color(1, 1, 1), 1.f);
	meshList[GEO_DIALOGUEUI]->textureID = LoadTGA("Image//DialogueBox.tga");

	meshList[GEO_LOG] = MeshBuilder::GenerateOBJMTL("rocks", "OBJ//log.obj", "OBJ//log.mtl");

	meshList[GEO_PAPER] = MeshBuilder::GenerateQuad("paper", Color(1, 1, 1), 1.f);
	meshList[GEO_PAPER]->material.kAmbient.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_PAPER]->material.kDiffuse.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_PAPER]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_PAPER]->material.kShininess = 1.f;
	meshList[GEO_PAPER]->textureID = LoadTGA("Image//PaperTexture.tga");

	meshList[GEO_ALUSKILLS] = MeshBuilder::GenerateQuad("paper", Color(1, 1, 1), 1.f);
	meshList[GEO_ALUSKILLS]->material.kAmbient.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_ALUSKILLS]->material.kDiffuse.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_ALUSKILLS]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_ALUSKILLS]->material.kShininess = 1.f;
	meshList[GEO_ALUSKILLS]->textureID = LoadTGA("Image//AlucardSkills.tga");

	meshList[GEO_HEART] = MeshBuilder::GenerateQuad("heart", Color(1, 1, 1), 1.f);
	meshList[GEO_HEART]->material.kAmbient.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_HEART]->material.kDiffuse.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_HEART]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_HEART]->material.kShininess = 1.f;
	meshList[GEO_HEART]->textureID = LoadTGA("Image//Heart.tga");
	meshList[GEO_COINS] = MeshBuilder::GenerateQuad("coins", Color(1, 1, 1), 1.f);
	meshList[GEO_COINS]->material.kAmbient.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_COINS]->material.kDiffuse.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_COINS]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_COINS]->material.kShininess = 1.f;
	meshList[GEO_COINS]->textureID = LoadTGA("Image//Cash.tga");

	meshList[GEO_TENTOPEN] = MeshBuilder::GenerateOBJMTL("tentopen", "OBJ//tent_detailedOpen.obj", "OBJ//tent_detailedOpen.mtl");
	meshList[GEO_TENTCLOSE] = MeshBuilder::GenerateOBJMTL("tentclose", "OBJ//tent_detailedClosed.obj", "OBJ//tent_detailedClosed.mtl");
	meshList[GEO_CAMPFIRE] = MeshBuilder::GenerateOBJMTL("tentclose", "OBJ//campfire_logs.obj", "OBJ//campfire_logs.mtl");
	meshList[GEO_LIGHTBALL] = MeshBuilder::GenerateSphere("Lightball", Color(1, 1, 1), 30, 30, 1.f);

	meshList[GEO_DIAMOND] = MeshBuilder::GenerateOBJMTL("jewelmodel", "OBJ//jewel.obj", "OBJ//jewel.mtl");
	meshList[GEO_DIAMONDIMAGE] = MeshBuilder::GenerateQuad("jewel", Color(1, 1, 1), 1.f);
	meshList[GEO_DIAMONDIMAGE]->material.kAmbient.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_DIAMONDIMAGE]->material.kDiffuse.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_DIAMONDIMAGE]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_DIAMONDIMAGE]->material.kShininess = 1.f;
	meshList[GEO_DIAMONDIMAGE]->textureID = LoadTGA("Image//jewel.tga");


	meshList[GEO_VICTORY] = MeshBuilder::GenerateQuad("victory", Color(1, 1, 1), 1.f);
	meshList[GEO_VICTORY]->material.kAmbient.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_VICTORY]->material.kDiffuse.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_VICTORY]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_VICTORY]->material.kShininess = 1.f;
	meshList[GEO_VICTORY]->textureID = LoadTGA("Image//VictoryGreenScreen.tga");
	meshList[GEO_VICTORYTEXT] = MeshBuilder::GenerateQuad("victoryText", Color(1, 1, 1), 1.f);
	meshList[GEO_VICTORYTEXT]->material.kAmbient.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_VICTORYTEXT]->material.kDiffuse.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_VICTORYTEXT]->material.kSpecular.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_VICTORYTEXT]->material.kShininess = 1.f;
	meshList[GEO_VICTORYTEXT]->textureID = LoadTGA("Image//VictoryText.tga");

	Mtx44 projection;
	projection.SetToPerspective(45.f, 4.f / 3.f, 0.1f, 1000.f);
	projectionStack.LoadMatrix(projection);

	window = GetFocus();

	//set mouse to centre of window
	if (GetWindowRect(window, &rect))
	{
		SetCursorPos((rect.right + rect.left) * 0.5, (rect.bottom + rect.top) * 0.5);
	}
}





bool Sp2_Minigame::Update(double dt)
{
	UpdateENV(dt);
	return false;
}






Vector3 Sp2_Minigame::CollisionCircleRect(float cx, float cy, float radius, float rx, float ry, float rw, float rh) {
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


Vector3 Sp2_Minigame::PlayerCollision(unsigned count, float Px, float Pz) {
	if (count == 0) {
		return CollisionCircleRect(Px, Pz, 5, -50, -50, 10, 10);
	}
	else if (count == 1) {
		return CollisionCircleRect(Px, Pz, 5, 50, -100, 10, 10);
	}
	else if (count == 2) {
		return CollisionCircleRect(Px, Pz, 5, -100, 100, 45, 45);
	}
	else if (count == 3) {
		return CollisionCircleRect(Px, Pz, 5, -60, 30, 8, 8);
	}
	else if (count == 4) {
		return CollisionCircleRect(Px, Pz, 5, 110, 100, 10, 10);
	}
	else if (count == 5) {
		return CollisionCircleRect(Px, Pz, 5, 50, 50, 10, 10);
	}
	else if (count == 6) {
		return CollisionCircleRect(Px, Pz, 5, 150, 0, 10, 10);
	}
	else if (count == 7) {
		return CollisionCircleRect(Px, Pz, 5, -60, -130, 10, 10);
	}
	else if (count == 8) {
		return CollisionCircleRect(Px, Pz, 5, 80, -50, 10, 10);
	}
	else if (count == 9) {
		return CollisionCircleRect(Px, Pz, 5, -10, -100, 10, 10);
	}
	else if (count == 10) {
		return CollisionCircleRect(Px, Pz, 5, 0, 150, 10, 10);
	}
	else if (count == 11) {
		return CollisionCircleRect(Px, Pz, 5, 130, -130, 10, 10);
	}
	else if (count == 12) {
		return CollisionCircleRect(Px, Pz, 5, -130, 30, 10, 10);
	}
	else if (count == 13) {
		return CollisionCircleRect(Px, Pz, 5, 10, -50, 25, 25);
	}
	else if (count == 14) {
		return CollisionCircleRect(Px, Pz, 5, -100, 155, 15, 15);
	}
	else if (count == 15) {
		return CollisionCircleRect(Px, Pz, 5, -30, 70, 45, 8);
	}
	else {
		return Vector3(Px, 0, Pz); //wont trigger
	}
}


bool Sp2_Minigame::isInHitbox(int hitboxX, int hitboxY, int Ex, int Ey, int radius)
{
	for (int i = 0; i <= radius; i++) {
		for (int j = 0; j <= radius; j++) {
			if (hitboxX == Ex + i and hitboxY == Ey + j) {
				return true;
			}
			if (hitboxX == Ex - i and hitboxY == Ey - j) {
				return true;
			}
			if (hitboxX == Ex + i and hitboxY == Ey - j) {
				return true;
			}
			if (hitboxX == Ex - i and hitboxY == Ey + j) {
				return true;
			}
		}
	}
	return false;
}

bool Sp2_Minigame::EnemyDetectPlayer(int Px, int Py, int Ex, int Ey)
{
	for (int i = 0; i <= 80; i++) {
		for (int j = 0; j <= 80; j++) {
			if (Px == Ex + i and Py == Ey + j) {
				return true;
			}
			if (Px == Ex - i and Py == Ey - j) {
				return true;
			}
			if (Px == Ex + i and Py == Ey - j) {
				return true;
			}
			if (Px == Ex - i and Py == Ey + j) {
				return true;
			}
		}
	}
	return false;
}

bool Sp2_Minigame::interactableObject(int Px, int Pz, int ObjectX, int ObjectZ, int x, int z)
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

float Sp2_Minigame::DistanceParameter(float EnemyXPos, float EnemyZPos, float OriginalPosX, float OriginalPosZ) {
	float squareofxz = (pow(EnemyXPos - OriginalPosX, 2) + pow(EnemyZPos - OriginalPosZ, 2));
	float magnitude = sqrt(squareofxz);

	return magnitude;
}

void Sp2_Minigame::UpdateENV(double dt)
{
	camera_posHori = Vector3(camera2.position.x, 20, camera2.position.z);
	camera_posVert = Vector3(camera2.position.x, camera2.position.y, camera2.position.z);
	point_pos = Vector3(EnemyPosX, 20, EnemyPosZ); // E1
	point_pos2 = Vector3(EnemyPosX2, 20, EnemyPosZ2); // E2
	point_pos3 = Vector3(EnemyPosX3, 20, EnemyPosZ3); // E2

	pitchVert = (camera_posVert - point_pos).Normalized();
	pitchVert2 = (camera_posVert - point_pos2).Normalized();
	pitchVert3 = (camera_posVert - point_pos3).Normalized();

	look = (camera_posHori - point_pos).Normalized();
	look2 = (camera_posHori - point_pos2).Normalized();
	look3 = (camera_posHori - point_pos3).Normalized();

	angleHori = (180.f / Math::PI) * (std::acos(look.Dot(target) / (look.Length()) * (target.Length())));
	angleVert = (180.f / Math::PI) * (std::acos(look.Dot(pitchVert) / (look.Length()) * (pitchVert.Length())));
	angleHori2 = (180.f / Math::PI) * (std::acos(look2.Dot(target) / (look2.Length()) * (target.Length())));
	angleVert2 = (180.f / Math::PI) * (std::acos(look2.Dot(pitchVert2) / (look2.Length()) * (pitchVert2.Length())));
	angleHori3 = (180.f / Math::PI) * (std::acos(look3.Dot(target) / (look3.Length()) * (target.Length())));
	angleVert3 = (180.f / Math::PI) * (std::acos(look3.Dot(pitchVert3) / (look3.Length()) * (pitchVert3.Length())));

	float totalObj = 17;
	static float time_eyelid = 0.f;
	// Set background color to dark blue
	glClearColor(0.0f, 0.5f, 0.7f, 0.0f);

	fps = 1.f / dt;


	POINT initmousepos;
	mousepos = &initmousepos;//init mousepos
	GetCursorPos(mousepos); //get mouse position







	static float resetbutton = 0.f;
	if (((isAlive == false) or (isAlive == true and Diamond >= 10)) and timer_gameover > 1.5 and timer_Retry > 1 and mousepos->x > rect.left + 310 and mousepos->x < rect.right - 315 and mousepos->y > rect.top + 495 and mousepos->y < rect.bottom - 70) {
		if (Application::IsKeyPressed(VK_LBUTTON) and resetbutton > 0.2) {
			translateX = 0;
			translateZ = 0;
			rotateBody = 0;

			rotateAngle = 0;
			translatebodyY_Jump = 0;

			allowMovement = true;
			allowJump = false;
			rotatelegs_jump = 0;

			isDemon = true;
			tail_shakeleft = false;
			tail_shakeright = true;
			rotateDemonTail = 0;
			SkillsTimer = 0;

			RotateRightArm_Movement = 0;
			RotateLeftArm_Movement = 0;
			rotate_Eyelid = -300;

			RotateRightArm_SwingSword = 0;
			ScaleFireEffect1 = 1;
			ScaleFireEffect2 = 1;
			ScaleFireEffect3 = 1;
			ScaleFireEffect4 = 1;
			ScaleFireEffect5 = 1;
			ScaleFireEffect6 = 1;

			allowAttack = false;
			SwordSwingForward = false;
			SwordSwingBackward = true;
			allowSwordSwing = false;
			TranslateUpandDown = 0;
			HopUp = true;
			HopDown = false;
			ScaleUpandDown = 4;
			HopUp_FIRE = true;
			HopDown_FIRE = false;

			allowUltimateAttack = false;
			allowSpinattack = false;
			rotateBodySpinAttack = 0;
			SpinStarts = true;
			SpinEnds = false;
			rotateSwordSpinAttack = 0;
			Temp = 0;

			time_eyelid = 0.f;
			rotateFusionWave = -90;
			translateFusionWaveX = 0;
			translateFusionWaveZ = 0;
			allowJumpAttack = false;
			jumpAttack_rotation = 0;
			allowReleaseofFusionWave = false;
			allowFusionWaveTransverse = false;
			UltiSwordRevert_rotation = false;
			skill = false;
			i = false;
			j = 0;
			rotateEnemyBody = 0;
			ShockwaveScale = 0.1;
			ShockwavedirectionX = 0;
			ShockwavedirectionZ = 15;
			DisplayShockwave = false;

			open = false;
			close = true;
			Health = 100;
			isAlive = true;
			EnemyHealth = 100;
			EnemyDieRotation = 0;
			timer_gameover = 0;
			resetbutton = 0;
			timer_Retry = 0;

			timer_Skill1 = 0;
			timer_Skill2 = 0;
			timer_Skill3 = 0;
			DialogueBoxOpen = false;
			GameDialogueTeleporter = false;
			Dialogue = 1;
			isShopUIOpen = false;

			EnemyPosX = 100;
			EnemyPosZ = -140;
			EnemyPosX2 = 100;
			EnemyPosZ2 = 140;
			EnemyPosX3 = -100;
			EnemyPosZ3 = -140;

			EnemyShockwaveScale = 0.1;
			EnemyShockwaveScale2 = 0.1;
			EnemyShockwaveScale3 = 0.1;

			rotateEnemyBody2 = 180;
			rotateEnemyBody3 = 0;
			allowEnemyMovement = true;
			allowEnemyMovement2 = true;
			allowEnemyMovement3 = true;

			EnemyHealth2 = 100;
			EnemyDieRotation2 = 0;

			EnemyHealth3 = 100;
			EnemyDieRotation3 = 0;

			test2 = 0.7f;
			jump2 = true;
			test3 = 0.7f;
			jump3 = true;
			Ground2 = false;
			Ground3 = false;
			Diamond = 0;
			EarnedCoins = true;
			EnemyViewAngle = 0;
			EnemyViewAngle2 = 0;
			EnemyViewAngle3 = 0;
			triggerEnemy = false;
			triggerEnemy2 = false;
			triggerEnemy3 = false;
			EnemyRightArm_JumpAttackRotation = 0;
			EnemyRightArm_JumpAttackRotation2 = 0;
			EnemyRightArm_JumpAttackRotation3 = 0;

			EnemyHopUp = true;
			EnemyHopDown = false;
			EnemyUpandDown_Translation = 0;
			EnemyHopUp2 = true;
			EnemyHopDown2 = false;
			EnemyUpandDown_Translation2 = 0;
			EnemyHopUp3 = true;
			EnemyHopDown3 = false;
			EnemyUpandDown_Translation3 = 0;

			min = 2, sec = 60;
			Application::Minigame_timer = 60;

			ShowCursor(false);
			Application::ChangeScene(Application::gs_game);

			for (int i = 0; i < 10; i++) {
				if (Diamonds[i] == nullptr) {
					Diamonds[i] = new CollectObjects;
					Diamonds[i]->SetXYZ(rand() % 360 - 180, -3, rand() % 360 - 180);
				}
			}
		}
		resetbutton += dt;
	}
	else {
		resetbutton = 0;
	}




	static const float LSPEED = 20.f;

	if (Application::IsKeyPressed('5'))
	{
		light[0].type = Light::LIGHT_POINT;
		glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
		//to do: switch light type to POINT and pass the information to shader
	}
	else if (Application::IsKeyPressed('6'))
	{
		light[0].type = Light::LIGHT_DIRECTIONAL;
		glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
		//to do: switch light type to DIRECTIONAL and pass the information to shader
	}
	else if (Application::IsKeyPressed('7'))
	{
		light[0].type = Light::LIGHT_SPOT;
		glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
		//to do: switch light type to SPOT and pass the information to shader
	}

	if (Application::IsKeyPressed('5'))
	{
		light[1].type = Light::LIGHT_POINT;
		glUniform1i(m_parameters[U_LIGHT1_TYPE], light[1].type);
	}
	else if (Application::IsKeyPressed('6'))
	{
		light[1].type = Light::LIGHT_DIRECTIONAL;
		glUniform1i(m_parameters[U_LIGHT1_TYPE], light[1].type);
	}
	else if (Application::IsKeyPressed('7'))
	{
		light[1].type = Light::LIGHT_SPOT;
		glUniform1i(m_parameters[U_LIGHT1_TYPE], light[1].type);
	}

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

	if (Application::IsKeyPressed('1')) //enable back face culling
		glEnable(GL_CULL_FACE);
	if (Application::IsKeyPressed('2')) //disable back face culling
		glDisable(GL_CULL_FACE);
	if (Application::IsKeyPressed('3'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //default fill mode
	if (Application::IsKeyPressed('4'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //wireframe mode


	static float ttemp = 0.f;
	RotateBlackHole += (float)(30 * dt);

	if (isAlive == true) {
		if (isAlive == true and timer_gameover < 1.5) {

			static bool bLButtonState = false;
			if (!bLButtonState && Application::IsMousePressed(0))
			{
				bLButtonState = true;
			}
			else if (bLButtonState && !Application::IsMousePressed(0))
			{
				bLButtonState = false;
				if (allowJumpAttack == false and DialogueBoxOpen == false and skill == false and isDemon == true and allowAttack == false and allowSpinattack == false and allowReleaseofFusionWave == false and UltiSwordRevert_rotation == false) {
					allowAttack = true;
					skill = false;
					jumpAttack_rotation = 0;
				}

			}


			if (Application::IsKeyPressed(VK_SPACE) and ttemp > 0.1 and DialogueBoxOpen == false and skill == false and allowJump == false and allowSpinattack == false and allowJumpAttack == false and UltiSwordRevert_rotation == false) {
				allowJump = true;
				allowMovement = false;
				jumpAttack_rotation = 0;
				ttemp = 0;
			}


			static bool Phase1 = false;
			static bool Phase2 = false;
			if (!Phase1 && Application::IsKeyPressed('E'))
			{
				Phase1 = true;
				if (ttemp > 0.1 and timer_Skill3 <= 1 and DialogueBoxOpen == false and allowJump == false and allowJumpAttack == false and allowSpinattack == false and allowAttack == false and isDemon == true and allowUltimateAttack == false and allowReleaseofFusionWave == false) {
					allowUltimateAttack = true;
					jumpAttack_rotation = 0;
					timer_Skill3 = 20;
					RotateRightArm_SwingSword = 0;
					SkillsTimer = 0;
					allowReleaseofFusionWave = false;
					skill = true;
					ttemp = 0;
					PlaySound(TEXT("AlucardUlti01.wav"), NULL, SND_FILENAME | SND_ASYNC);
				}
			}

			if (!Phase2 && Application::IsKeyPressed('E'))
			{
				Phase2 = true;
				if (ttemp > 0.2 and DialogueBoxOpen == false and allowJump == false and isDemon == true and allowUltimateAttack == true and allowReleaseofFusionWave == false and allowAttack == false and allowSpinattack == false and allowJumpAttack == false and SkillsTimer <= 400) {
					allowReleaseofFusionWave = true;
					ttemp = 0;
					PlaySound(TEXT("AlucardUlti02.wav"), NULL, SND_FILENAME | SND_ASYNC);
				}
			}
			else if (Phase2 && !Application::IsKeyPressed('E'))
			{
				Phase2 = false;
				Phase1 = false;
			}


			ttemp += dt;
			// Movement of the body
			if (Application::IsKeyPressed('W') and (not Application::IsKeyPressed('S')) and DialogueBoxOpen == false and skill == false and allowJump == false and allowAttack == false and allowSpinattack == false and allowReleaseofFusionWave == false) {
				rotateBody = 180;
				if (allowFusionWaveTransverse == false) {
					rotateFusionWave = -270;
				}
				ShockwavedirectionX = 0;
				ShockwavedirectionZ = -15;
				jumpAttack_rotation = 0;

				translateZ -= (float)(20 * dt);

				for (unsigned i = 0; i <= totalObj; ++i) {
					Vector3 finalPos = PlayerCollision(i, translateX, translateZ); //ignore y
					translateX = finalPos.x;
					translateZ = finalPos.z;
				}
				allowMovement = true;

				if (Application::IsKeyPressed('A') and skill == false and (not Application::IsKeyPressed('D'))) {
					rotateBody = 225;

					if (allowFusionWaveTransverse == false) {
						rotateFusionWave = -225;
					}
					ShockwavedirectionX = -15;
					ShockwavedirectionZ = -15;

					translateX -= (float)(20 * dt);
					for (unsigned i = 0; i <= totalObj; ++i) {
						Vector3 finalPos = PlayerCollision(i, translateX, translateZ); //ignore y
						translateX = finalPos.x;
						translateZ = finalPos.z;
					}

				}
				if (Application::IsKeyPressed('D') and skill == false and (not Application::IsKeyPressed('A'))) {
					rotateBody = 135;
					if (allowFusionWaveTransverse == false) {
						rotateFusionWave = -315;
					}
					ShockwavedirectionX = 15;
					ShockwavedirectionZ = -15;

					translateX += (float)(20 * dt);
					for (unsigned i = 0; i <= totalObj; ++i) {
						Vector3 finalPos = PlayerCollision(i, translateX, translateZ); //ignore y
						translateX = finalPos.x;
						translateZ = finalPos.z;
					}
				}
			}
			else if (Application::IsKeyPressed('A') and not(Application::IsKeyPressed('D')) and DialogueBoxOpen == false and skill == false and allowJump == false and allowAttack == false and allowSpinattack == false and allowReleaseofFusionWave == false) {
				rotateBody = 270;
				translateX -= (float)(20 * dt);
				for (unsigned i = 0; i <= totalObj; ++i) {
					Vector3 finalPos = PlayerCollision(i, translateX, translateZ); //ignore y
					translateX = finalPos.x;
					translateZ = finalPos.z;
				}

				if (allowFusionWaveTransverse == false) {
					rotateFusionWave = -180;
				}
				ShockwavedirectionX = -15;
				ShockwavedirectionZ = 0;
				jumpAttack_rotation = 0;
				allowMovement = true;
				if (Application::IsKeyPressed('S')) {
					rotateBody = 315;
					if (allowFusionWaveTransverse == false) {
						rotateFusionWave = -135;
					}
					ShockwavedirectionX = -15;
					ShockwavedirectionZ = 15;
					translateZ += (float)(20 * dt);
					for (unsigned i = 0; i <= totalObj; ++i) {
						Vector3 finalPos = PlayerCollision(i, translateX, translateZ); //ignore y
						translateX = finalPos.x;
						translateZ = finalPos.z;
					}
				}
			}
			else if (Application::IsKeyPressed('S') and (not Application::IsKeyPressed('W')) and DialogueBoxOpen == false and skill == false and allowJump == false and allowAttack == false and allowSpinattack == false and allowReleaseofFusionWave == false) {
				rotateBody = 0;
				ShockwavedirectionX = 0;
				if (allowFusionWaveTransverse == false) {
					rotateFusionWave = -90;
				}
				ShockwavedirectionZ = 15;
				translateZ += (float)(20 * dt);
				for (unsigned i = 0; i <= totalObj; ++i) {
					Vector3 finalPos = PlayerCollision(i, translateX, translateZ); //ignore y
					translateX = finalPos.x;
					translateZ = finalPos.z;
				}
				jumpAttack_rotation = 0;
				allowMovement = true;
				if (Application::IsKeyPressed('D')) {
					rotateBody = 45;
					if (allowFusionWaveTransverse == false) {
						rotateFusionWave = -45;
					}
					ShockwavedirectionX = 15;
					ShockwavedirectionZ = 15;
					translateX += (float)(20 * dt);
					for (unsigned i = 0; i <= totalObj; ++i) {
						Vector3 finalPos = PlayerCollision(i, translateX, translateZ); //ignore y
						translateX = finalPos.x;
						translateZ = finalPos.z;
					}
				}
			}
			else if (Application::IsKeyPressed('D') and (not Application::IsKeyPressed('A')) and DialogueBoxOpen == false and skill == false and allowJump == false and allowAttack == false and allowSpinattack == false and allowReleaseofFusionWave == false) {
				rotateBody = 90;
				ShockwavedirectionX = 15;
				if (allowFusionWaveTransverse == false) {
					rotateFusionWave = 0;
				}
				ShockwavedirectionZ = 0;
				translateX += (float)(20 * dt);
				for (unsigned i = 0; i <= totalObj; ++i) {
					Vector3 finalPos = PlayerCollision(i, translateX, translateZ); //ignore y
					translateX = finalPos.x;
					translateZ = finalPos.z;
				}
				allowMovement = true;
				jumpAttack_rotation = 0;
			}
			else {
				allowMovement = false;
				RotateLeftLeg_Movement = 0;
				RotateRightLeg_Movement = 0;
			}

			if (Application::IsKeyPressed('Z') and timer_Skill2 <= 1 and DialogueBoxOpen == false and skill == false and UltiSwordRevert_rotation == false and allowReleaseofFusionWave == false and allowJump == false and isDemon == true and allowAttack == false and allowSpinattack == false) {
				allowSpinattack = true;
				timer_Skill2 = 5;
				skill = false;
				jumpAttack_rotation = 0;
				PlaySound(TEXT("AlucardSwirling.wav"), NULL, SND_FILENAME | SND_ASYNC);
			}
			if (Application::IsKeyPressed('X') and timer_Skill1 <= 1 and DialogueBoxOpen == false and skill == false and UltiSwordRevert_rotation == false and allowReleaseofFusionWave == false and allowJump == false and allowJumpAttack == false and isDemon == true and allowAttack == false and allowSpinattack == false and jumpAttack_rotation <= 0) {
				allowJumpAttack = true;
				timer_Skill1 = 8;
				allowJump = true;
				allowMovement = false;
				DisplayShockwave = true;
				RotateRightArm_SwingSword = 0;
				jumpAttack_rotation = 0;
				skill = false;
				PlaySound(TEXT("AlucardSmash.wav"), NULL, SND_FILENAME | SND_ASYNC);
			}
		}


		// Movement animation
		if (allowMovement == true and allowJump == false and allowAttack == false and allowReleaseofFusionWave == false) {
			if (swingforward == true and swingbackward == false) {
				RotateLeftArm_Movement = RotateRightArm_Movement = RotateLeftLeg_Movement = RotateRightLeg_Movement += (float)(80 * dt);
			}

			if (swingbackward == true and swingforward == false) {
				RotateLeftArm_Movement = RotateRightArm_Movement = RotateLeftLeg_Movement = RotateRightLeg_Movement -= (float)(80 * dt);
			}

			if (RotateLeftArm_Movement >= 25 and RotateRightArm_Movement >= 25 and RotateLeftLeg_Movement >= 25 and RotateRightLeg_Movement >= 25 and swingforward == true and swingbackward == false) {
				swingforward = false;
				swingbackward = true;
			}
			if (RotateLeftArm_Movement <= -25 and RotateRightArm_Movement <= -25 and RotateLeftLeg_Movement <= -25 and RotateRightLeg_Movement <= -25 and swingforward == false and swingbackward == true) {
				swingforward = true;
				swingbackward = false;
			}

			if (HopUp == true and HopDown == false) {
				TranslateUpandDown += (float)(4 * dt);
			}
			if (HopDown == true and HopUp == false) {
				TranslateUpandDown -= (float)(4 * dt);
			}
			if (TranslateUpandDown >= 0.7 and HopUp == true and HopDown == false) {
				HopUp = false;
				HopDown = true;
			}
			if (TranslateUpandDown <= -0.7 and HopUp == false and HopDown == true) {
				HopUp = true;
				HopDown = false;
			}
		}

		if (allowMovement == false) {
			if (RotateRightArm_Movement >= 0) {
				RotateRightArm_Movement -= (float)(80 * dt);
			}
			if (RotateRightArm_Movement <= 0) {
				RotateRightArm_Movement += (float)(80 * dt);
			}
			if (RotateLeftArm_Movement <= 0) {
				RotateLeftArm_Movement += (float)(80 * dt);
			}
			if (RotateLeftArm_Movement >= 0) {
				RotateLeftArm_Movement -= (float)(80 * dt);
			}
			TranslateUpandDown = 0;
		}

		if (EnemyHopUp == true and EnemyHopDown == false) {
			EnemyUpandDown_Translation += (float)(4 * dt);
		}
		if (EnemyHopDown == true and EnemyHopUp == false) {
			EnemyUpandDown_Translation -= (float)(4 * dt);
		}
		if (EnemyUpandDown_Translation >= 0.7 and EnemyHopUp == true and EnemyHopDown == false) {
			EnemyHopUp = false;
			EnemyHopDown = true;
		}
		if (EnemyUpandDown_Translation <= -0.7 and EnemyHopUp == false and EnemyHopDown == true) {
			EnemyHopUp = true;
			EnemyHopDown = false;
		}
		if (EnemyHealth <= 0) {
			EnemyUpandDown_Translation = 0;
		}


		if (EnemyHopUp2 == true and EnemyHopDown2 == false) {
			EnemyUpandDown_Translation2 += (float)(4 * dt);
		}
		if (EnemyHopDown2 == true and EnemyHopUp2 == false) {
			EnemyUpandDown_Translation2 -= (float)(4 * dt);
		}
		if (EnemyUpandDown_Translation2 >= 0.7 and EnemyHopUp2 == true and EnemyHopDown2 == false) {
			EnemyHopUp2 = false;
			EnemyHopDown2 = true;
		}
		if (EnemyUpandDown_Translation2 <= -0.7 and EnemyHopUp2 == false and EnemyHopDown2 == true) {
			EnemyHopUp2 = true;
			EnemyHopDown2 = false;
		}
		if (EnemyHealth2 <= 0) {
			EnemyUpandDown_Translation2 = 0;
		}

		if (EnemyHopUp3 == true and EnemyHopDown3 == false) {
			EnemyUpandDown_Translation3 += (float)(4 * dt);
		}
		if (EnemyHopDown3 == true and EnemyHopUp3 == false) {
			EnemyUpandDown_Translation3 -= (float)(4 * dt);
		}
		if (EnemyUpandDown_Translation3 >= 0.7 and EnemyHopUp3 == true and EnemyHopDown3 == false) {
			EnemyHopUp3 = false;
			EnemyHopDown3 = true;
		}
		if (EnemyUpandDown_Translation3 <= -0.7 and EnemyHopUp3 == false and EnemyHopDown3 == true) {
			EnemyHopUp3 = true;
			EnemyHopDown3 = false;
		}
		if (EnemyHealth3 <= 0) {
			EnemyUpandDown_Translation3 = 0;
		}


		if (isDemon == true) {
			if (HopUp_FIRE == true and HopDown_FIRE == false) {
				ScaleUpandDown += (float)(3 * dt);
			}
			if (HopDown_FIRE == true and HopUp_FIRE == false) {
				ScaleUpandDown -= (float)(3 * dt);
			}
			if (ScaleUpandDown >= 4 and HopUp_FIRE == true and HopDown_FIRE == false) {
				HopUp_FIRE = false;
				HopDown_FIRE = true;
			}
			if (ScaleUpandDown <= 2 and HopUp_FIRE == false and HopDown_FIRE == true) {
				HopUp_FIRE = true;
				HopDown_FIRE = false;
			}
		}
		else {
			ScaleUpandDown = 4;
		}


		// Swinging of sword
		if (allowAttack == true) {
			if (SwordSwingForward == true and SwordSwingBackward == false) {
				RotateRightArm_SwingSword += (float)(300 * dt);
			}
			if (SwordSwingForward == false and SwordSwingBackward == true) {
				RotateRightArm_SwingSword -= (float)(300 * dt);
			}
			if (RotateRightArm_SwingSword <= -30 and SwordSwingForward == false and SwordSwingBackward == true) {
				SwordSwingForward = true;
				SwordSwingBackward = false;
			}
			if (RotateRightArm_SwingSword >= 50 and SwordSwingForward == true and SwordSwingBackward == false and allowSwordSwing == false) {
				SwordSwingForward = false;
				SwordSwingBackward = true;
				allowSwordSwing = true;
			}

			if (allowSwordSwing == true) {
				if (RotateRightArm_SwingSword <= 0) {
					RotateRightArm_SwingSword = 0;
					allowSwordSwing = false;
					allowAttack = false;
				}
			}

		}

		if (SkillsTimer <= 500 and allowUltimateAttack == true) {
			if (SkillsTimer <= 50 and skill == true) {
				if (jumpAttack_rotation >= -30) {
					jumpAttack_rotation -= (float)(100 * dt);
				}
				if (skill == true and jumpAttack_rotation <= -30) {
					skill = false;
				}
			}
			if (SkillsTimer >= 50 and jumpAttack_rotation <= 0) {
				jumpAttack_rotation += (float)(60 * dt);
			}
			SkillsTimer++;
		}
		if (SkillsTimer >= 500 and allowUltimateAttack == true) {
			allowUltimateAttack = false;
			skill = false;
			SkillsTimer = 0;
		}


		// Backflip with no movement while performing
		if (allowJump == true) {
			if (allowJumpAttack == false) {
				if (air == true and ground == false) {
					translatebodyY_Jump += (float)(20 * dt);
					rotateAngle -= (float)(177.5 * dt);
					rotatelegs_jump = 50;
				}
				if (ground == true and air == false) {
					translatebodyY_Jump -= (float)(20 * dt);
					rotateAngle -= (float)(177.5 * dt);
					rotatelegs_jump = 50;
				}

				if (translatebodyY_Jump >= 20 and air == true and ground == false) {
					air = false;
					ground = true;
				}
				if (translatebodyY_Jump <= 0 and air == false and ground == true) {
					air = true;
					ground = false;
					allowJump = false;
					allowJumpAttack = false;
					rotateAngle = 0;
					rotatelegs_jump = 0;
				}
			}
			else {
				jumpAttack_rotation = 40;

				if (RotateRightArm_Movement <= 50) {
					RotateRightArm_Movement += (float)(250 * dt);
				}
				if (RotateRightArm_Movement >= 50) {
					RotateRightArm_Movement = 50;
				}


				if (RotateLeftArm_Movement >= -50) {
					RotateLeftArm_Movement -= (float)(250 * dt);
				}
				if (RotateLeftArm_Movement <= -50) {
					RotateLeftArm_Movement = -50;
				}

				if (air == true and ground == false) {
					translatebodyY_Jump += (float)(40 * dt);
					rotateAngle += (float)(677.5 * dt);
					rotatelegs_jump = 50;
				}
				if (ground == true and air == false) {
					translatebodyY_Jump -= (float)(40 * dt);
					rotateAngle += (float)(677.5 * dt);
					rotatelegs_jump = 50;
				}
				if (translatebodyY_Jump <= 0 and air == false and ground == true and i == false) {
					air = true;
					ground = false;
				}

				if (translatebodyY_Jump >= 10 and air == true and ground == false) {
					air = false;
					ground = true;
					i = true;
				}

				if (translatebodyY_Jump > 0 and translatebodyY_Jump <= 10) {
					if (rotateBody == 0) {
						translateZ += 0.5;
						for (unsigned i = 0; i <= totalObj; ++i) {
							Vector3 finalPos = PlayerCollision(i, translateX, translateZ); //ignore y
							translateX = finalPos.x;
							translateZ = finalPos.z;
						}
					}
					if (rotateBody == 45) {
						translateX += 0.5;
						translateZ += 0.5;
						for (unsigned i = 0; i <= totalObj; ++i) {
							Vector3 finalPos = PlayerCollision(i, translateX, translateZ); //ignore y
							translateX = finalPos.x;
							translateZ = finalPos.z;
						}
					}
					if (rotateBody == 90) {
						translateX += 0.5;
						for (unsigned i = 0; i <= totalObj; ++i) {
							Vector3 finalPos = PlayerCollision(i, translateX, translateZ); //ignore y
							translateX = finalPos.x;
							translateZ = finalPos.z;
						}
					}
					if (rotateBody == 135) {
						translateX += 0.5;
						translateZ -= 0.5;
						for (unsigned i = 0; i <= totalObj; ++i) {
							Vector3 finalPos = PlayerCollision(i, translateX, translateZ); //ignore y
							translateX = finalPos.x;
							translateZ = finalPos.z;
						}
					}
					if (rotateBody == 180) {
						translateZ -= 0.5;
						for (unsigned i = 0; i <= totalObj; ++i) {
							Vector3 finalPos = PlayerCollision(i, translateX, translateZ); //ignore y
							translateX = finalPos.x;
							translateZ = finalPos.z;
						}
					}
					if (rotateBody == 225) {
						translateX -= 0.5;
						translateZ -= 0.5;
						for (unsigned i = 0; i <= totalObj; ++i) {
							Vector3 finalPos = PlayerCollision(i, translateX, translateZ); //ignore y
							translateX = finalPos.x;
							translateZ = finalPos.z;
						}
					}
					if (rotateBody == 270) {
						translateX -= 0.5;
						for (unsigned i = 0; i <= totalObj; ++i) {
							Vector3 finalPos = PlayerCollision(i, translateX, translateZ); //ignore y
							translateX = finalPos.x;
							translateZ = finalPos.z;
						}
					}
					if (rotateBody == 315) {
						translateX -= 0.5;
						translateZ += 0.5;
						for (unsigned i = 0; i <= totalObj; ++i) {
							Vector3 finalPos = PlayerCollision(i, translateX, translateZ); //ignore y
							translateX = finalPos.x;
							translateZ = finalPos.z;
						}
					}
				}

				if (translatebodyY_Jump <= 0 and air == false and ground == true and i == true) {
					translatebodyY_Jump = 0;
					if (ShockwaveScale <= 8 and DisplayShockwave == true) {
						ShockwaveScale += (float)(20 * dt);
					}
					if (ShockwaveScale >= 8 and DisplayShockwave == true) {
						DisplayShockwave = false;
					}
					if (j >= 30) {
						ShockwaveScale = 0.1;
						DisplayShockwave = false;
						i = false;
						air = true;
						ground = false;
						allowJump = false;
						allowJumpAttack = false;
						j = 0;
					}
					if (j <= 30) {
						j++;
					}
					rotateAngle = 0;
					rotatelegs_jump = 0;
				}
			}
		}

		if (allowJumpAttack == false and jumpAttack_rotation >= 0) {
			jumpAttack_rotation -= (float)(60 * dt);
		}




		// Demon tail/Mic movement
		if (isDemon == true) {
			if (tail_shakeleft == false and tail_shakeright == true) {
				rotateDemonTail += (float)(15 * dt);
			}
			if (tail_shakeright == false and tail_shakeleft == true) {
				rotateDemonTail -= (float)(15 * dt);
			}

			if (rotateDemonTail >= 15 and tail_shakeright == true and tail_shakeleft == false) {
				tail_shakeright = false;
				tail_shakeleft = true;
			}
			if (rotateDemonTail <= -15 and tail_shakeright == false and tail_shakeleft == true) {
				tail_shakeright = true;
				tail_shakeleft = false;
			}
		}



		// EyeLid movement
		if (time_eyelid >= 5 and time_eyelid <= 8) {
			if (open == true and close == false) {
				rotate_Eyelid += (float)(450 * dt);
			}
			if (close == true and open == false) {
				rotate_Eyelid -= (float)(450 * dt);
			}

			if (rotate_Eyelid >= -100 and open == true and close == false) {
				open = false;
				close = true;
			}
			if (rotate_Eyelid <= -300 and open == false and close == true) {
				open = true;
				close = false;
				time_eyelid = 0;
			}
		}
		time_eyelid += dt;



		if (isDemon == true and allowAttack == false and allowJump == false and allowSpinattack == true) {
			jumpAttack_rotation = 0;
			if (rotateBodySpinAttack <= 490) {
				if (rotateBodySpinAttack <= 470 and SpinStarts == true and SpinEnds == false) {
					rotateBodySpinAttack += (float)(1600 * dt);
					if (rotateBodySpinAttack >= 460) {
						SpinStarts = false;
						SpinEnds = true;
					}
					RotateRightArm_SwingSword += (float)(50 * dt);
					RotateLeftArm_Movement = 100;

					if (Temp <= 100) {
						Temp += (float)(150 * dt);
					}

					rotateSwordSpinAttack = 90;

				}

				if (rotateBodySpinAttack >= 360 and SpinStarts == false and SpinEnds == true) {
					rotateBodySpinAttack -= (float)(300 * dt);
					if (rotateBodySpinAttack <= 360) {
						rotateBodySpinAttack = 0;
						allowSpinattack = false;
						SpinStarts = true;
						SpinEnds = false;
						rotateSwordSpinAttack = 0;
						RotateRightArm_SwingSword = 0;
						Temp = 0;
					}
				}
			}
		}



		if (isDemon == true and allowAttack == false and allowJump == false and allowSpinattack == false and allowReleaseofFusionWave == true and allowUltimateAttack == true) {
			jumpAttack_rotation = 0;
			if (rotateBodySpinAttack >= -390) {
				if (rotateBodySpinAttack >= -360) {
					rotateBodySpinAttack -= (float)(900 * dt);
					rotateSwordSpinAttack = 90;

					if (RotateRightArm_SwingSword >= -50) {
						RotateRightArm_SwingSword -= (float)(140 * dt);
					}

					Temp = 135;
					j = 0;
				}
				if (rotateBodySpinAttack <= -180) {
					allowFusionWaveTransverse = true;
				}

				if (rotateBodySpinAttack <= -360) {
					skill = false;
					j = 0;
					Temp = 0;
					UltiSwordRevert_rotation = true;
					allowUltimateAttack = false;
					allowReleaseofFusionWave = false;
					rotateBodySpinAttack = 0;
				}
			}
		}

		if (UltiSwordRevert_rotation == true) {
			if (rotateSwordSpinAttack >= 0) {
				rotateSwordSpinAttack -= (float)(250 * dt);
			}
			if (rotateSwordSpinAttack <= 0) {
				UltiSwordRevert_rotation = false;
				rotateSwordSpinAttack = 0;
			}
		}

		if (allowFusionWaveTransverse == true) {
			if (rotateFusionWave == 0) {
				if (translateFusionWaveX <= 60) {
					translateFusionWaveX += (float)(80 * dt);
				}
				else {
					allowFusionWaveTransverse = false;
				}
				translateFusionWaveZ = 0;
			}

			if (rotateFusionWave == -45) {
				if (translateFusionWaveX >= -60) {
					translateFusionWaveX += (float)(80 * dt);
				}
				else {
					allowFusionWaveTransverse = false;
				}
				if (translateFusionWaveZ <= 60) {
					translateFusionWaveZ += (float)(80 * dt);
				}
				else {
					allowFusionWaveTransverse = false;
				}
			}

			if (rotateFusionWave == -90) {
				translateFusionWaveX = 0;
				if (translateFusionWaveZ <= 60) {
					translateFusionWaveZ += (float)(80 * dt);
				}
				else {
					allowFusionWaveTransverse = false;
				}
			}
			if (rotateFusionWave == -135) {
				if (translateFusionWaveX >= -60) {
					translateFusionWaveX -= (float)(80 * dt);
				}
				else {
					allowFusionWaveTransverse = false;
				}
				if (translateFusionWaveZ <= 60) {
					translateFusionWaveZ += (float)(80 * dt);
				}
				else {
					allowFusionWaveTransverse = false;
				}
			}
			if (rotateFusionWave == -180) {
				if (translateFusionWaveX >= -60) {
					translateFusionWaveX -= (float)(80 * dt);
				}
				else {
					allowFusionWaveTransverse = false;
				}
				translateFusionWaveZ = 0;
			}
			if (rotateFusionWave == -225) {
				if (translateFusionWaveX >= -60) {
					translateFusionWaveX -= (float)(80 * dt);
				}
				else {
					allowFusionWaveTransverse = false;
				}
				if (translateFusionWaveZ >= -60) {
					translateFusionWaveZ -= (float)(80 * dt);
				}
				else {
					allowFusionWaveTransverse = false;
				}
			}

			if (rotateFusionWave == -270) {
				translateFusionWaveX = 0;
				if (translateFusionWaveZ >= -60) {
					translateFusionWaveZ -= (float)(80 * dt);
				}
				else {
					allowFusionWaveTransverse = false;
				}
			}
			if (rotateFusionWave == -315) {
				if (translateFusionWaveX <= 60) {
					translateFusionWaveX += (float)(80 * dt);
				}
				else {
					allowFusionWaveTransverse = false;
				}
				if (translateFusionWaveZ >= -60) {
					translateFusionWaveZ -= (float)(80 * dt);
				}
				else {
					allowFusionWaveTransverse = false;
				}
			}
		}
		else {
			translateFusionWaveX = 0;
			translateFusionWaveZ = 0;
		}

		if (RotateRightArm_SwingSword <= 0 and allowUltimateAttack == false) {
			RotateRightArm_SwingSword += (float)(80 * dt);
		}



		// Demon Movement
		static float time_detect = 0;
		static bool ismaximumdistance = false;
		static float distancetravelled;

		distancetravelled = DistanceParameter(EnemyPosX, EnemyPosZ, 100, -140);
		EnemyXSpeed = (float)(15 * dt);
		EnemyZSpeed = (float)(15 * dt);
		if (EnemyDetectPlayer(translateX, translateZ, EnemyPosX, EnemyPosZ) == true and ismaximumdistance == false and triggerEnemy == false) {
			triggerEnemy = true;
		}
		if (distancetravelled > 100 and ismaximumdistance == false) {
			ismaximumdistance = true;
		}
		if (ismaximumdistance == true and EnemyHealth > 0) {
			Vector3 EnemyPosition = Vector3(EnemyPosX, 0, EnemyPosZ);
			Vector3 Player = Vector3(translateX, 0, translateZ);
			Vector3 EnemytoOGPos = (Vector3(100, 0, -140) - EnemyPosition).Normalized();
			Vector3 target = Vector3(0, 0, 1);
			angle = (180.f / Math::PI) * (std::acos(EnemytoOGPos.Dot(target) / (EnemytoOGPos.Length()) * (target.Length())));

			if (EnemytoOGPos.x > 0) {
				EnemyViewAngle = angle;
			}
			else {
				EnemyViewAngle = -angle;
			}
			if (EnemyPosX >= 100) {
				EnemyPosX -= EnemyXSpeed;
			}
			if (EnemyPosX <= 100) {
				EnemyPosX += EnemyXSpeed;
			}
			if (EnemyPosZ >= -140) {
				EnemyPosZ -= EnemyZSpeed;
			}
			if (EnemyPosZ <= -140) {
				EnemyPosZ += EnemyZSpeed;
			}
		}
		if (distancetravelled <= 0.5 and ismaximumdistance == true and time_detect <= 2 and EnemyHealth > 0) {
			if (RotateEnemyRightArm_Movement >= 0) {
				RotateEnemyRightArm_Movement -= (float)(80 * dt);
			}
			if (RotateEnemyRightArm_Movement <= 0) {
				RotateEnemyRightArm_Movement += (float)(80 * dt);
			}
			if (RotateEnemyLeftArm_Movement <= 0) {
				RotateEnemyLeftArm_Movement += (float)(80 * dt);
			}
			if (RotateEnemyLeftArm_Movement >= 0) {
				RotateEnemyLeftArm_Movement -= (float)(80 * dt);
			}
			triggerEnemy = false;
			EnemyViewAngle = 0;
			RotateEnemyLeftLeg_Movement = 0;
			RotateEnemyRightLeg_Movement = 0;
			time_detect += dt;
			if (time_detect <= 0.5) {
				EnemyHealth = 100;
			}
		}

		if (time_detect >= 2) {
			time_detect = 0;
			ismaximumdistance = false;
		}
		if (triggerEnemy == true and allowEnemyMovement == true) {
			if (ismaximumdistance == false) {
				Vector3 EnemyPosition = Vector3(EnemyPosX, 0, EnemyPosZ);
				Vector3 Player = Vector3(translateX, 0, translateZ);
				Vector3 distanceVector = (Player - EnemyPosition);
				Vector3 target = Vector3(0, 0, 1);
				angle = (180.f / Math::PI) * (std::acos(distanceVector.Dot(target) / (distanceVector.Length()) * (target.Length())));

				if (distanceVector.x > 0) {
					EnemyViewAngle = angle;
				}
				else {
					EnemyViewAngle = -angle;
				}

				if (distanceVector.Length() >= 30) {
					if (EnemyPosX > translateX) {
						EnemyPosX -= EnemyXSpeed;
					}
					else if (EnemyPosX < translateX) {
						EnemyPosX += EnemyXSpeed;
					}
					else {
						EnemyXSpeed = 0;
					}
					if (EnemyPosZ > translateZ) {
						EnemyPosZ -= EnemyZSpeed;
					}
					else if (EnemyPosZ < translateZ) {
						EnemyPosZ += EnemyZSpeed;
					}
					else {
						EnemyZSpeed = 0;
					}
				}
				else {
					EnemyXSpeed = 0;
					EnemyZSpeed = 0;
				}

				if (EnemyXSpeed == 0 and EnemyZSpeed == 0) {
					AllowEnemyAttack = true;
				}
				else {
					AllowEnemyAttack = false;
				}
			}
			if (EnemyXSpeed != 0 or EnemyZSpeed != 0) {
				if (swingforwardEnemy == true and swingbackwardEnemy == false) {
					RotateEnemyLeftArm_Movement = RotateEnemyRightArm_Movement = RotateEnemyLeftLeg_Movement = RotateEnemyRightLeg_Movement += (float)(60 * dt);
				}

				if (swingbackwardEnemy == true and swingforwardEnemy == false) {
					RotateEnemyLeftArm_Movement = RotateEnemyRightArm_Movement = RotateEnemyLeftLeg_Movement = RotateEnemyRightLeg_Movement -= (float)(60 * dt);
				}

				if (RotateEnemyLeftArm_Movement >= 25 and RotateEnemyRightArm_Movement >= 25 and RotateEnemyLeftLeg_Movement >= 25 and RotateEnemyRightLeg_Movement >= 25 and swingforwardEnemy == true and swingbackwardEnemy == false) {
					swingforwardEnemy = false;
					swingbackwardEnemy = true;
				}
				if (RotateEnemyLeftArm_Movement <= -25 and RotateEnemyRightArm_Movement <= -25 and RotateEnemyLeftLeg_Movement <= -25 and RotateEnemyRightLeg_Movement <= -25 and swingforwardEnemy == false and swingbackwardEnemy == true) {
					swingforwardEnemy = true;
					swingbackwardEnemy = false;
				}
			}
			else {
				if (allowEnemyMovement == true) {
					if (RotateEnemyRightArm_Movement >= 0) {
						RotateEnemyRightArm_Movement -= (float)(80 * dt);
					}
					if (RotateEnemyRightArm_Movement <= 0) {
						RotateEnemyRightArm_Movement += (float)(80 * dt);
					}
					if (RotateEnemyLeftArm_Movement <= 0) {
						RotateEnemyLeftArm_Movement += (float)(80 * dt);
					}
					if (RotateEnemyLeftArm_Movement >= 0) {
						RotateEnemyLeftArm_Movement -= (float)(80 * dt);
					}
					RotateEnemyLeftLeg_Movement = 0;
					RotateEnemyRightLeg_Movement = 0;
				}
			}
		}
		else {
			if (allowEnemyMovement == true) {
				if (RotateEnemyRightArm_Movement >= 0) {
					RotateEnemyRightArm_Movement -= (float)(80 * dt);
				}
				if (RotateEnemyRightArm_Movement <= 0) {
					RotateEnemyRightArm_Movement += (float)(80 * dt);
				}
				if (RotateEnemyLeftArm_Movement <= 0) {
					RotateEnemyLeftArm_Movement += (float)(80 * dt);
				}
				if (RotateEnemyLeftArm_Movement >= 0) {
					RotateEnemyLeftArm_Movement -= (float)(80 * dt);
				}
				RotateEnemyLeftLeg_Movement = 0;
				RotateEnemyRightLeg_Movement = 0;
				EnemyXSpeed = 0;
				EnemyZSpeed = 0;
			}
		}





		// Demon2 Movement
		static bool ismaximumdistance2 = false;
		static float distancetravelled2;
		static float time_detect2 = 0;
		distancetravelled2 = DistanceParameter(EnemyPosX2, EnemyPosZ2, 100, 140);
		EnemyXSpeed2 = (float)(15 * dt);
		EnemyZSpeed2 = (float)(15 * dt);
		if (EnemyDetectPlayer(translateX, translateZ, EnemyPosX2, EnemyPosZ2) == true and ismaximumdistance2 == false and triggerEnemy2 == false) {
			triggerEnemy2 = true;
		}
		if (distancetravelled2 > 100 and ismaximumdistance2 == false) {
			ismaximumdistance2 = true;
		}
		if (ismaximumdistance2 == true and EnemyHealth2 > 0) {
			Vector3 EnemyPosition = Vector3(EnemyPosX2, 0, EnemyPosZ2);
			Vector3 Player = Vector3(translateX, 0, translateZ);
			Vector3 EnemytoOGPos = (Vector3(100, 0, 140) - EnemyPosition).Normalized();
			Vector3 target = Vector3(0, 0, 1);
			angle2 = (180.f / Math::PI) * (std::acos(EnemytoOGPos.Dot(target) / (EnemytoOGPos.Length()) * (target.Length())));

			if (EnemytoOGPos.x > 0) {
				EnemyViewAngle2 = angle2;
			}
			else {
				EnemyViewAngle2 = -angle2;
			}

			if (EnemyPosX2 >= 100) {
				EnemyPosX2 -= EnemyXSpeed2;
				rotateEnemyBody2 = -90;
			}
			if (EnemyPosX2 <= 100) {
				EnemyPosX2 += EnemyXSpeed2;
				rotateEnemyBody2 = -90;
			}
			if (EnemyPosZ2 >= 140) {
				EnemyPosZ2 -= EnemyZSpeed2;
				rotateEnemyBody2 = 180;
			}
			if (EnemyPosZ2 <= 140) {
				EnemyPosZ2 += EnemyZSpeed2;
				rotateEnemyBody2 = 0;
			}
		}
		if (distancetravelled2 <= 0.5 and ismaximumdistance2 == true and time_detect2 <= 2 and EnemyHealth2 > 0) {
			if (RotateEnemyRightArm_Movement2 >= 0) {
				RotateEnemyRightArm_Movement2 -= (float)(80 * dt);
			}
			if (RotateEnemyRightArm_Movement2 <= 0) {
				RotateEnemyRightArm_Movement2 += (float)(80 * dt);
			}
			if (RotateEnemyLeftArm_Movement2 <= 0) {
				RotateEnemyLeftArm_Movement2 += (float)(80 * dt);
			}
			if (RotateEnemyLeftArm_Movement2 >= 0) {
				RotateEnemyLeftArm_Movement2 -= (float)(80 * dt);
			}
			RotateEnemyLeftLeg_Movement2 = 0;
			RotateEnemyRightLeg_Movement2 = 0;
			time_detect2 += dt;
			rotateEnemyBody2 = 180;
			if (time_detect2 <= 0.5) {
				EnemyHealth2 = 100;
			}
		}
		if (time_detect2 >= 2) {
			time_detect2 = 0;
			ismaximumdistance2 = false;
		}
		if (triggerEnemy2 == true and allowEnemyMovement2 == true) {
			if (ismaximumdistance2 == false) {
				Vector3 EnemyPosition = Vector3(EnemyPosX2, 0, EnemyPosZ2);
				Vector3 Player = Vector3(translateX, 0, translateZ);
				Vector3 distanceVector = (Player - EnemyPosition);
				Vector3 target = Vector3(0, 0, 1);
				angle2 = (180.f / Math::PI) * (std::acos(distanceVector.Dot(target) / (distanceVector.Length()) * (target.Length())));

				if (distanceVector.x > 0) {
					EnemyViewAngle2 = angle2;
				}
				else {
					EnemyViewAngle2 = -angle2;
				}

				if (distanceVector.Length() >= 30) {
					if (EnemyPosX2 > translateX) {
						EnemyPosX2 -= EnemyXSpeed2;
					}
					else if (EnemyPosX2 < translateX) {
						EnemyPosX2 += EnemyXSpeed2;
					}
					else {
						EnemyXSpeed2 = 0;
					}
					if (EnemyPosZ2 > translateZ) {
						EnemyPosZ2 -= EnemyZSpeed2;
					}
					else if (EnemyPosZ2 < translateZ) {
						EnemyPosZ2 += EnemyZSpeed2;
					}
					else {
						EnemyZSpeed2 = 0;
					}
				}
				else {
					EnemyXSpeed2 = 0;
					EnemyZSpeed2 = 0;
				}

				if (EnemyXSpeed2 == 0 and EnemyZSpeed2 == 0) {
					AllowEnemyAttack2 = true;
				}
				else {
					AllowEnemyAttack2 = false;
				}
			}
			if (EnemyXSpeed2 != 0 or EnemyZSpeed2 != 0) {
				if (swingforwardEnemy2 == true and swingbackwardEnemy2 == false) {
					RotateEnemyLeftArm_Movement2 = RotateEnemyRightArm_Movement2 = RotateEnemyLeftLeg_Movement2 = RotateEnemyRightLeg_Movement2 += (float)(60 * dt);
				}

				if (swingbackwardEnemy2 == true and swingforwardEnemy2 == false) {
					RotateEnemyLeftArm_Movement2 = RotateEnemyRightArm_Movement2 = RotateEnemyLeftLeg_Movement2 = RotateEnemyRightLeg_Movement2 -= (float)(60 * dt);
				}

				if (RotateEnemyLeftArm_Movement2 >= 25 and RotateEnemyRightArm_Movement2 >= 25 and RotateEnemyLeftLeg_Movement2 >= 25 and RotateEnemyRightLeg_Movement2 >= 25 and swingforwardEnemy2 == true and swingbackwardEnemy2 == false) {
					swingforwardEnemy2 = false;
					swingbackwardEnemy2 = true;
				}
				if (RotateEnemyLeftArm_Movement2 <= -25 and RotateEnemyRightArm_Movement2 <= -25 and RotateEnemyLeftLeg_Movement2 <= -25 and RotateEnemyRightLeg_Movement2 <= -25 and swingforwardEnemy2 == false and swingbackwardEnemy2 == true) {
					swingforwardEnemy2 = true;
					swingbackwardEnemy2 = false;
				}
			}
			else {
				if (allowEnemyMovement2 == true) {
					if (RotateEnemyRightArm_Movement2 >= 0) {
						RotateEnemyRightArm_Movement2 -= (float)(80 * dt);
					}
					if (RotateEnemyRightArm_Movement2 <= 0) {
						RotateEnemyRightArm_Movement2 += (float)(80 * dt);
					}
					if (RotateEnemyLeftArm_Movement2 <= 0) {
						RotateEnemyLeftArm_Movement2 += (float)(80 * dt);
					}
					if (RotateEnemyLeftArm_Movement2 >= 0) {
						RotateEnemyLeftArm_Movement2 -= (float)(80 * dt);
					}
					RotateEnemyLeftLeg_Movement2 = 0;
					RotateEnemyRightLeg_Movement2 = 0;
				}
			}
		}
		else {
			if (allowEnemyMovement2 == true) {
				if (RotateEnemyRightArm_Movement2 >= 0) {
					RotateEnemyRightArm_Movement2 -= (float)(80 * dt);
				}
				if (RotateEnemyRightArm_Movement2 <= 0) {
					RotateEnemyRightArm_Movement2 += (float)(80 * dt);
				}
				if (RotateEnemyLeftArm_Movement2 <= 0) {
					RotateEnemyLeftArm_Movement2 += (float)(80 * dt);
				}
				if (RotateEnemyLeftArm_Movement2 >= 0) {
					RotateEnemyLeftArm_Movement2 -= (float)(80 * dt);
				}
				RotateEnemyLeftLeg_Movement2 = 0;
				RotateEnemyRightLeg_Movement2 = 0;
				EnemyXSpeed2 = 0;
				EnemyZSpeed2 = 0;
			}
		}







		// Demon3 Movement
		static bool ismaximumdistance3 = false;
		static float distancetravelled3;
		static float time_detect3 = 0;
		distancetravelled3 = DistanceParameter(EnemyPosX3, EnemyPosX3, -100, -140);
		EnemyXSpeed3 = (float)(15 * dt);
		EnemyZSpeed3 = (float)(15 * dt);
		if (EnemyDetectPlayer(translateX, translateZ, EnemyPosX3, EnemyPosZ3) == true and ismaximumdistance3 == false and triggerEnemy3 == false) {
			triggerEnemy3 = true;
		}
		if (distancetravelled3 > 100 and ismaximumdistance3 == false) {
			ismaximumdistance3 = true;
		}
		if (ismaximumdistance3 == true and EnemyHealth3 > 0) {
			Vector3 EnemyPosition = Vector3(EnemyPosX3, 0, EnemyPosZ3);
			Vector3 Player = Vector3(translateX, 0, translateZ);
			Vector3 EnemytoOGPos = (Vector3(-100, 0, -140) - EnemyPosition).Normalized();
			Vector3 target = Vector3(0, 0, 1);
			angle3 = (180.f / Math::PI) * (std::acos(EnemytoOGPos.Dot(target) / (EnemytoOGPos.Length()) * (target.Length())));

			if (EnemytoOGPos.x > 0) {
				EnemyViewAngle3 = angle3;
			}
			else {
				EnemyViewAngle3 = -angle3;
			}

			if (EnemyPosX3 >= 100) {
				EnemyPosX3 -= EnemyXSpeed3;
				rotateEnemyBody3 = -90;
			}
			if (EnemyPosX3 <= 100) {
				EnemyPosX3 += EnemyXSpeed3;
				rotateEnemyBody3 = -90;
			}
			if (EnemyPosZ3 >= 140) {
				EnemyPosZ3 -= EnemyZSpeed3;
				rotateEnemyBody3 = 180;
			}
			if (EnemyPosZ3 <= 140) {
				EnemyPosZ3 += EnemyZSpeed3;
				rotateEnemyBody3 = 0;
			}
		}
		if (distancetravelled3 <= 0.5 and ismaximumdistance3 == true and time_detect3 <= 2 and EnemyHealth3 > 0) {
			if (RotateEnemyRightArm_Movement3 >= 0) {
				RotateEnemyRightArm_Movement3 -= (float)(80 * dt);
			}
			if (RotateEnemyRightArm_Movement3 <= 0) {
				RotateEnemyRightArm_Movement3 += (float)(80 * dt);
			}
			if (RotateEnemyLeftArm_Movement3 <= 0) {
				RotateEnemyLeftArm_Movement3 += (float)(80 * dt);
			}
			if (RotateEnemyLeftArm_Movement3 >= 0) {
				RotateEnemyLeftArm_Movement3 -= (float)(80 * dt);
			}
			RotateEnemyLeftLeg_Movement3 = 0;
			RotateEnemyRightLeg_Movement3 = 0;
			time_detect3 += dt;
			rotateEnemyBody3 = 0;
			if (time_detect3 <= 0.5) {
				EnemyHealth3 = 100;
			}
		}
		if (time_detect3 >= 2) {
			time_detect3 = 0;
			ismaximumdistance3 = false;
		}
		if (triggerEnemy3 == true and allowEnemyMovement3 == true) {
			if (ismaximumdistance3 == false) {
				Vector3 EnemyPosition = Vector3(EnemyPosX3, 0, EnemyPosZ3);
				Vector3 Player = Vector3(translateX, 0, translateZ);
				Vector3 distanceVector = (Player - EnemyPosition);
				Vector3 target = Vector3(0, 0, 1);
				angle3 = (180.f / Math::PI) * (std::acos(distanceVector.Dot(target) / (distanceVector.Length()) * (target.Length())));

				if (distanceVector.x > 0) {
					EnemyViewAngle3 = angle3;
				}
				else {
					EnemyViewAngle3 = -angle3;
				}

				if (distanceVector.Length() >= 30) {
					if (EnemyPosX3 > translateX) {
						EnemyPosX3 -= EnemyXSpeed3;
					}
					else if (EnemyPosX3 < translateX) {
						EnemyPosX3 += EnemyXSpeed3;
					}
					else {
						EnemyXSpeed3 = 0;
					}
					if (EnemyPosZ3 > translateZ) {
						EnemyPosZ3 -= EnemyZSpeed3;
					}
					else if (EnemyPosZ3 < translateZ) {
						EnemyPosZ3 += EnemyZSpeed3;
					}
					else {
						EnemyZSpeed3 = 0;
					}
				}
				else {
					EnemyXSpeed3 = 0;
					EnemyZSpeed3 = 0;
				}

				if (EnemyXSpeed3 == 0 and EnemyZSpeed3 == 0) {
					AllowEnemyAttack3 = true;
				}
				else {
					AllowEnemyAttack3 = false;
				}
			}
			if (EnemyXSpeed3 != 0 or EnemyZSpeed3 != 0) {
				if (swingforwardEnemy3 == true and swingbackwardEnemy3 == false) {
					RotateEnemyLeftArm_Movement3 = RotateEnemyRightArm_Movement3 = RotateEnemyLeftLeg_Movement3 = RotateEnemyRightLeg_Movement3 += (float)(60 * dt);
				}

				if (swingbackwardEnemy3 == true and swingforwardEnemy3 == false) {
					RotateEnemyLeftArm_Movement3 = RotateEnemyRightArm_Movement3 = RotateEnemyLeftLeg_Movement3 = RotateEnemyRightLeg_Movement3 -= (float)(60 * dt);
				}

				if (RotateEnemyLeftArm_Movement3 >= 25 and RotateEnemyRightArm_Movement3 >= 25 and RotateEnemyLeftLeg_Movement3 >= 25 and RotateEnemyRightLeg_Movement3 >= 25 and swingforwardEnemy3 == true and swingbackwardEnemy3 == false) {
					swingforwardEnemy3 = false;
					swingbackwardEnemy3 = true;
				}
				if (RotateEnemyLeftArm_Movement3 <= -25 and RotateEnemyRightArm_Movement3 <= -25 and RotateEnemyLeftLeg_Movement3 <= -25 and RotateEnemyRightLeg_Movement3 <= -25 and swingforwardEnemy3 == false and swingbackwardEnemy3 == true) {
					swingforwardEnemy3 = true;
					swingbackwardEnemy3 = false;
				}
			}
			else {
				if (allowEnemyMovement3 == true) {
					if (RotateEnemyRightArm_Movement3 >= 0) {
						RotateEnemyRightArm_Movement3 -= (float)(80 * dt);
					}
					if (RotateEnemyRightArm_Movement3 <= 0) {
						RotateEnemyRightArm_Movement3 += (float)(80 * dt);
					}
					if (RotateEnemyLeftArm_Movement3 <= 0) {
						RotateEnemyLeftArm_Movement3 += (float)(80 * dt);
					}
					if (RotateEnemyLeftArm_Movement3 >= 0) {
						RotateEnemyLeftArm_Movement3 -= (float)(80 * dt);
					}
					RotateEnemyLeftLeg_Movement3 = 0;
					RotateEnemyRightLeg_Movement3 = 0;
				}
			}
		}
		else {
			if (allowEnemyMovement3 == true) {
				if (RotateEnemyRightArm_Movement3 >= 0) {
					RotateEnemyRightArm_Movement3 -= (float)(80 * dt);
				}								
				if (RotateEnemyRightArm_Movement3 <= 0) {
					RotateEnemyRightArm_Movement3 += (float)(80 * dt);
				}
				if (RotateEnemyLeftArm_Movement3 <= 0) {
					RotateEnemyLeftArm_Movement3 += (float)(80 * dt);
				}							   
				if (RotateEnemyLeftArm_Movement3 >= 0) {
					RotateEnemyLeftArm_Movement3 -= (float)(80 * dt);
				}
				RotateEnemyLeftLeg_Movement3 = 0;
				RotateEnemyRightLeg_Movement3 = 0;
				EnemyXSpeed3 = 0;
				EnemyZSpeed3 = 0;
			}
		}






		if (Health > 0) {
			//Health -= (float)(10 * dt); //Health decrease tester
		}
		else {
			isAlive = false;
			PlaySound(TEXT("Alucard.projnext.death01.wav"), NULL, SND_FILENAME | SND_ASYNC);
		}
	}
	else {
		if (rotate_Eyelid >= -100) {
			rotate_Eyelid -= (float)(450 * dt);
		}
		if (rotate_Eyelid <= -100) {
			rotate_Eyelid += (float)(450 * dt);
		}
		if (jumpAttack_rotation >= -80) {
			jumpAttack_rotation -= (float)(80 * dt);
		}
		if (jumpAttack_rotation <= -80) {
			jumpAttack_rotation += (float)(80 * dt);
		}

		if (RotateRightArm_Movement >= -50) {
			RotateRightArm_Movement -= (float)(80 * dt);
		}
		if (RotateRightArm_Movement <= -50) {
			RotateRightArm_Movement += (float)(80 * dt);
		}
		if (RotateLeftArm_Movement <= 50) {
			RotateLeftArm_Movement += (float)(80 * dt);
		}
		if (RotateLeftArm_Movement >= 50) {
			RotateLeftArm_Movement -= (float)(80 * dt);
		}
		if (RotateEnemyRightArm_Movement >= 0) {
			RotateEnemyRightArm_Movement -= (float)(80 * dt);
		}
		if (RotateEnemyRightArm_Movement <= 0) {
			RotateEnemyRightArm_Movement += (float)(80 * dt);
		}
		if (RotateEnemyLeftArm_Movement <= 0) {
			RotateEnemyLeftArm_Movement += (float)(80 * dt);
		}
		if (RotateEnemyLeftArm_Movement >= 0) {
			RotateEnemyLeftArm_Movement -= (float)(80 * dt);
		}

		if (RotateEnemyRightArm_Movement2 >= 0) {
			RotateEnemyRightArm_Movement2 -= (float)(80 * dt);
		}
		if (RotateEnemyRightArm_Movement2 <= 0) {
			RotateEnemyRightArm_Movement2 += (float)(80 * dt);
		}
		if (RotateEnemyLeftArm_Movement2 <= 0) {
			RotateEnemyLeftArm_Movement2 += (float)(80 * dt);
		}
		if (RotateEnemyLeftArm_Movement2 >= 0) {
			RotateEnemyLeftArm_Movement2 -= (float)(80 * dt);
		}

		if (RotateEnemyRightArm_Movement3 >= 0) {
			RotateEnemyRightArm_Movement3 -= (float)(80 * dt);
		}
		if (RotateEnemyRightArm_Movement3 <= 0) {
			RotateEnemyRightArm_Movement3 += (float)(80 * dt);
		}
		if (RotateEnemyLeftArm_Movement3 <= 0) {
			RotateEnemyLeftArm_Movement3 += (float)(80 * dt);
		}
		if (RotateEnemyLeftArm_Movement3 >= 0) {
			RotateEnemyLeftArm_Movement3 -= (float)(80 * dt);
		}
		if (EarnedCoins == true) {
			Application::Cash -= 100;
			if (Application::Cash < 0) {
				Application::Cash = 0;
			}
			EarnedCoins = false;
		}
		RotateRightArm_SwingSword = 0;
		rotateAngle = 0;
		RotateLeftLeg_Movement = 0;
		RotateRightLeg_Movement = 0;
		translatebodyY_Jump = 0;
		ttemp = 0;
	}

	// Fire
	fireEffects1 += (float)(6 * dt);
	fireEffects2 += (float)(4 * dt);
	fireEffects3 += (float)(7 * dt);
	fireEffects4 += (float)(3 * dt);
	fireEffects5 += (float)(3.6 * dt);
	fireEffects6 += (float)(4.2 * dt);

	if (fireEffects1 >= 5) {
		if (ScaleFireEffect1 >= 0.1) {
			ScaleFireEffect1 -= (float)(2 * dt);
		}
	}
	if (ScaleFireEffect1 <= 0.1 and fireEffects1 >= 6) {
		fireEffects1 = 0;
		ScaleFireEffect1 = 1;
	}

	if (fireEffects2 >= 4) {
		if (ScaleFireEffect2 >= 0.1) {
			ScaleFireEffect2 -= (float)(2 * dt);
		}
	}
	if (ScaleFireEffect2 <= 0.1 and fireEffects2 >= 5) {
		fireEffects2 = 0;
		ScaleFireEffect2 = 1;
	}

	if (fireEffects3 >= 6) {
		if (ScaleFireEffect3 >= 0.1) {
			ScaleFireEffect3 -= (float)(2 * dt);
		}
	}
	if (ScaleFireEffect3 <= 0.1 and fireEffects3 >= 7) {
		fireEffects3 = 0;
		ScaleFireEffect3 = 1;
	}

	if (fireEffects4 >= 4) {
		if (ScaleFireEffect4 >= 0.1) {
			ScaleFireEffect4 -= (float)(2 * dt);
		}
	}
	if (ScaleFireEffect4 <= 0.1 and fireEffects4 >= 5) {
		fireEffects4 = 0;
		ScaleFireEffect4 = 1;
	}

	if (fireEffects5 >= 7) {
		if (ScaleFireEffect5 >= 0.1) {
			ScaleFireEffect5 -= (float)(2 * dt);
		}
	}
	if (ScaleFireEffect5 <= 0.1 and fireEffects5 >= 8) {
		fireEffects5 = 0;
		ScaleFireEffect5 = 1;
	}

	if (fireEffects6 >= 5) {
		if (ScaleFireEffect6 >= 0.1) {
			ScaleFireEffect6 -= (float)(2 * dt);
		}
	}
	if (ScaleFireEffect6 <= 0.1 and fireEffects6 >= 6) {
		fireEffects6 = 0;
		ScaleFireEffect6 = 1;
	}

	rotateSkyBox += (float)(1 * dt);

	static int Dialogue = 1;

	if (DialogueBoxOpen == true) {
		if (interactableObject(translateX, translateZ, 0, -100, 10, 10) == true) {
			GameStartsDialogue = "Looking for a missing young male. Has blonde hair, wears foreign clothes. Any information/assistance isgreatly welcomed.";
			if (GameStartsDialogue[x] != '\0' and ScrollingText % 3 == 0) {
				if (not Application::IsKeyPressed('F') and isRead == false) {
					if (x < 56) {
						it += GameStartsDialogue[x];
					}
					if (x >= 56 and x <= 102) {
						it2 += GameStartsDialogue[x];
					}
					if (x > 102) {
						it3 += GameStartsDialogue[x];
					}
					if (x >= GameStartsDialogue.length() - 1) {
						isRead = true;
						buttonsincelastpress = 0;
						x = 0;
					}
					x++;
				}
				if (Application::IsKeyPressed('F') and isRead == false and buttonsincelastpress > 0.2) {
					isRead = true;
					it = "Looking for a missing young male. Has blonde hair, wears";
					it2 = " foreign clothes. Any information/assistance is";
					it3 = "greatly welcomed.";
					buttonsincelastpress = 0;
				}
				if (isRead == true and Application::IsKeyPressed('F') and buttonsincelastpress > 0.2) {
					isRead = false;
					it = it2 = it3 = "";
					DialogueBoxOpen = false;
					x = 0;
					ScrollingText = 0;
					buttonsincelastpress = 0;
					move = true;
				}
				buttonsincelastpress += dt;
			}
		}
		ScrollingText++;
	}


	if (EnemyHealth <= 0 and allowEnemyMovement == false) {
		if (EarnedCoins == true and Diamond != 10) {
			Application::Cash += rand() % 5 + 10;
			EarnedCoins = false;
		}
		if (RotateEnemyRightArm_Movement >= -50) {
			RotateEnemyRightArm_Movement -= (float)(80 * dt);
		}
		if (RotateEnemyRightArm_Movement <= -50) {
			RotateEnemyRightArm_Movement += (float)(80 * dt);
		}
		if (RotateEnemyLeftArm_Movement <= 50) {
			RotateEnemyLeftArm_Movement += (float)(80 * dt);
		}
		if (RotateEnemyLeftArm_Movement >= 50) {
			RotateEnemyLeftArm_Movement -= (float)(80 * dt);
		}
		if (EnemyDieRotation >= -80) {
			EnemyDieRotation -= (float)(80 * dt);
		}
		triggerEnemy = false;
		RotateEnemyLeftLeg_Movement = 0;
		RotateEnemyRightLeg_Movement = 0;
	}

	if (EnemyHealth2 <= 0 and allowEnemyMovement2 == false) {
		if (EarnedCoins == true and Diamond != 10) {
			Application::Cash += rand() % 5 + 10;
			EarnedCoins = false;
		}
		if (RotateEnemyRightArm_Movement2 >= -50) {
			RotateEnemyRightArm_Movement2 -= (float)(80 * dt);
		}
		if (RotateEnemyRightArm_Movement2 <= -50) {
			RotateEnemyRightArm_Movement2 += (float)(80 * dt);
		}
		if (RotateEnemyLeftArm_Movement2 <= 50) {
			RotateEnemyLeftArm_Movement2 += (float)(80 * dt);
		}
		if (RotateEnemyLeftArm_Movement2 >= 50) {
			RotateEnemyLeftArm_Movement2 -= (float)(80 * dt);
		}
		if (EnemyDieRotation2 >= -80) {
			EnemyDieRotation2 -= (float)(80 * dt);
		}
		triggerEnemy2 = false;
		RotateEnemyLeftLeg_Movement2 = 0;
		RotateEnemyRightLeg_Movement2 = 0;
	}

	if (EnemyHealth3 <= 0 and allowEnemyMovement3 == false) {
		if (EarnedCoins == true and Diamond != 10) {
			Application::Cash += rand() % 5 + 10;
			EarnedCoins = false;
		}
		if (RotateEnemyRightArm_Movement3 >= -50) {
			RotateEnemyRightArm_Movement3 -= (float)(80 * dt);
		}
		if (RotateEnemyRightArm_Movement3 <= -50) {
			RotateEnemyRightArm_Movement3 += (float)(80 * dt);
		}
		if (RotateEnemyLeftArm_Movement3 <= 50) {
			RotateEnemyLeftArm_Movement3 += (float)(80 * dt);
		}
		if (RotateEnemyLeftArm_Movement3 >= 50) {
			RotateEnemyLeftArm_Movement3 -= (float)(80 * dt);
		}
		if (EnemyDieRotation3 >= -80) {
			EnemyDieRotation3 -= (float)(80 * dt);
		}
		triggerEnemy3 = false;
		RotateEnemyLeftLeg_Movement3 = 0;
		RotateEnemyRightLeg_Movement3 = 0;
	}


	static float time_EnemyRespawn = 0.f;
	static float time_EnemyRespawn2 = 0.f;
	static float time_EnemyRespawn3 = 0.f;

	if (EnemyHealth <= 0 and Diamond < 10) {
		time_EnemyRespawn += dt;
	}
	if (EnemyHealth <= 0 and time_EnemyRespawn >= 10) {
		allowEnemyMovement = true;
		time_EnemyRespawn = 0;
		EnemyHealth = 100;
		EnemyDieRotation = 0;
		EarnedCoins = true;
		EnemyPosX = 100;
		EnemyPosZ = -140;
		EnemyViewAngle = 0;
		EnemyShockwaveScale = 0.1;
	}

	if (EnemyHealth2 <= 0 and Diamond < 10) {
		time_EnemyRespawn2 += dt;
	}
	if (EnemyHealth2 <= 0 and time_EnemyRespawn2 >= 10) {
		allowEnemyMovement2 = true;
		time_EnemyRespawn2 = 0;
		EnemyHealth2 = 100;
		EnemyDieRotation2 = 0;
		EarnedCoins = true;
		EnemyPosX2 = 100;
		EnemyPosZ2 = 140;
		EnemyViewAngle2 = 0;
		EnemyShockwaveScale2 = 0.1;
	}

	if (EnemyHealth3 <= 0 and Diamond < 10) {
		time_EnemyRespawn3 += dt;
	}
	if (EnemyHealth3 <= 0 and time_EnemyRespawn3 >= 10) {
		allowEnemyMovement3 = true;
		time_EnemyRespawn3 = 0;
		EnemyHealth3 = 100;
		EnemyDieRotation3 = 0;
		EarnedCoins = true;
		EnemyPosX3 = -100;
		EnemyPosZ3 = -140;
		EnemyViewAngle3 = 0;
		EnemyShockwaveScale3 = 0.1;
	}


	if (Diamond >= 10) {
		EnemyHealth = 0;
		EnemyHealth2 = 0;
		EnemyHealth3 = 0;

		time_EnemyRespawn = 0;
		time_EnemyRespawn2 = 0;
		time_EnemyRespawn3 = 0;
		if (EarnedCoins == true) {
			Application::Cash += 250;
			EarnedCoins = false;
		}
	}


	if (translateX < -195) {
		translateX = -195;
	}
	if (translateX > 195) {
		translateX = 195;
	}

	if (translateZ < -195) {
		translateZ = -195;
	}
	if (translateZ > 195) {
		translateZ = 195;
	}

	// Enemy 1 Jump Attack
	if (EnemyHealth > 0 and isAlive == true) {
		if (AllowEnemyAttack == true) {
			if (test >= 0.7) {
				if (jump == true and Ground == false) {
					translateY_EnemyJump += (float)(55 * dt);
					if (EnemyRightArm_JumpAttackRotation >= -40) {
						EnemyRightArm_JumpAttackRotation -= (float)(200 * dt);
					}
				}
				if (jump == false and Ground == true) {
					translateY_EnemyJump -= (float)(55 * dt);
					if (EnemyRightArm_JumpAttackRotation <= 0) {
						EnemyRightArm_JumpAttackRotation += (float)(200 * dt);
					}
				}
				if (translateY_EnemyJump >= 15 and jump == true and Ground == false) {
					jump = false;
					Ground = true;
				}
				if (translateY_EnemyJump <= 0 and jump == false and Ground == true) {
					jump = true;
					Ground = false;
					ShockWaveEnemyAttack = true;
					test = 0;
				}
			}
			test += dt;
		}
		else {
			if (translateY_EnemyJump >= 0) {
				translateY_EnemyJump -= (float)(55 * dt);
			}
			if (EnemyRightArm_JumpAttackRotation <= 0) {
				EnemyRightArm_JumpAttackRotation += (float)(200 * dt);
			}
			if (EnemyRightArm_JumpAttackRotation >= 0) {
				EnemyRightArm_JumpAttackRotation -= (float)(200 * dt);
			}
			if (translateY_EnemyJump <= 0 and jump == false and Ground == true) {
				jump = true;
				Ground = false;
				test = 0;
				ShockWaveEnemyAttack = true;
			}
		}
	}
	else {
		translateY_EnemyJump = 0;
		AllowEnemyAttack = false;
		if (EnemyRightArm_JumpAttackRotation <= 0) {
			EnemyRightArm_JumpAttackRotation += (float)(200 * dt);
		}
	}

	if (ShockWaveEnemyAttack == true) {
		EnemyShockwaveScale += (15 * dt);
		static bool temp = false;
		if (isInHitbox(translateX, translateZ, EnemyPosX, EnemyPosZ, EnemyShockwaveScale * 6) == true and temp == false and translatebodyY_Jump <= 1) {
			Health -= 1;
			temp = true;
		}
		else {
			temp = false;
		}
		if (EnemyShockwaveScale >= 15) {
			EnemyShockwaveScale = 0.1;
			ShockWaveEnemyAttack = false;
		}
	}

	// Enemy 2 Attack
	if (EnemyHealth2 > 0 and isAlive == true) {
		if (AllowEnemyAttack2 == true) {
			if (test2 >= 0.7) {
				if (jump2 == true and Ground2 == false) {
					translateY_EnemyJump2 += (float)(55 * dt);
					if (EnemyRightArm_JumpAttackRotation2 >= -40) {
						EnemyRightArm_JumpAttackRotation2 -= (float)(200 * dt);
					}
				}
				if (jump2 == false and Ground2 == true) {
					translateY_EnemyJump2 -= (float)(55 * dt);
					if (EnemyRightArm_JumpAttackRotation2 <= 0) {
						EnemyRightArm_JumpAttackRotation2 += (float)(200 * dt);
					}
				}
				if (translateY_EnemyJump2 >= 15 and jump2 == true and Ground2 == false) {
					jump2 = false;
					Ground2 = true;
				}
				if (translateY_EnemyJump2 <= 0 and jump2 == false and Ground2 == true) {
					jump2 = true;
					Ground2 = false;
					ShockWaveEnemyAttack2 = true;
					test2 = 0;
				}
			}
			test2 += dt;
		}
		else {
			if (translateY_EnemyJump2 >= 0) {
				translateY_EnemyJump2 -= (float)(55 * dt);
			}
			if (EnemyRightArm_JumpAttackRotation2 <= 0) {
				EnemyRightArm_JumpAttackRotation2 += (float)(200 * dt);
			}									
			if (EnemyRightArm_JumpAttackRotation2 >= 0) {
				EnemyRightArm_JumpAttackRotation2 -= (float)(200 * dt);
			}
			if (translateY_EnemyJump2 <= 0 and jump2 == false and Ground2 == true) {
				jump2 = true;
				Ground2 = false;
				test2 = 0;
				ShockWaveEnemyAttack2 = true;
			}
		}
	}
	else {
		translateY_EnemyJump2 = 0;
		AllowEnemyAttack2 = false;
	}

	if (ShockWaveEnemyAttack2 == true) {
		EnemyShockwaveScale2 += (15 * dt);
		static bool temp2 = false;
		if (isInHitbox(translateX, translateZ, EnemyPosX2, EnemyPosZ2, EnemyShockwaveScale2 * 6) == true and temp2 == false and translatebodyY_Jump <= 1) {
			Health -= 1;
			temp2 = true;
		}
		else {
			temp2 = false;
		}
		if (EnemyShockwaveScale2 >= 15) {
			EnemyShockwaveScale2 = 0.1;
			ShockWaveEnemyAttack2 = false;
		}
	}



	// Enemy 3 Jump Attack
	if (EnemyHealth3 > 0 and isAlive == true) {
		if (AllowEnemyAttack3 == true) {
			if (test3 >= 0.7) {
				if (jump3 == true and Ground3 == false) {
					translateY_EnemyJump3 += (float)(55 * dt);
					if (EnemyRightArm_JumpAttackRotation3 >= -40) {
						EnemyRightArm_JumpAttackRotation3 -= (float)(200 * dt);
					}
				}
				if (jump3 == false and Ground3 == true) {
					translateY_EnemyJump3 -= (float)(55 * dt);
					if (EnemyRightArm_JumpAttackRotation3 <= 0) {
						EnemyRightArm_JumpAttackRotation3 += (float)(200 * dt);
					}
				}
				if (translateY_EnemyJump3 >= 15 and jump3 == true and Ground3 == false) {
					jump3 = false;
					Ground3 = true;
				}
				if (translateY_EnemyJump3 <= 0 and jump3 == false and Ground3 == true) {
					jump3 = true;
					Ground3 = false;
					ShockWaveEnemyAttack3 = true;
					test3 = 0;
				}
			}
			test3 += dt;
		}
		else {
			if (translateY_EnemyJump3 >= 0) {
				translateY_EnemyJump3 -= (float)(55 * dt);
			}
			if (translateY_EnemyJump3 <= 0) {
				translateY_EnemyJump3 += (float)(55 * dt);
			}
			if (EnemyRightArm_JumpAttackRotation3 <= 0) {
				EnemyRightArm_JumpAttackRotation3 += (float)(200 * dt);
			}
			if (EnemyRightArm_JumpAttackRotation3 >= 0) {
				EnemyRightArm_JumpAttackRotation3 -= (float)(200 * dt);
			}
			if (translateY_EnemyJump3 <= 0 and jump3 == false and Ground3 == true) {
				jump3 = true;
				Ground3 = false;
				test3 = 0;
				ShockWaveEnemyAttack3 = true;
			}
		}
	}
	else {
		translateY_EnemyJump3 = 0;
		AllowEnemyAttack3 = false;
		if (EnemyRightArm_JumpAttackRotation3 <= 0) {
			EnemyRightArm_JumpAttackRotation3 += (float)(200 * dt);
		}
	}

	if (ShockWaveEnemyAttack3 == true) {
		EnemyShockwaveScale3 += (15 * dt);
		static bool temp3 = false;
		if (isInHitbox(translateX, translateZ, EnemyPosX3, EnemyPosZ3, EnemyShockwaveScale3 * 6) == true and temp3 == false and translatebodyY_Jump <= 1) {
			Health -= 1;
			temp3 = true;
		}
		else {
			temp3 = false;
		}
		if (EnemyShockwaveScale3 >= 15) {
			EnemyShockwaveScale3 = 0.1;
			ShockWaveEnemyAttack3 = false;
		}
	}






	if (isAlive == false and is1stPersonCam == false and timer_gameover < 1.5) {
		timer_gameover += dt;
	}
	if (isAlive == false and timer_gameover > 1.5 and timer_Retry < 1) {
		timer_Retry += dt;
	}

	if (isAlive == true and is1stPersonCam == false and timer_gameover < 1.5 and Diamond >= 10) {
		timer_gameover += dt;
	}
	if (isAlive == true and timer_gameover > 1.5 and timer_Retry < 1 and Diamond >= 10) {
		timer_Retry += dt;
	}


	light[1].position.Set(0, 600, 0);
	camera2.ThirdPersonCam(dt, Vector3(translateX, 5.5, translateZ));

	if (timer_Skill1 > 0) {
		timer_Skill1 -= dt;
	}
	if (timer_Skill2 > 0) {
		timer_Skill2 -= dt;
	}
	if (timer_Skill3 > 0) {
		timer_Skill3 -= dt;
	}




	if ((int)min == 0 and (int)sec == 0) {
		PlaySound(TEXT("Alucard.projnext.death01.wav"), NULL, SND_FILENAME | SND_ASYNC);
		Health = 0;
		isAlive = false;
	}
	if (min >= 0) {
		sec -= dt;
	}
	if (sec < 0) {
		min--;
		if (min >= 0) {
			sec = 60;
		}
	}


}


void Sp2_Minigame::RenderText(Mesh* mesh, std::string text, Color color)
{
	if (!mesh || mesh->textureID <= 0) //Proper error check
		return;
	glDisable(GL_DEPTH_TEST); //uncomment for RenderTextOnScreen
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
	glEnable(GL_DEPTH_TEST); //uncomment for RenderTextOnScreen
}

void Sp2_Minigame::RenderImageOnScreen(Mesh* mesh, Color color, float Rotate, float sizex, float sizey, float sizez, float x, float y)
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
	modelStack.Rotate(Rotate, 0, 0, 1);
	modelStack.Scale(sizex, sizey, sizez);

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


void Sp2_Minigame::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
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


void Sp2_Minigame::Render()
{
	//check if mouse is in window
	if (GetWindowRect(window, &rect))
	{
		POINT initmousepos;
		mousepos = &initmousepos;//init mousepos
		GetCursorPos(mousepos); //get mouse position
	}


	// Render VBO here
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	srand((unsigned)time(NULL));


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


	viewStack.LoadIdentity();
	viewStack.LookAt(camera2.position.x, camera2.position.y, camera2.position.z, camera2.target.x, camera2.target.y, camera2.target.z, camera2.up.x, camera2.up.y, camera2.up.z);
	modelStack.LoadIdentity();

	//Position lightPosition_cameraspace = viewStack.Top() * light[0].position;
	//glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightPosition_cameraspace.x);

	RenderSkybox();
	//RenderMesh(meshList[GEO_AXES], false);

	//diamonds
	for (int i = 0; i < 10; i++) {
		if (Diamonds[i] != nullptr) {
			modelStack.PushMatrix();
			{
				modelStack.Translate(Diamonds[i]->GetX(), -3, Diamonds[i]->GetZ());
				modelStack.Rotate(0, 0, 1, 0);
				modelStack.Scale(20, 20, 20);
				RenderMesh(meshList[GEO_DIAMOND], true);
			}
			modelStack.PopMatrix();
		}
	}




	//Tent
	if (isOpenTent == true) {
		modelStack.PushMatrix();
		{
			modelStack.Translate(-100, -4, 100);
			modelStack.Rotate(0, 0, 1, 0);
			modelStack.Scale(50, 50, 50);
			RenderMesh(meshList[GEO_TENTOPEN], true);
		}
		modelStack.PopMatrix();
	}
	else {
		modelStack.PushMatrix();
		{
			modelStack.Translate(-100, -4, 100);
			modelStack.Rotate(0, 0, 1, 0);
			modelStack.Scale(50, 50, 50);
			RenderMesh(meshList[GEO_TENTCLOSE], true);
		}
		modelStack.PopMatrix();
	}



	modelStack.PushMatrix();
	{
		modelStack.Translate(-50, -4, -50);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(70, 50, 70);
		RenderMesh(meshList[GEO_REGULARTREE], true);
	}
	modelStack.PopMatrix();
	modelStack.PushMatrix();
	{
		modelStack.Translate(50, -4, -100);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(70, 50, 70);
		RenderMesh(meshList[GEO_REGULARTREE], true);
	}
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	{
		modelStack.Translate(110, -4, 100);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(70, 50, 70);
		RenderMesh(meshList[GEO_REGULARTREE], true);
	}
	modelStack.PopMatrix();
	modelStack.PushMatrix();
	{
		modelStack.Translate(50, -4, 50);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(70, 50, 70);
		RenderMesh(meshList[GEO_REGULARTREE], true);
	}
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	{
		modelStack.Translate(150, -4, 0);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(70, 50, 70);
		RenderMesh(meshList[GEO_REGULARTREE], true);
	}
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	{
		modelStack.Translate(-60, -4, -130);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(70, 50, 70);
		RenderMesh(meshList[GEO_REGULARTREE], true);
	}
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	{
		modelStack.Translate(80, -4, -50);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(70, 50, 70);
		RenderMesh(meshList[GEO_REGULARTREE], true);
	}
	modelStack.PopMatrix();

	for (int i = 0; i < 500; i += 15) {
		modelStack.PushMatrix();
		{
			modelStack.Translate(-235 + i, -4, -200);
			modelStack.Rotate(0, 0, 1, 0);
			modelStack.Scale(70, 50, 70);
			RenderMesh(meshList[GEO_LARGETREES], true);
		}
		modelStack.PopMatrix();
	}
	for (int i = 0; i < 500; i += 25) {
		modelStack.PushMatrix();
		{
			modelStack.Translate(-235 + i, -4, -210);
			modelStack.Rotate(0, 0, 1, 0);
			modelStack.Scale(70, 50, 70);
			RenderMesh(meshList[GEO_LARGETREES], true);
		}
		modelStack.PopMatrix();
	}
	for (int i = 0; i < 500; i += 15) {
		modelStack.PushMatrix();
		{
			modelStack.Translate(-235 + i, -4, -220);
			modelStack.Rotate(0, 0, 1, 0);
			modelStack.Scale(70, 50, 70);
			RenderMesh(meshList[GEO_LARGETREES], true);
		}
		modelStack.PopMatrix();
	}
	for (int i = 0; i < 500; i += 10) {
		modelStack.PushMatrix();
		{
			modelStack.Translate(-235 + i, -4, -230);
			modelStack.Rotate(0, 0, 1, 0);
			modelStack.Scale(70, 50, 70);
			RenderMesh(meshList[GEO_LARGETREES], true);
		}
		modelStack.PopMatrix();
	}


	for (int i = 0; i < 410; i += 15) {
		modelStack.PushMatrix();
		{
			modelStack.Translate(205, -4, -200 + i);
			modelStack.Rotate(0, 0, 1, 0);
			modelStack.Scale(70, 50, 70);
			RenderMesh(meshList[GEO_LARGETREES], true);
		}
		modelStack.PopMatrix();
	}
	for (int i = 0; i < 410; i += 25) {
		modelStack.PushMatrix();
		{
			modelStack.Translate(215, -4, -200 + i);
			modelStack.Rotate(0, 0, 1, 0);
			modelStack.Scale(70, 50, 70);
			RenderMesh(meshList[GEO_LARGETREES], true);
		}
		modelStack.PopMatrix();
	}

	for (int i = 0; i < 410; i += 15) {
		modelStack.PushMatrix();
		{
			modelStack.Translate(225, -4, -200 + i);
			modelStack.Rotate(0, 0, 1, 0);
			modelStack.Scale(70, 50, 70);
			RenderMesh(meshList[GEO_LARGETREES], true);
		}
		modelStack.PopMatrix();
	}
	for (int i = 0; i < 410; i += 10) {
		modelStack.PushMatrix();
		{
			modelStack.Translate(235, -4, -200 + i);
			modelStack.Rotate(0, 0, 1, 0);
			modelStack.Scale(70, 50, 70);
			RenderMesh(meshList[GEO_LARGETREES], true);
		}
		modelStack.PopMatrix();
	}



	for (int i = 0; i < 410; i += 15) {
		modelStack.PushMatrix();
		{
			modelStack.Translate(-205, -4, -200 + i);
			modelStack.Rotate(0, 0, 1, 0);
			modelStack.Scale(70, 50, 70);
			RenderMesh(meshList[GEO_LARGETREES], true);
		}
		modelStack.PopMatrix();
	}
	for (int i = 0; i < 410; i += 25) {
		modelStack.PushMatrix();
		{
			modelStack.Translate(-215, -4, -200 + i);
			modelStack.Rotate(0, 0, 1, 0);
			modelStack.Scale(70, 50, 70);
			RenderMesh(meshList[GEO_LARGETREES], true);
		}
		modelStack.PopMatrix();
	}

	for (int i = 0; i < 410; i += 15) {
		modelStack.PushMatrix();
		{
			modelStack.Translate(-225, -4, -200 + i);
			modelStack.Rotate(0, 0, 1, 0);
			modelStack.Scale(70, 50, 70);
			RenderMesh(meshList[GEO_LARGETREES], true);
		}
		modelStack.PopMatrix();
	}
	for (int i = 0; i < 410; i += 10) {
		modelStack.PushMatrix();
		{
			modelStack.Translate(-235, -4, -200 + i);
			modelStack.Rotate(0, 0, 1, 0);
			modelStack.Scale(70, 50, 70);
			RenderMesh(meshList[GEO_LARGETREES], true);
		}
		modelStack.PopMatrix();
	}



	for (int i = 0; i < 500; i += 15) {
		modelStack.PushMatrix();
		{
			modelStack.Translate(-235 + i, -4, 200);
			modelStack.Rotate(0, 0, 1, 0);
			modelStack.Scale(70, 50, 70);
			RenderMesh(meshList[GEO_LARGETREES], true);
		}
		modelStack.PopMatrix();
	}
	for (int i = 0; i < 500; i += 25) {
		modelStack.PushMatrix();
		{
			modelStack.Translate(-235 + i, -4, 210);
			modelStack.Rotate(0, 0, 1, 0);
			modelStack.Scale(70, 50, 70);
			RenderMesh(meshList[GEO_LARGETREES], true);
		}
		modelStack.PopMatrix();
	}
	for (int i = 0; i < 500; i += 15) {
		modelStack.PushMatrix();
		{
			modelStack.Translate(-235 + i, -4, 220);
			modelStack.Rotate(0, 0, 1, 0);
			modelStack.Scale(70, 50, 70);
			RenderMesh(meshList[GEO_LARGETREES], true);
		}
		modelStack.PopMatrix();
	}
	for (int i = 0; i < 500; i += 10) {
		modelStack.PushMatrix();
		{
			modelStack.Translate(-235 + i, -4, 230);
			modelStack.Rotate(0, 0, 1, 0);
			modelStack.Scale(70, 50, 70);
			RenderMesh(meshList[GEO_LARGETREES], true);
		}
		modelStack.PopMatrix();
	}


	modelStack.PushMatrix();
	{
		modelStack.Translate(-10, -4, -100);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(50, 80, 50);
		RenderMesh(meshList[GEO_PALMTREES], true);
	}
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	{
		modelStack.Translate(0, -4, 150);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(50, 60, 50);
		RenderMesh(meshList[GEO_PALMTREES], true);
	}
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	{
		modelStack.Translate(130, -4, -130);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(50, 60, 50);
		RenderMesh(meshList[GEO_PALMTREES], true);
	}
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	{
		modelStack.Translate(-130, -4, 30);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(30, 60, 30);
		RenderMesh(meshList[GEO_PALMTREES], true);
	}
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	{
		modelStack.Translate(-100, -3, 155);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(40, 40, 40);
		RenderMesh(meshList[GEO_CAMPFIRE], true);
	}
	modelStack.PopMatrix();

	static bool allowfire = false;
	if (interactableObject(translateX + ShockwavedirectionX, translateZ + ShockwavedirectionX, -100, 155, 40, 40) == true and allowUltimateAttack == true and (allowAttack == true or allowJumpAttack == true or allowSpinattack == true)) {
		allowfire = true;
	}
	else if (interactableObject(translateX + translateFusionWaveX, translateZ + translateFusionWaveZ, -100, 155, 40, 40) == true and allowFusionWaveTransverse == true) {
		allowfire = true;
	}

	if (allowfire == true) {
		//Fire 1
		modelStack.PushMatrix();
		{
			modelStack.Translate(-100, 2, 155);
			modelStack.Translate(rand() % 5 - 2, -1 + fireEffects2, rand() % 5 - 2);
			modelStack.Scale(ScaleFireEffect1, ScaleFireEffect1, ScaleFireEffect1);
			modelStack.Rotate(0, 0, 1, 0);
			modelStack.Scale(0.7, 2, 0.7);
			RenderMesh(meshList[GEO_FIRE], true);
		}
		modelStack.PopMatrix();
		//Fire 2
		modelStack.PushMatrix();
		{
			modelStack.Translate(-100, 2, 155);
			modelStack.Translate(rand() % 5 - 2, -1 + fireEffects2, rand() % 5 - 2);
			modelStack.Scale(ScaleFireEffect2, ScaleFireEffect2, ScaleFireEffect2);
			modelStack.Rotate(0, 0, 1, 0);
			modelStack.Scale(0.7, 2, 0.7);
			RenderMesh(meshList[GEO_FIRE], true);
		}
		modelStack.PopMatrix();
		//Fire 3
		modelStack.PushMatrix();
		{
			modelStack.Translate(-100, 2, 155);
			modelStack.Translate(rand() % 5 - 2, -1 + fireEffects3, rand() % 5 - 2);
			modelStack.Scale(ScaleFireEffect3, ScaleFireEffect3, ScaleFireEffect3);
			modelStack.Rotate(0, 0, 1, 0);
			modelStack.Scale(0.7, 2, 0.7);
			RenderMesh(meshList[GEO_FIRE], true);
		}
		modelStack.PopMatrix();
		//Fire 4
		modelStack.PushMatrix();
		{
			modelStack.Translate(-100, 2, 155);
			modelStack.Translate(rand() % 5 - 2, -1 + fireEffects4, rand() % 5 - 2);
			modelStack.Scale(ScaleFireEffect4, ScaleFireEffect4, ScaleFireEffect4);
			modelStack.Rotate(0, 0, 1, 0);
			modelStack.Scale(0.7, 2, 0.7);
			RenderMesh(meshList[GEO_FIRE], true);
		}
		modelStack.PopMatrix();
		//Fire 5
		modelStack.PushMatrix();
		{
			modelStack.Translate(-100, 2, 155);
			modelStack.Translate(rand() % 5 - 2, -1 + fireEffects5, rand() % 5 - 2);
			modelStack.Scale(ScaleFireEffect5, ScaleFireEffect5, ScaleFireEffect5);
			modelStack.Rotate(0, 0, 1, 0);
			modelStack.Scale(0.7, 2, 0.7);
			RenderMesh(meshList[GEO_FIRE], true);
		}
		modelStack.PopMatrix();
	}

	modelStack.PushMatrix();
	{
		modelStack.Translate(10, -4, -50);
		modelStack.Rotate(90, 0, 1, 0);
		modelStack.Scale(30, 50, 30);
		RenderMesh(meshList[GEO_ROCKS_SMALL], true);
	}
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	{
		modelStack.Translate(-130, -4, -50);
		modelStack.Rotate(0, 0, 1, 0);
		modelStack.Scale(100, 80, 100);
		RenderMesh(meshList[GEO_LARGETREES], true);
	}
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	{
		modelStack.Translate(-30, -3, 70);
		modelStack.Rotate(90, 0, 1, 0);
		modelStack.Scale(50, 30, 50);
		RenderMesh(meshList[GEO_LOG], true);
	}
	modelStack.PopMatrix();

	//ground
	modelStack.PushMatrix();
	{
		modelStack.Translate(0, -3, 0);
		modelStack.Rotate(-90, 1, 0, 0);
		modelStack.Scale(550, 550, 550);
		RenderMesh(meshList[GEO_GRASS], true);
	}
	modelStack.PopMatrix();



	modelStack.PushMatrix();
	{
		modelStack.Translate(-60, -3, 30);
		modelStack.Rotate(0, 1, 0, 0);
		modelStack.Scale(35, 35, 35);
		RenderMesh(meshList[GEO_LAMPPOST], true);
	}
	modelStack.PopMatrix();



	//Forest Text
	modelStack.PushMatrix();
	modelStack.Translate(992, 25, -200);
	modelStack.Rotate(0, 0, 1, 0);
	modelStack.Scale(8, 4.5, 0);
	RenderText(meshList[GEO_TEXT], "Forest", Color(0, 1, 0));
	modelStack.PopMatrix();

	//Demon Jigglypuff
	modelStack.PushMatrix();
	{
		if (EnemyXSpeed > 0 or EnemyZSpeed > 0) {
			modelStack.Translate(0, EnemyUpandDown_Translation, 0);
		}
		modelStack.Translate(EnemyPosX, 5.5 + translateY_EnemyJump, EnemyPosZ);
		modelStack.Rotate(EnemyViewAngle, 0, 1, 0);

		modelStack.Rotate(EnemyDieRotation, 1, 0, 0);

		//hair
		modelStack.PushMatrix();
		{
			modelStack.Translate(0.1, 6, 3.5);

			modelStack.PushMatrix();
			{
				modelStack.Translate(-0.3, 0.3, -1);
				modelStack.Rotate(0, 1, 0, 0);
				modelStack.Rotate(0, 0, 0, 1);
				modelStack.Scale(2.6, 2.6, 2.6);
				meshList[GEO_SPHERE]->material.kAmbient.Set(1 * 0.7, 0, 0);
				meshList[GEO_SPHERE]->material.kDiffuse.Set(1, 0, 0);
				RenderMesh(meshList[GEO_SPHERE], true);
			}
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			{
				modelStack.Translate(0.3, -0.6, 1.1);
				modelStack.Rotate(0, 1, 0, 0);
				modelStack.Rotate(0, 0, 0, 1);
				modelStack.Scale(1.5, 1.5, 1.5);
				meshList[GEO_SPHERE]->material.kAmbient.Set(1 * 0.7, 0, 0);
				meshList[GEO_SPHERE]->material.kDiffuse.Set(1, 0, 0);
				RenderMesh(meshList[GEO_SPHERE], true);
			}
			modelStack.PopMatrix();

			modelStack.Rotate(20, 1, 0, 0);
			modelStack.Rotate(-20, 0, 0, 1);
			modelStack.Scale(0.7, 0.7, 0.7);
			meshList[GEO_TORUS]->material.kAmbient.Set(1 * 0.7, 0, 0);
			meshList[GEO_TORUS]->material.kDiffuse.Set(1, 0, 0);
			RenderMesh(meshList[GEO_TORUS], true);
		}
		modelStack.PopMatrix();

		//Eyes left mirror
		modelStack.PushMatrix();
		{
			modelStack.Translate(1.8, 2, 3.55);
			{
				// Left eyelid
				modelStack.PushMatrix();
				{
					modelStack.Translate(0.6, 0.8, 1.2);
					// -300 to -100
					modelStack.Rotate(-300, 1, 0, 0);
					modelStack.Scale(3, 3, 3);
					meshList[GEO_EYELID]->material.kAmbient.Set(1 * 0.7, 0, 0);
					meshList[GEO_EYELID]->material.kDiffuse.Set(1, 0, 0);
					RenderMesh(meshList[GEO_EYELID], true);
				}
				modelStack.PopMatrix();

				// Dark Cyan Pupil
				modelStack.PushMatrix();
				{
					modelStack.Translate(0.65, 0.73, 1.5);

					// Pupil eyes reflection
					modelStack.PushMatrix();
					{
						modelStack.Translate(0.1, 1.1, 1.2);
						modelStack.Rotate(0, 0, 1, 0);
						modelStack.Scale(1, 1, 1);
						meshList[GEO_SPHERE]->material.kAmbient.Set(1, 0, 0);
						meshList[GEO_SPHERE]->material.kDiffuse.Set(1, 0, 0);
						RenderMesh(meshList[GEO_SPHERE], true);
					}
					modelStack.PopMatrix();

					modelStack.Rotate(0, 0, 1, 0);
					modelStack.Scale(2.55, 2.55, 2.55);
					meshList[GEO_SPHERE]->material.kAmbient.Set(1 * 0.25, 0, 0);
					meshList[GEO_SPHERE]->material.kDiffuse.Set(1, 0, 0);
					RenderMesh(meshList[GEO_SPHERE], true);
				}
				modelStack.PopMatrix();
			}
			modelStack.Rotate(0, 0, 1, 0);
			modelStack.Scale(4, 4, 4);
			meshList[GEO_SPHERE]->material.kAmbient.Set(0, 0, 0);
			meshList[GEO_SPHERE]->material.kDiffuse.Set(0, 0, 0);
			RenderMesh(meshList[GEO_SPHERE], true);
		}
		modelStack.PopMatrix();

		//Eyes right mirror
		modelStack.PushMatrix();
		{
			modelStack.Translate(-1.8, 2, 3.55);
			{
				// Eye lid
				modelStack.PushMatrix();
				{
					modelStack.Translate(-0.6, 0.8, 1.2);
					modelStack.Rotate(-300, 1, 0, 0);
					modelStack.Scale(3, 3, 3);
					meshList[GEO_EYELID]->material.kAmbient.Set(1 * 0.7, 0, 0);
					meshList[GEO_EYELID]->material.kDiffuse.Set(1, 0, 0);
					RenderMesh(meshList[GEO_EYELID], true);
				}
				modelStack.PopMatrix();

				// Dark Cyan Pupil
				modelStack.PushMatrix();
				{
					modelStack.Translate(-0.65, 0.73, 1.5);
					// Pupil eyes reflection
					modelStack.PushMatrix();
					{
						modelStack.Translate(-0.1, 1.1, 1.2);
						modelStack.Rotate(0, 0, 1, 0);
						modelStack.Scale(1, 1, 1);
						meshList[GEO_SPHERE]->material.kAmbient.Set(1, 0, 0);
						meshList[GEO_SPHERE]->material.kDiffuse.Set(1, 0, 0);
						RenderMesh(meshList[GEO_SPHERE], true);
					}
					modelStack.PopMatrix();
					modelStack.Rotate(0, 0, 1, 0);
					modelStack.Scale(2.55, 2.55, 2.55);
					meshList[GEO_SPHERE]->material.kAmbient.Set(1 * 0.25, 0, 0);
					meshList[GEO_SPHERE]->material.kDiffuse.Set(1, 0, 0);
					RenderMesh(meshList[GEO_SPHERE], true);
				}
				modelStack.PopMatrix();
			}
			modelStack.Rotate(0, 0, 1, 0);
			modelStack.Scale(4, 4, 4);
			meshList[GEO_SPHERE]->material.kAmbient.Set(0, 0, 0);
			meshList[GEO_SPHERE]->material.kDiffuse.Set(0, 0, 0);
			RenderMesh(meshList[GEO_SPHERE], true);
		}
		modelStack.PopMatrix();

		//Mouth
		modelStack.PushMatrix();
		{
			modelStack.Translate(0, 1, 7.68);
			modelStack.Translate(0, -1.5, 0.25);
			modelStack.Rotate(-180, 1, 0, 0);

			modelStack.Scale(1.8, 2.1, 0.3);
			meshList[GEO_MOUTH]->material.kAmbient.Set(0.4 * 0.7, 0, 0);
			meshList[GEO_MOUTH]->material.kDiffuse.Set(0.4, 0, 0);
			RenderMesh(meshList[GEO_MOUTH], true);
		}
		modelStack.PopMatrix();

		//Right ear
		modelStack.PushMatrix();
		{
			modelStack.Translate(6.8, 5.8, 0);
			{
				modelStack.PushMatrix();
				{
					modelStack.Translate(-0.3, -0.3, 0.41);
					modelStack.Rotate(-5, 1, 0, 0);
					modelStack.Rotate(-38, 0, 0, 1);
					modelStack.Scale(4.1, 5, 1);
					meshList[GEO_PYRAMID]->material.kAmbient.Set(0, 0, 0);
					meshList[GEO_PYRAMID]->material.kDiffuse.Set(0, 0, 0);
					RenderMesh(meshList[GEO_PYRAMID], true);
				}
				modelStack.PopMatrix();
			}
			modelStack.Rotate(-40, 0, 0, 1);
			modelStack.Scale(7, 7, 2);
			meshList[GEO_PYRAMID]->material.kAmbient.Set(1 * 0.7, 0, 0);
			meshList[GEO_PYRAMID]->material.kDiffuse.Set(1, 0, 0);
			RenderMesh(meshList[GEO_PYRAMID], true);
		}
		modelStack.PopMatrix();

		//Left ear
		modelStack.PushMatrix();
		{
			modelStack.Translate(-6.8, 5.8, 0);
			{
				modelStack.PushMatrix();
				{
					modelStack.Translate(0.3, -0.3, 0.41);
					modelStack.Rotate(-5, 1, 0, 0);
					modelStack.Rotate(38, 0, 0, 1);
					modelStack.Scale(4.1, 5, 1);
					meshList[GEO_PYRAMID]->material.kAmbient.Set(0, 0, 0);
					meshList[GEO_PYRAMID]->material.kDiffuse.Set(0, 0, 0);
					RenderMesh(meshList[GEO_PYRAMID], true);
				}
				modelStack.PopMatrix();
			}
			modelStack.Rotate(40, 0, 0, 1);
			modelStack.Scale(7, 7, 2);
			meshList[GEO_PYRAMID]->material.kAmbient.Set(1 * 0.7, 0, 0);
			meshList[GEO_PYRAMID]->material.kDiffuse.Set(1, 0, 0);
			RenderMesh(meshList[GEO_PYRAMID], true);
		}
		modelStack.PopMatrix();

		//Right arm
		modelStack.PushMatrix();
		{
			modelStack.Rotate(RotateEnemyRightArm_Movement, 0, 1, 0); // Moving right arms
			modelStack.Rotate(EnemyRightArm_JumpAttackRotation, 0, 0, 1);
			modelStack.Translate(-6.5, -1, 3.5);
			modelStack.Rotate(45, 0, 1, 0);
			modelStack.Scale(3, 1, 0.5);
			meshList[GEO_SPHERE]->material.kAmbient.Set(1 * 0.7, 0, 0);
			meshList[GEO_SPHERE]->material.kDiffuse.Set(1, 0, 0);
			RenderMesh(meshList[GEO_SPHERE], true);
		}
		modelStack.PopMatrix();

		//Left arm
		modelStack.PushMatrix();
		{
			modelStack.Rotate(RotateEnemyLeftArm_Movement, 0, 1, 0); // Moving left arms
			modelStack.Rotate(-EnemyRightArm_JumpAttackRotation, 0, 0, 1);
			modelStack.Translate(6.5, -1, 3.5);
			modelStack.Rotate(-45, 0, 1, 0);
			modelStack.Scale(3, 1, 0.5);
			meshList[GEO_SPHERE]->material.kAmbient.Set(1 * 0.7, 0, 0);
			meshList[GEO_SPHERE]->material.kDiffuse.Set(1, 0, 0);
			RenderMesh(meshList[GEO_SPHERE], true);
		}
		modelStack.PopMatrix();

		//Right Leg
		modelStack.PushMatrix();
		{
			modelStack.Rotate(-RotateEnemyRightLeg_Movement, 1, 0, 0); // Moving right leg
			modelStack.Translate(4.1, -8, 2);
			modelStack.Rotate(35, 0, 1, 0);
			modelStack.Scale(2, 0.8, 3);
			meshList[GEO_SPHERE]->material.kAmbient.Set(1 * 0.7, 0, 0);
			meshList[GEO_SPHERE]->material.kDiffuse.Set(1, 0, 0);
			RenderMesh(meshList[GEO_SPHERE], true);
		}
		modelStack.PopMatrix();

		//Left Leg
		modelStack.PushMatrix();
		{
			modelStack.Rotate(RotateEnemyLeftLeg_Movement, 1, 0, 0); // Moving left leg
			modelStack.Translate(-4.1, -8, 2);
			modelStack.Rotate(-35, 0, 1, 0);
			modelStack.Scale(2, 0.8, 3);
			meshList[GEO_SPHERE]->material.kAmbient.Set(1 * 0.7, 0, 0);
			meshList[GEO_SPHERE]->material.kDiffuse.Set(1, 0, 0);
			RenderMesh(meshList[GEO_SPHERE], true);
		}
		modelStack.PopMatrix();

		modelStack.Rotate(rotateBody, 0, 1, 0);
		modelStack.Scale(8, 8, 8);
		meshList[GEO_SPHERE]->material.kAmbient.Set(1 * 0.7, 0, 0);
		meshList[GEO_SPHERE]->material.kDiffuse.Set(1, 0, 0);
		RenderMesh(meshList[GEO_SPHERE], true);
	}
	modelStack.PopMatrix();



	//Demon Jigglypuff2
	modelStack.PushMatrix();
	{
		if (EnemyXSpeed2 > 0 or EnemyZSpeed2 > 0) {
			modelStack.Translate(0, EnemyUpandDown_Translation2, 0);
		}
		modelStack.Translate(EnemyPosX2, 5.5 + translateY_EnemyJump2, EnemyPosZ2);
		modelStack.Rotate(EnemyViewAngle2, 0, 1, 0);

		modelStack.Rotate(EnemyDieRotation2, 1, 0, 0);
		//hair
		modelStack.PushMatrix();
		{
			modelStack.Translate(0.1, 6, 3.5);

			modelStack.PushMatrix();
			{
				modelStack.Translate(-0.3, 0.3, -1);
				modelStack.Rotate(0, 1, 0, 0);
				modelStack.Rotate(0, 0, 0, 1);
				modelStack.Scale(2.6, 2.6, 2.6);
				meshList[GEO_SPHERE]->material.kAmbient.Set(1 * 0.7, 0, 0);
				meshList[GEO_SPHERE]->material.kDiffuse.Set(1, 0, 0);
				RenderMesh(meshList[GEO_SPHERE], true);
			}
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			{
				modelStack.Translate(0.3, -0.6, 1.1);
				modelStack.Rotate(0, 1, 0, 0);
				modelStack.Rotate(0, 0, 0, 1);
				modelStack.Scale(1.5, 1.5, 1.5);
				meshList[GEO_SPHERE]->material.kAmbient.Set(1 * 0.7, 0, 0);
				meshList[GEO_SPHERE]->material.kDiffuse.Set(1, 0, 0);
				RenderMesh(meshList[GEO_SPHERE], true);
			}
			modelStack.PopMatrix();

			modelStack.Rotate(20, 1, 0, 0);
			modelStack.Rotate(-20, 0, 0, 1);
			modelStack.Scale(0.7, 0.7, 0.7);
			meshList[GEO_TORUS]->material.kAmbient.Set(1 * 0.7, 0, 0);
			meshList[GEO_TORUS]->material.kDiffuse.Set(1, 0, 0);
			RenderMesh(meshList[GEO_TORUS], true);
		}
		modelStack.PopMatrix();

		//Eyes left mirror
		modelStack.PushMatrix();
		{
			modelStack.Translate(1.8, 2, 3.55);
			{
				// Left eyelid
				modelStack.PushMatrix();
				{
					modelStack.Translate(0.6, 0.8, 1.2);
					// -300 to -100
					modelStack.Rotate(-300, 1, 0, 0);
					modelStack.Scale(3, 3, 3);
					meshList[GEO_EYELID]->material.kAmbient.Set(1 * 0.7, 0, 0);
					meshList[GEO_EYELID]->material.kDiffuse.Set(1, 0, 0);
					RenderMesh(meshList[GEO_EYELID], true);
				}
				modelStack.PopMatrix();

				// Dark Cyan Pupil
				modelStack.PushMatrix();
				{
					modelStack.Translate(0.65, 0.73, 1.5);

					// Pupil eyes reflection
					modelStack.PushMatrix();
					{
						modelStack.Translate(0.1, 1.1, 1.2);
						modelStack.Rotate(0, 0, 1, 0);
						modelStack.Scale(1, 1, 1);
						meshList[GEO_SPHERE]->material.kAmbient.Set(1, 0, 0);
						meshList[GEO_SPHERE]->material.kDiffuse.Set(1, 0, 0);
						RenderMesh(meshList[GEO_SPHERE], true);
					}
					modelStack.PopMatrix();

					modelStack.Rotate(0, 0, 1, 0);
					modelStack.Scale(2.55, 2.55, 2.55);
					meshList[GEO_SPHERE]->material.kAmbient.Set(1 * 0.25, 0, 0);
					meshList[GEO_SPHERE]->material.kDiffuse.Set(1, 0, 0);
					RenderMesh(meshList[GEO_SPHERE], true);
				}
				modelStack.PopMatrix();
			}
			modelStack.Rotate(0, 0, 1, 0);
			modelStack.Scale(4, 4, 4);
			meshList[GEO_SPHERE]->material.kAmbient.Set(0, 0, 0);
			meshList[GEO_SPHERE]->material.kDiffuse.Set(0, 0, 0);
			RenderMesh(meshList[GEO_SPHERE], true);
		}
		modelStack.PopMatrix();

		//Eyes right mirror
		modelStack.PushMatrix();
		{
			modelStack.Translate(-1.8, 2, 3.55);
			{
				// Eye lid
				modelStack.PushMatrix();
				{
					modelStack.Translate(-0.6, 0.8, 1.2);
					modelStack.Rotate(-300, 1, 0, 0);
					modelStack.Scale(3, 3, 3);
					meshList[GEO_EYELID]->material.kAmbient.Set(1 * 0.7, 0, 0);
					meshList[GEO_EYELID]->material.kDiffuse.Set(1, 0, 0);
					RenderMesh(meshList[GEO_EYELID], true);
				}
				modelStack.PopMatrix();

				// Dark Cyan Pupil
				modelStack.PushMatrix();
				{
					modelStack.Translate(-0.65, 0.73, 1.5);
					// Pupil eyes reflection
					modelStack.PushMatrix();
					{
						modelStack.Translate(-0.1, 1.1, 1.2);
						modelStack.Rotate(0, 0, 1, 0);
						modelStack.Scale(1, 1, 1);
						meshList[GEO_SPHERE]->material.kAmbient.Set(1, 0, 0);
						meshList[GEO_SPHERE]->material.kDiffuse.Set(1, 0, 0);
						RenderMesh(meshList[GEO_SPHERE], true);
					}
					modelStack.PopMatrix();
					modelStack.Rotate(0, 0, 1, 0);
					modelStack.Scale(2.55, 2.55, 2.55);
					meshList[GEO_SPHERE]->material.kAmbient.Set(1 * 0.25, 0, 0);
					meshList[GEO_SPHERE]->material.kDiffuse.Set(1, 0, 0);
					RenderMesh(meshList[GEO_SPHERE], true);
				}
				modelStack.PopMatrix();
			}
			modelStack.Rotate(0, 0, 1, 0);
			modelStack.Scale(4, 4, 4);
			meshList[GEO_SPHERE]->material.kAmbient.Set(0, 0, 0);
			meshList[GEO_SPHERE]->material.kDiffuse.Set(0, 0, 0);
			RenderMesh(meshList[GEO_SPHERE], true);
		}
		modelStack.PopMatrix();

		//Mouth
		modelStack.PushMatrix();
		{
			modelStack.Translate(0, 1, 7.68);
			modelStack.Translate(0, -1.5, 0.25);
			modelStack.Rotate(-180, 1, 0, 0);

			modelStack.Scale(1.8, 2.1, 0.3);
			meshList[GEO_MOUTH]->material.kAmbient.Set(0.4 * 0.7, 0, 0);
			meshList[GEO_MOUTH]->material.kDiffuse.Set(0.4, 0, 0);
			RenderMesh(meshList[GEO_MOUTH], true);
		}
		modelStack.PopMatrix();

		//Right ear
		modelStack.PushMatrix();
		{
			modelStack.Translate(6.8, 5.8, 0);
			{
				modelStack.PushMatrix();
				{
					modelStack.Translate(-0.3, -0.3, 0.41);
					modelStack.Rotate(-5, 1, 0, 0);
					modelStack.Rotate(-38, 0, 0, 1);
					modelStack.Scale(4.1, 5, 1);
					meshList[GEO_PYRAMID]->material.kAmbient.Set(0, 0, 0);
					meshList[GEO_PYRAMID]->material.kDiffuse.Set(0, 0, 0);
					RenderMesh(meshList[GEO_PYRAMID], true);
				}
				modelStack.PopMatrix();
			}
			modelStack.Rotate(-40, 0, 0, 1);
			modelStack.Scale(7, 7, 2);
			meshList[GEO_PYRAMID]->material.kAmbient.Set(1 * 0.7, 0, 0);
			meshList[GEO_PYRAMID]->material.kDiffuse.Set(1, 0, 0);
			RenderMesh(meshList[GEO_PYRAMID], true);
		}
		modelStack.PopMatrix();

		//Left ear
		modelStack.PushMatrix();
		{
			modelStack.Translate(-6.8, 5.8, 0);
			{
				modelStack.PushMatrix();
				{
					modelStack.Translate(0.3, -0.3, 0.41);
					modelStack.Rotate(-5, 1, 0, 0);
					modelStack.Rotate(38, 0, 0, 1);
					modelStack.Scale(4.1, 5, 1);
					meshList[GEO_PYRAMID]->material.kAmbient.Set(0, 0, 0);
					meshList[GEO_PYRAMID]->material.kDiffuse.Set(0, 0, 0);
					RenderMesh(meshList[GEO_PYRAMID], true);
				}
				modelStack.PopMatrix();
			}
			modelStack.Rotate(40, 0, 0, 1);
			modelStack.Scale(7, 7, 2);
			meshList[GEO_PYRAMID]->material.kAmbient.Set(1 * 0.7, 0, 0);
			meshList[GEO_PYRAMID]->material.kDiffuse.Set(1, 0, 0);
			RenderMesh(meshList[GEO_PYRAMID], true);
		}
		modelStack.PopMatrix();

		//Right arm
		modelStack.PushMatrix();
		{
			modelStack.Rotate(RotateEnemyRightArm_Movement2, 0, 1, 0); // Moving right arms
			modelStack.Rotate(EnemyRightArm_JumpAttackRotation2, 0, 0, 1);
			modelStack.Translate(-6.5, -1, 3.5);
			modelStack.Rotate(45, 0, 1, 0);
			modelStack.Scale(3, 1, 0.5);
			meshList[GEO_SPHERE]->material.kAmbient.Set(1 * 0.7, 0, 0);
			meshList[GEO_SPHERE]->material.kDiffuse.Set(1, 0, 0);
			RenderMesh(meshList[GEO_SPHERE], true);
		}
		modelStack.PopMatrix();

		//Left arm
		modelStack.PushMatrix();
		{
			modelStack.Rotate(RotateEnemyLeftArm_Movement2, 0, 1, 0); // Moving left arms
			modelStack.Rotate(-EnemyRightArm_JumpAttackRotation2, 0, 0, 1);
			modelStack.Translate(6.5, -1, 3.5);
			modelStack.Rotate(-45, 0, 1, 0);
			modelStack.Scale(3, 1, 0.5);
			meshList[GEO_SPHERE]->material.kAmbient.Set(1 * 0.7, 0, 0);
			meshList[GEO_SPHERE]->material.kDiffuse.Set(1, 0, 0);
			RenderMesh(meshList[GEO_SPHERE], true);
		}
		modelStack.PopMatrix();

		//Right Leg
		modelStack.PushMatrix();
		{
			modelStack.Rotate(-RotateEnemyRightLeg_Movement2, 1, 0, 0); // Moving right leg
			modelStack.Translate(4.1, -8, 2);
			modelStack.Rotate(35, 0, 1, 0);
			modelStack.Scale(2, 0.8, 3);
			meshList[GEO_SPHERE]->material.kAmbient.Set(1 * 0.7, 0, 0);
			meshList[GEO_SPHERE]->material.kDiffuse.Set(1, 0, 0);
			RenderMesh(meshList[GEO_SPHERE], true);
		}
		modelStack.PopMatrix();

		//Left Leg
		modelStack.PushMatrix();
		{
			modelStack.Rotate(RotateEnemyLeftLeg_Movement2, 1, 0, 0); // Moving left leg
			modelStack.Translate(-4.1, -8, 2);
			modelStack.Rotate(-35, 0, 1, 0);
			modelStack.Scale(2, 0.8, 3);
			meshList[GEO_SPHERE]->material.kAmbient.Set(1 * 0.7, 0, 0);
			meshList[GEO_SPHERE]->material.kDiffuse.Set(1, 0, 0);
			RenderMesh(meshList[GEO_SPHERE], true);
		}
		modelStack.PopMatrix();

		modelStack.Rotate(rotateBody, 0, 1, 0);
		modelStack.Scale(8, 8, 8);
		meshList[GEO_SPHERE]->material.kAmbient.Set(1 * 0.7, 0, 0);
		meshList[GEO_SPHERE]->material.kDiffuse.Set(1, 0, 0);
		RenderMesh(meshList[GEO_SPHERE], true);
	}
	modelStack.PopMatrix();





	//Demon Jigglypuff3
	modelStack.PushMatrix();
	{
		if (EnemyXSpeed3 > 0 or EnemyZSpeed3 > 0) {
			modelStack.Translate(0, EnemyUpandDown_Translation3, 0);
		}
		modelStack.Translate(EnemyPosX3, 5.5 + translateY_EnemyJump3, EnemyPosZ3);
		modelStack.Rotate(EnemyViewAngle3, 0, 1, 0);

		modelStack.Rotate(EnemyDieRotation3, 1, 0, 0);

		//hair
		modelStack.PushMatrix();
		{
			modelStack.Translate(0.1, 6, 3.5);

			modelStack.PushMatrix();
			{
				modelStack.Translate(-0.3, 0.3, -1);
				modelStack.Rotate(0, 1, 0, 0);
				modelStack.Rotate(0, 0, 0, 1);
				modelStack.Scale(2.6, 2.6, 2.6);
				meshList[GEO_SPHERE]->material.kAmbient.Set(1 * 0.7, 0, 0);
				meshList[GEO_SPHERE]->material.kDiffuse.Set(1, 0, 0);
				RenderMesh(meshList[GEO_SPHERE], true);
			}
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			{
				modelStack.Translate(0.3, -0.6, 1.1);
				modelStack.Rotate(0, 1, 0, 0);
				modelStack.Rotate(0, 0, 0, 1);
				modelStack.Scale(1.5, 1.5, 1.5);
				meshList[GEO_SPHERE]->material.kAmbient.Set(1 * 0.7, 0, 0);
				meshList[GEO_SPHERE]->material.kDiffuse.Set(1, 0, 0);
				RenderMesh(meshList[GEO_SPHERE], true);
			}
			modelStack.PopMatrix();

			modelStack.Rotate(20, 1, 0, 0);
			modelStack.Rotate(-20, 0, 0, 1);
			modelStack.Scale(0.7, 0.7, 0.7);
			meshList[GEO_TORUS]->material.kAmbient.Set(1 * 0.7, 0, 0);
			meshList[GEO_TORUS]->material.kDiffuse.Set(1, 0, 0);
			RenderMesh(meshList[GEO_TORUS], true);
		}
		modelStack.PopMatrix();

		//Eyes left mirror
		modelStack.PushMatrix();
		{
			modelStack.Translate(1.8, 2, 3.55);
			{
				// Left eyelid
				modelStack.PushMatrix();
				{
					modelStack.Translate(0.6, 0.8, 1.2);
					// -300 to -100
					modelStack.Rotate(-300, 1, 0, 0);
					modelStack.Scale(3, 3, 3);
					meshList[GEO_EYELID]->material.kAmbient.Set(1 * 0.7, 0, 0);
					meshList[GEO_EYELID]->material.kDiffuse.Set(1, 0, 0);
					RenderMesh(meshList[GEO_EYELID], true);
				}
				modelStack.PopMatrix();

				// Dark Cyan Pupil
				modelStack.PushMatrix();
				{
					modelStack.Translate(0.65, 0.73, 1.5);

					// Pupil eyes reflection
					modelStack.PushMatrix();
					{
						modelStack.Translate(0.1, 1.1, 1.2);
						modelStack.Rotate(0, 0, 1, 0);
						modelStack.Scale(1, 1, 1);
						meshList[GEO_SPHERE]->material.kAmbient.Set(1, 0, 0);
						meshList[GEO_SPHERE]->material.kDiffuse.Set(1, 0, 0);
						RenderMesh(meshList[GEO_SPHERE], true);
					}
					modelStack.PopMatrix();

					modelStack.Rotate(0, 0, 1, 0);
					modelStack.Scale(2.55, 2.55, 2.55);
					meshList[GEO_SPHERE]->material.kAmbient.Set(1 * 0.25, 0, 0);
					meshList[GEO_SPHERE]->material.kDiffuse.Set(1, 0, 0);
					RenderMesh(meshList[GEO_SPHERE], true);
				}
				modelStack.PopMatrix();
			}
			modelStack.Rotate(0, 0, 1, 0);
			modelStack.Scale(4, 4, 4);
			meshList[GEO_SPHERE]->material.kAmbient.Set(0, 0, 0);
			meshList[GEO_SPHERE]->material.kDiffuse.Set(0, 0, 0);
			RenderMesh(meshList[GEO_SPHERE], true);
		}
		modelStack.PopMatrix();

		//Eyes right mirror
		modelStack.PushMatrix();
		{
			modelStack.Translate(-1.8, 2, 3.55);
			{
				// Eye lid
				modelStack.PushMatrix();
				{
					modelStack.Translate(-0.6, 0.8, 1.2);
					modelStack.Rotate(-300, 1, 0, 0);
					modelStack.Scale(3, 3, 3);
					meshList[GEO_EYELID]->material.kAmbient.Set(1 * 0.7, 0, 0);
					meshList[GEO_EYELID]->material.kDiffuse.Set(1, 0, 0);
					RenderMesh(meshList[GEO_EYELID], true);
				}
				modelStack.PopMatrix();

				// Dark Cyan Pupil
				modelStack.PushMatrix();
				{
					modelStack.Translate(-0.65, 0.73, 1.5);
					// Pupil eyes reflection
					modelStack.PushMatrix();
					{
						modelStack.Translate(-0.1, 1.1, 1.2);
						modelStack.Rotate(0, 0, 1, 0);
						modelStack.Scale(1, 1, 1);
						meshList[GEO_SPHERE]->material.kAmbient.Set(1, 0, 0);
						meshList[GEO_SPHERE]->material.kDiffuse.Set(1, 0, 0);
						RenderMesh(meshList[GEO_SPHERE], true);
					}
					modelStack.PopMatrix();
					modelStack.Rotate(0, 0, 1, 0);
					modelStack.Scale(2.55, 2.55, 2.55);
					meshList[GEO_SPHERE]->material.kAmbient.Set(1 * 0.25, 0, 0);
					meshList[GEO_SPHERE]->material.kDiffuse.Set(1, 0, 0);
					RenderMesh(meshList[GEO_SPHERE], true);
				}
				modelStack.PopMatrix();
			}
			modelStack.Rotate(0, 0, 1, 0);
			modelStack.Scale(4, 4, 4);
			meshList[GEO_SPHERE]->material.kAmbient.Set(0, 0, 0);
			meshList[GEO_SPHERE]->material.kDiffuse.Set(0, 0, 0);
			RenderMesh(meshList[GEO_SPHERE], true);
		}
		modelStack.PopMatrix();

		//Mouth
		modelStack.PushMatrix();
		{
			modelStack.Translate(0, 1, 7.68);
			modelStack.Translate(0, -1.5, 0.25);
			modelStack.Rotate(-180, 1, 0, 0);

			modelStack.Scale(1.8, 2.1, 0.3);
			meshList[GEO_MOUTH]->material.kAmbient.Set(0.4 * 0.7, 0, 0);
			meshList[GEO_MOUTH]->material.kDiffuse.Set(0.4, 0, 0);
			RenderMesh(meshList[GEO_MOUTH], true);
		}
		modelStack.PopMatrix();

		//Right ear
		modelStack.PushMatrix();
		{
			modelStack.Translate(6.8, 5.8, 0);
			{
				modelStack.PushMatrix();
				{
					modelStack.Translate(-0.3, -0.3, 0.41);
					modelStack.Rotate(-5, 1, 0, 0);
					modelStack.Rotate(-38, 0, 0, 1);
					modelStack.Scale(4.1, 5, 1);
					meshList[GEO_PYRAMID]->material.kAmbient.Set(0, 0, 0);
					meshList[GEO_PYRAMID]->material.kDiffuse.Set(0, 0, 0);
					RenderMesh(meshList[GEO_PYRAMID], true);
				}
				modelStack.PopMatrix();
			}
			modelStack.Rotate(-40, 0, 0, 1);
			modelStack.Scale(7, 7, 2);
			meshList[GEO_PYRAMID]->material.kAmbient.Set(1 * 0.7, 0, 0);
			meshList[GEO_PYRAMID]->material.kDiffuse.Set(1, 0, 0);
			RenderMesh(meshList[GEO_PYRAMID], true);
		}
		modelStack.PopMatrix();

		//Left ear
		modelStack.PushMatrix();
		{
			modelStack.Translate(-6.8, 5.8, 0);
			{
				modelStack.PushMatrix();
				{
					modelStack.Translate(0.3, -0.3, 0.41);
					modelStack.Rotate(-5, 1, 0, 0);
					modelStack.Rotate(38, 0, 0, 1);
					modelStack.Scale(4.1, 5, 1);
					meshList[GEO_PYRAMID]->material.kAmbient.Set(0, 0, 0);
					meshList[GEO_PYRAMID]->material.kDiffuse.Set(0, 0, 0);
					RenderMesh(meshList[GEO_PYRAMID], true);
				}
				modelStack.PopMatrix();
			}
			modelStack.Rotate(40, 0, 0, 1);
			modelStack.Scale(7, 7, 2);
			meshList[GEO_PYRAMID]->material.kAmbient.Set(1 * 0.7, 0, 0);
			meshList[GEO_PYRAMID]->material.kDiffuse.Set(1, 0, 0);
			RenderMesh(meshList[GEO_PYRAMID], true);
		}
		modelStack.PopMatrix();

		//Right arm
		modelStack.PushMatrix();
		{
			modelStack.Rotate(RotateEnemyRightArm_Movement3, 0, 1, 0); // Moving right arms
			modelStack.Rotate(EnemyRightArm_JumpAttackRotation3, 0, 0, 1);
			modelStack.Translate(-6.5, -1, 3.5);
			modelStack.Rotate(45, 0, 1, 0);
			modelStack.Scale(3, 1, 0.5);
			meshList[GEO_SPHERE]->material.kAmbient.Set(1 * 0.7, 0, 0);
			meshList[GEO_SPHERE]->material.kDiffuse.Set(1, 0, 0);
			RenderMesh(meshList[GEO_SPHERE], true);
		}
		modelStack.PopMatrix();

		//Left arm
		modelStack.PushMatrix();
		{
			modelStack.Rotate(RotateEnemyLeftArm_Movement3, 0, 1, 0); // Moving left arms
			modelStack.Rotate(-EnemyRightArm_JumpAttackRotation3, 0, 0, 1);
			modelStack.Translate(6.5, -1, 3.5);
			modelStack.Rotate(-45, 0, 1, 0);
			modelStack.Scale(3, 1, 0.5);
			meshList[GEO_SPHERE]->material.kAmbient.Set(1 * 0.7, 0, 0);
			meshList[GEO_SPHERE]->material.kDiffuse.Set(1, 0, 0);
			RenderMesh(meshList[GEO_SPHERE], true);
		}
		modelStack.PopMatrix();

		//Right Leg
		modelStack.PushMatrix();
		{
			modelStack.Rotate(-RotateEnemyRightLeg_Movement3, 1, 0, 0); // Moving right leg
			modelStack.Translate(4.1, -8, 2);
			modelStack.Rotate(35, 0, 1, 0);
			modelStack.Scale(2, 0.8, 3);
			meshList[GEO_SPHERE]->material.kAmbient.Set(1 * 0.7, 0, 0);
			meshList[GEO_SPHERE]->material.kDiffuse.Set(1, 0, 0);
			RenderMesh(meshList[GEO_SPHERE], true);
		}
		modelStack.PopMatrix();

		//Left Leg
		modelStack.PushMatrix();
		{
			modelStack.Rotate(RotateEnemyLeftLeg_Movement3, 1, 0, 0); // Moving left leg
			modelStack.Translate(-4.1, -8, 2);
			modelStack.Rotate(-35, 0, 1, 0);
			modelStack.Scale(2, 0.8, 3);
			meshList[GEO_SPHERE]->material.kAmbient.Set(1 * 0.7, 0, 0);
			meshList[GEO_SPHERE]->material.kDiffuse.Set(1, 0, 0);
			RenderMesh(meshList[GEO_SPHERE], true);
		}
		modelStack.PopMatrix();

		modelStack.Rotate(rotateBody, 0, 1, 0);
		modelStack.Scale(8, 8, 8);
		meshList[GEO_SPHERE]->material.kAmbient.Set(1 * 0.7, 0, 0);
		meshList[GEO_SPHERE]->material.kDiffuse.Set(1, 0, 0);
		RenderMesh(meshList[GEO_SPHERE], true);
	}
	modelStack.PopMatrix();






	if (ShockWaveEnemyAttack == true) {
		modelStack.PushMatrix();
		{
			modelStack.Translate(EnemyPosX, -1, EnemyPosZ);
			modelStack.Rotate(0, 0, 1, 0);
			modelStack.Scale(EnemyShockwaveScale, 1, EnemyShockwaveScale);
			meshList[GEO_TORUS]->material.kAmbient.Set(1 * 0.4, 0.27 * 0.4, 0);
			meshList[GEO_TORUS]->material.kDiffuse.Set(1, 0.27, 0);
			RenderMesh(meshList[GEO_TORUS], true);
		}
		modelStack.PopMatrix();
	}
	if (ShockWaveEnemyAttack2 == true) {
		modelStack.PushMatrix();
		{
			modelStack.Translate(EnemyPosX2, -1, EnemyPosZ2);
			modelStack.Rotate(0, 0, 1, 0);
			modelStack.Scale(EnemyShockwaveScale2, 1, EnemyShockwaveScale2);
			meshList[GEO_TORUS]->material.kAmbient.Set(1 * 0.4, 0.27 * 0.4, 0);
			meshList[GEO_TORUS]->material.kDiffuse.Set(1, 0.27, 0);
			RenderMesh(meshList[GEO_TORUS], true);
		}
		modelStack.PopMatrix();
	}
	if (ShockWaveEnemyAttack3 == true) {
		modelStack.PushMatrix();
		{
			modelStack.Translate(EnemyPosX3, -1, EnemyPosZ3);
			modelStack.Rotate(0, 0, 1, 0);
			modelStack.Scale(EnemyShockwaveScale3, 1, EnemyShockwaveScale3);
			meshList[GEO_TORUS]->material.kAmbient.Set(1 * 0.4, 0.27 * 0.4, 0);
			meshList[GEO_TORUS]->material.kDiffuse.Set(1, 0.27, 0);
			RenderMesh(meshList[GEO_TORUS], true);
		}
		modelStack.PopMatrix();
	}





	if (is1stPersonCam == false) {
		//Jigglypuff Body
		modelStack.PushMatrix();
		{
			modelStack.Translate(0, TranslateUpandDown, 0);
			//modelStack.Rotate(180, 0, 1, 0);
			modelStack.Translate(translateX, 5.5 + translatebodyY_Jump, translateZ);
			modelStack.Rotate(rotateBody + rotateBodySpinAttack, 0, 1, 0);
			modelStack.Rotate(rotateAngle, 1, 0, 0);

			modelStack.Rotate(jumpAttack_rotation, 1, 0, 0);

			//hair
			modelStack.PushMatrix();
			{
				modelStack.Translate(0.1, 6, 3.5);

				modelStack.PushMatrix();
				{
					modelStack.Translate(-0.3, 0.3, -1);
					modelStack.Rotate(0, 1, 0, 0);
					modelStack.Rotate(0, 0, 0, 1);
					modelStack.Scale(2.6, 2.6, 2.6);
					meshList[GEO_SPHERE]->material.kAmbient.Set(BodyR * 0.7, BodyG * 0.7, BodyB * 0.7);
					meshList[GEO_SPHERE]->material.kDiffuse.Set(BodyR, BodyG, BodyB);
					RenderMesh(meshList[GEO_SPHERE], true);
				}
				modelStack.PopMatrix();

				modelStack.PushMatrix();
				{
					modelStack.Translate(0.3, -0.6, 1.1);
					modelStack.Rotate(0, 1, 0, 0);
					modelStack.Rotate(0, 0, 0, 1);
					modelStack.Scale(1.5, 1.5, 1.5);
					meshList[GEO_SPHERE]->material.kAmbient.Set(BodyR * 0.7, BodyG * 0.7, BodyB * 0.7);
					meshList[GEO_SPHERE]->material.kDiffuse.Set(BodyR, BodyG, BodyB);
					RenderMesh(meshList[GEO_SPHERE], true);
				}
				modelStack.PopMatrix();

				modelStack.Rotate(20, 1, 0, 0);
				modelStack.Rotate(-20, 0, 0, 1);
				modelStack.Scale(0.7, 0.7, 0.7);
				meshList[GEO_TORUS]->material.kAmbient.Set(BodyR * 0.7, BodyG * 0.7, BodyB * 0.7);
				meshList[GEO_TORUS]->material.kDiffuse.Set(BodyR, BodyG, BodyB);
				RenderMesh(meshList[GEO_TORUS], true);
			}
			modelStack.PopMatrix();

			//Eyes left mirror
			modelStack.PushMatrix();
			{
				modelStack.Translate(1.8, 2, 3.55);
				{
					// Left eyelid
					modelStack.PushMatrix();
					{
						modelStack.Translate(0.6, 0.8, 1.2);
						// -300 to -100
						modelStack.Rotate(rotate_Eyelid, 1, 0, 0);
						modelStack.Scale(3, 3, 3);
						meshList[GEO_EYELID]->material.kAmbient.Set(BodyR * 0.7, BodyG * 0.7, BodyB * 0.7);
						meshList[GEO_EYELID]->material.kDiffuse.Set(BodyR, BodyG, BodyB);
						RenderMesh(meshList[GEO_EYELID], true);
					}
					modelStack.PopMatrix();

					// Dark Cyan Pupil
					modelStack.PushMatrix();
					{
						modelStack.Translate(0.65, 0.73, 1.5);

						// Pupil eyes reflection
						modelStack.PushMatrix();
						{
							modelStack.Translate(0.1, 1.1, 1.2);
							modelStack.Rotate(0, 0, 1, 0);
							modelStack.Scale(1, 1, 1);
							meshList[GEO_SPHERE]->material.kAmbient.Set(1, 1, 1);
							meshList[GEO_SPHERE]->material.kDiffuse.Set(1, 1, 1);
							RenderMesh(meshList[GEO_SPHERE], true);
						}
						modelStack.PopMatrix();

						modelStack.Rotate(0, 0, 1, 0);
						modelStack.Scale(2.55, 2.55, 2.55);
						meshList[GEO_SPHERE]->material.kAmbient.Set(0.1 * 0.7, 0.51 * 0.7, 0.57 * 0.7);
						meshList[GEO_SPHERE]->material.kDiffuse.Set(0.1, 0.51, 0.57);
						RenderMesh(meshList[GEO_SPHERE], true);
					}
					modelStack.PopMatrix();
				}
				modelStack.Rotate(0, 0, 1, 0);
				modelStack.Scale(4, 4, 4);
				meshList[GEO_SPHERE]->material.kAmbient.Set(1 * 0.7, 1 * 0.7, 1 * 0.7);
				meshList[GEO_SPHERE]->material.kDiffuse.Set(1, 1, 1);
				RenderMesh(meshList[GEO_SPHERE], true);
			}
			modelStack.PopMatrix();

			//Eyes right mirror
			modelStack.PushMatrix();
			{
				modelStack.Translate(-1.8, 2, 3.55);
				{
					// Eye lid
					modelStack.PushMatrix();
					{
						modelStack.Translate(-0.6, 0.8, 1.2);
						modelStack.Rotate(rotate_Eyelid, 1, 0, 0);
						modelStack.Scale(3, 3, 3);
						meshList[GEO_EYELID]->material.kAmbient.Set(BodyR * 0.7, BodyG * 0.7, BodyB * 0.7);
						meshList[GEO_EYELID]->material.kDiffuse.Set(BodyR, BodyG, BodyB);
						RenderMesh(meshList[GEO_EYELID], true);
					}
					modelStack.PopMatrix();

					// Dark Cyan Pupil
					modelStack.PushMatrix();
					{
						modelStack.Translate(-0.65, 0.73, 1.5);
						// Pupil eyes reflection
						modelStack.PushMatrix();
						{
							modelStack.Translate(-0.1, 1.1, 1.2);
							modelStack.Rotate(0, 0, 1, 0);
							modelStack.Scale(1, 1, 1);
							meshList[GEO_SPHERE]->material.kAmbient.Set(1, 1, 1);
							meshList[GEO_SPHERE]->material.kDiffuse.Set(1, 1, 1);
							RenderMesh(meshList[GEO_SPHERE], true);
						}
						modelStack.PopMatrix();
						modelStack.Rotate(0, 0, 1, 0);
						modelStack.Scale(2.55, 2.55, 2.55);
						meshList[GEO_SPHERE]->material.kAmbient.Set(0.1 * 0.7, 0.51 * 0.7, 0.57 * 0.7);
						meshList[GEO_SPHERE]->material.kDiffuse.Set(0.1, 0.51, 0.57);
						RenderMesh(meshList[GEO_SPHERE], true);
					}
					modelStack.PopMatrix();
				}
				modelStack.Rotate(0, 0, 1, 0);
				modelStack.Scale(4, 4, 4);
				meshList[GEO_SPHERE]->material.kAmbient.Set(1 * 0.7, 1 * 0.7, 1 * 0.7);
				meshList[GEO_SPHERE]->material.kDiffuse.Set(1, 1, 1);
				RenderMesh(meshList[GEO_SPHERE], true);
			}
			modelStack.PopMatrix();

			//Mouth
			modelStack.PushMatrix();
			{
				modelStack.Translate(0, 1, 7.68);
				if (isAlive == false) {
					modelStack.Translate(0, -1.5, 0.25);
					modelStack.Rotate(-180, 1, 0, 0);
				}
				modelStack.Rotate(-5, 1, 0, 0);
				modelStack.Scale(1.8, 2.1, 0.3);
				meshList[GEO_MOUTH]->material.kAmbient.Set(0.89 * 0.85, 0.67 * 0.85, 0.71 * 0.85);
				meshList[GEO_MOUTH]->material.kDiffuse.Set(0.89, 0.67, 0.71);
				RenderMesh(meshList[GEO_MOUTH], true);
			}
			modelStack.PopMatrix();

			//Right ear
			modelStack.PushMatrix();
			{
				modelStack.Translate(6.8, 5.8, 0);
				{
					modelStack.PushMatrix();
					{
						modelStack.Translate(-0.3, -0.3, 0.41);
						modelStack.Rotate(-5, 1, 0, 0);
						modelStack.Rotate(-38, 0, 0, 1);
						modelStack.Scale(4.1, 5, 1);
						meshList[GEO_PYRAMID]->material.kAmbient.Set(0, 0, 0);
						meshList[GEO_PYRAMID]->material.kDiffuse.Set(0, 0, 0);
						RenderMesh(meshList[GEO_PYRAMID], true);
					}
					modelStack.PopMatrix();
				}
				modelStack.Rotate(-40, 0, 0, 1);
				modelStack.Scale(7, 7, 2);
				meshList[GEO_PYRAMID]->material.kAmbient.Set(BodyR * 0.7, BodyG * 0.7, BodyB * 0.7);
				meshList[GEO_PYRAMID]->material.kDiffuse.Set(BodyR, BodyG, BodyB);
				RenderMesh(meshList[GEO_PYRAMID], true);
			}
			modelStack.PopMatrix();

			//Left ear
			modelStack.PushMatrix();
			{
				modelStack.Translate(-6.8, 5.8, 0);
				{
					modelStack.PushMatrix();
					{
						modelStack.Translate(0.3, -0.3, 0.41);
						modelStack.Rotate(-5, 1, 0, 0);
						modelStack.Rotate(38, 0, 0, 1);
						modelStack.Scale(4.1, 5, 1);
						meshList[GEO_PYRAMID]->material.kAmbient.Set(0, 0, 0);
						meshList[GEO_PYRAMID]->material.kDiffuse.Set(0, 0, 0);
						RenderMesh(meshList[GEO_PYRAMID], true);
					}
					modelStack.PopMatrix();
				}
				modelStack.Rotate(40, 0, 0, 1);
				modelStack.Scale(7, 7, 2);
				meshList[GEO_PYRAMID]->material.kAmbient.Set(BodyR * 0.7, BodyG * 0.7, BodyB * 0.7);
				meshList[GEO_PYRAMID]->material.kDiffuse.Set(BodyR, BodyG, BodyB);
				RenderMesh(meshList[GEO_PYRAMID], true);
			}
			modelStack.PopMatrix();

			//Right arm
			modelStack.PushMatrix();
			{
				modelStack.Rotate(RotateRightArm_Movement, 0, 1, 0); // Moving right arms

				modelStack.Rotate(RotateRightArm_SwingSword, 0, 1, 0); // Moving right arms for sword

				if (allowUltimateAttack == true and allowJumpAttack == false and allowJump == false and allowAttack == false and allowSpinattack == false and isAlive == true) {
					modelStack.Rotate(jumpAttack_rotation, 0, 0, 1); // Moving right arms for sword
				}

				modelStack.Translate(-6.5, -0.5, 3.5);
				//Microphone handle
				modelStack.PushMatrix();
				{
					if (isDemon == true and allowJumpAttack == true and allowSpinattack == false) {
						modelStack.Rotate(90, 0, 0, 1);
					}
					if (isDemon == true and allowJumpAttack == true and allowSpinattack == false) {
						modelStack.Translate(-2, 4, 1.2);
						modelStack.Rotate(90, 0, 0, 1);
						modelStack.Rotate(45, 0, 1, 0);
						modelStack.Rotate(65, 1, 0, 0);
					}


					if (isDemon == true and allowJumpAttack == true and allowSpinattack == false) {
						modelStack.Rotate(80, 1, 0, 0);
					}
					if (isDemon == true and allowSpinattack == true and allowAttack == false and allowJumpAttack == false) {
						modelStack.Translate(-2, 0, 0);
						modelStack.Rotate(RotateRightArm_Movement, 0, 1, 0);
					}

					modelStack.Translate(0, 0, 0);

					if (isDemon == true and allowSpinattack == false and allowJumpAttack == false and allowAttack == true) {
						modelStack.Rotate(RotateRightArm_SwingSword * 2, 1, 0, 0);
					}
					if (isDemon == true and allowSpinattack == true) {
						modelStack.Translate(-2, 2, 0);
						modelStack.Rotate(rotateSwordSpinAttack, 0, 0, 1);
						modelStack.Rotate(Temp, 1, 0, 0);
					}
					if (isDemon == true and allowReleaseofFusionWave == true and allowUltimateAttack == true) {
						modelStack.Translate(-4.5, 2, 2.3);
						modelStack.Rotate(rotateSwordSpinAttack, 0, 0, 1);
						modelStack.Rotate(Temp, 1, 0, 0);
					}
					if (isDemon == true and UltiSwordRevert_rotation == true) {
						modelStack.Rotate(rotateSwordSpinAttack, 0, 0, 1);
					}

					if (isDemon == true and allowJumpAttack == true and allowSpinattack == false) {
						modelStack.Rotate(-30, 1, 0, 0);
					}

					modelStack.Translate(-2, 0.2, 2.5);
					if (isDemon == true and allowAttack == true) {
						modelStack.Translate(0, 0, -0.5);
					}

					//Microphone Speaker
					modelStack.PushMatrix();
					{
						modelStack.Translate(0, 2, 0.1);

						if (allowUltimateAttack == true) {
							//Fire 1
							modelStack.PushMatrix();
							{
								modelStack.Translate(0, 2, 0);
								modelStack.Translate(rand() % 3 - 1, -1 + fireEffects2, rand() % 2 - 1);
								modelStack.Scale(ScaleFireEffect1, ScaleFireEffect1, ScaleFireEffect1);
								modelStack.Rotate(0, 0, 1, 0);
								modelStack.Scale(0.7, 2, 0.7);
								RenderMesh(meshList[GEO_FIRE], true);
							}
							modelStack.PopMatrix();
							//Fire 2
							modelStack.PushMatrix();
							{
								modelStack.Translate(0, 2, 0);
								modelStack.Translate(rand() % 3 - 1, -1 + fireEffects2, rand() % 2 - 1);
								modelStack.Scale(ScaleFireEffect2, ScaleFireEffect2, ScaleFireEffect2);
								modelStack.Rotate(0, 0, 1, 0);
								modelStack.Scale(0.7, 2, 0.7);
								RenderMesh(meshList[GEO_FIRE], true);
							}
							modelStack.PopMatrix();
							//Fire 3
							modelStack.PushMatrix();
							{
								modelStack.Translate(0, 2, 0);
								modelStack.Translate(rand() % 3 - 1, -1 + fireEffects3, rand() % 2 - 1);
								modelStack.Scale(ScaleFireEffect3, ScaleFireEffect3, ScaleFireEffect3);
								modelStack.Rotate(0, 0, 1, 0);
								modelStack.Scale(0.7, 2, 0.7);
								RenderMesh(meshList[GEO_FIRE], true);
							}
							modelStack.PopMatrix();
							//Fire 4
							modelStack.PushMatrix();
							{
								modelStack.Translate(0, 2, 0);
								modelStack.Translate(rand() % 3 - 1, -1 + fireEffects4, rand() % 2 - 1);
								modelStack.Scale(ScaleFireEffect4, ScaleFireEffect4, ScaleFireEffect4);
								modelStack.Rotate(0, 0, 1, 0);
								modelStack.Scale(0.7, 2, 0.7);
								RenderMesh(meshList[GEO_FIRE], true);
							}
							modelStack.PopMatrix();
							//Fire 5
							modelStack.PushMatrix();
							{
								modelStack.Translate(0, 2, 0);
								modelStack.Translate(rand() % 3 - 1, -1 + fireEffects5, rand() % 2 - 1);
								modelStack.Scale(ScaleFireEffect5, ScaleFireEffect5, ScaleFireEffect5);
								modelStack.Rotate(0, 0, 1, 0);
								modelStack.Scale(0.7, 2, 0.7);
								RenderMesh(meshList[GEO_FIRE], true);
							}
							modelStack.PopMatrix();
						}


						// Microphone to sword
						modelStack.Rotate(90, 0, 0, 1);
						modelStack.Rotate(90, 1, 0, 0);
						// Spawn the blade
						if (isDemon == true) {
							modelStack.PushMatrix();
							{
								modelStack.Translate(5, 0, 0);
								modelStack.Scale(1, 1, 1);
								modelStack.PushMatrix();
								{
									modelStack.Translate(5.5, 0, 0);
									modelStack.Rotate(-90, 0, 0, 1);
									modelStack.Scale(2, 2, 0.5);
									if (allowUltimateAttack == false) {
										meshList[GEO_PYRAMID]->material.kAmbient.Set(1 * 0.7, 1 * 0.7, 1 * 0.7);
										meshList[GEO_PYRAMID]->material.kDiffuse.Set(1, 1, 1);
										RenderMesh(meshList[GEO_PYRAMID], true);
									}
									else {
										meshList[GEO_PYRAMID]->material.kAmbient.Set(1 * 0.4, 0.27 * 0.4, 0);
										meshList[GEO_PYRAMID]->material.kDiffuse.Set(1, 0.27, 0);
										RenderMesh(meshList[GEO_PYRAMID], true);
									}

								}
								modelStack.PopMatrix();
								modelStack.Rotate(0, 0, 0, 1);
								modelStack.Scale(9, 2, 0.5);
								if (allowUltimateAttack == false) {
									meshList[GEO_CUBE]->material.kAmbient.Set(1 * 0.7, 1 * 0.7, 1 * 0.7);
									meshList[GEO_CUBE]->material.kDiffuse.Set(1, 1, 1);
									RenderMesh(meshList[GEO_CUBE], true);
								}
								else {
									meshList[GEO_CUBE]->material.kAmbient.Set(1 * 0.4, 0.27 * 0.4, 0);
									meshList[GEO_CUBE]->material.kDiffuse.Set(1, 0.27, 0);
									RenderMesh(meshList[GEO_CUBE], true);
								}

							}
							modelStack.PopMatrix();
						}
						modelStack.Scale(0.6, 3, 0.6);
						meshList[GEO_CYLINDER]->material.kAmbient.Set(0.29 * 0.7, 0.7 * 0.8, 0.32 * 0.7);
						meshList[GEO_CYLINDER]->material.kDiffuse.Set(0.29, 0.33, 0.32);
						RenderMesh(meshList[GEO_CYLINDER], true);
					}
					modelStack.PopMatrix();

					modelStack.Rotate(0, 0, 1, 0);
					modelStack.Scale(0.4, 3.0, 0.4);
					meshList[GEO_CYLINDER]->material.kAmbient.Set(0, 0, 0);
					meshList[GEO_CYLINDER]->material.kDiffuse.Set(0, 0, 0);
					RenderMesh(meshList[GEO_CYLINDER], true);
				}
				modelStack.PopMatrix();

				modelStack.Rotate(45, 0, 1, 0);
				modelStack.Scale(3, 1, 0.5);
				meshList[GEO_SPHERE]->material.kAmbient.Set(BodyR * 0.7, BodyG * 0.7, BodyB * 0.7);
				meshList[GEO_SPHERE]->material.kDiffuse.Set(BodyR, BodyG, BodyB);
				RenderMesh(meshList[GEO_SPHERE], true);
			}
			modelStack.PopMatrix();

			//Left arm
			modelStack.PushMatrix();
			{
				modelStack.Rotate(RotateLeftArm_Movement, 0, 1, 0); // Moving left arms

				modelStack.Translate(6.5, -1, 3.5);

				modelStack.Rotate(-45, 0, 1, 0);
				modelStack.Scale(3, 1, 0.5);
				meshList[GEO_SPHERE]->material.kAmbient.Set(BodyR * 0.7, BodyG * 0.7, BodyB * 0.7);
				meshList[GEO_SPHERE]->material.kDiffuse.Set(BodyR, BodyG, BodyB);
				RenderMesh(meshList[GEO_SPHERE], true);
			}
			modelStack.PopMatrix();

			//Right Leg
			modelStack.PushMatrix();
			{
				modelStack.Rotate(-RotateRightLeg_Movement, 1, 0, 0); // Moving right leg
				if (isAlive == true) {
					modelStack.Rotate(-jumpAttack_rotation, 1, 0, 0);
				}
				if (rotateBodySpinAttack >= 360) {
					modelStack.Rotate(-rotateBodySpinAttack, 0, 1, 0);
				}
				modelStack.Translate(4.1, -8, 2);
				modelStack.Rotate(rotatelegs_jump, 1, 0, 0); // right leg jump
				modelStack.Rotate(35, 0, 1, 0);
				modelStack.Scale(2, 0.8, 3);
				meshList[GEO_SPHERE]->material.kAmbient.Set(BodyR * 0.7, BodyG * 0.7, BodyB * 0.7);
				meshList[GEO_SPHERE]->material.kDiffuse.Set(BodyR, BodyG, BodyB);
				RenderMesh(meshList[GEO_SPHERE], true);
			}
			modelStack.PopMatrix();

			//Left Leg
			modelStack.PushMatrix();
			{
				modelStack.Rotate(RotateLeftLeg_Movement, 1, 0, 0); // Moving left leg
				if (isAlive == true) {
					modelStack.Rotate(-jumpAttack_rotation, 1, 0, 0);
				}
				if (rotateBodySpinAttack >= 360) {
					modelStack.Rotate(-rotateBodySpinAttack, 0, 1, 0);
				}
				modelStack.Translate(-4.1, -8, 2);
				modelStack.Rotate(rotatelegs_jump, 1, 0, 0); // left leg jump
				modelStack.Rotate(-35, 0, 1, 0);
				modelStack.Scale(2, 0.8, 3);
				meshList[GEO_SPHERE]->material.kAmbient.Set(BodyR * 0.7, BodyG * 0.7, BodyB * 0.7);
				meshList[GEO_SPHERE]->material.kDiffuse.Set(BodyR, BodyG, BodyB);
				RenderMesh(meshList[GEO_SPHERE], true);
			}
			modelStack.PopMatrix();


			modelStack.Rotate(rotateBody, 0, 1, 0);
			modelStack.Scale(8, 8, 8);
			meshList[GEO_SPHERE]->material.kAmbient.Set(BodyR * 0.7, BodyG * 0.7, BodyB * 0.7);
			meshList[GEO_SPHERE]->material.kDiffuse.Set(BodyR, BodyG, BodyB);
			RenderMesh(meshList[GEO_SPHERE], true);
		}
		modelStack.PopMatrix();

		//Wave testing
		if (translatebodyY_Jump <= 0 and air == false and ground == true and i == true and DisplayShockwave == true and isAlive == true) {
			modelStack.PushMatrix();
			{
				modelStack.Translate(translateX + ShockwavedirectionX, -1, translateZ + ShockwavedirectionZ);
				modelStack.Rotate(0, 0, 1, 0);
				modelStack.Scale(ShockwaveScale, 1, ShockwaveScale);
				if (allowUltimateAttack == true) {
					meshList[GEO_TORUS]->material.kAmbient.Set(1 * 0.4, 0.27 * 0.4, 0);
					meshList[GEO_TORUS]->material.kDiffuse.Set(1, 0.27, 0);
				}
				else {
					meshList[GEO_TORUS]->material.kAmbient.Set(1, 1, 1);
					meshList[GEO_TORUS]->material.kDiffuse.Set(1, 1, 1);
				}
				RenderMesh(meshList[GEO_TORUS], true);
			}
			modelStack.PopMatrix();
		}

		//Fusion Wave
		if (allowFusionWaveTransverse == false) {
			TranslateXEqualFusionWaveX = translateX;
			TranslateZEqualFusionWaveZ = translateZ;
		}
		if (allowFusionWaveTransverse == true and isAlive == true) {
			modelStack.PushMatrix();
			{
				modelStack.Translate(TranslateXEqualFusionWaveX + translateFusionWaveX, 5, TranslateZEqualFusionWaveZ + translateFusionWaveZ);
				modelStack.Rotate(rotateFusionWave, 0, 1, 0);
				modelStack.Scale(2, 1, 3.5);
				meshList[GEO_HALFTORUS]->material.kAmbient.Set(1 * 0.4, 0.27 * 0.4, 0);
				meshList[GEO_HALFTORUS]->material.kDiffuse.Set(1, 0.27, 0);
				RenderMesh(meshList[GEO_HALFTORUS], true);
			}
			modelStack.PopMatrix();
		}
	}

	if (isInHitbox(translateX + ShockwavedirectionX, translateZ + ShockwavedirectionZ, EnemyPosX, EnemyPosZ, 30) == true and air == false and ground == true and EnemyHealth > 0 and allowAttack == false and allowJumpAttack == true and translatebodyY_Jump <= 0 and ShockwaveScale <= 1) {
		EnemyHealth -= 20;
		if (allowUltimateAttack == true) {
			Health += 10;
		}
	}

	if (isInHitbox(translateX + ShockwavedirectionX, translateZ + ShockwavedirectionZ, EnemyPosX, EnemyPosZ, 20) == true and EnemyHealth > 0 and allowAttack == true and allowJumpAttack == false and RotateRightArm_SwingSword >= 50) {
		EnemyHealth -= 10;
		if (allowUltimateAttack == true) {
			Health += 20;
		}
	}

	if (isInHitbox(translateX, translateZ, EnemyPosX, EnemyPosZ, 30) == true and EnemyHealth > 0 and allowAttack == false and allowJumpAttack == false and allowSpinattack == true and rotateBodySpinAttack >= 455) {
		EnemyHealth -= 20;
		if (allowUltimateAttack == true) {
			Health += 20;
		}
	}

	static bool at = false;

	if (isInHitbox(translateX + translateFusionWaveX, translateZ + translateFusionWaveZ, EnemyPosX, EnemyPosZ, 20) == true and at == false and allowAttack == false and EnemyHealth > 0 and allowFusionWaveTransverse == true) {
		EnemyHealth -= 3;
		if (allowFusionWaveTransverse == true) {
			Health += 5;
		}
		at = true;
	}
	else {
		at = false;
	}




	if (isInHitbox(translateX + ShockwavedirectionX, translateZ + ShockwavedirectionZ, EnemyPosX2, EnemyPosZ2, 30) == true and air == false and ground == true and EnemyHealth2 > 0 and allowAttack == false and allowJumpAttack == true and translatebodyY_Jump <= 0 and ShockwaveScale <= 1) {
		EnemyHealth2 -= 20;
		if (allowUltimateAttack == true) {
			Health += 10;
		}
	}

	if (isInHitbox(translateX + ShockwavedirectionX, translateZ + ShockwavedirectionZ, EnemyPosX2, EnemyPosZ2, 20) == true and EnemyHealth2 > 0 and allowAttack == true and allowJumpAttack == false and RotateRightArm_SwingSword >= 50) {
		EnemyHealth2 -= 10;
		if (allowUltimateAttack == true) {
			Health += 20;
		}
	}

	if (isInHitbox(translateX, translateZ, EnemyPosX2, EnemyPosZ2, 30) == true and EnemyHealth2 > 0 and allowAttack == false and allowJumpAttack == false and allowSpinattack == true and rotateBodySpinAttack >= 455) {
		EnemyHealth2 -= 20;
		if (allowUltimateAttack == true) {
			Health += 20;
		}
	}

	static bool at2 = false;

	if (isInHitbox(translateX + translateFusionWaveX, translateZ + translateFusionWaveZ, EnemyPosX2, EnemyPosZ2, 20) == true and at2 == false and allowAttack == false and EnemyHealth2 > 0 and allowFusionWaveTransverse == true) {
		EnemyHealth2 -= 3;
		if (allowFusionWaveTransverse == true) {
			Health += 20;
		}
		at2 = true;
	}
	else {
		at2 = false;
	}





	if (isInHitbox(translateX + ShockwavedirectionX, translateZ + ShockwavedirectionZ, EnemyPosX3, EnemyPosZ3, 30) == true and air == false and ground == true and EnemyHealth3 > 0 and allowAttack == false and allowJumpAttack == true and translatebodyY_Jump <= 0 and ShockwaveScale <= 1) {
		EnemyHealth3 -= 20;
		if (allowUltimateAttack == true) {
			Health += 10;
		}
	}

	if (isInHitbox(translateX + ShockwavedirectionX, translateZ + ShockwavedirectionZ, EnemyPosX3, EnemyPosZ3, 20) == true and EnemyHealth3 > 0 and allowAttack == true and allowJumpAttack == false and RotateRightArm_SwingSword >= 50) {
		EnemyHealth3 -= 10;
		if (allowUltimateAttack == true) {
			Health += 20;
		}
	}

	if (isInHitbox(translateX, translateZ, EnemyPosX3, EnemyPosZ3, 30) == true and EnemyHealth3 > 0 and allowAttack == false and allowJumpAttack == false and allowSpinattack == true and rotateBodySpinAttack >= 455) {
		EnemyHealth3 -= 20;
		if (allowUltimateAttack == true) {
			Health += 20;
		}
	}

	static bool at3 = false;

	if (isInHitbox(translateX + translateFusionWaveX, translateZ + translateFusionWaveZ, EnemyPosX3, EnemyPosZ3, 20) == true and at3 == false and allowAttack == false and EnemyHealth3 > 0 and allowFusionWaveTransverse == true) {
		EnemyHealth3 -= 3;
		if (allowFusionWaveTransverse == true) {
			Health += 20;
		}
		at3 = true;
	}
	else {
		at3 = false;
	}



	if (Health > 100) {
		Health = 100;
	}

	modelStack.PushMatrix();
	modelStack.Translate(0, -2.8, -100);
	modelStack.Rotate(-90, 1, 0, 0);
	modelStack.Scale(5, 5, 5);
	RenderMesh(meshList[GEO_PAPER], true);
	modelStack.PopMatrix();


	if (EnemyHealth > 80) {
		modelStack.PushMatrix();
		{
			modelStack.Translate(EnemyPosX, 20, EnemyPosZ);
			if (look.x > 0) {
				modelStack.Rotate(angleHori, 0, 1, 0);
			}
			else {
				modelStack.Rotate(-angleHori, 0, 1, 0);
			}

			modelStack.Rotate(-angleVert, 1, 0, 0);
			{
				modelStack.PushMatrix();
				modelStack.Translate(-6.8, 0, 0.1);
				modelStack.Rotate(0, 0, 1, 0);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();

				modelStack.PushMatrix();
				modelStack.Translate(-3.8, 0, 0.1);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();

				modelStack.PushMatrix();
				modelStack.Translate(0, 0, 0.1);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();

				modelStack.PushMatrix();
				modelStack.Translate(3.8, 0, 0.1);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();

				modelStack.PushMatrix();
				modelStack.Translate(6.8, 0, 0.1);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();
			}
			modelStack.Scale(19, 3.2, 1);
			RenderMesh(meshList[GEO_HEALTHBACKGROUND], false);
		}
		modelStack.PopMatrix();
	}

	else if (EnemyHealth > 60 and EnemyHealth <= 80) {
		modelStack.PushMatrix();
		{
			modelStack.Translate(EnemyPosX, 20, EnemyPosZ);
			if (look.x > 0) {
				modelStack.Rotate(angleHori, 0, 1, 0);
			}
			else {
				modelStack.Rotate(-angleHori, 0, 1, 0);
			}
			modelStack.Rotate(-angleVert, 1, 0, 0);
			{
				modelStack.PushMatrix();
				modelStack.Translate(-6.8, 0, 0.1);
				modelStack.Rotate(0, 0, 1, 0);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();

				modelStack.PushMatrix();
				modelStack.Translate(-3.8, 0, 0.1);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();

				modelStack.PushMatrix();
				modelStack.Translate(0, 0, 0.1);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();

				modelStack.PushMatrix();
				modelStack.Translate(3.8, 0, 0.1);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();
			}
			modelStack.Scale(19, 3.2, 1);
			RenderMesh(meshList[GEO_HEALTHBACKGROUND], false);
		}
		modelStack.PopMatrix();
	}
	else if (EnemyHealth > 40 and EnemyHealth <= 60) {
		modelStack.PushMatrix();
		{
			modelStack.Translate(EnemyPosX, 20, EnemyPosZ);
			if (look.x > 0) {
				modelStack.Rotate(angleHori, 0, 1, 0);
			}
			else {
				modelStack.Rotate(-angleHori, 0, 1, 0);
			}
			modelStack.Rotate(-angleVert, 1, 0, 0);
			{
				modelStack.PushMatrix();
				modelStack.Translate(-6.8, 0, 0.1);
				modelStack.Rotate(0, 0, 1, 0);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();

				modelStack.PushMatrix();
				modelStack.Translate(-3.8, 0, 0.1);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();

				modelStack.PushMatrix();
				modelStack.Translate(0, 0, 0.1);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();
			}
			modelStack.Scale(19, 3.2, 1);
			RenderMesh(meshList[GEO_HEALTHBACKGROUND], false);
		}
		modelStack.PopMatrix();
	}
	else if (EnemyHealth > 20 and EnemyHealth <= 40) {
		modelStack.PushMatrix();
		{
			modelStack.Translate(EnemyPosX, 20, EnemyPosZ);
			if (look.x > 0) {
				modelStack.Rotate(angleHori, 0, 1, 0);
			}
			else {
				modelStack.Rotate(-angleHori, 0, 1, 0);
			}
			modelStack.Rotate(-angleVert, 1, 0, 0);
			{
				modelStack.PushMatrix();
				modelStack.Translate(-6.8, 0, 0.1);
				modelStack.Rotate(0, 0, 1, 0);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();

				modelStack.PushMatrix();
				modelStack.Translate(-3.8, 0, 0.1);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();
			}
			modelStack.Scale(19, 3.2, 1);
			RenderMesh(meshList[GEO_HEALTHBACKGROUND], false);
		}
		modelStack.PopMatrix();
	}
	else if (EnemyHealth > 0 and EnemyHealth <= 20) {
		modelStack.PushMatrix();
		{
			modelStack.Translate(EnemyPosX, 20, EnemyPosZ);
			if (look.x > 0) {
				modelStack.Rotate(angleHori, 0, 1, 0);
			}
			else {
				modelStack.Rotate(-angleHori, 0, 1, 0);
			}
			modelStack.Rotate(-angleVert, 1, 0, 0);
			{
				modelStack.PushMatrix();
				modelStack.Translate(-6.8, 0, 0.1);
				modelStack.Rotate(0, 0, 1, 0);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();
			}
			modelStack.Scale(19, 3.2, 1);
			RenderMesh(meshList[GEO_HEALTHBACKGROUND], false);
		}
		modelStack.PopMatrix();
	}
	else {
		allowEnemyMovement = false;
	}



	if (EnemyHealth2 > 80) {
		modelStack.PushMatrix();
		{
			modelStack.Translate(EnemyPosX2, 20, EnemyPosZ2);
			if (look2.x > 0) {
				modelStack.Rotate(angleHori2, 0, 1, 0);
			}
			else {
				modelStack.Rotate(-angleHori2, 0, 1, 0);
			}

			modelStack.Rotate(-angleVert2, 1, 0, 0);
			{
				modelStack.PushMatrix();
				modelStack.Translate(-6.8, 0, 0.1);
				modelStack.Rotate(0, 0, 1, 0);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();

				modelStack.PushMatrix();
				modelStack.Translate(-3.8, 0, 0.1);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();

				modelStack.PushMatrix();
				modelStack.Translate(0, 0, 0.1);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();

				modelStack.PushMatrix();
				modelStack.Translate(3.8, 0, 0.1);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();

				modelStack.PushMatrix();
				modelStack.Translate(6.8, 0, 0.1);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();
			}
			modelStack.Scale(19, 3.2, 1);
			RenderMesh(meshList[GEO_HEALTHBACKGROUND], false);
		}
		modelStack.PopMatrix();
	}

	else if (EnemyHealth2 > 60 and EnemyHealth2 <= 80) {
		modelStack.PushMatrix();
		{
			modelStack.Translate(EnemyPosX2, 20, EnemyPosZ2);
			if (look2.x > 0) {
				modelStack.Rotate(angleHori2, 0, 1, 0);
			}
			else {
				modelStack.Rotate(-angleHori2, 0, 1, 0);
			}
			modelStack.Rotate(-angleVert2, 1, 0, 0);
			{
				modelStack.PushMatrix();
				modelStack.Translate(-6.8, 0, 0.1);
				modelStack.Rotate(0, 0, 1, 0);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();

				modelStack.PushMatrix();
				modelStack.Translate(-3.8, 0, 0.1);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();

				modelStack.PushMatrix();
				modelStack.Translate(0, 0, 0.1);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();

				modelStack.PushMatrix();
				modelStack.Translate(3.8, 0, 0.1);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();
			}
			modelStack.Scale(19, 3.2, 1);
			RenderMesh(meshList[GEO_HEALTHBACKGROUND], false);
		}
		modelStack.PopMatrix();
	}
	else if (EnemyHealth2 > 40 and EnemyHealth2 <= 60) {
		modelStack.PushMatrix();
		{
			modelStack.Translate(EnemyPosX2, 20, EnemyPosZ2);
			if (look2.x > 0) {
				modelStack.Rotate(angleHori2, 0, 1, 0);
			}
			else {
				modelStack.Rotate(-angleHori2, 0, 1, 0);
			}
			modelStack.Rotate(-angleVert2, 1, 0, 0);
			{
				modelStack.PushMatrix();
				modelStack.Translate(-6.8, 0, 0.1);
				modelStack.Rotate(0, 0, 1, 0);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();

				modelStack.PushMatrix();
				modelStack.Translate(-3.8, 0, 0.1);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();

				modelStack.PushMatrix();
				modelStack.Translate(0, 0, 0.1);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();
			}
			modelStack.Scale(19, 3.2, 1);
			RenderMesh(meshList[GEO_HEALTHBACKGROUND], false);
		}
		modelStack.PopMatrix();
	}
	else if (EnemyHealth2 > 20 and EnemyHealth2 <= 40) {
		modelStack.PushMatrix();
		{
			modelStack.Translate(EnemyPosX2, 20, EnemyPosZ2);
			if (look2.x > 0) {
				modelStack.Rotate(angleHori2, 0, 1, 0);
			}
			else {
				modelStack.Rotate(-angleHori2, 0, 1, 0);
			}
			modelStack.Rotate(-angleVert2, 1, 0, 0);
			{
				modelStack.PushMatrix();
				modelStack.Translate(-6.8, 0, 0.1);
				modelStack.Rotate(0, 0, 1, 0);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();

				modelStack.PushMatrix();
				modelStack.Translate(-3.8, 0, 0.1);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();
			}
			modelStack.Scale(19, 3.2, 1);
			RenderMesh(meshList[GEO_HEALTHBACKGROUND], false);
		}
		modelStack.PopMatrix();
	}
	else if (EnemyHealth2 > 0 and EnemyHealth2 <= 20) {
		modelStack.PushMatrix();
		{
			modelStack.Translate(EnemyPosX2, 20, EnemyPosZ2);
			if (look2.x > 0) {
				modelStack.Rotate(angleHori2, 0, 1, 0);
			}
			else {
				modelStack.Rotate(-angleHori2, 0, 1, 0);
			}
			modelStack.Rotate(-angleVert2, 1, 0, 0);
			{
				modelStack.PushMatrix();
				modelStack.Translate(-6.8, 0, 0.1);
				modelStack.Rotate(0, 0, 1, 0);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();
			}
			modelStack.Scale(19, 3.2, 1);
			RenderMesh(meshList[GEO_HEALTHBACKGROUND], false);
		}
		modelStack.PopMatrix();
	}
	else {
		allowEnemyMovement2 = false;
	}



	if (EnemyHealth3 > 80) {
		modelStack.PushMatrix();
		{
			modelStack.Translate(EnemyPosX3, 20, EnemyPosZ3);
			if (look3.x > 0) {
				modelStack.Rotate(angleHori3, 0, 1, 0);
			}
			else {
				modelStack.Rotate(-angleHori3, 0, 1, 0);
			}

			modelStack.Rotate(-angleVert3, 1, 0, 0);
			{
				modelStack.PushMatrix();
				modelStack.Translate(-6.8, 0, 0.1);
				modelStack.Rotate(0, 0, 1, 0);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();

				modelStack.PushMatrix();
				modelStack.Translate(-3.8, 0, 0.1);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();

				modelStack.PushMatrix();
				modelStack.Translate(0, 0, 0.1);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();

				modelStack.PushMatrix();
				modelStack.Translate(3.8, 0, 0.1);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();

				modelStack.PushMatrix();
				modelStack.Translate(6.8, 0, 0.1);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();
			}
			modelStack.Scale(19, 3.2, 1);
			RenderMesh(meshList[GEO_HEALTHBACKGROUND], false);
		}
		modelStack.PopMatrix();
	}

	else if (EnemyHealth3 > 60 and EnemyHealth3 <= 80) {
		modelStack.PushMatrix();
		{
			modelStack.Translate(EnemyPosX3, 20, EnemyPosZ3);
			if (look3.x > 0) {
				modelStack.Rotate(angleHori3, 0, 1, 0);
			}
			else {
				modelStack.Rotate(-angleHori3, 0, 1, 0);
			}
			modelStack.Rotate(-angleVert3, 1, 0, 0);
			{
				modelStack.PushMatrix();
				modelStack.Translate(-6.8, 0, 0.1);
				modelStack.Rotate(0, 0, 1, 0);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();

				modelStack.PushMatrix();
				modelStack.Translate(-3.8, 0, 0.1);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();

				modelStack.PushMatrix();
				modelStack.Translate(0, 0, 0.1);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();

				modelStack.PushMatrix();
				modelStack.Translate(3.8, 0, 0.1);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();
			}
			modelStack.Scale(19, 3.2, 1);
			RenderMesh(meshList[GEO_HEALTHBACKGROUND], false);
		}
		modelStack.PopMatrix();
	}
	else if (EnemyHealth3 > 40 and EnemyHealth3 <= 60) {
		modelStack.PushMatrix();
		{
			modelStack.Translate(EnemyPosX3, 20, EnemyPosZ3);
			if (look3.x > 0) {
				modelStack.Rotate(angleHori3, 0, 1, 0);
			}
			else {
				modelStack.Rotate(-angleHori3, 0, 1, 0);
			}
			modelStack.Rotate(-angleVert3, 1, 0, 0);
			{
				modelStack.PushMatrix();
				modelStack.Translate(-6.8, 0, 0.1);
				modelStack.Rotate(0, 0, 1, 0);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();

				modelStack.PushMatrix();
				modelStack.Translate(-3.8, 0, 0.1);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();

				modelStack.PushMatrix();
				modelStack.Translate(0, 0, 0.1);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();
			}
			modelStack.Scale(19, 3.2, 1);
			RenderMesh(meshList[GEO_HEALTHBACKGROUND], false);
		}
		modelStack.PopMatrix();
	}
	else if (EnemyHealth3 > 20 and EnemyHealth3 <= 40) {
		modelStack.PushMatrix();
		{
			modelStack.Translate(EnemyPosX3, 20, EnemyPosZ3);
			if (look2.x > 0) {
				modelStack.Rotate(angleHori3, 0, 1, 0);
			}
			else {
				modelStack.Rotate(-angleHori3, 0, 1, 0);
			}
			modelStack.Rotate(-angleVert3, 1, 0, 0);
			{
				modelStack.PushMatrix();
				modelStack.Translate(-6.8, 0, 0.1);
				modelStack.Rotate(0, 0, 1, 0);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();

				modelStack.PushMatrix();
				modelStack.Translate(-3.8, 0, 0.1);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();
			}
			modelStack.Scale(19, 3.2, 1);
			RenderMesh(meshList[GEO_HEALTHBACKGROUND], false);
		}
		modelStack.PopMatrix();
	}
	else if (EnemyHealth3 > 0 and EnemyHealth3 <= 20) {
		modelStack.PushMatrix();
		{
			modelStack.Translate(EnemyPosX3, 20, EnemyPosZ3);
			if (look2.x > 0) {
				modelStack.Rotate(angleHori3, 0, 1, 0);
			}
			else {
				modelStack.Rotate(-angleHori3, 0, 1, 0);
			}
			modelStack.Rotate(-angleVert3, 1, 0, 0);
			{
				modelStack.PushMatrix();
				modelStack.Translate(-6.8, 0, 0.1);
				modelStack.Rotate(0, 0, 1, 0);
				modelStack.Scale(4, 2.5, 0);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
				modelStack.PopMatrix();
			}
			modelStack.Scale(19, 3.2, 1);
			RenderMesh(meshList[GEO_HEALTHBACKGROUND], false);
		}
		modelStack.PopMatrix();
	}
	else {
		allowEnemyMovement3 = false;
	}




	if (isAlive == true and timer_gameover < 1.5 and DialogueBoxOpen == false) {
		RenderImageOnScreen(meshList[GEO_ALUSKILLS], Color(1, 1, 1), 0, 29, 8, 0, 40, 8);
		RenderTextOnScreen(meshList[GEO_TEXT], "X               Z                E", Color(1, 1, 1), 3.5, 29.5, 1);

		if ((int)timer_Skill1 > 0) {
			RenderTextOnScreen(meshList[GEO_TEXT], to_string((int)timer_Skill1), Color(1, 1, 1), 4.5, 29.3, 5.7);
		}
		if ((int)timer_Skill2 > 0) {
			RenderTextOnScreen(meshList[GEO_TEXT], to_string((int)timer_Skill2), Color(1, 1, 1), 4.5, 38.8, 5.7);
		}
		if ((int)timer_Skill3 > 0) {
			if ((int)timer_Skill3 < 10) {
				RenderTextOnScreen(meshList[GEO_TEXT], to_string((int)timer_Skill3), Color(1, 1, 1), 4.5, 48.3, 5.7);
			}
			else {
				RenderTextOnScreen(meshList[GEO_TEXT], to_string((int)timer_Skill3), Color(1, 1, 1), 4.5, 47.8, 5.7);
			}
		}
	}


	if (DialogueBoxOpen == true and interactableObject(translateX, translateZ, 0, -100, 10, 10) == true) {
		RenderImageOnScreen(meshList[GEO_DIALOGUEUI], Color(1, 1, 1), 0, 60, 15, 60, 40, 10);
		RenderTextOnScreen(meshList[GEO_TEXT], "Missing Person Poster", Color(1, 1, 1), 2.7, 20.2, 13.5);
		RenderTextOnScreen(meshList[GEO_TEXT], it, Color(0, 0, 0), 3, 13.5, 10);
		RenderTextOnScreen(meshList[GEO_TEXT], it2, Color(0, 0, 0), 3, 13, 7.5);
		RenderTextOnScreen(meshList[GEO_TEXT], it3, Color(0, 0, 0), 3, 13.5, 5);
		RenderTextOnScreen(meshList[GEO_TEXT], "Press 'F' to speed up", Color(1, 1, 1), 3, 30.2, 0.8);
	}


	std::ostringstream ss;
	ss.str("");
	ss << "FPS: " << fps;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 0);
	RenderImageOnScreen(meshList[GEO_HEALTHBACKGROUND], Color(1, 1, 1), 0, 19, 3.2, 0, 14, 55);
	RenderImageOnScreen(meshList[GEO_HEART], Color(1, 1, 1), 0, 3.2, 3.2, 0, 2.5, 55);

	if (Health > 80) {
		RenderImageOnScreen(meshList[GEO_HEALTHBAR], Color(1, 1, 1), 0, 4, 2.5, 0, 6.8, 55);
		RenderImageOnScreen(meshList[GEO_HEALTHBAR], Color(1, 1, 1), 0, 4, 2.5, 0, 9.8, 55);
		RenderImageOnScreen(meshList[GEO_HEALTHBAR], Color(1, 1, 1), 0, 4, 2.5, 0, 13.8, 55);
		RenderImageOnScreen(meshList[GEO_HEALTHBAR], Color(1, 1, 1), 0, 4, 2.5, 0, 17.8, 55);
		RenderImageOnScreen(meshList[GEO_HEALTHBAR], Color(1, 1, 1), 0, 4, 2.5, 0, 21.2, 55);
	}
	else if (Health > 60 and Health <= 80) {
		RenderImageOnScreen(meshList[GEO_HEALTHBAR], Color(1, 1, 1), 0, 4, 2.5, 0, 6.8, 55);
		RenderImageOnScreen(meshList[GEO_HEALTHBAR], Color(1, 1, 1), 0, 4, 2.5, 0, 9.8, 55);
		RenderImageOnScreen(meshList[GEO_HEALTHBAR], Color(1, 1, 1), 0, 4, 2.5, 0, 13.8, 55);
		RenderImageOnScreen(meshList[GEO_HEALTHBAR], Color(1, 1, 1), 0, 4, 2.5, 0, 17.8, 55);
	}
	else if (Health > 40 and Health <= 60) {
		RenderImageOnScreen(meshList[GEO_HEALTHBAR], Color(1, 1, 1), 0, 4, 2.5, 0, 6.8, 55);
		RenderImageOnScreen(meshList[GEO_HEALTHBAR], Color(1, 1, 1), 0, 4, 2.5, 0, 9.8, 55);
		RenderImageOnScreen(meshList[GEO_HEALTHBAR], Color(1, 1, 1), 0, 4, 2.5, 0, 13.8, 55);
	}
	else if (Health > 20 and Health <= 40) {
		RenderImageOnScreen(meshList[GEO_HEALTHBAR], Color(1, 1, 1), 0, 4, 2.5, 0, 6.8, 55);
		RenderImageOnScreen(meshList[GEO_HEALTHBAR], Color(1, 1, 1), 0, 4, 2.5, 0, 9.8, 55);
	}
	else if (Health > 0 and Health <= 20) {
		RenderImageOnScreen(meshList[GEO_HEALTHBAR], Color(1, 1, 1), 0, 4, 2.5, 0, 6.8, 55);
	}




	if (Health > 0) {
		if ((int)sec < 10) {
			RenderTextOnScreen(meshList[GEO_TEXT], to_string((int)min) + ":0" + to_string((int)sec), Color(1, 1, 1), 4, 38, 52);
		}
		else {
			RenderTextOnScreen(meshList[GEO_TEXT], to_string((int)min) + ":" + to_string((int)sec), Color(1, 1, 1), 4, 38, 52);
		}
	}


	if (interactableObject(translateX, translateZ, 0, -100, 10, 10) == true) { // Missing person poster
		if (DialogueBoxOpen == false) {
			RenderTextOnScreen(meshList[GEO_TEXT], "Press 'F' to read", Color(1, 1, 1), 3, 50, 30);
		}
		if (Application::IsKeyPressed('F') and DialogueBoxOpen == false and buttonsincelastpress > 0.2) {
			DialogueBoxOpen = true;
			buttonsincelastpress = 0;
		}
		buttonsincelastpress += 0.01;
	}
	else if (interactableObject(translateX, translateZ, -60, 30, 15, 15) == true) { // Lamp post
		if (DialogueBoxOpen == false and light[0].power == 0) {
			RenderTextOnScreen(meshList[GEO_TEXT], "Press 'F' to on", Color(1, 1, 1), 3, 50, 30);
		}
		else if (DialogueBoxOpen == false and light[0].power == 1) {
			RenderTextOnScreen(meshList[GEO_TEXT], "Press 'F' to off", Color(1, 1, 1), 3, 50, 30);
		}
		if (Application::IsKeyPressed('F') and buttonsincelastpress > 0.1 and light[0].power == 1) {
			light[0].power = 0;
			glUniform1f(m_parameters[U_LIGHT0_POWER], light[0].power);
			buttonsincelastpress = 0;
		}
		else if (Application::IsKeyPressed('F') and buttonsincelastpress > 0.1 and light[0].power == 0) {
			light[0].power = 1;
			glUniform1f(m_parameters[U_LIGHT0_POWER], light[0].power);
			buttonsincelastpress = 0;
		}
		buttonsincelastpress += 0.01;
	}
	else if (interactableObject(translateX, translateZ, 0, 0, 3, 3) == true and GameDialogueTeleporter == true) {

	}
	//tent
	else if (interactableObject(translateX, translateZ, -100, 100, 30, 30) == true) {
		if (isOpenTent == true) {
			RenderTextOnScreen(meshList[GEO_TEXT], "Press 'F' to close", Color(1, 1, 1), 3, 50, 30);
		}
		else {
			RenderTextOnScreen(meshList[GEO_TEXT], "Press 'F' to open", Color(1, 1, 1), 3, 50, 30);
		}

		if (Application::IsKeyPressed('F') and buttonsincelastpress > 0.2 and isOpenTent == true) {
			buttonsincelastpress = 0;
			isOpenTent = false;
		}
		else if (Application::IsKeyPressed('F') and buttonsincelastpress > 0.2 and isOpenTent == false) {
			buttonsincelastpress = 0;
			isOpenTent = true;
		}
		buttonsincelastpress += 0.01;
	}

	for (int i = 0; i < 10; i++) {
		if (Diamonds[i] != nullptr) {
			if (interactableObject(translateX, translateZ, Diamonds[i]->GetX(), Diamonds[i]->GetZ(), 20, 20) == true) {
				RenderTextOnScreen(meshList[GEO_TEXT], "Press 'F' to collect", Color(1, 1, 1), 3, 50, 30);
				if (Application::IsKeyPressed('F') and buttonsincelastpress > 0.2) {
					buttonsincelastpress = 0;
					Diamond++;
					Diamonds[i] = nullptr;
					break;
				}
				buttonsincelastpress += 0.01;
			}
		}
	}

	RenderTextOnScreen(meshList[GEO_TEXT], "$" + to_string(Application::Cash), Color(0, 0.9, 0), 3.3, 5.5, 48.9);
	RenderImageOnScreen(meshList[GEO_COINS], Color(1, 1, 1), 0, 5, 5, 0, 2.5, 50.5);
	RenderImageOnScreen(meshList[GEO_DIAMONDIMAGE], Color(1, 1, 1), 0, 3.5, 3.3, 0, 2.5, 46);
	RenderTextOnScreen(meshList[GEO_TEXT], to_string(Diamond) + "/10", Color(1, 1, 1), 3.3, 5, 44.4);

	if (isAlive == false and timer_gameover > 1.5) {
		RenderImageOnScreen(meshList[GEO_GAMEOVER], Color(1, 1, 1), 0, 80, 60, 0, 40, 30);
		RenderImageOnScreen(meshList[GEO_GAMEOVERTEXT], Color(1, 1, 1), 0, 60, 10, 0, 40, 50);
		RenderTextOnScreen(meshList[GEO_TEXT], "-$100 Cash", Color(1, 1, 1), 6, 31, 39);
		if (timer_Retry > 1) {
			RenderImageOnScreen(meshList[GEO_RETRY], Color(1, 1, 1), 0, 21, 9, 0, 40, 10);
		}
	}


	if (isAlive == true and timer_gameover > 1.5 and Diamond >= 10) {
		RenderImageOnScreen(meshList[GEO_VICTORY], Color(1, 1, 1), 0, 80, 60, 0, 40, 30);
		RenderImageOnScreen(meshList[GEO_VICTORYTEXT], Color(1, 1, 1), 0, 60, 10, 0, 40, 50);
		RenderTextOnScreen(meshList[GEO_TEXT], "+$250 Cash", Color(1, 1, 1), 6, 31, 39);
		if (timer_Retry > 1) {
			RenderImageOnScreen(meshList[GEO_RETRY], Color(1, 1, 1), 0, 21, 9, 0, 40, 10);
		}
	}

	//modelStack.PushMatrix();
	//modelStack.Translate(light[0].position.x, light[0].position.y, light[0].position.z);
	//RenderMesh(meshList[GEO_LIGHTBALL], false);
	//modelStack.PopMatrix();

	//modelStack.PushMatrix();
	//modelStack.Translate(light[1].position.x, light[1].position.y, light[1].position.z);
	//RenderMesh(meshList[GEO_LIGHTBALL], false);
	//modelStack.PopMatrix();
}

void Sp2_Minigame::RenderSkybox()
{
	modelStack.PushMatrix();
	{
		modelStack.Translate(camera2.position.x, camera2.position.y, camera2.position.z);
		{
			modelStack.PushMatrix();
			modelStack.Rotate(rotateSkyBox, 0, 1, 0);
			modelStack.Translate(0, 0, 500);
			modelStack.Rotate(180, 0, 1, 0);
			modelStack.Scale(1000, 1000, 1000);
			RenderMesh(meshList[GEO_FRONT], false);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Rotate(rotateSkyBox, 0, 1, 0);
			modelStack.Translate(500, 0, 0);
			modelStack.Rotate(-90, 0, 1, 0);
			modelStack.Scale(1000, 1000, 1000);
			RenderMesh(meshList[GEO_LEFT], false);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Rotate(rotateSkyBox, 0, 1, 0);
			modelStack.Translate(-500, 0, 0);
			modelStack.Rotate(90, 0, 1, 0);
			modelStack.Scale(1000, 1000, 1000);
			RenderMesh(meshList[GEO_RIGHT], false);
			modelStack.PopMatrix();


			modelStack.PushMatrix();
			modelStack.Rotate(rotateSkyBox, 0, 1, 0);
			modelStack.Translate(0, 500, 0);
			modelStack.Rotate(90, 1, 0, 0);
			modelStack.Rotate(-180, 0, 0, 1);
			modelStack.Scale(1000, 1000, 1000);
			RenderMesh(meshList[GEO_TOP], false);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Rotate(rotateSkyBox, 0, 1, 0);
			modelStack.Translate(0, -500, 0);
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(1000, 1000, 1000);
			RenderMesh(meshList[GEO_BOTTOM], false);
			modelStack.PopMatrix();


			modelStack.PushMatrix();
			modelStack.Rotate(rotateSkyBox, 0, 1, 0);
			modelStack.Translate(0, 0, -500);
			modelStack.Scale(1000, 1000, 1000);
			RenderMesh(meshList[GEO_BACK], false);
			modelStack.PopMatrix();
		}
		modelStack.Rotate(0, 0, 0, 1);
		modelStack.Scale(1, 1, 1);
	}
	modelStack.PopMatrix();
}

void Sp2_Minigame::Exit()
{
	// Cleanup VBO here
	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		if (meshList[i])
		{
			delete meshList[i];
		}
	}
	for (int i = 0; i < 10; i++) {
		if (Diamonds[i] != nullptr) {
			delete Diamonds[i];
			Diamonds[i] = nullptr;
		}
	}
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}
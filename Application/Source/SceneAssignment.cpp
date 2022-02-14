#include "SceneAssignment.h"
#include "GL\glew.h"

#include "shader.hpp"
#include "Mtx44.h"
#include "Utility.h"

#include "Application.h"
#include "MeshBuilder.h"

SceneAssignment::SceneAssignment()
{
}

SceneAssignment::~SceneAssignment()
{
}

void SceneAssignment::Init()
{	
	// Set background color to dark blue
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	//Enable depth buffer and depth testing
	glEnable(GL_DEPTH_TEST);

	//Enable back face culling
	glEnable(GL_CULL_FACE);

	//Default to fill mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Generate a default VAO for now
	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);
	
	m_programID = LoadShaders("Shader//Shading.vertexshader", "Shader//LightSource.fragmentshader");
	m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");
	m_parameters[U_MODELVIEW] = glGetUniformLocation(m_programID, "MV");
	m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(m_programID, "MV_inverse_transpose");
	m_parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(m_programID, "material.kAmbient");
	m_parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(m_programID, "material.kDiffuse");
	m_parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(m_programID, "material.kSpecular");
	m_parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(m_programID, "material.kShininess");
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
	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
	m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");

	glUseProgram(m_programID);

	light[0].type = Light::LIGHT_DIRECTIONAL;
	light[0].position.Set(0, 40, 0);
	light[0].color.Set(1, 1, 1);
	light[0].power = 0.5;
	light[0].kC = 1.f;
	light[0].kL = 0.1f;
	light[0].kQ = 0.01f;
	light[0].cosCutoff = cos(Math::DegreeToRadian(45));
	light[0].cosInner = cos(Math::DegreeToRadian(30));
	light[0].exponent = 3.f;
	light[0].spotDirection.Set(0.f, 1.f, 0.f);

	glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
	glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &light[0].color.r);
	glUniform1f(m_parameters[U_LIGHT0_POWER], light[0].power);
	glUniform1f(m_parameters[U_LIGHT0_KC], light[0].kC);
	glUniform1f(m_parameters[U_LIGHT0_KL], light[0].kL);
	glUniform1f(m_parameters[U_LIGHT0_KQ], light[0].kQ);
	glUniform1f(m_parameters[U_LIGHT0_COSCUTOFF], light[0].cosCutoff);
	glUniform1f(m_parameters[U_LIGHT0_COSINNER], light[0].cosInner);
	glUniform1f(m_parameters[U_LIGHT0_EXPONENT], light[0].exponent);


	glUniform1i(m_parameters[U_NUMLIGHTS], 1);
	
	//variable to rotate geometry
	rotateAngle = 0;

	//Initialize camera settings
	camera.Init(Vector3(200, 200, 200), Vector3(0, 0, 0), Vector3(0, 1, 0));

	// Init VBO
	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		meshList[i] = nullptr;
	}
	meshList[GEO_AXES] = MeshBuilder::GenerateAxes("reference", 1000, 1000, 1000);
	meshList[GEO_QUAD] = MeshBuilder::GenerateQuad("quad", Color(1, 1, 0), 1.f);
	meshList[GEO_QUAD]->material.kAmbient.Set(0.3f, 0.3f, 0.f);
	meshList[GEO_QUAD]->material.kDiffuse.Set(0.5f, 0.5f, 0.f);
	meshList[GEO_QUAD]->material.kSpecular.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_QUAD]->material.kShininess = 1.f;

	meshList[GEO_MUDKIP_SPHERE] = MeshBuilder::GenerateSphere("Mudkip Sphere", Color(0.f, 158.0 / 255.0f, 197.0 / 255.0f), 20, 20, 3);
	meshList[GEO_MUDKIP_SPHERE]->material.kAmbient.Set(0.f, 108.0/255.0f, 147.0/255.0f);
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

	meshList[GEO_ORAN_BERRY_SPHERE] = MeshBuilder::GenerateSphere("oran berry sphere", Color(34.0 / 255.0f, 110.0 / 255.0f, 224.0 / 255.0f), 20, 20, 10);
	meshList[GEO_ORAN_BERRY_SPHERE]->material.kAmbient.Set(34.0 / 255.0f, 110.0 / 255.0f, 224.0 / 255.0f);
	meshList[GEO_ORAN_BERRY_SPHERE]->material.kDiffuse.Set(34.0 / 255.0f, 110.0 / 255.0f, 224.0 / 255.0f);
	meshList[GEO_ORAN_BERRY_SPHERE]->material.kSpecular.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_ORAN_BERRY_SPHERE]->material.kShininess = 0.5f;

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

	meshList[GEO_TRAINING_DUMMY] = MeshBuilder::GenerateSphere("training dummy", Color(117.0 / 255.0f, 51.0 / 255.0f, 0), 20, 20, 10);
	meshList[GEO_TRAINING_DUMMY]->material.kAmbient.Set(67.0 / 255.0f, 1.0 / 255.0f, 0);
	meshList[GEO_TRAINING_DUMMY]->material.kDiffuse.Set(107.0 / 255.0f, 41.0 / 255.0f, 0);
	meshList[GEO_TRAINING_DUMMY]->material.kSpecular.Set(0.1f, 0.1f, 0.1f);
	meshList[GEO_TRAINING_DUMMY]->material.kShininess = 0.5f;

	meshList[GEO_WATER] = MeshBuilder::GenerateSphere("water ball", Color(0, 0, 1), 20, 20, 2);
	meshList[GEO_WATER]->material.kAmbient.Set(0, 0, 0.5f);
	meshList[GEO_WATER]->material.kDiffuse.Set(0, 0, 0.5f);
	meshList[GEO_WATER]->material.kSpecular.Set(0.1f, 0.1f, 0.1f);
	meshList[GEO_WATER]->material.kShininess = 0.5f;
	
	meshList[GEO_TREETOP] = MeshBuilder::GenerateCone("tree top", Color(0, 1, 0), 20, 5, 5);
	meshList[GEO_TREETOP]->material.kAmbient.Set(0, 0.5f, 0.0f);
	meshList[GEO_TREETOP]->material.kDiffuse.Set(0, 0.5f, 0.0f);
	meshList[GEO_TREETOP]->material.kSpecular.Set(0.1f, 0.1f, 0.1f);
	meshList[GEO_TREETOP]->material.kShininess = 0.5f;

	meshList[GEO_BLACKBALL] = MeshBuilder::GenerateSphere("black ball", Color(1.f, 1.f, 1.f), 20, 20, 10);

	meshList[GEO_LIGHTBALL] = MeshBuilder::GenerateSphere("LightSphere", Color(1, 1, 1), 10, 10, 2);

	Mtx44 projection;
	projection.SetToPerspective(45.f, 4.f / 3.f, 0.1f, 1000.f);
	projectionStack.LoadMatrix(projection);

	//init variables
	rotateAngle = 90;
	rotateAngle2 = 90;
	rotateAngle3 = 0;
	rotateAngle4 = 0;
	rotateAngle5 = 0;
	rotateAngle6 = 0;
	rotateAngle7 = 0;
	rotateAngle8 = 0;
	rotateAngle9 = 0;
	animation = A_NOTHING;
	animation_phase = AA_NOTHING;
	mudkiptranslate[0] = 0;
	mudkiptranslate[1] = 8;
	mudkiptranslate[2] = 0;
	berrytranslate[0] = 0;
	berrytranslate[1] = -10;
	berrytranslate[2] = 0;
	dummyscale = (float)1 / 10000;
	dummytranslation[0] = 20;
	dummytranslation[1] = 30;
	dummytranslation[2] = 20;
	legtranslateY = 0;
	renderaxes = false;
	for (int idx = 0; idx < 24; idx++)
	{
		waterposition[idx][0] = 0;
		waterposition[idx][1] = 0;
		waterposition[idx][2] = 0;
	}
	buttontimer.startTimer();
}

bool SceneAssignment::Update(double dt)
{
	if(Application::IsKeyPressed('1')) //enable back face culling
		glEnable(GL_CULL_FACE);
	if(Application::IsKeyPressed('2')) //disable back face culling
		glDisable(GL_CULL_FACE);
	if(Application::IsKeyPressed('3'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //default fill mode
	if(Application::IsKeyPressed('4'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //wireframe mode

	float LSPEED = 30.f;

	if (Application::IsKeyPressed('I'))
		light[0].position.z -= (float)(LSPEED * dt);
	if (Application::IsKeyPressed('K'))
		light[0].position.z += (float)(LSPEED * dt);
	if (Application::IsKeyPressed('J'))
		light[0].position.x -= (float)(LSPEED * dt);
	if (Application::IsKeyPressed('L'))
		light[0].position.x += (float)(LSPEED * dt);
	if (Application::IsKeyPressed('O'))
		light[0].position.y -= (float)(LSPEED * dt);
	if (Application::IsKeyPressed('P'))
		light[0].position.y += (float)(LSPEED * dt);

	if (Application::IsKeyPressed('5'))
	{
		light[0].type = Light::LIGHT_SPOT;
		glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
	}
	else if (Application::IsKeyPressed('6'))
	{
		//to do: switch light type to DIRECTIONAL and pass the information to shader
		light[0].type = Light::LIGHT_DIRECTIONAL;
		glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
	}
	else if (Application::IsKeyPressed('7'))
	{
		//to do: switch light type to SPOT and pass the information to shader
		light[0].type = Light::LIGHT_POINT;
		glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
	}

	//deal with animation changes
	if (Application::IsKeyPressed(VK_UP) && animation == A_NOTHING) //walk forward
	{
		animation = A_WALK_FORWARD;
		animation_phase = A_WALK_ROTATE_LEFT;
		timer.startTimer();
	}
	else if (Application::IsKeyPressed(VK_DOWN) && animation == A_NOTHING) //walk backward
	{
		animation = A_WALK_BACKWARD;
		animation_phase = A_WALK_ROTATE_RIGHT;
		timer.startTimer();
	}
	else if (Application::IsKeyPressed(VK_LEFT) && animation == A_NOTHING) //walk left
	{
		animation = A_WALK_LEFT;
		animation_phase = A_WALK_ROTATE_FORWARD;
		timer.startTimer();
	}
	else if (Application::IsKeyPressed(VK_RIGHT) && animation == A_NOTHING) //walk right
	{
		animation = A_WALK_RIGHT;
		animation_phase = A_WALK_ROTATE_BACKWARD;
		timer.startTimer();
	}
	else if (Application::IsKeyPressed(VK_BACK) && animation == A_NOTHING) //eat
	{
		animation = A_EATING;
		animation_phase = A_EATING_SETUP;
	}
	else if (Application::IsKeyPressed(VK_RSHIFT) && animation == A_NOTHING) //eat
	{
		animation = A_ATTACK;
		animation_phase = A_ATTACK_SETUP;
	}
	if (Application::IsKeyPressed(VK_RCONTROL) && timesincelastbuttonpress > 0.2)
	{
		if (renderaxes == true) renderaxes = false;
		else renderaxes = true;
		timesincelastbuttonpress = 0;
	}
	if (Application::IsKeyPressed('R')) //reset
	{
		rotateAngle = 90;
		rotateAngle2 = 90;
		rotateAngle3 = 0;
		rotateAngle4 = 0;
		rotateAngle5 = 0;
		rotateAngle6 = 0;
		rotateAngle7 = 0;
		rotateAngle8 = 0;
		rotateAngle9 = 0;
		animation = A_NOTHING;
		animation_phase = AA_NOTHING;
		mudkiptranslate[0] = 0;
		mudkiptranslate[1] = 8;
		mudkiptranslate[2] = 0;
		berrytranslate[0] = 0;
		berrytranslate[1] = -10;
		berrytranslate[2] = 0;
		dummyscale = (float)1 / 10000;
		dummytranslation[0] = 20;
		dummytranslation[1] = 30;
		dummytranslation[2] = 20;
		legtranslateY = 0;
		renderaxes = false;
		for (int idx = 0; idx < 24; idx++)
		{
			waterposition[idx][0] = 0;
			waterposition[idx][1] = 0;
			waterposition[idx][2] = 0;
		}
	}
	if (Application::IsKeyPressed(VK_RETURN))
	{
		camera.Update(dt);
		return false;
	}
	timesincelastbuttonpress += buttontimer.getElapsedTime();

	switch(animation) //deal with animations
	{
	case A_ATTACK:
		switch (animation_phase)
		{
		case A_ATTACK_SETUP:
			//set the position of the training dummy
			dummytranslation[0] = mudkiptranslate[0];
			dummytranslation[1] = mudkiptranslate[1] + 20;
			dummytranslation[2] = mudkiptranslate[2] + 150;

			//set the scale size
			dummyscale = (float)0.05;
			animation_phase = A_ATTACK_DUMMY_GROW;
			break;
		case A_ATTACK_DUMMY_GROW:
			dummyscale += 0.5 * dt;
			if (dummyscale >= 1)
			{
				dummyscale = 1;
				animation_phase = A_ATTACK_TACKLE;
				mudkipdirection = 1;
				timer.getElapsedTime();
			}
			break;
		case A_ATTACK_TACKLE:
			if (mudkipdirection == 1) rotateAngle += 80 * dt;
			else if (mudkipdirection == 0) rotateAngle -= 80 * dt;

			if (rotateAngle > 110) mudkipdirection = 0;
			else if (rotateAngle < 70) mudkipdirection = 1;

			mudkiptranslate[2] += 40 * dt;
			rotateAngle9 = rotateAngle - 90;

			totalelaspedtime += timer.getElapsedTime();
			if ((totalelaspedtime > 2 && rotateAngle > 89 && rotateAngle < 91) || totalelaspedtime > 20) //end animation after 5 secs
			{
				animation_phase = A_ATTACK_JUMP;
				rotateAngle = 90;
				totalelaspedtime = 0;
				rotateAngle9 = rotateAngle - 90;
				tempvaluestore = mudkiptranslate[1];
			}
			break;
		case A_ATTACK_JUMP:
			if (legtranslateY < 1)
			{
				legtranslateY += 5 * dt;
			}
			mudkiptranslate[1] += 10 * dt;
			mudkiptranslate[2] += 40 * dt;
			if (mudkiptranslate[1] - tempvaluestore >= 6)
			{
				animation_phase = A_ATTACK_JUMP_APPROACH;
				tempvaluestore = mudkiptranslate[1];
			}
			break;
		case A_ATTACK_JUMP_APPROACH:
			if (legtranslateY >= -2)
			{
				legtranslateY -= 10 * dt;
			}
			rotateAngle4 += 90 * dt;
			if (rotateAngle4 >= 70)
			{
				rotateAngle4 = 70;
				A_ATTACK_BOUNCEOFF;
			}
			mudkiptranslate[1] += 10 * dt;
			mudkiptranslate[2] += 40 * dt;
			if (mudkiptranslate[1] - tempvaluestore >= 6)
			{
				animation_phase = A_ATTACK_BOUNCEOFF;
				tempvaluestore = mudkiptranslate[1];
			}
			break;
		case A_ATTACK_BOUNCEOFF:
			rotateAngle3 += 40 * dt;
			if (rotateAngle3 >= 160)
			{
				rotateAngle3 = 160;
			}
			if (legtranslateY <= 1)
			{
				legtranslateY += 10 * dt;
			}
			rotateAngle7 -= 40 * dt;
			if (rotateAngle7 <= -50)
			{
				rotateAngle7 = -50;
			}
			rotateAngle8 -= 10 * dt;
			rotateAngle6 += 10 * dt;
			rotateAngle5 += 20 * dt;

			mudkiptranslate[1] += 20 * dt;
			mudkiptranslate[2] -= 30 * dt;
			if (mudkiptranslate[1] - tempvaluestore >= 40)
			{
				animation_phase = A_ATTACK_WATERGUN;
			}
			break;
		case A_ATTACK_WATERGUN:
			for (int idx = 0; idx < 23; idx++)
			{
				if (waterposition[idx][0] == 0 && idx == 0)
				{
					waterposition[idx][0] = 1;
					waterposition[idx][1] = 0;
					waterposition[idx][2] = -1;
					waterposition[idx][3] = 1;
				}
				else if (waterposition[idx][0] == 0 && idx != 0)
				{
					waterposition[idx][0] = 1;
					waterposition[idx][1] = waterposition[idx - 1][1];
					waterposition[idx][2] =  -waterposition[idx - 1][2];
					waterposition[idx][3] = 3 - waterposition[idx - 1][3];
					break;
				}
				if (idx == 22)
				{
					animation_phase = A_ATTACK_WATERREMOVE;
				}
			}

			if (temp == 1) rotateAngle5 += 20 * dt;
			else if (temp == 2) rotateAngle5 -= 20 * dt;

			if (rotateAngle5 > 30) temp = 2;
			else if (rotateAngle5 < -30) temp = 1;
			break;



		case A_ATTACK_WATERREMOVE:
			for (int idx = 22; idx >= 0; idx--)
			{
				if (waterposition[idx][0] != 0)
				{
					waterposition[idx][0] = 0;
					break;
				}
				if (idx == 0)
				{
					animation_phase = A_ATTACK_DROP_DOWN;
				}
			}

			if (temp == 1) rotateAngle5 += 20 * dt;
			else if (temp == 2) rotateAngle5 -= 20 * dt;

			if (rotateAngle5 > 30) temp = 2;
			else if (rotateAngle5 < -30) temp = 1;

			break;
		case A_ATTACK_DROP_DOWN:
			//reset tail angle,

			//leg translation
			if (legtranslateY < 3)
			{
				legtranslateY += 10 * dt;
			}

			//reset cheek rotation
			if (temp == 1) rotateAngle5 += 20 * dt;
			else if (temp == 2) rotateAngle5 -= 20 * dt;
			if (rotateAngle5 < 1 && rotateAngle5 > -1)
			{
				rotateAngle5 = 0;
			}

			//reset head rotation
			if (rotateAngle3 > 0)
			{
				rotateAngle3 -= 60 * dt;
			}
			if (rotateAngle3 < 1)
			{
				rotateAngle3 = 0;
			}

			//eye
			if (rotateAngle6 > 0)
			{
				rotateAngle6 -= 20 * dt;
			}
			if (rotateAngle6 < 1)
			{
				rotateAngle6 = 0;
			}

			//dummy
			if (dummyscale > 0.06)
			{
				dummyscale -= 10 * dt;
			}

			//fin
			if (rotateAngle8 < 0)
			{
				rotateAngle8 += 40 * dt;
			}
			if (rotateAngle8 > 1)
			{
				rotateAngle8 = 0;
			}

			//body
			if (rotateAngle4 > 0)
			{
				rotateAngle4 -= 60 * dt;
			}
			if (rotateAngle4 < 1)
			{
				rotateAngle4 = 0;
			}

			//tail
			if (rotateAngle7 < 0)
			{
				rotateAngle7 += 20 * dt;
			}
			if (rotateAngle > -1)
			{
				rotateAngle7 = 0;
			}

			//body translation
			mudkiptranslate[1] -= 30 * dt;
			if (mudkiptranslate[1] < 20)
			{
				mudkiptranslate[1] = 20;
				animation_phase = A_ATTACK_DROP_DOWN_APPROACH;
			}
			break;
		case A_ATTACK_DROP_DOWN_APPROACH:
			mudkiptranslate[1] -= 30 * dt;
			if (mudkiptranslate[1] < 8)
			{
				mudkiptranslate[1] = 8;
				animation_phase = AA_NOTHING;
				animation = A_NOTHING;
			}

			if (dummyscale > 0.06)
			{
				dummyscale -= 10 * dt;
			}

			if (legtranslateY > 0)
			{
				legtranslateY -= 5 * dt;
			}
			break;
		}
		break;
	case A_NOTHING:
		break;
	case A_EATING:

		switch (animation_phase)
		{
		case A_EATING_SETUP:
			//move berry to location
			berrytranslate[2] = mudkiptranslate[2] + 115;
			berrytranslate[1] = mudkiptranslate[1];
			berrytranslate[0] = mudkiptranslate[0];
			berrytranslate[1] = 80;
			animation_phase = A_EATING_BERRY_DROP;
			break;
		case A_EATING_BERRY_DROP:
			berrytranslate[1] -= 48.4 * dt;
			if (berrytranslate[1] < 2.5)
			{
				berrytranslate[1] = 2.5;
				animation_phase = A_EATING_MUDKIP_MOVE_WHOLE;
				mudkipdirection = 1;
				timer.startTimer();
			}
			break;
		case A_EATING_MUDKIP_MOVE_WHOLE:
			if (mudkipdirection == 1) rotateAngle += 40 * dt;
			else if (mudkipdirection == 0) rotateAngle -= 40 * dt;

			if (rotateAngle > 110) mudkipdirection = 0;
			else if (rotateAngle < 70) mudkipdirection = 1;

			mudkiptranslate[2] += 20 * dt;
			rotateAngle9 = rotateAngle - 90;

			totalelaspedtime += timer.getElapsedTime();
			if ((totalelaspedtime > 5 && rotateAngle > 89 && rotateAngle < 91) || totalelaspedtime > 20) //end animation after 5 secs
			{
				animation_phase = A_EATING_MUDKIP_MOVE_HEAD_DOWN;
				rotateAngle = 90;
				totalelaspedtime = 0;
				rotateAngle9 = rotateAngle - 90;
			}
			break;
		case A_EATING_MUDKIP_MOVE_HEAD_DOWN:
			rotateAngle3 += 60 * dt;
			rotateAngle5 += 20 * dt;
			if (rotateAngle3 >= 100) animation_phase = A_EATING_MUDKIP_EAT;
			break;
		case A_EATING_MUDKIP_EAT:
			berrytranslate[1] = -10;
			animation_phase = A_EATING_MUDKIP_MOVE_HEAD_UP;
			break;
		case A_EATING_MUDKIP_MOVE_HEAD_UP:
			rotateAngle3 -= 60 * dt;
			rotateAngle5 -= 20 * dt;
			if (rotateAngle3 <= 0) 
			{
				animation_phase = AA_NOTHING;
				animation = A_NOTHING;
				rotateAngle3 = 0;
				rotateAngle5 = 0;
			}
			break;
		}
		break;
	case A_WALK_FORWARD:
		if (animation_phase == A_WALK_ROTATE_LEFT) rotateAngle += 40 * dt;
		else if (animation_phase == A_WALK_ROTATE_RIGHT) rotateAngle -= 40 * dt;

		if (rotateAngle > 110) animation_phase = A_WALK_ROTATE_RIGHT;
		else if (rotateAngle < 70) animation_phase = A_WALK_ROTATE_LEFT;

		mudkiptranslate[2] += 20 * dt;
		rotateAngle9 = rotateAngle - 90;

		totalelaspedtime += timer.getElapsedTime();
		if ((totalelaspedtime > 5 && rotateAngle > 89 && rotateAngle < 91) || totalelaspedtime > 20) //end animation after 5 secs
		{
			animation_phase = AA_NOTHING;
			animation = A_NOTHING;
			rotateAngle = 90;
			rotateAngle9 = rotateAngle - 90;
			totalelaspedtime = 0;
		}
		break;
	case A_WALK_BACKWARD:
		if (animation_phase == A_WALK_ROTATE_LEFT) rotateAngle += 40 * dt;
		else if (animation_phase == A_WALK_ROTATE_RIGHT) rotateAngle -= 40 * dt;

		if (rotateAngle > 110) animation_phase = A_WALK_ROTATE_RIGHT;
		else if (rotateAngle < 70) animation_phase = A_WALK_ROTATE_LEFT;

		mudkiptranslate[2] -= 20 * dt;

		totalelaspedtime += timer.getElapsedTime();
		if ((totalelaspedtime > 5 && rotateAngle > 89 && rotateAngle < 91) || totalelaspedtime > 20) //end animation after 5 secs
		{
			animation_phase = AA_NOTHING;
			animation = A_NOTHING;
			rotateAngle = 90;
			totalelaspedtime = 0;
		}
		break;
	case A_WALK_LEFT:
		if (animation_phase == A_WALK_ROTATE_FORWARD) rotateAngle2 += 40 * dt;
		else if (animation_phase == A_WALK_ROTATE_BACKWARD) rotateAngle2 -= 40 * dt;

		if (rotateAngle2 > 110) animation_phase = A_WALK_ROTATE_BACKWARD;
		else if (rotateAngle2 < 70) animation_phase = A_WALK_ROTATE_FORWARD;

		mudkiptranslate[0] += 20 * dt;

		totalelaspedtime += timer.getElapsedTime();
		if ((totalelaspedtime > 5 && rotateAngle2 > 89 && rotateAngle2 < 91) || totalelaspedtime > 20) //end animation after 5 secs
		{
			animation_phase = AA_NOTHING;
			animation = A_NOTHING;
			rotateAngle2 = 90;
			totalelaspedtime = 0;
		}
		break;
	case A_WALK_RIGHT:
		if (animation_phase == A_WALK_ROTATE_BACKWARD) rotateAngle2 += 40 * dt;
		else if (animation_phase == A_WALK_ROTATE_FORWARD) rotateAngle2 -= 40 * dt;

		if (rotateAngle2 > 110) animation_phase = A_WALK_ROTATE_FORWARD;
		else if (rotateAngle2 < 70) animation_phase = A_WALK_ROTATE_BACKWARD;

		mudkiptranslate[0] -= 20 * dt;

		totalelaspedtime += timer.getElapsedTime();
		if ((totalelaspedtime > 5 && rotateAngle2 > 89 && rotateAngle2 < 91) || totalelaspedtime > 20) //end animation after 5 secs
		{
			animation_phase = AA_NOTHING;
			animation = A_NOTHING;
			rotateAngle2 = 90;
			totalelaspedtime = 0;
		}
		break;
	}
	camera.Update(dt);
	return false;
}

void SceneAssignment::Render()
{
	// Render VBO here
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//Set view matrix using camera settings
	viewStack.LoadIdentity();
	viewStack.LookAt(
		camera.position.x, camera.position.y, camera.position.z,
		camera.target.x, camera.target.y, camera.target.z,
		camera.up.x, camera.up.y, camera.up.z
					);
	modelStack.LoadIdentity();

	Position lightPosition_cameraspace = viewStack.Top() * light[0].position;
	glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightPosition_cameraspace.x);

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

	/*modelStack.PushMatrix();
	modelStack.Scale(1, 1, 1);
	modelStack.Translate(0, 5, 0);
	RenderMesh(meshList[GEO_MUDKIP_CONE], true);
	modelStack.PopMatrix();*/
	

	//body
	modelStack.PushMatrix();
	modelStack.Translate(mudkiptranslate[0], mudkiptranslate[1], mudkiptranslate[2]);
	modelStack.Rotate(-rotateAngle4, 1, 0, 0);

		//head
		modelStack.PushMatrix();
		modelStack.Scale(2.5, 2.5, 2.5);
		modelStack.Translate(0, 0, 2.5);
		modelStack.Rotate(rotateAngle3, 1, 0, 0);
		modelStack.Translate(0, 3, 0);

			//cheeks of mudkip
			modelStack.PushMatrix();
			modelStack.Translate(-1.1, -0.6, 0.8);
			modelStack.Rotate(-rotateAngle5, 0, 1, 0);

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
			modelStack.Rotate(rotateAngle5, 0, 1, 0);

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
			modelStack.PopMatrix();

			//eye left
			modelStack.PushMatrix();
			modelStack.Rotate(rotateAngle6, 1, 0, 0);
			modelStack.Translate(-1.05, 1.2, 2.27);

				//mudkip eye white spot left
				modelStack.PushMatrix();
				modelStack.Translate(-0.05, 0.1, 0.13);
				modelStack.Scale(0.015, 0.015, 0.015);
				RenderMesh(meshList[GEO_MUDKIP_WHITE_SPOT], false);
				modelStack.PopMatrix();

			modelStack.Rotate(-35, 1, 0, 0);
			modelStack.Scale(0.03, 0.06, 0.03);
			meshList[GEO_BLACKBALL]->material.kAmbient.Set(0.1f, 0.1f, 0.1f);
			meshList[GEO_BLACKBALL]->material.kDiffuse.Set(0.1f, 0.1f, 0.1f);
			meshList[GEO_BLACKBALL]->material.kSpecular.Set(0.3f, 0.3f, 0.3f);
			meshList[GEO_BLACKBALL]->material.kShininess = .5f;
			RenderMesh(meshList[GEO_BLACKBALL], true);
			modelStack.PopMatrix();

			//eye right
			modelStack.PushMatrix();
			modelStack.Rotate(rotateAngle6, 1, 0, 0);
			modelStack.Translate(1.05, 1.2, 2.27);

				//eye white spot right
				modelStack.PushMatrix();
				modelStack.Translate(0.05, 0.1, 0.13);
				modelStack.Scale(0.015, 0.015, 0.015);
				RenderMesh(meshList[GEO_MUDKIP_WHITE_SPOT], false);
				modelStack.PopMatrix();

			modelStack.Rotate(-35, 1, 0, 0);
			modelStack.Scale(0.03, 0.06, 0.03);
			RenderMesh(meshList[GEO_BLACKBALL], true);
			modelStack.PopMatrix();

			//fin on the head
			modelStack.PushMatrix();
			modelStack.Rotate(rotateAngle8, 1, 0, 0);
			modelStack.Rotate(rotateAngle9 / 3, 0, 0, 1);
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
			modelStack.Scale(0.15, 0.05, 0.2);
			RenderMesh(meshList[GEO_MUDKIP_LIGHTER_SPHERE], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Rotate(-32, 1, 0, 0);
			modelStack.Translate(0, -2.2, -0.17);
			modelStack.Scale(0.16, 0.1, 0.16);
			RenderMesh(meshList[GEO_MUDKIP_LIGHTER_SPHERE], true);
			modelStack.PopMatrix();

			unsigned int count = 0;
			for (int idx = 23; idx >= 0; idx--) //render water spheres
			{
				if (waterposition[idx][0] == 0 && animation_phase != A_ATTACK_WATERREMOVE) continue;
				modelStack.PushMatrix();
				if (waterposition[idx][0] == 0 && animation_phase == A_ATTACK_WATERREMOVE)
				{
					modelStack.Translate(waterposition[idx][1], waterposition[idx][2], waterposition[idx][3]);
				}
				if (waterposition[idx][0] == 1)
				{
					modelStack.Translate(waterposition[idx][1], waterposition[idx][2], waterposition[idx][3]);
					RenderMesh(meshList[GEO_WATER], true);
				}
				count++;
			}
			for (int idx = 0; idx < count; idx++)
			{
				modelStack.PopMatrix();
			}

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
		modelStack.Rotate(-(rotateAngle - 90), 0, 1, 0);
		modelStack.Rotate(rotateAngle7, 1, 0, 0);
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
		modelStack.Translate(4, 0 - legtranslateY, 4);
		modelStack.Rotate(rotateAngle - 90, 1, 0, 0);
		modelStack.Rotate(rotateAngle2 - 90, 0, 0, 1);
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
		modelStack.Translate(-4, 0 - legtranslateY, 4);
		modelStack.Rotate(-(rotateAngle - 90), 1, 0, 0);
		modelStack.Rotate(-(rotateAngle2 - 90), 0, 0, 1);
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
		modelStack.Translate(-4, 0 - legtranslateY, -4);
		modelStack.Rotate(rotateAngle - 90, 1, 0, 0);
		modelStack.Rotate(-(rotateAngle2 - 90), 0, 0, 1);
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
		modelStack.Translate(4, 0 - legtranslateY, -4);
		modelStack.Rotate(-(rotateAngle - 90), 1, 0, 0);
		modelStack.Rotate(rotateAngle2 - 90, 0, 0, 1);
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


	if (berrytranslate[1] != -10)
	{
		//oran berry
		modelStack.PushMatrix();
		modelStack.Translate(berrytranslate[0], berrytranslate[1], berrytranslate[2]);
		modelStack.Scale(0.35, 0.25, 0.35);

			modelStack.PushMatrix();
			modelStack.Scale(0.5, 0.25, 0.5);
			modelStack.Translate(0, 35, 0);

			meshList[GEO_ORAN_BERRY_SPHERE]->material.kAmbient.Set(48.0 / 255.0f, 209.0 / 255.0f, 70.0 / 255.0f);
			meshList[GEO_ORAN_BERRY_SPHERE]->material.kDiffuse.Set(68.0 / 255.0f, 229.0 / 255.0f, 90.0 / 255.0f);
			meshList[GEO_ORAN_BERRY_SPHERE]->material.kSpecular.Set(0.3f, 0.3f, 0.3f);
			meshList[GEO_ORAN_BERRY_SPHERE]->material.kShininess = 0.5f;

			RenderMesh(meshList[GEO_ORAN_BERRY_SPHERE], true);

			modelStack.PopMatrix();

		//set color of the berry
		meshList[GEO_ORAN_BERRY_SPHERE]->material.kAmbient.Set(34.0 / 255.0f, 110.0 / 255.0f, 224.0 / 255.0f);
		meshList[GEO_ORAN_BERRY_SPHERE]->material.kDiffuse.Set(34.0 / 255.0f, 110.0 / 255.0f, 224.0 / 255.0f);
		meshList[GEO_ORAN_BERRY_SPHERE]->material.kSpecular.Set(0.3f, 0.3f, 0.3f);
		meshList[GEO_ORAN_BERRY_SPHERE]->material.kShininess = 0.5f;

		RenderMesh(meshList[GEO_ORAN_BERRY_SPHERE], true);
		modelStack.PopMatrix();
	}
	if (dummyscale > 0.06)
	{
		modelStack.PushMatrix();
		modelStack.Translate(dummytranslation[0], dummytranslation[1], dummytranslation[2]);
		modelStack.Scale(dummyscale, dummyscale, dummyscale);
		modelStack.Scale(1.5, 3, 1.5);
		RenderMesh(meshList[GEO_TRAINING_DUMMY], true);
		modelStack.PopMatrix();
	}

	if (renderaxes == true)
	{
		RenderMesh(meshList[GEO_AXES], false);
	}

	//trees
	modelStack.PushMatrix();
	modelStack.Translate(150, 20, 150);

		modelStack.PushMatrix();
		modelStack.Translate(0, 40, 0);
		modelStack.Scale(7.5, 10, 7.5);
		RenderMesh(meshList[GEO_TREETOP], true);
		modelStack.PopMatrix();

	modelStack.Scale(1.5, 3, 1.5);
	RenderMesh(meshList[GEO_TRAINING_DUMMY], true);
	modelStack.PopMatrix();

	//tree 2
	modelStack.PushMatrix();
	modelStack.Translate(-150, 20, 150);

		modelStack.PushMatrix();
		modelStack.Translate(0, 40, 0);
		modelStack.Scale(7.5, 10, 7.5);
		RenderMesh(meshList[GEO_TREETOP], true);
		modelStack.PopMatrix();

	modelStack.Scale(1.5, 3, 1.5);
	RenderMesh(meshList[GEO_TRAINING_DUMMY], true);
	modelStack.PopMatrix();

	//tree 3
	modelStack.PushMatrix();
	modelStack.Translate(-150, 20, -150);

	modelStack.PushMatrix();
	modelStack.Translate(0, 40, 0);
	modelStack.Scale(7.5, 10, 7.5);
	RenderMesh(meshList[GEO_TREETOP], true);
	modelStack.PopMatrix();

	modelStack.Scale(1.5, 3, 1.5);
	RenderMesh(meshList[GEO_TRAINING_DUMMY], true);
	modelStack.PopMatrix();

	//tree 4
	modelStack.PushMatrix();
	modelStack.Translate(150, 20, -150);

		modelStack.PushMatrix();
		modelStack.Translate(0, 40, 0);
		modelStack.Scale(7.5, 10, 7.5);
		RenderMesh(meshList[GEO_TREETOP], true);
		modelStack.PopMatrix();

	modelStack.Scale(1.5, 3, 1.5);
	RenderMesh(meshList[GEO_TRAINING_DUMMY], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(light[0].position.x, light[0].position.y, light[0].position.z);
	RenderMesh(meshList[GEO_LIGHTBALL], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Scale(2000, 2000, 2000);
	modelStack.Rotate(-90, 1, 0, 0);
	RenderMesh(meshList[GEO_QUAD], true);
	modelStack.PopMatrix();

}

void SceneAssignment::Exit()
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

void SceneAssignment::RenderMesh(Mesh* mesh, bool enableLight)
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
	mesh->Render();
}

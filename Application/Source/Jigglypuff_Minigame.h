#ifndef Sp2_Minigame_H
#define Sp2_Minigame_H

#include "Scene.h"
#include "Camera.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "Light.h"
#include "Camera2.h"

#include "CollectObjects.h";
#include <windows.h>


class Sp2_Minigame : public Scene
{
	MS modelStack, viewStack, projectionStack;


	enum UNIFORM_TYPE
	{
		U_MVP = 0,
		U_MODELVIEW,
		U_MODELVIEW_INVERSE_TRANSPOSE,
		U_MATERIAL_AMBIENT,
		U_MATERIAL_DIFFUSE,
		U_MATERIAL_SPECULAR,
		U_MATERIAL_SHININESS,
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
		U_COLOR_TEXTURE_ENABLED,
		U_COLOR_TEXTURE,
		U_NUMLIGHTS,
		U_TEXT_ENABLED,
		U_TEXT_COLOR,

		U_TOTAL,
	};


	enum GEOMETRY_TYPE
	{
		GEO_AXES,

		GEO_LEFT,
		GEO_RIGHT,
		GEO_TOP,
		GEO_BOTTOM,
		GEO_FRONT,
		GEO_BACK,


		GEO_CUBE,
		GEO_SPHERE,
		GEO_TORUS,
		GEO_HALFTORUS,
		GEO_EYELID,
		GEO_MOUTH,
		GEO_PYRAMID,
		GEO_CYLINDER,


		GEO_TAIL_CONNECTOR,
		GEO_CONE,
		GEO_TAIL_CYLINDER,

		GEO_FIRE,

		GEO_LIGHTBALL,


		GEO_GRASS,
		GEO_PALMTREES,
		GEO_LARGETREES,
		GEO_REGULARTREE,
		GEO_TEXT,
		GEO_ROCKS_SMALL,
		GEO_HEALTHBAR,
		GEO_HEALTHBACKGROUND,
		GEO_LOG,
		GEO_HEART,
		GEO_PAPER,
		GEO_GAMEOVER,
		GEO_GAMEOVERTEXT,
		GEO_COINS,
		GEO_RETRY,
		GEO_LAMPPOST,
		GEO_BLOCK,
		GEO_ALUSKILLS,

		GEO_TENTOPEN,
		GEO_TENTCLOSE,
		GEO_CAMPFIRE,


		GEO_VICTORY,
		GEO_VICTORYTEXT,
		GEO_DIAMONDIMAGE,
		GEO_DIAMOND,
		NUM_GEOMETRY,
	};
	Light light[2];

public:
	Sp2_Minigame();
	~Sp2_Minigame();

	virtual void Init();
	virtual bool Update(double dt);
	virtual void UpdateENV(double dt);
	virtual void Render();
	virtual void Exit();
	virtual void UseScene();
private:
	HWND window;
	RECT rect;
	LPPOINT mousepos;


	void RenderSkybox();
	unsigned m_vertexArrayID;
	Mesh* meshList[NUM_GEOMETRY];

	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];

	float rotateAngle;
	float rotateBody;
	float translateX;
	float translateZ;

	float RotateLeftArm_Movement;
	float RotateRightArm_Movement;
	float RotateLeftLeg_Movement;
	float RotateRightLeg_Movement;
	bool allowMovement;
	bool swingforward;
	bool swingbackward;
	float rotatelegs_jump;
	bool HopUp, HopDown;
	float TranslateUpandDown;

	float translatebodyY_Jump;
	bool allowJump;
	bool air;
	bool ground;


	float rotate_Eyelid;
	bool open;
	bool close;

	bool i;
	int j;
	float ShockwaveScale;
	int ShockwavedirectionX;
	int ShockwavedirectionZ;
	bool DisplayShockwave;


	float BodyR;
	float BodyG;
	float BodyB;


	bool isDemon;
	bool tail_shakeleft;
	bool tail_shakeright;
	float rotateDemonTail;
	float fireEffects1;
	float ScaleFireEffect1;
	float fireEffects2;
	float ScaleFireEffect2;
	float fireEffects3;
	float ScaleFireEffect3;
	float fireEffects4;
	float ScaleFireEffect4;
	float fireEffects5;
	float ScaleFireEffect5;
	float fireEffects6;
	float ScaleFireEffect6;

	bool allowAttack;
	bool SwordSwingBackward, SwordSwingForward, allowSwordSwing;
	float RotateRightArm_SwingSword;
	bool skill;
	int SkillsTimer;
	float ScaleUpandDown;
	bool HopUp_FIRE;
	bool HopDown_FIRE;
	float translateFusionWaveX;
	float translateFusionWaveZ;
	float rotateFusionWave;
	bool allowUltimateAttack;
	bool allowReleaseofFusionWave;
	bool allowSpinattack;
	bool allowFusionWaveTransverse;
	bool UltiSwordRevert_rotation;
	float rotateBodySpinAttack;
	float rotateSwordSpinAttack;
	bool SpinStarts;
	bool SpinEnds;
	float Temp;
	float TranslateXEqualFusionWaveX, TranslateZEqualFusionWaveZ;
	bool allowJumpAttack;
	float jumpAttack_rotation;
	float rotateSkyBox;
	float fps;


	bool isAlive;
	float Health;
	bool isCollided;

	bool testcollect;


	bool is1stPersonCam;
	bool EarnedCoins;
	float buttonsincelastpress;

	bool EnemySpotted = false;
	bool isOpenTent = true;

	void RenderMesh(Mesh* mesh, bool enableLight);
	void RenderText(Mesh* mesh, std::string text, Color color);

	void RenderImageOnScreen(Mesh* mesh, Color color, float Rotate, float sizex, float sizey, float sizez, float x, float y);
	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);

	float DistanceParameter(float EnemyXPos, float EnemyZPos, float OriginalPosX, float OriginalPosZ);
	bool interactableObject(int Px, int Pz, int ObjectX, int ObjectZ, int x, int z);
	bool EnemyDetectPlayer(int Px, int Py, int Ex, int Ey);
	bool isInHitbox(int hitboxX, int hitboxY, int Ex, int Ey, int radius);
	Vector3 PlayerCollision(unsigned count, float Px, float Pz);
	Vector3 CollisionCircleRect(float cx, float cy, float radius, float rx, float ry, float rw, float rh);



	//Camera camera;
	int textMaxWidth;
	int textSpacing[256];

	// Demon
	float test;
	bool jump;
	bool Ground;
	float EnemyXSpeed, EnemyZSpeed;
	bool allowEnemyMovement;
	bool swingforwardEnemy;
	bool swingbackwardEnemy;
	float EnemyPosX, EnemyPosZ;
	float RotateEnemyLeftArm_Movement;
	float RotateEnemyRightArm_Movement;
	float RotateEnemyLeftLeg_Movement;
	float RotateEnemyRightLeg_Movement;
	bool AllowEnemyAttack;
	bool ShockWaveEnemyAttack;
	float translateY_EnemyJump;
	float EnemyShockwaveScale;
	float EnemyDieRotation;
	float EnemyHealth;


	float test2;
	bool jump2;
	bool Ground2;
	float EnemyXSpeed2, EnemyZSpeed2;
	bool allowEnemyMovement2;
	bool swingforwardEnemy2;
	bool swingbackwardEnemy2;
	float EnemyPosX2, EnemyPosZ2;
	float RotateEnemyLeftArm_Movement2;
	float RotateEnemyRightArm_Movement2;
	float RotateEnemyLeftLeg_Movement2;
	float RotateEnemyRightLeg_Movement2;
	bool AllowEnemyAttack2;
	bool ShockWaveEnemyAttack2;
	float translateY_EnemyJump2;
	float EnemyShockwaveScale2;
	float EnemyDieRotation2;
	float EnemyHealth2;



	float test3;
	bool jump3;
	bool Ground3;
	float EnemyXSpeed3, EnemyZSpeed3;
	bool allowEnemyMovement3;
	bool swingforwardEnemy3;
	bool swingbackwardEnemy3;
	float EnemyPosX3, EnemyPosZ3;
	float RotateEnemyLeftArm_Movement3;
	float RotateEnemyRightArm_Movement3;
	float RotateEnemyLeftLeg_Movement3;
	float RotateEnemyRightLeg_Movement3;
	bool AllowEnemyAttack3;
	bool ShockWaveEnemyAttack3;
	float translateY_EnemyJump3;
	float EnemyShockwaveScale3;
	float EnemyDieRotation3;
	float EnemyHealth3;





	bool move = true;

	float timer_Retry = 0.f;
	float timer_gameover;
	float timer_Skill1 = 0;
	float timer_Skill2 = 0;
	float timer_Skill3 = 0;


	int Diamond;

	float angle, angle2, angle3;
	float EnemyViewAngle, EnemyViewAngle2, EnemyViewAngle3;
	bool triggerEnemy = false, triggerEnemy2 = false, triggerEnemy3 = false;
	float EnemyRightArm_JumpAttackRotation, EnemyRightArm_JumpAttackRotation2, EnemyRightArm_JumpAttackRotation3;
	bool EnemyHopUp, EnemyHopDown, EnemyHopUp2, EnemyHopDown2, EnemyHopUp3, EnemyHopDown3;
	float EnemyUpandDown_Translation, EnemyUpandDown_Translation2, EnemyUpandDown_Translation3;



	CollectObjects* Diamonds[10];
	Camera2 camera2;

	Vector3 look, look2, look3;
	Vector3 camera_posHori;
	Vector3 camera_posVert;
	Vector3 pitchVert, pitchVert2, pitchVert3;
	Vector3 point_pos, point_pos2, point_pos3;
	Vector3 target = Vector3(0, 0, 1);
	float angleHori;
	float angleVert;
	float angleHori2;
	float angleVert2;
	float angleHori3;
	float angleVert3;

	float min = 2, sec = 60;

};

#endif
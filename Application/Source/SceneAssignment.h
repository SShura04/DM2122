#ifndef SCENE_LIGHT2_H
#define SCENE_LIGHT2_H

#include "Scene.h"
#include "Camera3.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "Light.h"
#include "timer.h"

class SceneAssignment : public Scene
{
	enum GEOMETRY_TYPE
	{
		GEO_AXES,
		GEO_QUAD,
		GEO_MUDKIP_SPHERE,
		GEO_MUDKIP_CHEEKS,
		GEO_MUDKIP_TAIL,
		GEO_MUDKIP_CONE,
		GEO_MUDKIP_WHITE_SPOT,
		GEO_BLACKBALL,
		GEO_MUDKIP_LIGHTER_SPHERE,
		GEO_ORAN_BERRY_SPHERE,
		GEO_TRAINING_DUMMY,
		GEO_WATER,
		GEO_TREETOP,
		GEO_LIGHTBALL,
		NUM_GEOMETRY,
		//total 14
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
		U_NUMLIGHTS,
		U_LIGHTENABLED,
		U_TOTAL,
	};

	enum ANIMATION
	{
		A_WALK_FORWARD = 0,
		A_WALK_BACKWARD,
		A_WALK_LEFT,
		A_WALK_RIGHT,
		A_NOTHING,
		A_EATING,
		A_ATTACK,
		A_TOTAL,
	};

	enum ANIMATION_PHASES
	{
		AA_NOTHING,
		A_WALK_ROTATE_LEFT,
		A_WALK_ROTATE_RIGHT,
		A_WALK_ROTATE_FORWARD,
		A_WALK_ROTATE_BACKWARD,
		A_EATING_SETUP,
		A_EATING_BERRY_DROP,
		A_EATING_MUDKIP_MOVE_WHOLE,
		A_EATING_MUDKIP_MOVE_HEAD_DOWN,
		A_EATING_MUDKIP_EAT,
		A_EATING_MUDKIP_MOVE_HEAD_UP,
		A_ATTACK_SETUP,
		A_ATTACK_DUMMY_GROW,
		A_ATTACK_TACKLE,
		A_ATTACK_JUMP,
		A_ATTACK_JUMP_APPROACH,
		A_ATTACK_BOUNCEOFF,
		A_ATTACK_WATERGUN,
		A_ATTACK_WATERREMOVE,
		A_ATTACK_DROP_DOWN,
		A_ATTACK_DROP_DOWN_APPROACH,
		A_COUNT,
	};

public:
	SceneAssignment();
	~SceneAssignment();

	virtual void Init();
	virtual bool Update(double dt);
	virtual void Render();
	virtual void Exit();
	
private:
	unsigned m_vertexArrayID;
	Mesh *meshList[NUM_GEOMETRY];

	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];
	MS modelStack, viewStack, projectionStack;

	int animation;
	int animation_phase;
	int temp;
	StopWatch timer, buttontimer;
	float totalelaspedtime;
	float timesincelastbuttonpress;
	float berrytranslate[3];
	float mudkiptranslate[3];
	float dummytranslation[3];
	float dummyscale;
	float legtranslateY;
	float tempvaluestore;
	unsigned int mudkipdirection;
	float rotateAngle, rotateAngle2, rotateAngle3, rotateAngle4,
		rotateAngle5, rotateAngle6, rotateAngle7, rotateAngle8, 
		rotateAngle9; //8:fin-x 7: tails 6:eyes 5: cheeks 4:full body 3:head 2 & 1:leg
	bool renderaxes;
	float waterposition[24][4];

	Light light[1];
	void RenderMesh(Mesh* mesh, bool enableLight);

	Camera3 camera;
};

#endif
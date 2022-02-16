#ifndef CAMERA_TEST_H
#define CAMERA_TEST_H

#include "Camera.h"
//#include "Hitbox.h"
#include <Windows.h>
#include "Objects.h"

class CameraTest : public Camera
{
public:
	//Vector3 position;
	//Vector3 target;
	//Vector3 up;

	Vector3 defaultPosition;
	Vector3 defaultTarget;
	Vector3 defaultUp;
	RECT windowcenter;
	POINT center;

	CameraTest();
	~CameraTest();
	virtual void Init(const Vector3& pos, const Vector3& target, const Vector3& up);
	virtual void Update(double dt, POINT mousepos, bool move);
	virtual void Reset();


	//unsigned gameobj;
	//static const int totalobj = 9;
	//Objects testingobj[totalobj];


	//Vector3 PlayerCollision(unsigned count);
	//Vector3 CollisionCircleRect(float cx, float cy, float radius, float rx, float ry, float rw, float rh);
};

#endif
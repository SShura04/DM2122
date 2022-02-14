#ifndef CAMERA_3_H
#define CAMERA_3_H

#include "Camera.h"
#include <Windows.h>

class Camera3 : public Camera
{
public:
	//Vector3 position;
	//Vector3 target;
	//Vector3 up;

	Vector3 defaultPosition;
	Vector3 defaultTarget;
	Vector3 defaultUp;
	Vector3 view;
	Vector3 right;

	RECT windowcenter;
	POINT center;

	Camera3();
	~Camera3();
	virtual void Init(const Vector3& pos, const Vector3& target, const Vector3& up);
	virtual Vector3 Update(double dt, POINT mousepos);
	virtual void Update(double dt);
	virtual void Reset();
};

#endif
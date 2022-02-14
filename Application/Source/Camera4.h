#ifndef CAMERA_4_H
#define CAMERA_4_H

#include "Camera.h"
#include <Windows.h>

class Camera4 : public Camera
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
	Vector3 prevPosition;

	RECT windowcenter;
	POINT center;

	Camera4();
	~Camera4();
	virtual void Init(const Vector3& pos, const Vector3& target, const Vector3& up);
	virtual Vector3 Update(double dt, POINT mousepos, bool allowmovement, bool debug, float MOVEMENTSPEED = 0.15f); //for first person
	virtual void gotoprevpos(bool *x,bool *y, bool *z);
	virtual void gotoprevpos();
	virtual void Update(double dt); //for azimuth
	virtual void MoveCam(Vector3 moveparameters);
	virtual void Settarget(Vector3 newtarget);
	virtual void Reset();
};

#endif
#include "Camera2.h"
#include "Application.h"
#include "Mtx44.h"
#include <iostream>

Camera2::Camera2()
{
}

Camera2::~Camera2()
{
}

void Camera2::Init(const Vector3& pos, const Vector3& target, const Vector3& up)
{
	this->position = defaultPosition = pos;
	this->target = defaultTarget = target;
	Vector3 view = (target - position).Normalized();
	Vector3 right = view.Cross(up);
	right.y = 0;
	right.Normalize();
	this->up = defaultUp = right.Cross(view).Normalized();
	camMagnitude = 85;
}

void Camera2::ThirdPersonCam(double& dt, const Vector3& ionPos)
{
	Vector3 view = (target - position).Normalized();

	position = ionPos - (view * camMagnitude);
	target = ionPos;

	static const float CAM_ROT_SPEED = 50.0f;
	static const float CAM_ZOOM_SPEED = 100.0f;

	//if (Application::IsKeyPressed(VK_LEFT))
	//{
	//	float yaw = CAM_ROT_SPEED * static_cast<float>(dt);
	//	Mtx44 rotation;
	//	rotation.SetToRotation(yaw, 0.0f, 1.0f, 0.0f);
	//	position -= ionPos;
	//	position = rotation * position;
	//	position += ionPos;
	//	up = rotation * up;
	//}
	//if (Application::IsKeyPressed(VK_RIGHT))
	//{
	//	float yaw = -CAM_ROT_SPEED * static_cast<float>(dt);
	//	Mtx44 rotation;
	//	rotation.SetToRotation(yaw, 0.0f, 1.0f, 0.0f);
	//	position -= ionPos;
	//	position = rotation * position;
	//	position += ionPos;
	//	up = rotation * up;
	//}

	if (Application::IsKeyPressed('N'))
	{
		camMagnitude -= CAM_ZOOM_SPEED * static_cast<float>(dt);
		if (camMagnitude < 55.0f)
		{
			camMagnitude = 55.0f;
		}

	}
	if (Application::IsKeyPressed('M'))
	{
		camMagnitude += CAM_ZOOM_SPEED * static_cast<float>(dt);
		if (camMagnitude > 85.0f)
		{
			camMagnitude = 85.0f;
		}
	}
}

void Camera2::Update(double dt)
{
	static const float CAMERA_SPEED = 45.f;
	static const float ZOOM_SPEED = 20.f;
	if (Application::IsKeyPressed(VK_LEFT))
	{
		float yaw = -CAMERA_SPEED * static_cast<float>(dt);
		Mtx44 rotation;
		rotation.SetToRotation(yaw, 0, 1, 0);
		position = rotation * position;
		up = rotation * up;
	}
	if (Application::IsKeyPressed(VK_RIGHT))
	{
		float yaw = CAMERA_SPEED * static_cast<float>(dt);
		Mtx44 rotation;
		rotation.SetToRotation(yaw, 0, 1, 0);
		position = rotation * position;
		up = rotation * up;
	}
	if (Application::IsKeyPressed(VK_UP))
	{
		float pitch = -CAMERA_SPEED * static_cast<float>(dt);
		Vector3 view = (target - position).Normalized();
		Vector3 right = view.Cross(up);
		right.y = 0;
		right.Normalize();
		up = right.Cross(view).Normalized();
		Mtx44 rotation;
		rotation.SetToRotation(pitch, right.x, right.y, right.z);
		position = rotation * position;
	}
	if (Application::IsKeyPressed(VK_DOWN))
	{
		float pitch = CAMERA_SPEED * static_cast<float>(dt);
		Vector3 view = (target - position).Normalized();
		Vector3 right = view.Cross(up);
		right.y = 0;
		right.Normalize();
		up = right.Cross(view).Normalized();
		Mtx44 rotation;
		rotation.SetToRotation(pitch, right.x, right.y, right.z);
		position = rotation * position;
	}
	if (Application::IsKeyPressed('N'))
	{
		Vector3 view = target - position;
		if (view.Length() > 5) //Prevent the camera from touching the origin
		{
			view.Normalize();
			position += view * ZOOM_SPEED * dt;
		}
	}
	if (Application::IsKeyPressed('M'))
	{
		Vector3 view = (target - position).Normalized();
		position -= view * ZOOM_SPEED * dt;
	}
}

void Camera2::Reset()
{
	position = defaultPosition;
	target = defaultTarget;
	up = defaultUp;
}
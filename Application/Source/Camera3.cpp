#include "Camera3.h"
#include "Application.h"
#include "Mtx44.h"

Camera3::Camera3()
{
}

Camera3::~Camera3()
{
}

void Camera3::Init(const Vector3& pos, const Vector3& target, const Vector3& up)
{
	this->position = defaultPosition = pos;
	this->target = defaultTarget = target;
	Vector3 view = (target - position).Normalized();
	Vector3 right = view.Cross(up);
	right.y = 0;
	right.Normalize();
	this->up = defaultUp = right.Cross(view).Normalized();
}

Vector3 Camera3::Update(double dt, POINT mousepos)
{
	GetWindowRect(GetFocus(), &windowcenter);
	center.x = (windowcenter.left + windowcenter.right) * 0.5;
	center.y = (windowcenter.top + windowcenter.bottom) * 0.5;
	static const float CAMERA_ROTATION_SPEED = 60.f;
	float CAMERA_ROTATION_SPEEDX = -(mousepos.x - center.x) * 0.1;
	float CAMERA_ROTATION_SPEEDY = -(mousepos.y - center.y) * 0.1;
	view = target - position;
	right = view.Cross(up);
	right.y = 0;
	right.Normalize();
	this->up = defaultUp = right.Cross(view).Normalized();
	if (Application::IsKeyPressed('A'))
	{
		position = position - right;
		target = position + view;
	}
	if (Application::IsKeyPressed('D'))
	{
		position = position + right;
		target = position + view;
	}
	if (Application::IsKeyPressed('W'))
	{
		position = position + view;
		target = position + view;
	}
	if (Application::IsKeyPressed('S'))
	{
		position = position - view;
		target = position + view;
	}
	if (Application::IsKeyPressed('N'))
	{
		position = position + up;
		target = position + view;
	}
	if (Application::IsKeyPressed('M'))
	{
		position = position - up;
		target = position + view;
	}

	//deal with horizontal rotation of camera
	Mtx44 rotation;
	rotation.SetToRotation(CAMERA_ROTATION_SPEEDX, 0, 1, 0);
	view = rotation * view;
	target = position + view;
	//deal with vertical rotation of camera
	rotation.SetToRotation(CAMERA_ROTATION_SPEEDY, right.x, right.y, right.z);
	view = rotation * view;
	target = position + view;
	//reset mouse position
	SetCursorPos(center.x, center.y);

	if (Application::IsKeyPressed(VK_LEFT))
	{
		float yaw = CAMERA_ROTATION_SPEED * static_cast<float>(dt);
		Mtx44 rotation;
		rotation.SetToRotation(yaw, up.x, up.y, up.z);
		view = rotation * view;
		target = position + view;
	}
	if (Application::IsKeyPressed(VK_RIGHT))
	{
		float yaw = -CAMERA_ROTATION_SPEED * static_cast<float>(dt);
		Mtx44 rotation;
		rotation.SetToRotation(yaw, up.x, up.y, up.z);
		view = rotation * view;
		target = position + view;
	}
	if (Application::IsKeyPressed(VK_UP))
	{
		float pitch = CAMERA_ROTATION_SPEED * static_cast<float>(dt);
		Mtx44 rotation;
		rotation.SetToRotation(pitch, right.x, right.y, right.z);
		view = rotation * view;
		target = position + view;
	}
	if (Application::IsKeyPressed(VK_DOWN))
	{
		float pitch = -CAMERA_ROTATION_SPEED * static_cast<float>(dt);
		Mtx44 rotation;
		rotation.SetToRotation(pitch, right.x, right.y, right.z);
		view = rotation * view;
		target = position + view;
	}
	if(Application::IsKeyPressed('R'))
	{
		Reset();
	}
	return position;
}

void Camera3::Update(double dt)
{
	static const float CAMERA_SPEED = 60.f;
	static const float ZOOM_SPEED = 80.f;
	if (Application::IsKeyPressed('A'))
	{
		float yaw = -CAMERA_SPEED * static_cast<float>(dt);
		Mtx44 rotation;
		rotation.SetToRotation(yaw, 0, 1, 0);
		position = rotation * position;
		up = rotation * up;
	}
	if (Application::IsKeyPressed('D'))
	{
		float yaw = CAMERA_SPEED * static_cast<float>(dt);
		Mtx44 rotation;
		rotation.SetToRotation(yaw, 0, 1, 0);
		position = rotation * position;
		up = rotation * up;
	}
	if (Application::IsKeyPressed('W'))
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
	if (Application::IsKeyPressed('S'))
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
	if (Application::IsKeyPressed('R'))
	{
		Reset();
	}
}

void Camera3::Reset()
{
	position = defaultPosition;
	target = defaultTarget;
	up = defaultUp;
}
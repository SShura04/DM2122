#include "Camera4.h"
#include "Application.h"
#include "Mtx44.h"

Camera4::Camera4()
{
}

Camera4::~Camera4()
{
}

Vector3 Camera4::CollisionCircleRect(float cx, float cy, float radius, float rx, float ry, float rw, float rh) {
    //assume all y = 0 since y not needed
    Vector3 endPos = Vector3(cx, 0.01, cy);
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


Vector3 Camera4::PlayerCollision(unsigned count) {
	if (count == 0) { //collision with house 1
		return CollisionCircleRect(position.x, position.z, 5, -19, 20, 50, 50);
	}
	else {
		return Vector3(0, 0, 0);
	}
}

void Camera4::Init(const Vector3& pos, const Vector3& target, const Vector3& up)
{
	this->position = defaultPosition = pos;
	this->target = defaultTarget = target;
	view = (target - position).Normalized();
	right = view.Cross(up);
	right.y = 0;
	right.Normalize();
	this->up = defaultUp = right.Cross(view).Normalized();
}


Vector3 Camera4::Update(double dt, POINT mousepos, bool allowmovement, bool debug, float MOVEMENTSPEED)
{
	prevPosition = position;
	GetWindowRect(GetFocus(), &windowcenter);
	center.x = (windowcenter.left + windowcenter.right) * 0.5;
	center.y = (windowcenter.top + windowcenter.bottom) * 0.5;
	if (allowmovement)
	{
		static const float CAMERA_ROTATION_SPEED = 60.f;
		float CAMERA_ROTATION_SPEEDX = -(mousepos.x - center.x) * 0.1;
		float CAMERA_ROTATION_SPEEDY = -(mousepos.y - center.y) * 0.1;
		view = (target - position).Normalized();
		right = view.Cross(up);
		right.y = 0;
		right.Normalize();
		this->up = defaultUp = right.Cross(view).Normalized();
		if (Application::IsKeyPressed('A'))
		{
			position = position - (right * MOVEMENTSPEED);
			target = position + view;
		}
		if (Application::IsKeyPressed('D'))
		{
			position = position + (right * MOVEMENTSPEED);
			target = position + view;
		}
		if (Application::IsKeyPressed('W'))
		{
			position = position + (view * MOVEMENTSPEED);
			//position.z = position.z + (view.z * MOVEMENTSPEED);

			target = position + view;
		}
		if (Application::IsKeyPressed('S'))
		{
			position.x = position.x - (view.x * MOVEMENTSPEED);
			position.z = position.z - (view.z * MOVEMENTSPEED);
			target = position + view;
		}
		if (debug)
		{
			if (Application::IsKeyPressed('N'))
			{
				position.y = position.y + (2 * MOVEMENTSPEED);
				target = position + view;
			}
			if (Application::IsKeyPressed('M'))
			{
				position.y = position.y - (2 * MOVEMENTSPEED);
				target = position + view;
			}
		}

		position.y = 2;
		//deal with horizontal rotation of camera
		Mtx44 rotation;
		rotation.SetToRotation(CAMERA_ROTATION_SPEEDX, 0, 1, 0);
		view = rotation * view;
		target = position + view;
		//deal with vertical rotation of camera
		rotation.SetToRotation(CAMERA_ROTATION_SPEEDY, right.x, right.y, right.z);
		view = rotation * view;
		target = position + view;
	}
	//reset mouse position
	SetCursorPos(center.x, center.y);

	/*if (Application::IsKeyPressed(VK_LEFT))
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
	}*/
	if (Application::IsKeyPressed('R'))
	{
		Reset();
	}
	return position;
}


void Camera4::Update1(double dt, POINT mousepos, bool allowmovement, bool debug, float MOVEMENTSPEED)
{
	//prevPosition = position;
	GetWindowRect(GetFocus(), &windowcenter);
	center.x = (windowcenter.left + windowcenter.right) * 0.5;
	center.y = (windowcenter.top + windowcenter.bottom) * 0.5;
	if (allowmovement)
	{
		static const float CAMERA_ROTATION_SPEED = 60.f;
		float CAMERA_ROTATION_SPEEDX = -(mousepos.x - center.x) * 0.1;
		float CAMERA_ROTATION_SPEEDY = -(mousepos.y - center.y) * 0.1;
		Vector3 view = (target - position).Normalized();
		Vector3 right = view.Cross(up);
		right.y = 0;
		right.Normalize();
		this->up = defaultUp = right.Cross(view).Normalized();

		if (Application::IsKeyPressed('A'))
		{
			position = position - (right * MOVEMENTSPEED);
			target = position + view;
		}
		if (Application::IsKeyPressed('D'))
		{
			position = position + (right * MOVEMENTSPEED);
			target = position + view;
		}
		if (Application::IsKeyPressed('W'))
		{
			position = position + (view * MOVEMENTSPEED);

			for (unsigned i = 0; i <= 1; ++i) {
				Vector3 finalPos = PlayerCollision(i); //ignore y
				position.x = finalPos.x;
				position.z = finalPos.z;
			}

			target = position + view;
		}
		if (Application::IsKeyPressed('S'))
		{
			position = position - (view * MOVEMENTSPEED);
			target = position + view;
		}
		if (debug)
		{
			if (Application::IsKeyPressed('N'))
			{
				position.y = position.y + (2 * MOVEMENTSPEED);
				target = position + view;
			}
			if (Application::IsKeyPressed('M'))
			{
				position.y = position.y - (2 * MOVEMENTSPEED);
				target = position + view;
			}
		}

		position.y = 2;
		//deal with horizontal rotation of camera
		Mtx44 rotation;
		rotation.SetToRotation(CAMERA_ROTATION_SPEEDX, 0, 1, 0);
		view = rotation * view;
		target = position + view;
		//deal with vertical rotation of camera
		rotation.SetToRotation(CAMERA_ROTATION_SPEEDY, right.x, right.y, right.z);
		view = rotation * view;
		target = position + view;
	}
	//reset mouse position
	SetCursorPos(center.x, center.y);

	/*if (Application::IsKeyPressed(VK_LEFT))
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
	}*/
	if (Application::IsKeyPressed('R'))
	{
		Reset();
	}
	//return position;
}
void Camera4::gotoprevpos(bool* x, bool* y, bool* z)
{
	//view = target - position;
	//if (*x == true)
	//	position.x = prevPosition.x;
	//if (*y == true)
	//	position.y = prevPosition.y;
	//if (*z == true)
	//	position.z = prevPosition.z;
	//target = position + view;
}

void Camera4::gotoprevpos()
{
	//view = target - position;
	//position = prevPosition;
	//target = position + view;
}

void Camera4::Update(double dt)
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

void Camera4::MoveCam(Vector3 moveparameters)
{
	position = moveparameters;
	target = position + view;
}

void Camera4::Settarget(Vector3 newtarget)
{
	target = newtarget;
}

void Camera4::Reset()
{
	position = defaultPosition;
	target = defaultTarget;
	up = defaultUp;
}
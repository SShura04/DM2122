#include "CameraTest.h"
#include "Application.h"
#include "Mtx44.h"

#include <iostream>
using namespace std;

CameraTest::CameraTest()
{
}

CameraTest::~CameraTest()
{
}

void CameraTest::Init(const Vector3& pos, const Vector3& target, const Vector3& up)
{
    this->position = defaultPosition = pos;
    this->target = defaultTarget = target;
    view = (target - position).Normalized();
    right = view.Cross(up);
    right.y = 0;
    right.Normalize();
    this->up = defaultUp = right.Cross(view).Normalized();
}

void CameraTest::Update(double dt, POINT mousepos, bool move)
{
    static const float CAMERA_SPEED = 45.f;
    static float angley = 0;
    GetWindowRect(GetFocus(), &windowcenter);
    center.x = (windowcenter.left + windowcenter.right) * 0.5;
    center.y = (windowcenter.top + windowcenter.bottom) * 0.5;

    view = (target - position).Normalized();
    right = view.Cross(up);
    right.y = 0;
    right.Normalize();

    float CAMERA_ROTATION_SPEEDX = -(mousepos.x - center.x) * 0.1;
    float CAMERA_ROTATION_SPEEDY = -(mousepos.y - center.y) * 0.1;


    if (move == true) {
        if (Application::IsKeyPressed('A'))
        {
            position -= right * static_cast<float>(dt) * ZOOM_SPEED;
            target = position + view;
        }

        if (Application::IsKeyPressed('D'))
        {
            position += right * static_cast<float>(dt) * ZOOM_SPEED;
            target = position + view;
        }


        if (Application::IsKeyPressed('W'))
        {
            position += view * static_cast<float>(dt) * ZOOM_SPEED;
            target = position + view;
        }
        if (Application::IsKeyPressed('S'))
        {
            position -= view * static_cast<float>(dt) * ZOOM_SPEED;
            target = position + view;
        }

        if (position.x >= 30) {
            position.x = 30;
        }
        if (position.x <= -21) {
            position.x = -21;
        }
        if (position.z >= 30) {
            position.z = 30;
        }
        if (position.z <= -43) {
            position.z = -43;
        }

        position.y = Position_Y;
        //deal with horizontal rotation of camera
        Mtx44 rotation;
        rotation.SetToRotation(CAMERA_ROTATION_SPEEDX, 0, 1, 0);
        view = rotation * view;
        target = position + view;
        angley += CAMERA_ROTATION_SPEEDY;
        if (angley > 60 || angley < -60)
        {
            if (angley < 0)
            {
                angley = -60;
            }
            else
            {
                angley = 60;
            }
            CAMERA_ROTATION_SPEEDY = 0;
        }
        //deal with vertical rotation of camera
        up = right.Cross(view).Normalized();
        rotation.SetToRotation(CAMERA_ROTATION_SPEEDY, right.x, right.y, right.z);
        view = rotation * view;
        target = position + view;

        SetCursorPos(center.x, center.y);
    }
}

void CameraTest::Reset()
{
    position = defaultPosition;
    target = defaultTarget;
    up = defaultUp;
}

void CameraTest::setposition(Vector3 newcamerapos)
{
    view = target - position;
    position = newcamerapos;
    target = position + view;
}
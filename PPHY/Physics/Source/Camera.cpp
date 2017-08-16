#include "Camera.h"
#include "Application.h"
#include "Mtx44.h"

Camera::Camera()
{
	Reset();
}

Camera::~Camera()
{
}

void Camera::Init(const Vector3& pos, const Vector3& target, const Vector3& up)
{
	this->position = pos;
	this->target = target;
	this->up = up;
}

void Camera::Reset()
{
	position.Set(1, 0, 0);
	target.Set(0, 0, 0);
	up.Set(0, 1, 0);
}

void Camera::Update(double dt)
{
}

Mtx44 Camera::GetViewMatrix()
{
	Mtx44 result;
	result.SetToLookAt(position.x, position.y, position.z,
		target.x, target.y, target.z,
		up.x, up.y, up.z);
	return result;
}

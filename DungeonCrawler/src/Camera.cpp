#include "Camera.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

using glm::vec3;
using glm::quat;

static vec3 position;
//static quat rotation;
static vec3 forward;
static vec3 up;
static vec3 euler;
static float fov;
static float near;
static float far;

void InitializeCamera()
{
	position = vec3(0.f, 0.f, 0.f);
	forward = vec3(0.f, 0.f, -1.f);
	up = vec3(0.f, 1.f, 0.f);
	euler = vec3(0.f, -90.f, 0.f);
	fov = 45.f;
	near = 0.1f;
	far = 100.f;
}

glm::vec3 GetCameraPosition()
{
	return position;
}

glm::vec3 GetCameraForward()
{
	return forward;
}

glm::vec3 GetCameraUp()
{
	return up;
}

void SetCameraPosition(vec3 _position)
{
	position = _position;
}

void TranslateCamera(vec3 translation)
{
	position += translation;
}

void SetCameraRotation(glm::vec3 _euler)
{
	euler = _euler;

	//Update forward vector as rotation has changed
	forward.x = cos(glm::radians(euler.y)) * cos(glm::radians(euler.x));
	forward.y = sin(glm::radians(euler.x));
	forward.z = sin(glm::radians(euler.y)) * cos(glm::radians(euler.x));
}

void RotateCamera(glm::vec3 _euler)
{
	SetCameraRotation(euler += _euler);
}

void ClampCameraPitch(float min, float max)
{
	if (euler.x > max)
	{
		SetCameraRotation(vec3(max, euler.y, euler.z));
	}
	else if (euler.x < min)
	{
		SetCameraRotation(vec3(min, euler.y, euler.z));
	}
}

void SetCameraFOV(float _fov)
{
	fov = _fov;
}

float GetCameraFOV()
{
	return fov;
}

void SetCameraNearFar(float _near, float _far)
{
	near = _near;
	far = _far;
}

float GetCameraNear()
{
	return near;
}

float GetCameraFar()
{
	return far;
}

glm::mat4 GetCameraView()
{
	return glm::lookAt(position, position + forward, up);
}

glm::mat4 GetCameraProjection()
{
	return glm::perspective(glm::radians(fov), 800.f / 600.f, near, far);
}
#pragma once;

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include <vector>

void InitializeCamera();

glm::vec3 GetCameraPosition();
glm::vec3 GetCameraForward();
glm::vec3 GetCameraUp();
void SetCameraPosition(glm::vec3 position);
void TranslateCamera(glm::vec3 translation);
void SetCameraRotation(glm::vec3 euler);
void RotateCamera(glm::vec3 euler);
void ClampCameraPitch(float min, float max);

void SetCameraFOV(float fov);
float GetCameraFOV();
void SetCameraNearFar(float near, float far);
float GetCameraNear();
float GetCameraFar();

glm::mat4 GetCameraView();
glm::mat4 GetCameraProjection();
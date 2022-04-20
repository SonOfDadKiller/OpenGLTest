#include <glad.h>
#include <glfw3.h>
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "ModelInstance.h"
#include "Camera.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;

static vec3 lightPosition;
static vec3 lightDiffuseColor;
static vec3 lightSpecularColor;
static vec3 lightAmbientColor;
static vec3 lightAttenuation;

ModelInstance::ModelInstance(Model* model, unsigned int shader)
{
	this->model = model;
	this->shader = shader;
	scale = vec3(1.f);
	transform = mat4(1.f);
	SetUniformAddresses();
}

ModelInstance::ModelInstance(Model* model, unsigned int shader, glm::vec3 position)
{
	this->model = model;
	this->shader = shader;
	scale = vec3(1.f);
	SetPosition(position);
	SetUniformAddresses();
}

ModelInstance::ModelInstance(Model* model, unsigned int shader, glm::vec3 position, glm::vec3 eulerRotation, glm::vec3 scale)
{
	this->model = model;
	this->shader = shader;
	SetPosition(position);
	SetRotation(eulerRotation);
	SetScale(scale);
	SetUniformAddresses();
}

void ModelInstance::SetUniformAddresses()
{
	modelUniform = glGetUniformLocation(shader, "model");
	viewUniform = glGetUniformLocation(shader, "view");
	projectionUniform = glGetUniformLocation(shader, "projection");
	viewPositionUniform = glGetUniformLocation(shader, "viewPos");
	nearUniform = glGetUniformLocation(shader, "near");
	farUniform = glGetUniformLocation(shader, "far");

	diffuseMapUniform = glGetUniformLocation(shader, "material.diffuseMap");
	specularMapUniform = glGetUniformLocation(shader, "material.specularMap");
	shininessUniform = glGetUniformLocation(shader, "material.shininess");

	lightPositionUniform = glGetUniformLocation(shader, "light.position");
	lightAmbientUniform = glGetUniformLocation(shader, "light.ambient");
	lightDiffuseUniform = glGetUniformLocation(shader, "light.diffuse");
	lightSpecularUniform = glGetUniformLocation(shader, "light.specular");
	lightAttenConstantUniform = glGetUniformLocation(shader, "light.attenConstant");
	lightAttenLinearUniform = glGetUniformLocation(shader, "light.attenLinear");
	lightAttenQuadraticUniform = glGetUniformLocation(shader, "light.attenQuadratic");
}

void ModelInstance::SetShader(unsigned int shader)
{
	this->shader = shader;
	SetUniformAddresses();
}

unsigned int ModelInstance::GetShader()
{
	return shader;
}

void ModelInstance::SetPosition(glm::vec3 position)
{
	this->position = position;
	UpdateTransform();
}

void ModelInstance::SetRotation(glm::vec3 eulerRotation)
{
	this->eulerRotation = eulerRotation;
	UpdateTransform();
}

void ModelInstance::SetScale(glm::vec3 scale)
{
	this->scale = scale;
	UpdateTransform();
}

void ModelInstance::UpdateTransform()
{
	transform = mat4(1.f);
	transform = glm::translate(transform, position);
	transform = glm::scale(transform, scale);
	transform *= glm::eulerAngleYXZ(glm::radians(eulerRotation.y), glm::radians(eulerRotation.x), glm::radians(eulerRotation.z));
}

glm::vec3 ModelInstance::GetPosition()
{
	return position;
}

glm::vec3 ModelInstance::GetRotation()
{
	return eulerRotation;
}

glm::vec3 ModelInstance::GetScale()
{
	return scale;
}

void ModelInstance::Draw()
{
	//Lighting uniforms
	glUseProgram(shader);

	if (lightPositionUniform != -1) glUniform3f(lightPositionUniform, lightPosition.x, lightPosition.y, lightPosition.z);
	if (lightDiffuseUniform != -1) glUniform3f(lightDiffuseUniform, lightDiffuseColor.x, lightDiffuseColor.y, lightDiffuseColor.z);
	if (lightSpecularUniform != -1) glUniform3f(lightSpecularUniform, lightSpecularColor.x, lightSpecularColor.y, lightSpecularColor.z);
	if (lightAmbientUniform != -1) glUniform3f(lightAmbientUniform, lightAmbientColor.x, lightAmbientColor.y, lightAmbientColor.z);
	if (lightAttenConstantUniform != -1) glUniform1f(lightAttenConstantUniform, lightAttenuation.x);
	if (lightAttenLinearUniform != -1) glUniform1f(lightAttenLinearUniform, lightAttenuation.y);
	if (lightAttenQuadraticUniform != -1) glUniform1f(lightAttenQuadraticUniform, lightAttenuation.z);

	//Transformation and Camera uniforms
	if (viewUniform != -1) glUniformMatrix4fv(viewUniform, 1, GL_FALSE, glm::value_ptr(GetCameraView()));
	if (projectionUniform != -1) glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, glm::value_ptr(GetCameraProjection()));
	if (modelUniform != -1) glUniformMatrix4fv(modelUniform, 1, GL_FALSE, glm::value_ptr(transform));
	vec3 camPos = GetCameraPosition();
	if (viewPositionUniform != -1) glUniform3f(viewPositionUniform, camPos.x, camPos.y, camPos.z);
	if (nearUniform != -1)  glUniform1f(nearUniform, GetCameraNear());
	if (farUniform != -1)  glUniform1f(farUniform, GetCameraFar());

	//Draw model, this will also assign texture maps
	//glUseProgram(shader);
	model->Draw(shader);
}

void SetLightPosition(glm::vec3 position)
{
	lightPosition = position;
}

void SetLightColor(glm::vec3 diffuse, glm::vec3 specular, glm::vec3 ambient)
{
	lightDiffuseColor = diffuse;
	lightSpecularColor = specular;
	lightAmbientColor = ambient;
}

void SetLightAttenuation(float constant, float linear, float quadratic)
{
	lightAttenuation = vec3(constant, linear, quadratic);
}


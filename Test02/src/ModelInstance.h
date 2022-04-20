#pragma once
#include <vector>
#include "glm/glm.hpp"
#include "Model.h"

class ModelInstance
{
private:
	Model* model;
	unsigned int shader;

	//Transform
	glm::mat4 transform;
	glm::vec3 position;
	glm::vec3 eulerRotation;
	glm::vec3 scale;

	//Shader addresses
	unsigned int modelUniform;
	unsigned int viewUniform;
	unsigned int projectionUniform;
	unsigned int viewPositionUniform;
	unsigned int nearUniform;
	unsigned int farUniform;

	unsigned int diffuseMapUniform;
	unsigned int specularMapUniform;
	unsigned int shininessUniform;

	unsigned int lightPositionUniform;
	unsigned int lightAmbientUniform;
	unsigned int lightDiffuseUniform;
	unsigned int lightSpecularUniform;
	unsigned int lightAttenConstantUniform;
	unsigned int lightAttenLinearUniform;
	unsigned int lightAttenQuadraticUniform;

	void SetUniformAddresses();
	void UpdateTransform();

public:
	ModelInstance(Model* model, unsigned int shader);
	ModelInstance(Model* model, unsigned int shader, glm::vec3 position);
	ModelInstance(Model* model, unsigned int shader, glm::vec3 position, glm::vec3 eulerRotation, glm::vec3 scale);

	void SetShader(unsigned int shader);
	unsigned int GetShader();

	void SetPosition(glm::vec3 position);
	void SetRotation(glm::vec3 eulerRotation);
	void SetScale(glm::vec3 scale);

	glm::vec3 GetPosition();
	glm::vec3 GetRotation();
	glm::vec3 GetScale();

	void Draw();

};

void SetLightPosition(glm::vec3 position);
void SetLightColor(glm::vec3 diffuse, glm::vec3 specular, glm::vec3 ambient);
void SetLightAttenuation(float constant, float linear, float quadratic);

#pragma once

struct Model
{
	unsigned int vao;
	unsigned int indexCount;
	unsigned int texture;
	unsigned int specMap;
};

struct ModelInstance
{
	unsigned int modelIndex;
	unsigned int shaderProgram = -1;

	//Shader uniforms
	unsigned int modelMatrixUniform = -1;
	unsigned int viewMatrixUniform = -1;
	unsigned int projMatrixUniform = -1;
	unsigned int textureUniform = -1;
	unsigned int colorUniform = -1;
	unsigned int lightColorUniform = -1;
	unsigned int lightPositionUniform = -1;
	unsigned int viewPositionUniform = -1;
	unsigned int timeUniform = -1;
	unsigned int indexUniform = -1;
	unsigned int ambientUniform = -1;
	unsigned int diffuseUniform = -1;
	unsigned int specularUniform = -1;
	unsigned int shininessUniform = -1;
	unsigned int diffuseMapUniform = -1;
	unsigned int specularMapUniform = -1;

	unsigned int lightAmbientUniform = -1;
	unsigned int lightDiffuseUniform = -1;
	unsigned int lightSpecularUniform = -1;

	unsigned int lightDirectionUniform = -1;

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale = glm::vec3(1.f);

	glm::mat4 transform;
	glm::vec3 color;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};


unsigned int CreateModel(float vertices[], int vertCount, unsigned int indices[], unsigned int indexCount, const char* texturePath, const char* specMapPath);
unsigned int CreateModelInstance(unsigned int modelIndex);
ModelInstance* GetModelInstance(unsigned int modelInstanceIndex);
void SetModelInstanceShader(unsigned int modelInstanceIndex, unsigned int shaderProgramID);

void SetInstancePosition(unsigned int modelInstanceIndex, glm::vec3 position);
void SetInstanceRotation(unsigned int modelInstanceIndex, glm::vec3 euler);
void SetInstanceScale(unsigned int modelInstanceIndex, glm::vec3 scale);

glm::vec3 GetInstancePosition(unsigned int modelInstanceIndex);
glm::vec3 GetInstanceRotation(unsigned int modelInstanceIndex);
glm::vec3 GetInstanceScale(unsigned int modelInstanceIndex);

void SetModelInstanceColor(unsigned int modelInstanceIndex, glm::vec3 color);
void SetLightPosition(glm::vec3 position);
void SetLightDirection(glm::vec3 direction);
void SetLightAmbientColor(glm::vec3 color);
void SetLightDiffuseColor(glm::vec3 color);
void SetLightSpecularColor(glm::vec3 color);

void DrawModelInstances();


#pragma once

struct Model
{
	unsigned int vao;
	unsigned int indexCount;
	unsigned int texture;
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

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale = glm::vec3(1.f);

	glm::mat4 transform;
	glm::vec3 color;
};


unsigned int CreateModel(float vertices[], int vertCount, unsigned int indices[], unsigned int indexCount, const char* texturePath);
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
void SetLightColor(glm::vec3 color);
void SetLightPosition(glm::vec3 position);

void DrawModelInstances();


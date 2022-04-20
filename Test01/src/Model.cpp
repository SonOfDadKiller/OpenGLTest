#include <glad.h>
#include <glfw3.h>
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/euler_angles.hpp"
#include <vector>
#include <iostream>
#include "Texture.h"
#include "Model.h"
#include "Camera.h"

using std::vector;
using glm::vec3;
using glm::vec4;
using glm::mat4;

static vector<Model> models;
static vector<ModelInstance> instances;
static vec3 lightAmbient;
static vec3 lightDiffuse;
static vec3 lightSpecular;
static vec3 lightPosition;
static vec3 lightDirection;
static vec3 lightAttenuation;
static float lightCutoff;
static float lightCutoffInner;

unsigned int CreateModel(float vertices[], int vertCount, unsigned int indices[], unsigned int indexCount, const char* texturePath, const char* specMapPath)
{
	//Generate and bind VAO
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//Generate, bind and fill VBO
	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertCount, vertices, GL_DYNAMIC_DRAW);

	//Generate, bind and fill EBO
	unsigned int ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount, indices, GL_STATIC_DRAW);

	//Set up vertex attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); //position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); //normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); //uv
	glEnableVertexAttribArray(2);

	//Unbind buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//Create modelIndex, load texture and return modelIndex index
	Model model;
	model.vao = vao;
	model.indexCount = indexCount;
	model.texture = LoadTexture(texturePath);
	model.specMap = LoadTexture(specMapPath);
	models.push_back(model);
	return models.size() - 1;
}

unsigned int CreateModelInstance(unsigned int modelIndex)
{
	ModelInstance instance;
	instance.modelIndex = modelIndex;
	instance.transform = mat4(1.f);
	instance.position = vec3(0.f);
	instance.rotation = vec3(0.f);
	instance.scale = vec3(1.f);
	instance.color = vec4(1.f);
	instances.push_back(instance);
	return instances.size() - 1;
}

ModelInstance* GetModelInstance(unsigned int modelInstanceIndex)
{
	return &instances[modelInstanceIndex];
}

void SetModelInstanceShader(unsigned int modelInstanceIndex, unsigned int shaderProgramID)
{
#ifdef _DEBUG
	//Check if model is valid
	if (modelInstanceIndex < 0 || modelInstanceIndex > instances.size() - 1)
	{
		std::cout << "Tried to reference non-existent model instance!!";
		return;
	}
#endif

	//Set shader program
	instances[modelInstanceIndex].shaderProgram = shaderProgramID;

	//Find uniforms
	instances[modelInstanceIndex].modelMatrixUniform = glGetUniformLocation(shaderProgramID, "model");
	instances[modelInstanceIndex].viewMatrixUniform = glGetUniformLocation(shaderProgramID, "view");
	instances[modelInstanceIndex].projMatrixUniform = glGetUniformLocation(shaderProgramID, "projection");
	instances[modelInstanceIndex].textureUniform = glGetUniformLocation(shaderProgramID, "main_tex");
	instances[modelInstanceIndex].colorUniform = glGetUniformLocation(shaderProgramID, "color");
	instances[modelInstanceIndex].lightColorUniform = glGetUniformLocation(shaderProgramID, "lightColor");
	instances[modelInstanceIndex].lightPositionUniform = glGetUniformLocation(shaderProgramID, "lightPos");
	instances[modelInstanceIndex].viewPositionUniform = glGetUniformLocation(shaderProgramID, "viewPos");
	instances[modelInstanceIndex].timeUniform = glGetUniformLocation(shaderProgramID, "time");
	instances[modelInstanceIndex].indexUniform = glGetUniformLocation(shaderProgramID, "index");

	instances[modelInstanceIndex].ambientUniform = glGetUniformLocation(shaderProgramID, "material.ambient");
	instances[modelInstanceIndex].diffuseUniform = glGetUniformLocation(shaderProgramID, "material.diffuse");
	instances[modelInstanceIndex].specularUniform = glGetUniformLocation(shaderProgramID, "material.specular");
	instances[modelInstanceIndex].shininessUniform = glGetUniformLocation(shaderProgramID, "material.shininess");
	instances[modelInstanceIndex].diffuseMapUniform = glGetUniformLocation(shaderProgramID, "material.diffuseMap");
	instances[modelInstanceIndex].specularMapUniform = glGetUniformLocation(shaderProgramID, "material.specularMap");

	instances[modelInstanceIndex].lightAmbientUniform = glGetUniformLocation(shaderProgramID, "light.ambient");
	instances[modelInstanceIndex].lightDiffuseUniform = glGetUniformLocation(shaderProgramID, "light.diffuse");
	instances[modelInstanceIndex].lightSpecularUniform = glGetUniformLocation(shaderProgramID, "light.specular");

	instances[modelInstanceIndex].lightDirectionUniform = glGetUniformLocation(shaderProgramID, "light.direction");
	instances[modelInstanceIndex].lightCutoffUniform = glGetUniformLocation(shaderProgramID, "light.cutoff");
	instances[modelInstanceIndex].lightCutoffInnerUniform = glGetUniformLocation(shaderProgramID, "light.cutoffInner");

	instances[modelInstanceIndex].lightAttenConstantUniform = glGetUniformLocation(shaderProgramID, "light.attenConstant");
	instances[modelInstanceIndex].lightAttenLinearUniform = glGetUniformLocation(shaderProgramID, "light.attenLinear");
	instances[modelInstanceIndex].lightAttenQuadraticUniform = glGetUniformLocation(shaderProgramID, "light.attenQuadratic");
}

void RecalculateTransform(ModelInstance* instance)
{
	//Calculate transform matrix
	instance->transform = mat4(1.f);
	instance->transform = glm::translate(instance->transform, instance->position);
	instance->transform = glm::scale(instance->transform, instance->scale);
	instance->transform *= glm::eulerAngleYXZ(instance->rotation.y, instance->rotation.x, instance->rotation.z);
}

void SetInstancePosition(unsigned int modelInstanceIndex, glm::vec3 position)
{
	ModelInstance* instance = GetModelInstance(modelInstanceIndex);
	instance->position = position;
	RecalculateTransform(instance);
}

void SetInstanceRotation(unsigned int modelInstanceIndex, glm::vec3 euler)
{
	ModelInstance* instance = GetModelInstance(modelInstanceIndex);
	instance->rotation = vec3(glm::radians(euler.x), glm::radians(euler.y), glm::radians(euler.z));
	RecalculateTransform(instance);
}

void SetInstanceScale(unsigned int modelInstanceIndex, glm::vec3 scale)
{
	ModelInstance* instance = GetModelInstance(modelInstanceIndex);
	instance->scale = scale;
	RecalculateTransform(instance);
}

glm::vec3 GetInstancePosition(unsigned int modelInstanceIndex)
{
	return GetModelInstance(modelInstanceIndex)->position;
}

glm::vec3 GetInstanceRotation(unsigned int modelInstanceIndex)
{
	return GetModelInstance(modelInstanceIndex)->rotation;
}

glm::vec3 GetInstanceScale(unsigned int modelInstanceIndex)
{
	return GetModelInstance(modelInstanceIndex)->scale;
}

void SetInstanceColor(unsigned int modelInstanceIndex, vec3 color)
{
	instances[modelInstanceIndex].color = color;
}

void SetLightAmbientColor(vec3 color)
{
	lightAmbient = color;
}

void SetLightDiffuseColor(vec3 color)
{
	lightDiffuse = color;
}

void SetLightSpecularColor(vec3 color)
{
	lightSpecular = color;
}

void SetLightPosition(vec3 position)
{
	lightPosition = position;
}

void SetLightDirection(vec3 direction)
{
	lightDirection = direction;
}

void SetLightAttenuation(float constant, float linear, float quadratic)
{
	lightAttenuation = vec3(constant, linear, quadratic);
}

void SetLightCutoffOuter(float angle)
{
	lightCutoff = angle;
}

void SetLightCutoffInner(float angle)
{
	lightCutoffInner = angle;
}

void DrawModelInstances()
{
	float time = glfwGetTime();

	for (auto it = instances.begin(); it != instances.end(); it++)
	{
		glUseProgram(it->shaderProgram);

		if (it->colorUniform != -1)
		{
			glUniform3f(it->colorUniform, it->color.x, it->color.y, it->color.z);
		}

		if (it->lightColorUniform != -1)
		{
			glUniform3f(it->lightColorUniform, lightDiffuse.x, lightDiffuse.y, lightDiffuse.z);
		}

		if (it->lightPositionUniform != -1)
		{
			glUniform3f(it->lightPositionUniform, lightPosition.x, lightPosition.y, lightPosition.z);
		}

		if (it->lightDirectionUniform != -1)
		{
			glUniform3f(it->lightDirectionUniform, lightDirection.x, lightDirection.y, lightDirection.z);
		}

		if (it->lightCutoffUniform != -1)
		{
			glUniform1f(it->lightCutoffUniform, lightCutoff);
		}

		if (it->lightCutoffInnerUniform != -1)
		{
			glUniform1f(it->lightCutoffInnerUniform, lightCutoffInner);
		}

		if (it->viewPositionUniform != -1)
		{
			vec3 camPos = GetCameraPosition();
			glUniform3f(it->viewPositionUniform, camPos.x, camPos.y, camPos.z);
		}
		
		if (it->textureUniform != -1)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, models[it->modelIndex].texture);
			glUniform1i(it->textureUniform, 0);
		}

		if (it->timeUniform != -1)
		{
			glUniform1f(it->timeUniform, time);
		}

		if (it->indexUniform != -1)
		{
			glUniform1i(it->indexUniform, it - instances.begin());
		}

		if (it->ambientUniform != -1)
		{
			glUniform3f(it->ambientUniform, it->ambient.x, it->ambient.y, it->ambient.z);
		}

		if (it->diffuseUniform != -1)
		{
			glUniform3f(it->diffuseUniform, it->diffuse.x, it->diffuse.y, it->diffuse.z);
		}

		if (it->specularUniform != -1)
		{
			glUniform3f(it->specularUniform, it->specular.x, it->specular.y, it->specular.z);
		}

		if (it->shininessUniform != -1)
		{
			glUniform1f(it->shininessUniform, it->shininess);
		}

		if (it->diffuseMapUniform != -1)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, models[it->modelIndex].texture);
			glUniform1i(it->diffuseMapUniform, 0);
		}

		if (it->specularMapUniform != -1)
		{
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, models[it->modelIndex].specMap);
			glUniform1i(it->specularMapUniform, 1);
		}

		if (it->lightAmbientUniform != -1)
		{
			glUniform3f(it->lightAmbientUniform, lightAmbient.x, lightAmbient.y, lightAmbient.z);
		}

		if (it->lightDiffuseUniform != -1)
		{
			glUniform3f(it->lightDiffuseUniform, lightDiffuse.x, lightDiffuse.y, lightDiffuse.z);
		}

		if (it->lightSpecularUniform != -1)
		{
			glUniform3f(it->lightSpecularUniform, lightSpecular.x, lightSpecular.y, lightSpecular.z);
		}

		if (it->lightAttenConstantUniform != -1)
		{
			glUniform1f(it->lightAttenConstantUniform, lightAttenuation.x);
		}

		if (it->lightAttenLinearUniform != -1)
		{
			glUniform1f(it->lightAttenLinearUniform, lightAttenuation.y);
		}

		if (it->lightAttenQuadraticUniform != -1)
		{
			glUniform1f(it->lightAttenQuadraticUniform, lightAttenuation.z);
		}
		
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, models[it->modelIndex].texture);

		//Set view and projection matrices
		glUniformMatrix4fv(it->viewMatrixUniform, 1, GL_FALSE, glm::value_ptr(GetCameraView()));
		glUniformMatrix4fv(it->projMatrixUniform, 1, GL_FALSE, glm::value_ptr(GetCameraProjection()));

		//Bind and draw
		glUniformMatrix4fv(it->modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(it->transform));
		glBindVertexArray(models[it->modelIndex].vao);
		glDrawElements(GL_TRIANGLES, models[it->modelIndex].indexCount, GL_UNSIGNED_INT, 0);
	}

	//Unbind
	glBindVertexArray(0);
}

#include <iostream>
#include <glad.h>
#include <glfw3.h>
#include <vector>
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "ModelInstance.h"
#include "Meshes.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/color_space.hpp"
#include "glm/gtc/random.hpp"

#include "assimp/scene.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;

#define WINDOW_WIDTH 1440
#define WINDOW_HEIGHT 1080

void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void ProcessInput(GLFWwindow* window, float dt)
{
	//Close window if escape key is pressed
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	//Camera controls
	float cameraMoveSpeed = 1.f * dt;

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		cameraMoveSpeed *= 2.f;
	}

	//Forward/Back
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		TranslateCamera(GetCameraForward() * cameraMoveSpeed);
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		TranslateCamera(-GetCameraForward() * cameraMoveSpeed);
	}
	
	//Left/Right
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		TranslateCamera(-glm::normalize(glm::cross(GetCameraForward(), GetCameraUp())) * cameraMoveSpeed);
	}
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		TranslateCamera(glm::normalize(glm::cross(GetCameraForward(), GetCameraUp())) * cameraMoveSpeed);
	}

	//Up/Down
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		TranslateCamera(GetCameraUp() * cameraMoveSpeed);
	}
	else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		TranslateCamera(-GetCameraUp() * cameraMoveSpeed);
	}
}

static float prevMouseX = WINDOW_WIDTH / 2.f;
static float prevMouseY = WINDOW_HEIGHT / 2.f;
static bool firstMouseFrame = true;

void MousePositionCallback(GLFWwindow* window, double mouseX, double mouseY)
{
	if (firstMouseFrame)
	{
		prevMouseX = mouseX;
		prevMouseY = mouseY;
		firstMouseFrame = false;
	}

	float mouseDeltaX = mouseX - prevMouseX;
	float mouseDeltaY = mouseY - prevMouseY;
	prevMouseX = mouseX;
	prevMouseY = mouseY;

	const float sensitivity = 0.1f;

	//Affect camera
	RotateCamera(vec3(-mouseDeltaY, mouseDeltaX, 0.f) * sensitivity);
	ClampCameraPitch(-89.f, 89.f);
}

void MouseScrollCallback(GLFWwindow* window, double scrollX, double scrollY)
{
	SetCameraFOV(GetCameraFOV() - (float)scrollY);
}

int main()
{
	//aiScene scene;

	//Initialize GLFW and create window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Test01 - Lighting", nullptr, nullptr);

	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window\n";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	//Initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD\n";
		return -1;
	}

	//Set up viewport
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	//Hook up window size change callback
	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

	//Setup mouse input
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, MousePositionCallback);
	glfwSetScrollCallback(window, MouseScrollCallback);

	//Initialize game systems
	InitializeCamera();

	//Time keeping
	float deltaTime = 0.f;
	float previousTime = 0.f;

	//Set up camera
	SetCameraPosition(vec3(2.f, 1.f, 2.f));
	SetCameraRotation(vec3(-20.f, 230.f, 0.f));
	SetCameraFOV(75.f);
	SetCameraNearFar(0.1f, 50.f);

	//Load shaders
	unsigned int testShader = CreateShaderProgram("shaders/test.vert", "shaders/test.frag");
	unsigned int colorShader = CreateShaderProgram("shaders/color.vert", "shaders/color.frag");
	unsigned int phongShader = CreateShaderProgram("shaders/tex_phong.vert", "shaders/tex_phong.frag");
	unsigned int gouraudShader = CreateShaderProgram("shaders/tex_gouraud.vert", "shaders/tex_gouraud.frag");
	unsigned int materialShader = CreateShaderProgram("shaders/tex_material.vert", "shaders/tex_material.frag");
	unsigned int materialMapShader = CreateShaderProgram("shaders/tex_material_map.vert", "shaders/tex_material_map.frag");
	unsigned int materialMapDirectionalShader = CreateShaderProgram("shaders/tex_material_map_directional.vert", "shaders/tex_material_map_directional.frag");
	unsigned int materialMapPointShader = CreateShaderProgram("shaders/tex_material_map_point.vert", "shaders/tex_material_map_point.frag");
	unsigned int materialMapSpotShader = CreateShaderProgram("shaders/tex_material_map_spot.vert", "shaders/tex_material_map_spot.frag");
	unsigned int materialMapMultiLightShader = CreateShaderProgram("shaders/tex_material_map_multi_light.vert", "shaders/tex_material_map_multi_light.frag");
	unsigned int foliageShader = CreateShaderProgram("shaders/foliage.vert", "shaders/foliage.frag");
	
	//Default draw settings
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	glEnable(GL_STENCIL_TEST);
	glStencilMask(0x00);
	glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
	glEnable(GL_BLEND);

	//Load models
	Model sponzaModel("Models/sponza/sponza.obj");
	ModelInstance sponza(&sponzaModel, materialMapPointShader);
	sponza.SetScale(vec3(0.01f, 0.01f, 0.01f));

	Model backpackModel("Models/backpack/backpack.obj");
	ModelInstance backpack(&backpackModel, materialMapPointShader);

	//Set up grass
	Model grassModel(quadVerts, sizeof(quadVerts) / 8 / 4, quadIndices, sizeof(quadIndices) / 4);
	Texture grassTexture(Texture::Diffuse, LoadTexture("textures/grass.png", GL_CLAMP_TO_EDGE));
	grassModel.meshes[0].textures.push_back(grassTexture);

	std::vector<ModelInstance> foliage;
	
	for (int i = 0; i < 1000; i++)
	{
		ModelInstance grass(&grassModel, foliageShader);
		grass.SetPosition(vec3(glm::linearRand(-14.f, 14.f), 0.f, glm::linearRand(-7.f, 7.f)));
		grass.SetRotation(vec3(0.f, glm::linearRand(0.f, 360.f), 0.f));
		grass.SetScale(vec3(0.5f));
		foliage.push_back(grass);
	}

	Model monkeyModel("Models/smooth_monke.obj");
	//Model monkeyModel(cubeVerts, sizeof(cubeVerts) / 8 / 4, cubeIndices, sizeof(cubeIndices) / 4);
	ModelInstance monkey(&monkeyModel, materialMapPointShader);
	monkey.SetPosition(vec3(-3.f, 1.f, 0.f));
	monkey.SetScale(vec3(0.35f));

	//Add texture manually
	Texture monkeyTexture(Texture::Diffuse, LoadTexture("textures/test.png"));
	monkeyModel.meshes[0].textures.push_back(monkeyTexture);

	//Set up light
	SetLightPosition(vec3(2.f, 2.f, 2.f));
	SetLightColor(vec3(1.f, 0.3f, 0.2f), vec3(0.5f, 0.7f, 0.3f), vec3(0.3f));
	SetLightAttenuation(1.f, 0.09f, 0.032f);

	//Update loop
	while (!glfwWindowShouldClose(window))
	{
		//Track time
		float time = glfwGetTime();
		deltaTime = time - previousTime;
		previousTime = time;

		//Update
		ProcessInput(window, deltaTime);
		monkey.SetPosition(vec3(sin(time / 2.5f) * 8.f, 1.2f, 0.f));
		monkey.SetRotation(vec3(0.f, time * 90.f, 0.f));
		SetLightPosition(monkey.GetPosition());

		//Clear
		glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		//Disable writing to stencil buffer
		glStencilMask(0x00);

		//Draw
		sponza.Draw();

		for (ModelInstance grass : foliage)
		{
			grass.Draw();
		}
		

		//Draw monkey with stencil outline
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
		monkey.Draw();

		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00); // disable writing to the stencil buffer
		glDisable(GL_DEPTH_TEST);
		vec3 oldScale = monkey.GetScale();
		unsigned int oldShader = monkey.GetShader();
		monkey.SetShader(colorShader);
		monkey.Draw();
		monkey.SetShader(oldShader);
		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glEnable(GL_DEPTH_TEST);


		//glDepthMask(GL_FALSE);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//Cleanup
	CleanupShaders();

	//Clean up GLFW
	glfwTerminate();

	return 0;
}

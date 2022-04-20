#include <iostream>
#include <glad.h>
#include <glfw3.h>
#include <vector>
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Meshes.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/color_space.hpp"

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
	SetCameraPosition(vec3(-2.f, 1.f, -2.f));
	SetCameraRotation(vec3(-20.f, 45.f, 0.f));
	SetCameraFOV(75.f);
	SetCameraNearFar(0.01f, 300.f);

	//Load shaders
	unsigned int colorShader = CreateShaderProgram("shaders/color.vert", "shaders/color.frag");
	unsigned int phongShader = CreateShaderProgram("shaders/tex_phong.vert", "shaders/tex_phong.frag");
	unsigned int gouraudShader = CreateShaderProgram("shaders/tex_gouraud.vert", "shaders/tex_gouraud.frag");
	unsigned int materialShader = CreateShaderProgram("shaders/tex_material.vert", "shaders/tex_material.frag");
	unsigned int materialMapShader = CreateShaderProgram("shaders/tex_material_map.vert", "shaders/tex_material_map.frag");
	unsigned int materialMapDirectionalShader = CreateShaderProgram("shaders/tex_material_map_directional.vert", "shaders/tex_material_map_directional.frag");
	unsigned int materialMapPointShader = CreateShaderProgram("shaders/tex_material_map_point.vert", "shaders/tex_material_map_point.frag");
	unsigned int materialMapSpotShader = CreateShaderProgram("shaders/tex_material_map_spot.vert", "shaders/tex_material_map_spot.frag");
	unsigned int materialMapMultiLightShader = CreateShaderProgram("shaders/tex_material_map_multi_light.vert", "shaders/tex_material_map_multi_light.frag");


	//Default draw settings
	glEnable(GL_DEPTH_TEST);

	//Load models
	unsigned int cube = CreateModel(cubeVerts, sizeof(cubeVerts), cubeIndices, sizeof(cubeIndices), "textures/container2.png", "textures/container2_specular.png");

	//lightingShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
	//lightingShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f); // darken diffuse light a bit
	//lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

	//Set up light
	unsigned int lightCube = CreateModelInstance(cube);
	vec3 lightPosition(1.2f, 1.0f, 2.0f);
	vec3 lightColor(0.5f);
	SetLightDiffuseColor(lightColor);
	SetLightAmbientColor(vec3(0.2f));
	SetLightSpecularColor(vec3(1.f));
	SetLightPosition(lightPosition);
	SetLightDirection(vec3(0.2f, -0.2f, 0.6f));
	SetLightCutoffInner(glm::cos(glm::radians(15.5f)));
	SetLightCutoffOuter(glm::cos(glm::radians(25.5f)));
	SetLightAttenuation(1.f, 0.07f, 0.017f);
	SetModelInstanceShader(lightCube, colorShader);
	SetInstanceColor(lightCube, lightColor);

	SetInstancePosition(lightCube, lightPosition);
	SetInstanceScale(lightCube, vec3(0.2f, 0.2f, 0.2f));

	//Set up cubes
	std::vector<unsigned int> cubes;
	int cubefieldSize = 4;

	for (int x = 0; x < cubefieldSize; x++)
	{
		for (int y = 0; y < cubefieldSize; y++)
		{
			for (int z = 0; z < cubefieldSize; z++)
			{
				unsigned int cubeInstance = CreateModelInstance(cube);
				SetModelInstanceShader(cubeInstance, materialMapPointShader);
				SetInstancePosition(cubeInstance, (vec3(x, y, z) - (float)cubefieldSize / 2.f) * 2.f);
				//SetModelInstanceColor(cubeInstance, vec4(1.0f, 1.0f, 1.0f, 1.0f));
				ModelInstance* p_cubeInstance = GetModelInstance(cubeInstance);
				p_cubeInstance->ambient = vec3(1.f, 0.5f, 0.31f);
				//p_cubeInstance->diffuse = vec3(1.f, 0.5f, 0.31f);
				p_cubeInstance->specular = vec3(0.06f);
				p_cubeInstance->shininess = 16.f;
				cubes.push_back(cubeInstance);
			}
		}
	}

	float lightHue = 0.f;

	//Update loop
	while (!glfwWindowShouldClose(window))
	{
		//Track time
		float time = glfwGetTime();
		deltaTime = time - previousTime;
		previousTime = time;

		//Update
		ProcessInput(window, deltaTime);
		lightPosition = vec3(sin(time) * 5.5f, sin(time * 0.5f) * 1.5f, cos(time) * 5.5f);
		SetInstancePosition(lightCube, lightPosition);
		SetLightPosition(lightPosition);

		//Make spotlight follow camera
		/*lightPosition = GetCameraPosition();
		SetLightPosition(lightPosition);
		SetInstanceScale(lightCube, vec3(0.f));
		SetLightDirection(GetCameraForward());*/

		vec3 lightColor = glm::rgbColor(vec3(lightHue, 0.35f, 1.0f));
		lightHue = lightHue < 360.f ? lightHue + deltaTime * 20.f : 0.f;
		SetLightDiffuseColor(lightColor);
		SetLightAmbientColor(lightColor / 5.f);
		SetLightSpecularColor(lightColor);
		SetInstanceColor(lightCube, lightColor);

		//Rotate cubes
		auto it = cubes.begin();

		while (it != cubes.end())
		{
			SetInstanceRotation(*it, vec3(time * 15.f, sin(time * 0.15f) * 90.f, time * 10.f));
			it++;
		}
		

		//Clear
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Draw
		DrawModelInstances();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//Cleanup
	CleanupShaders();

	//Clean up GLFW
	glfwTerminate();

	return 0;
}

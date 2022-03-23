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
	unsigned int colorVertShader = CreateShader(Vertex, "shaders/color.vert");
	unsigned int colorFragShader = CreateShader(Fragment, "shaders/color.frag");
	unsigned int colorShader = CreateShaderProgram(colorVertShader, colorFragShader);

	unsigned int texPhongVert = CreateShader(Vertex, "shaders/tex_phong.vert");
	unsigned int texPhongFrag = CreateShader(Fragment, "shaders/tex_phong.frag");
	unsigned int phongShader = CreateShaderProgram(texPhongVert, texPhongFrag);

	unsigned int texGouraudVert = CreateShader(Vertex, "shaders/tex_gouraud.vert");
	unsigned int texGouraudFrag = CreateShader(Fragment, "shaders/tex_gouraud.frag");
	unsigned int gouraudShader = CreateShaderProgram(texGouraudVert, texGouraudFrag);
	
	//Default draw settings
	glEnable(GL_DEPTH_TEST);

	//Load models
	unsigned int cube = CreateModel(cubeVerts, sizeof(cubeVerts), cubeIndices, sizeof(cubeIndices), "textures/wood_new_lowres_00.png");

	//Set up light
	unsigned int light = CreateModelInstance(cube);
	vec3 lightPosition(1.2f, 1.0f, 2.0f);
	vec3 lightColor(1.f, 1.f, 1.f);
	SetLightColor(lightColor);
	SetLightPosition(lightPosition);
	SetModelInstanceShader(light, colorShader);
	SetModelInstanceColor(light, lightColor);

	SetInstancePosition(light, lightPosition);
	SetInstanceScale(light, vec3(0.2f, 0.2f, 0.2f));

	//Set up cube
	unsigned int cubeInstance = CreateModelInstance(cube);
	SetModelInstanceShader(cubeInstance, gouraudShader);
	SetModelInstanceColor(cubeInstance, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	//Update loop
	while (!glfwWindowShouldClose(window))
	{
		//Track time
		float time = glfwGetTime();
		deltaTime = time - previousTime;
		previousTime = time;

		//Update
		ProcessInput(window, deltaTime);
		lightPosition = vec3(sin(time) * 1.3f, sin(time * 0.5f), cos(time) * 1.3f);
		SetInstancePosition(light, lightPosition);
		SetInstanceRotation(cubeInstance, vec3(time * 15.f, sin(time * 0.5f) * 90.f, time * 10.f));
		SetLightPosition(lightPosition);
		vec3 lightColor = glm::rgbColor(vec3(128.f + sin(time / 5.f) * 128.f, 1.f, 1.f));
		SetLightColor(lightColor);
		SetModelInstanceColor(light, lightColor);

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

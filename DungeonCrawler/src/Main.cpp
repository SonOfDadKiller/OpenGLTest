#include <iostream>
#include <glad.h>
#include <glfw3.h>
#include <vector>
#include "Shader.h"
#include "Camera.h"
#include "Map.h"
#include "Player.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

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

	//Player controls
	//Forward/Back
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		MovePlayer(0);
	}
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		MovePlayer(1);
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		MovePlayer(2);
	}
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		MovePlayer(3);
	}
	else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		RotatePlayer(true);
	}
	else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		RotatePlayer(false);
	}
	

	////Camera controls
	//const float cameraMoveSpeed = 5.f * dt;

	////Forward/Back
	//if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	//{
	//	TranslateCamera(GetCameraForward() * cameraMoveSpeed);
	//}
	//else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	//{
	//	TranslateCamera(-GetCameraForward() * cameraMoveSpeed);
	//}
	//
	////Left/Right
	//if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	//{
	//	TranslateCamera(-glm::normalize(glm::cross(GetCameraForward(), GetCameraUp())) * cameraMoveSpeed);
	//}
	//else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	//{
	//	TranslateCamera(glm::normalize(glm::cross(GetCameraForward(), GetCameraUp())) * cameraMoveSpeed);
	//}

	////Up/Down
	//if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	//{
	//	TranslateCamera(GetCameraUp() * cameraMoveSpeed);
	//}
	//else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	//{
	//	TranslateCamera(-GetCameraUp() * cameraMoveSpeed);
	//}
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
	RotateCamera(glm::vec3(-mouseDeltaY, mouseDeltaX, 0.f) * sensitivity);
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

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Dungeon Crawler", nullptr, nullptr);

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
	InitializeMap();
	LoadLevel("levels/00.txt");
	InitializePlayer(0, 0, 0.f);

	//Time keeping
	float deltaTime = 0.f;
	float previousTime = 0.f;

	//Set up camera
	SetCameraFOV(75.f);
	SetCameraNearFar(0.1f, 300.f);
	
	//Default draw settings
	glEnable(GL_DEPTH_TEST);

	//Update loop
	while (!glfwWindowShouldClose(window))
	{
		float time = glfwGetTime();
		deltaTime = time - previousTime;
		previousTime = time;
		
		ProcessInput(window, deltaTime);
		TickPlayer(deltaTime);

		//Clear
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Draw
		DrawMap();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//Cleanup
	CleanupShaders();

	//Clean up GLFW
	glfwTerminate();

	return 0;
}
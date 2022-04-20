#include <iostream>
#include <glad.h>
#include <glfw3.h>
#include <vector>
#include "Shader.h"
#include "Camera.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define WINDOW_WIDTH 1440
#define WINDOW_HEIGHT 1080

float triVerts[] = {
   -0.5f, -0.5f, 0.0f,
	0.0f,  0.5f, 0.0f,
	0.5f, -0.5f, 0.0f
};

float doubleTriVerts[] = {
	//Tri 1
	-0.9f, -0.3f, 0.0f,
	-0.1f, -0.3f, 0.0f,
	-0.5f,  0.3f, 0.0f,
	//Tri 2
	0.9f, -0.3f, 0.0f,
	0.1f, -0.3f, 0.0f,
	0.5f,  0.3f, 0.0f
};

float quadVerts[] = {
	0.5f,  0.5f, 0.0f,  // top right
	0.5f, -0.5f, 0.0f,  // bottom right
   -0.5f, -0.5f, 0.0f,  // bottom left
   -0.5f,  0.5f, 0.0f   // top left 
};

float texQuadVerts[] = {
	// positions          // colors           // texture coordinates
	 1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
	 1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
	-1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
	-1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
};

float cubeVerts[] = {
	// positions		  // colors			 // UVs
	-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f
};

unsigned int quadIndices[] = {
	0, 1, 3,
	1, 2, 3
};

float coloredTriVerts[] = {
	// positions         // colors
	 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
	-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
	 0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
};

float coloredTriUVs[] = {
	1.0f, 0.0f, // bottom right
	0.0f, 0.0f, // bottom left
	0.5f, 1.0f  // top
};

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
	const float cameraMoveSpeed = 10.f * dt;

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
	RotateCamera(glm::vec3(-mouseDeltaY, mouseDeltaX, 0.f) * sensitivity);
	ClampCameraPitch(-89.f, 89.f);
}

void MouseScrollCallback(GLFWwindow* window, double scrollX, double scrollY)
{
	SetCameraFOV(GetCameraFOV() - (float)scrollY);

	
}

unsigned int RegisterVAO(float vertices[], unsigned int size)
{
	//Set up vertex array object and vertex buffer objects
	unsigned int vaoID, vboID;
	glGenVertexArrays(1, &vaoID);
	glGenBuffers(1, &vboID);

	//Bind VAO
	glBindVertexArray(vaoID);

	//Bind VBO and copy the vertices into it
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

	//Set up vertex attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); //Position
	glEnableVertexAttribArray(0);

	//Unbind VAO, then unbind VBO (order does not matter, VBO is bound to the VAO and can be unbound after glVertexAttribPointer is called)
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return vaoID;
}

unsigned int RegisterVAO(float vertices[], unsigned int vertArraySize, unsigned int indices[], unsigned int indexArraySize)
{
	//Set up vertex array object and element buffer objects
	unsigned int vaoID, vboID, eboID;
	glGenVertexArrays(1, &vaoID);
	glGenBuffers(1, &vboID);
	glGenBuffers(1, &eboID);

	//Bind VAO
	glBindVertexArray(vaoID);

	//Bind VBO and copy the vertices into it
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, vertArraySize, vertices, GL_STATIC_DRAW);

	//Bind EBO and copy the indices into it
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexArraySize, indices, GL_STATIC_DRAW);

	//Set up vertex attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); //Position
	glEnableVertexAttribArray(0);

	//Unbind VAO, then unbind VBO and EBO (order is important in regards to EBO!)
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return vaoID;
}

std::vector<struct Shape*> shapes;

struct Shape
{
	unsigned int vaoID;
	unsigned int vboID;
	unsigned int vertexCount;
	unsigned int shaderProgram;

	Shape(float vertices[], const int arraySize, unsigned int shaderProgram, float xOffset, float yOffset, float zOffset, float scale)
	{
		vertexCount = arraySize;
		this->shaderProgram = shaderProgram;

		//Set up vertex array object and vertex buffer objects
		vaoID = vboID = 0;
		glGenVertexArrays(1, &vaoID);
		glGenBuffers(1, &vboID);

		//Bind VAO
		glBindVertexArray(vaoID);

		//Offset vertices
		std::vector<float> newVerts;
		
		for (int i = 0; i < arraySize; i += 3)
		{
			newVerts.push_back(vertices[i] * scale + xOffset);
			newVerts.push_back(vertices[i + 1] * scale + yOffset);
			newVerts.push_back(vertices[i + 2] * scale + zOffset);
		}

		//Bind VBO and copy the vertices into it
		glBindBuffer(GL_ARRAY_BUFFER, vboID);
		glBufferData(GL_ARRAY_BUFFER, arraySize, newVerts.data(), GL_STATIC_DRAW);

		//Set up vertex attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); //Position
		glEnableVertexAttribArray(0);

		//Unbind VAO, then unbind VBO (order does not matter, VBO is bound to the VAO and can be unbound after glVertexAttribPointer is called)
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void Draw()
	{
		glUseProgram(shaderProgram);
		glBindVertexArray(vaoID);
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
		glBindVertexArray(0);
	}

	~Shape()
	{
		//std::cout << "Shape deleted\n";
		glDeleteVertexArrays(1, &vaoID);
		glDeleteBuffers(1, &vboID);
	}
};

int main()
{
	//Vector test
	/*glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
	glm::mat4 translation(1.0f);
	translation = glm::translate(translation, glm::vec3(1.0f, 1.0f, 0.0f));
	vec = translation * vec;
	std::cout << vec.x << ", " << vec.y << ", " << vec.z << std::endl;*/

	//Camera test
	//position = glm::vec3(1.0f);
	InitializeCamera();

	//std::cout << cameraPosition.x << ", " << cameraPosition.y << ", " << cameraPosition.z << std::endl;

	//position = 0;
	

	//Initialize GLFW and create window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Test00", nullptr, nullptr);

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



	unsigned int triangleID = RegisterVAO(doubleTriVerts, sizeof(doubleTriVerts));
	unsigned int quadID = RegisterVAO(quadVerts, sizeof(quadVerts), quadIndices, sizeof(quadIndices));



	//Create textured quad
	//Set up vertex array object and element buffer objects
	unsigned int texQuadVAO, texQuadVBO, texQuadEBO;
	glGenVertexArrays(1, &texQuadVAO);
	glGenBuffers(1, &texQuadVBO);
	glGenBuffers(1, &texQuadEBO);

	//Bind VAO
	glBindVertexArray(texQuadVAO);

	//Bind VBO and copy the vertices into it
	glBindBuffer(GL_ARRAY_BUFFER, texQuadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texQuadVerts), texQuadVerts, GL_STATIC_DRAW);

	//Bind EBO and copy the indices into it
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, texQuadEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

	//Set up vertex attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); //Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); //Color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); //UV
	glEnableVertexAttribArray(2);

	//Unbind VAO, then unbind VBO and EBO (order is important in regards to EBO!)
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);



	//Create cube
	unsigned int cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);

	//Bind VAO
	glBindVertexArray(cubeVAO);

	//Bind VBO and copy the vertices into it
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_DYNAMIC_DRAW);

	//Set up vertex attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); //Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); //Color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); //UV
	glEnableVertexAttribArray(2);

	//Unbind VAO, then unbind VBO (order does not matter, VBO is bound to the VAO and can be unbound after glVertexAttribPointer is called)
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);



	//Init shaders
	unsigned int basicVertShader = CreateShader(VertShader, "shaders/basic.vertShader");
	unsigned int colorVertShader = CreateShader(VertShader, "shaders/color.vertShader");
	unsigned int textureVertShader = CreateShader(VertShader, "shaders/texture.vertShader");
	unsigned int basicFragShader = CreateShader(FragShader, "shaders/basic.fragShader");
	unsigned int textureFragShader = CreateShader(FragShader, "shaders/texture.fragShader");
	unsigned int orangeShader = CreateShaderProgram(basicVertShader, basicFragShader);
	unsigned int colorShader = CreateShaderProgram(colorVertShader, basicFragShader);
	unsigned int textureShader = CreateShaderProgram(textureVertShader, textureFragShader);

	glUseProgram(colorShader);
	int vertColorUniform = glGetUniformLocation(colorShader, "myColor");
	glUniform4f(vertColorUniform, 1.0f, 1.0, 1.0f, 1.0f);
	int vertOffsetUniform = glGetUniformLocation(colorShader, "offset");
	glUniform3f(vertOffsetUniform, 0.0f, 0.0f, 0.0f);

	//Register textures with openGL
	unsigned int crateTexture, spyroTexture;
	glGenTextures(1, &crateTexture);
	glBindTexture(GL_TEXTURE_2D, crateTexture);

	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Load crate image from file
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* imageData = stbi_load("hank.jpg", &width, &height, &nrChannels, 0);

	if (imageData)
	{
		//Bind image to texture
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Image load failed!" << std::endl;
	}

	//Free image
	stbi_image_free(imageData);

	glGenTextures(1, &spyroTexture);
	glBindTexture(GL_TEXTURE_2D, spyroTexture);

	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	//Load spyro image from file
	
	int width2, height2, nrChannels2;
	imageData = stbi_load("hank2.jpg", &width2, &height2, &nrChannels2, 0);

	if (imageData)
	{
		//Bind image to texture
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width2, height2, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Image load failed!" << std::endl;
	}

	//Free image
	stbi_image_free(imageData);

	//Init shapes vector
	{


		int width = 20;
		int height = 20;
		float halfWidth = width / 2.f;
		float halfHeight = height / 2.f;
		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				bool even = (((x + y) % 2) == 0);
				unsigned int sp = orangeShader;
				shapes.push_back(new Shape(triVerts, sizeof(triVerts), sp, (x - (halfWidth - 0.5f)) / halfWidth, (y - (halfHeight - 0.5f)) / halfHeight, 0.f, (2.f / width) * (even ? 1.f : -1.f)));
			}
		}
	}


	//int nrAttributes;
	//glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	//std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;


	//Set polygon mode
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBindVertexArray(cubeVAO);
	glUseProgram(textureShader);

	glEnable(GL_DEPTH_TEST);


	unsigned int mainTexLocation = glGetUniformLocation(textureShader, "mainTex");
	unsigned int secondTexLocation = glGetUniformLocation(textureShader, "secondTex");
	unsigned int timeLocation = glGetUniformLocation(textureShader, "time");
	unsigned int modelLocation = glGetUniformLocation(textureShader, "model");
	unsigned int viewLocation = glGetUniformLocation(textureShader, "view");
	unsigned int projectionLocation = glGetUniformLocation(textureShader, "projection");

	//Activate textures and assign to sampler2D uniforms in the shader
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, crateTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, spyroTexture);
	glUniform1i(mainTexLocation, 0);
	glUniform1i(secondTexLocation, 1);

	


	//Set up projection
	SetCameraFOV(45.f);
	SetCameraNearFar(0.1f, 300.f);
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(GetCameraProjection()));
	

#define CUBE_ARRAY_WIDTH 20
#define CUBE_ARRAY_HEIGHT 20
#define CUBE_ARRAY_SPACING 1.75f

	//Spawn cubes
	glm::mat4 cubeTransforms[CUBE_ARRAY_WIDTH * CUBE_ARRAY_HEIGHT];
	for (int x = 0; x < CUBE_ARRAY_WIDTH; x++)
	{
		for (int y = 0; y < CUBE_ARRAY_HEIGHT; y++)
		{
			int index = x * CUBE_ARRAY_WIDTH + y;
			//std::cout << "Cube Array Index: " << index << std::endl;
			cubeTransforms[index] = glm::mat4(1.0f);
			cubeTransforms[index] = glm::translate(cubeTransforms[index], glm::vec3(
				x * CUBE_ARRAY_SPACING - (CUBE_ARRAY_WIDTH / 2.f) * CUBE_ARRAY_SPACING, 
				y * CUBE_ARRAY_SPACING - (CUBE_ARRAY_HEIGHT / 2.f) * CUBE_ARRAY_SPACING, 
				0.f));
		}
	}

	//Time keeping
	float deltaTime = 0.f;
	float previousTime = 0.f;

	//Update loop
	while (!glfwWindowShouldClose(window))
	{
		float time = glfwGetTime();
		deltaTime = time - previousTime;
		previousTime = time;
		
		ProcessInput(window, deltaTime);

		//Clear
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Set time uniform
		glUniform1f(timeLocation, time);

		//Generate transformation matrices
		//glm::mat4 view = glm::mat4(1.0f);
		//view = glm::translate(view, glm::vec3(0.0f, 0.0f, -22.f + sin(time / 5.f) * 17.f));
		//view = glm::rotate(view, glm::radians(sin(time / 3.f) * 45.f), glm::vec3(0.0f, 1.0f, 0.0f));
		//view = glm::rotate(view, glm::radians(cos(time / 3.f) * 45.f), glm::vec3(1.0f, 0.0f, 0.0f));


		const float radius = 10.0f;
		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;
		//glm::mat4 view;
		//view = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
		

		//Assign transformation matrices to shader
		//glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

		//Update view and projection matrices
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(GetCameraView()));
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(GetCameraProjection()));
		

		//Draw
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		for (int i = 0; i < CUBE_ARRAY_WIDTH * CUBE_ARRAY_HEIGHT; i++)
		{
			//glm::mat4 model = cubeTransforms[i];
			//glm::mat4 model
			glm::mat4 model = glm::rotate(cubeTransforms[i], sin(sin(sin(sin(glm::radians(i / CUBE_ARRAY_WIDTH * 15.f) + glm::radians(-55.0f) * time)))) * 8.f, glm::vec3(0.5f, 1.0f, 0.0f));
			//model = glm::rotate(model, sin(sin(sin(sin(glm::radians(i / CUBE_ARRAY_HEIGHT * 15.f) + glm::radians(-55.0f) * time*2.f)))) * 8.f, glm::vec3(0.5f, 1.0f, 0.0f));
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//Clean up openGL resources
	glDeleteVertexArrays(1, &triangleID);
	glDeleteVertexArrays(1, &quadID);

	for (Shape* shape : shapes)
	{
		delete shape;
	}
	shapes.clear();

	CleanupShaders();

	//Clean up GLFW
	glfwTerminate();

	return 0;
}
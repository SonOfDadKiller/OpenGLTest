#pragma once

#include <glad.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include "Shader.h"

using std::string;
using std::ifstream;
using std::stringstream;

static std::vector<int> vertShaders;
static std::vector<int> fragShaders;
static std::vector<int> shaderPrograms;

static bool ShaderSuccess(unsigned int shaderID)
{
	int success;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
	return success == GL_TRUE;
}

static bool ProgramSuccess(unsigned int programID)
{
	int success;
	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	return success == GL_TRUE;
}

unsigned int CreateShader(ShaderType type, const char* filePath)
{
	//Read file
	string data;
	ifstream file;

	//Ensure ifstream can throw exceptions
	file.exceptions(ifstream::failbit | ifstream::badbit);

	try
	{
		//Open file
		file.open(filePath);

		stringstream stream;

		//Read file buffer into stream
		stream << file.rdbuf();

		//Close file handler
		file.close();

		//Convert stream into string
		data = stream.str();
	}
	catch (ifstream::failure e)
	{
		std::cout << "File read failed!\n";
	}

	const char* source = data.c_str();

	unsigned int id = glCreateShader(type == VertShader ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
	glShaderSource(id, 1, &source, nullptr);
	glCompileShader(id);

	//Check if shaders compiled successfully
	if (!ShaderSuccess(id))
	{
		char infoLog[512];
		glGetShaderInfoLog(id, 512, nullptr, infoLog);
		std::cout << (type == VertShader ? "Vertex" : "Fragment") << "Shader compilation failed! LOG: " << infoLog << "\n";
		return -1;
	}

	if (type == VertShader)
	{
		vertShaders.push_back(id);
		std::cout << "Created Vertex Shader: " << id << "\n";
	}
	else
	{
		fragShaders.push_back(id);
		std::cout << "Created Fragment Shader: " << id << "\n";
	}

	return id;
}

unsigned int CreateShaderProgram(unsigned int vertShaderID, unsigned int fragShaderID)
{
#ifdef _DEBUG
	//Check if ids match existing shaders
	if (std::find(vertShaders.begin(), vertShaders.end(), vertShaderID) == vertShaders.end())
	{
		std::cout << "No vertex shader with id '" << vertShaderID << "'\n";
	}

	if (std::find(fragShaders.begin(), fragShaders.end(), fragShaderID) == fragShaders.end())
	{
		std::cout << "No vertex shader with id '" << fragShaderID << "'\n";
	}
#endif

	//Set up shader program
	unsigned int programID = glCreateProgram();
	glAttachShader(programID, vertShaderID);
	glAttachShader(programID, fragShaderID);
	glLinkProgram(programID);

	if (!ProgramSuccess(programID))
	{
		char infoLog[512];
		glGetProgramInfoLog(programID, 512, nullptr, infoLog);
		std::cout << "Shader Program linking failed! LOG: " << infoLog << "\n";
		return -1;
	}

	shaderPrograms.push_back(programID);

	return programID;
}

void CleanupShaders()
{
	for (int i : vertShaders)
	{
		glDeleteShader(i);
	}

	for (int i : fragShaders)
	{
		glDeleteShader(i);
	}

	for (int i : shaderPrograms)
	{
		glDeleteProgram(i);
	}

	vertShaders.clear();
	fragShaders.clear();
	shaderPrograms.clear();
}
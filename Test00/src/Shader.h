#pragma once

enum ShaderType { Vertex, Fragment };

unsigned int CreateShader(ShaderType type, const char* filePath);
unsigned int CreateShaderProgram(unsigned int vertShaderID, unsigned int fragShaderID);

void CleanupShaders();
#pragma once

enum ShaderType { VertShader, FragShader };

unsigned int CreateShader(ShaderType type, const char* filePath);
unsigned int CreateShaderProgram(unsigned int vertShaderID, unsigned int fragShaderID);
unsigned int CreateShaderProgram(const char* vertShaderPath, const char* fragShaderPath);

void CleanupShaders();
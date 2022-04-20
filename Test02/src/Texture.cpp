#include <glad.h>
#include <glfw3.h>
#include <vector>
#include <iostream>
#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


//using std::string;
//using std::vector;

//static vector<unsigned char*> images;

unsigned int LoadTexture(const char* path)
{
	return LoadTexture(path, GL_REPEAT);
}

unsigned int LoadTexture(const char* path, unsigned int wrapMode)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	//Set parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Load texture from file
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);

	unsigned char* textureData = stbi_load(path, &width, &height, &nrChannels, 0);

	std::cout << "Load Texture @" << path << "	nrChannels = " << nrChannels << std::endl;

	unsigned int imageFormat = 0;

	switch (nrChannels)
	{
	case 2: imageFormat = GL_RG; break;
	case 3: imageFormat = GL_RGB; break;
	case 4: imageFormat = GL_RGBA; break;
	}

	if (textureData)
	{
		//Bind image to texture
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, imageFormat, GL_UNSIGNED_BYTE, textureData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Image load failed!" << std::endl;
		stbi_image_free(textureData);
		return -1;
	}

	return textureID;
}
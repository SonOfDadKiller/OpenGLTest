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
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	//Set parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//Load texture from file
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);

	unsigned char* textureData = stbi_load(path, &width, &height, &nrChannels, 0);

	if (textureData)
	{
		//Bind image to texture
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
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
#include <glad.h>
#include <glfw3.h>
#include "glm/gtc/type_ptr.hpp"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "Map.h"
#include "Meshes.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"

using glm::vec3;
using glm::mat4;

#define MAP_WIDTH 20
#define MAP_HEIGHT 20

static std::vector<MapTile*> tileRefs;
static MapTile* tiles[MAP_WIDTH][MAP_HEIGHT];

static unsigned int openTileVAO;
static unsigned int wallTileVAO;
static unsigned int cornerTileVAO;
static unsigned int hallwayTileVAO;
static unsigned int deadEndTileVAO;

static unsigned int tileShader;
static unsigned int modelMatrixUniform;
static unsigned int viewMatrixUniform;
static unsigned int projMatrixUniform;
static unsigned int textureUniform;
static unsigned int colorUniform;
static unsigned int wallTexture;
static unsigned int groundTexture;

//enum MapTileType;

MapTile::MapTile()
{
	type = Open;
	isPath = false;
}

MapTile::MapTile(int x, int y, float angleDegrees, MapTileType type)
{
	this->x = x;
	this->y = y;
	this->type = type;
	this->rotation = angleDegrees;
	isPath = false;
}

void InitializeTileVAO(unsigned int cubeVBO, unsigned int& tileVAO, unsigned int indices[], unsigned int indexCount)
{
	//Generate and bind VAO
	unsigned int tileEBO;
	glGenVertexArrays(1, &tileVAO);
	glBindVertexArray(tileVAO);

	//Bind VBO
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);

	//Generate, bind and fill EBO
	glGenBuffers(1, &tileEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tileEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount, indices, GL_STATIC_DRAW);

	//Set up vertex attributes (position, uv)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//Unbind buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void InitializeMap()
{
	//Set up tile cube VBO
	unsigned int cubeVBO;
	glGenBuffers(1, &cubeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW);

	//Set up tile VAOs
	InitializeTileVAO(cubeVBO, openTileVAO, openTileIndices, sizeof(openTileIndices));
	InitializeTileVAO(cubeVBO, wallTileVAO, wallTileIndices, sizeof(wallTileIndices));
	InitializeTileVAO(cubeVBO, cornerTileVAO, cornerTileIndices, sizeof(cornerTileIndices));
	InitializeTileVAO(cubeVBO, hallwayTileVAO, hallwayTileIndices, sizeof(hallwayTileIndices));
	InitializeTileVAO(cubeVBO, deadEndTileVAO, deadEndTileIndices, sizeof(deadEndTileIndices));

	//Load shaders and compile program
	unsigned int vert = CreateShader(Vertex, "shaders/tile.vert");
	unsigned int frag = CreateShader(Fragment, "shaders/tile.frag");
	tileShader = CreateShaderProgram(vert, frag);
	modelMatrixUniform = glGetUniformLocation(tileShader, "model");
	viewMatrixUniform = glGetUniformLocation(tileShader, "view");
	projMatrixUniform = glGetUniformLocation(tileShader, "projection");
	textureUniform = glGetUniformLocation(tileShader, "main_tex");
	colorUniform = glGetUniformLocation(tileShader, "color");

	//Load textures
	wallTexture = LoadTexture("textures/wall.jpg");
	groundTexture = LoadTexture("textures/ground.jpg");

	//Assign textures to shader uniforms
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D)

	//Create tileRefs
	//tileRefs.push_back(new MapTile(0, 0, 0.f, Open));

	tileRefs.push_back(new MapTile(0, -2, 0.f, Open));
	tileRefs.push_back(new MapTile(2, -2, 90.f, Open));
	tileRefs.push_back(new MapTile(4, -2, 180.f, Open));
	tileRefs.push_back(new MapTile(6, -2, 270.f, Open));

	tileRefs.push_back(new MapTile(0, 0, 0.f, Wall));
	tileRefs.push_back(new MapTile(2, 0, 90.f, Wall));
	tileRefs.push_back(new MapTile(4, 0, 180.f, Wall));
	tileRefs.push_back(new MapTile(6, 0, 270.f, Wall));

	tileRefs.push_back(new MapTile(0, 2, 0.f, Corner));
	tileRefs.push_back(new MapTile(2, 2, 90.f, Corner));
	tileRefs.push_back(new MapTile(4, 2, 180.f, Corner));
	tileRefs.push_back(new MapTile(6, 2, 270.f, Corner));

	tileRefs.push_back(new MapTile(0, 4, 0.f, Hallway));
	tileRefs.push_back(new MapTile(2, 4, 90.f, Hallway));
	tileRefs.push_back(new MapTile(4, 4, 180.f, Hallway));
	tileRefs.push_back(new MapTile(6, 4, 270.f, Hallway));

	tileRefs.push_back(new MapTile(0, 6, 0.f, DeadEnd));
	tileRefs.push_back(new MapTile(2, 6, 90.f, DeadEnd));
	tileRefs.push_back(new MapTile(4, 6, 180.f, DeadEnd));
	tileRefs.push_back(new MapTile(6, 6, 270.f, DeadEnd));
}

void ClearMap()
{
	for (int i = 0; i < tileRefs.size(); i++)
	{
		delete tileRefs[i];
	}

	tileRefs.clear();

	for (int x = 0; x < MAP_WIDTH; x++)
	{
		for (int y = 0; y < MAP_WIDTH; y++)
		{
			delete tiles[x][y];
			tiles[x][y] = new MapTile(x, y, 0.f, Open);
		}
	}
}

void LoadLevel(const char* path)
{
	//Clear map
	ClearMap();

	//Read file
	std::string data;
	std::ifstream file;

	//Ensure ifstream can throw exceptions
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		//Open and read file into stream file
		file.open(path);
		std::stringstream stream;
		stream << file.rdbuf();

		//Close file handler
		file.close();

		//Convert to string
		data = stream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "File read failed!\n";
		return;
	}

	int x = 0, y = 0;
	
	auto it = data.begin();

	while (it != data.end())
	{
		char c = (*it);

		if (c == '*')
		{
			tiles[x][y]->isPath = true;
			x++;
		}
		else if (c == '\n')
		{
			y++;
			x = 0;
		}
		else
		{
			x++;
		}

		if (x > MAP_WIDTH)
		{
			std::cout << "Map width exceeded!";
			return;
		}
		
		if (y > MAP_HEIGHT)
		{
			std::cout << "Map height exceeded!";
			return;
		}

		it++;
	}

	//Initialize tiles to correct type and rotation, and fill ref list
	for (int x = 0; x < MAP_WIDTH; x++)
	{
		for (int y = 0; y < MAP_WIDTH; y++)
		{
			MapTile* tile = tiles[x][y];

			if (tile->isPath)
			{

				bool pathNorth = false;
				bool pathEast = false;
				bool pathSouth = false;
				bool pathWest = false;

				if (y + 1 < MAP_HEIGHT && tiles[x][y + 1]->isPath)
				{
					pathNorth = true;
				}

				if (x + 1 < MAP_WIDTH && tiles[x + 1][y]->isPath)
				{
					pathEast = true;
				}

				if (y - 1 >= 0 && tiles[x][y - 1]->isPath)
				{
					pathSouth = true;
				}

				if (x - 1 >= 0 && tiles[x - 1][y]->isPath)
				{
					pathWest = true;
				}

				if (pathNorth && pathEast && pathSouth && pathWest)
				{
					tile->type = Open;
				}
				else if (pathNorth && pathEast && pathSouth)
				{
					//Wall facing west
					tile->type = Wall;
					tile->rotation = 90.f;
				}
				else if (pathNorth && pathWest && pathSouth)
				{
					//Wall facing east
					tile->type = Wall;
					tile->rotation = 270.f;
				}
				else if (pathNorth && pathWest && pathEast)
				{
					//Wall facing south
					tile->type = Wall;
					tile->rotation = 0.f;
				}
				else if (pathEast && pathWest && pathSouth)
				{
					//Wall facing north
					tile->type = Wall;
					tile->rotation = 180.f;
				}
				else if (pathNorth && pathEast)
				{
					//Corner connecting north and east
					tile->type = Corner;
					tile->rotation = 90.f;
				}
				else if (pathEast && pathSouth)
				{
					//Corner connecting east and south
					tile->type = Corner;
					tile->rotation = 180.f;
				}
				else if (pathSouth && pathWest)
				{
					//Corner connecting south and west
					tile->type = Corner;
					tile->rotation = 270.f;
				}
				else if (pathWest && pathNorth)
				{
					//Corner connecting west and north
					tile->type = Corner;
					tile->rotation = 0.f;
				}
				else if (pathNorth && pathSouth)
				{
					//Hallway running north/south
					tile->type = Hallway;
					tile->rotation = 0.f;
				}
				else if (pathEast && pathWest)
				{
					//Hallway running east/west
					tile->type = Hallway;
					tile->rotation = 90.f;
				}
				else if (pathNorth)
				{
					tile->type = DeadEnd;
					tile->rotation = 180.f;
				}
				else if (pathEast)
				{
					tile->type = DeadEnd;
					tile->rotation = 270.f;
				}
				else if (pathSouth)
				{
					tile->type = DeadEnd;
					tile->rotation = 0.f;
				}
				else if (pathWest)
				{
					tile->type = DeadEnd;
					tile->rotation = 90.f;
				}
			}

			tileRefs.push_back(tiles[x][y]);
		}
	}
}

void DrawTile(vec3 position, float rotation, unsigned int tileEBO, unsigned int elementCount)
{
	mat4 model = mat4(1.0f);
	model = glm::translate(model, position);
	model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.f, 1.f, 0.f));
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(model));
	glBindVertexArray(tileEBO);
	glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_INT, 0);
}

void DrawMap()
{
	glUseProgram(tileShader);

	glUniform4f(colorUniform, 1.0f, 1.0f, 1.0f, 1.0f);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(textureUniform, 0);

	glBindTexture(GL_TEXTURE_2D, wallTexture);

	//Set view and projection matrices
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, glm::value_ptr(GetCameraView()));
	glUniformMatrix4fv(projMatrixUniform, 1, GL_FALSE, glm::value_ptr(GetCameraProjection()));

	//Draw dat map!
	for (int i = 0; i < tileRefs.size(); i++)
	{
		vec3 pos = vec3((float)tileRefs[i]->x, 0.f, (float)tileRefs[i]->y);
		float rot = tileRefs[i]->rotation;

		switch (tileRefs[i]->type)
		{
		case Open:
			DrawTile(pos, rot, openTileVAO, sizeof(openTileIndices));
			break;
		case Wall:
			DrawTile(pos, rot, wallTileVAO, sizeof(wallTileIndices));
			break;
		case Corner:
			DrawTile(pos, rot, cornerTileVAO, sizeof(cornerTileIndices));
			break;
		case Hallway:
			DrawTile(pos, rot, hallwayTileVAO, sizeof(hallwayTileIndices));
			break;
		case DeadEnd:
			DrawTile(pos, rot, deadEndTileVAO, sizeof(deadEndTileIndices));
			break;
		}
	}

}

MapTile* GetMapTile(unsigned int x, unsigned int y)
{
	if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT)
	{
		return tiles[x][y];
	}

	return nullptr;
}
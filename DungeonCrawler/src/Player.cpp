#include <iostream>
#include "glm/common.hpp"
#include "Camera.h"
#include "Player.h"
#include "Map.h"

using glm::vec3;

static unsigned int playerX, playerY, prevPlayerX, prevPlayerY;

static float playerRotation, prevRotation;
static float turnTimer;

void InitializePlayer(unsigned int x, unsigned int y, float rotation)
{
	playerX = x;
	playerY = y;
	playerRotation = rotation;
}

void MovePlayer(unsigned int direction)
{
	if (turnTimer < 1.f)
	{
		return;
	}

	prevPlayerX = playerX;
	prevPlayerY = playerY;
	prevRotation = playerRotation;

	direction += (int)(playerRotation / 90.f);
	direction = direction % 4;

	std::cout << direction << std::endl;

	switch (direction)
	{
	case 0: playerX++; break;
	case 1: playerY++; break;
	case 2: playerX--; break;
	case 3: playerY--; break;
	}

	//Check for wall
	MapTile* tile = GetMapTile(playerX, playerY);

	if (tile == nullptr || !tile->isPath)
	{
		playerX = prevPlayerX;
		playerY = prevPlayerY;
		return;
	}

	turnTimer = 0.f;
}

void RotatePlayer(bool clockwise)
{
	if (turnTimer < 1.f)
	{
		return;
	}

	prevRotation = playerRotation;
	prevPlayerX = playerX;
	prevPlayerY = playerY;

	if (clockwise)
	{
		playerRotation += 90.f;
	}
	else
	{
		playerRotation -= 90.f;
	}

	turnTimer = 0.f;
}

void TickPlayer(float dt)
{
	vec3 position((float)playerX, 0.f, (float)playerY);
	vec3 oldPosition((float)prevPlayerX, 0.f, (float)prevPlayerY);

	float turnTimerClamped = glm::min(turnTimer, 1.f);

	float smoothed = glm::smoothstep(0.f, 1.f, turnTimerClamped);

	vec3 lerpPos = glm::mix(oldPosition, position, smoothed);
	float lerpRot = glm::mix(prevRotation, playerRotation, smoothed);
	


	//Update camera
	SetCameraPosition(lerpPos);
	SetCameraRotation(glm::vec3(0.f, lerpRot, 0.f));

	//Update timer
	turnTimer += dt * 2.f;
}
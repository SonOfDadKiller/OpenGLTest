#pragma once



void InitializePlayer(unsigned int x, unsigned int y, float rotation);
void MovePlayer(unsigned int direction);
void RotatePlayer(bool clockwise);
void TickPlayer(float dt);
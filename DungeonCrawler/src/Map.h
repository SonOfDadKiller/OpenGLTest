#pragma once

enum MapTileType { Open, Wall, Corner, Hallway, DeadEnd };

struct MapTile
{
	int x, y;
	float rotation;
	MapTileType type;
	bool isPath;

	MapTile();
	MapTile(int x, int y, float rotation, MapTileType type);
};

void InitializeMap();
void LoadLevel(const char* path);
void DrawMap();
MapTile* GetMapTile(unsigned int x, unsigned int y);
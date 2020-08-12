#pragma once

#include "Tileset.h"
#include "utils.h"
#include <vector>

struct Tile {
	unsigned char index : 5;
	unsigned char solid : 3;
};

class Tilemap {
	Tileset* tls;
	float tilesize;

public:
	unsigned int VAO, VBO;
	Tile* map;
	unsigned int tilecount;
	float* vertices;

	glm::fvec2 offset;
	int mapWidth;
	int mapHeight;

	Tilemap(float tilesize, Tileset* tls, int w, int h, unsigned char* mapData, glm::fvec2 offset);
	~Tilemap();
	void prepareOpenGL();
	void initOpenGL();
	void draw(unsigned int shader) const;
	unsigned char alterTile(int x, int y, int damages);
	std::vector<Bounds> getBBs(glm::fvec2 pos) const;
};


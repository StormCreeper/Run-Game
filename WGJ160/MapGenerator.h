#pragma once
#include "utils.h"
#include "Tilemap.h"
#include "SimplexNoise.h"
#include <map>

struct Chunk {
	int coord;
	Tilemap *map;
};

class MapGenerator {

	static const int chunkW;
	static const int chunkH;

	static void generateChunk(int x);
	static void deleteChunk(int x);

	static std::map<int, Chunk> chunks;
	static Tileset* tls;

	static SimplexNoise pn;

	static int seed;
public:
	static void initGenerator();
	static void drawMap(unsigned int shader);
	static void updateChunks(int playerPos);
	static Chunk getChunk(int x);
	static bool hasChunk(int x);
	static void deleteGenerator();
};
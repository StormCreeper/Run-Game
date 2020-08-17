#include "MapGenerator.h"
#include <algorithm>
#include "NPC.h"

std::map<int, Chunk> MapGenerator::chunks;
Tileset *MapGenerator::tls;

const int MapGenerator::chunkW(20);
const int MapGenerator::chunkH(80);
SimplexNoise MapGenerator::pn;

int MapGenerator::seed;

void MapGenerator::initGenerator() {
	seed = rand();
	tls = new Tileset(4, 4, "Textures/tls.png", GL_TEXTURE0);
	generateChunk(0);
}
void MapGenerator::generateChunk(int x) {

	unsigned char data[20 * 80] = {31};

	for (int i = 0; i < chunkW; i++) {
		int height = (pn.fractal(5, (i + x * chunkW) * 0.005f  + 20 + seed) * 0.5f + 0.5f) * chunkH / 2;
		for (int j = 0; j < chunkH; j++) {
			data[i + j * chunkW] = (j > height) ? 1 : 31;
		}
	}
	for (int i = 0; i < chunkW; i++) {
		for (int j = 0; j < chunkH; j++) {
			if (data[i + j * chunkW] != 31) {
				if (j - 1 < 0 || data[i + (j - 1) * chunkW] == 31) {
					data[i + j * chunkW] = 0;
				} else {
					float val = pn.noise((i + x * chunkW) * 0.1f, j * 0.1f + seed) + j / 40.0f - 0.5f;
					if (val > 0.7f) {
						data[i + j * chunkW] = (val > 0.9f) ? 6 : 5;
						if (rand() % 100 > 96) data[i + j * chunkW] = 10 + rand() % 6;
					} else {
						val = pn.noise((i + x * chunkW) * 0.1f, 100 + j * 0.1f + seed);
						data[i + j * chunkW] = (val < 0.5f) ? 1 : 2;
					}
				}
			}
		}
	}

	Tilemap *tlm = new Tilemap(1, tls, chunkW, chunkH, data, glm::fvec2(x * chunkW, 0));
	tlm->prepareOpenGL();
	tlm->initOpenGL();
	chunks[x] = {x, tlm};

	int npcNum = rand() % 4;
	for (int i = 0; i < npcNum; i++)
		NPC::addNPC(new NPC(glm::vec2(x * chunkW + rand() % chunkW, 0)));
}
void MapGenerator::drawMap(unsigned int shader) {
	setUniformInt(shader, "Texture", 0);
	for (std::pair<int, Chunk> el : chunks) {
		if(el.second.map != nullptr)
			el.second.map->draw(shader);
	}
}
void MapGenerator::updateChunks(int playerPos) {
	int dying = -1;
	for (std::pair<int, Chunk> el : chunks) {
		if (abs(el.first - playerPos / chunkW) > 2) dying = el.first;
	}
	if (dying >= 0) deleteChunk(dying);
	for (int i = std::max(playerPos / chunkW - 1, 0); i <= playerPos / chunkW + 1; i++) {
		if (chunks[i].map == nullptr) {
			generateChunk(i);
		}
	}
}
Chunk MapGenerator::getChunk(int x) {
	if (chunks.find(x / chunkW) == chunks.end()) return { -1, NULL };
	return chunks[x / chunkW];
}
bool MapGenerator::hasChunk(int x) {
	return chunks.find(x / chunkW) != chunks.end();
}
void MapGenerator::deleteChunk(int x) {
	delete chunks[x].map;
	chunks.erase(x);
}
void MapGenerator::deleteGenerator() {
	for (std::pair<int, Chunk> el : chunks) {
		delete el.second.map;
	}
	chunks.clear();
	delete tls;
}

std::vector<Bounds> MapGenerator::getBBs(glm::fvec2 pos) {
	int x = pos.x / chunkW;
	std::vector<Bounds> bounds;
	if (chunks[x - 1].map != nullptr) {
		std::vector<Bounds> b2 = chunks[x - 1].map->getBBs(pos);
		bounds.insert(bounds.end(), b2.begin(), b2.end());
	}
	if (chunks[x].map != nullptr) {
		std::vector<Bounds> b2 = chunks[x].map->getBBs(pos);
		bounds.insert(bounds.end(), b2.begin(), b2.end());
	}
	if (chunks[x + 1].map != nullptr) {
		std::vector<Bounds> b2 = chunks[x + 1].map->getBBs(pos);
		bounds.insert(bounds.end(), b2.begin(), b2.end());
	}
	return bounds;
}

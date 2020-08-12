#include "Tilemap.h"
#include "Particle.h"

Tilemap::Tilemap(float tilesize_, Tileset* tls_, int w, int h, unsigned char* mapData, glm::fvec2 _offset) {
	tilesize = tilesize_;
	offset = _offset;
	mapWidth = w;
	mapHeight = h;

	tls = tls_;

	map = (Tile*) malloc(w * h * sizeof(Tile));
	for (int y = 0; y < mapHeight; y++) {
		for (int x = 0; x < mapWidth; x++) {
			map[x + y * mapWidth] = { (unsigned char) mapData[x + y * mapWidth], 7 };
		}
	}
}

Tilemap::~Tilemap() {
	free(map);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void Tilemap::prepareOpenGL() {
	tilecount = 0;

	for (int y = 0; y < mapHeight; y++) {
		for (int x = 0; x < mapWidth; x++) {
			if (map[x + y * mapWidth].index != 31) tilecount++;
		}
	}


	vertices = (float*) malloc(tilecount * 24 * sizeof(float));
	unsigned int n = 0;
	for (int i = 0; i < mapWidth; i++) {
		for (int j = 0; j < mapHeight; j++) {
			unsigned char tile = map[i + j * mapWidth].index;

			if (tile != 31) {
				float six = i * tilesize;
				float siy = (j + 1) * tilesize;
				float eix = (i + 1) * tilesize;
				float eiy = (j) *tilesize;
				glm::fvec4 tc = tls->getTextureCoords(tile);

				vertices[n] = eix; vertices[n + 1] = siy; vertices[n + 2] = tc.z; vertices[n + 3] = tc.w;
				vertices[n + 4] = eix; vertices[n + 5] = eiy; vertices[n + 6] = tc.z; vertices[n + 7] = tc.y;
				vertices[n + 8] = six; vertices[n + 9] = eiy; vertices[n + 10] = tc.x; vertices[n + 11] = tc.y;
				vertices[n + 12] = six; vertices[n + 13] = siy; vertices[n + 14] = tc.x; vertices[n + 15] = tc.w;
				vertices[n + 16] = eix; vertices[n + 17] = siy; vertices[n + 18] = tc.z; vertices[n + 19] = tc.w;
				vertices[n + 20] = six; vertices[n + 21] = eiy; vertices[n + 22] = tc.x; vertices[n + 23] = tc.y;

				n += 24;
			}
		}
	}
}

void Tilemap::initOpenGL() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, tilecount * 24 * sizeof(float), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) (2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	free(vertices);
}

void Tilemap::draw(unsigned int shader) const {
	setUniformVec2(shader, "offset", offset);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, tilecount * 6);
}

unsigned char Tilemap::alterTile(int x, int y, int damages) {
	if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight) return 31;
	if (map[x + y * mapWidth].index == 31) return 31;
	for (int i = 0; i < damages; i++) {
		map[x + y * mapWidth].solid--;
		if (map[x + y * mapWidth].solid == 0) {
			for (int i = 0; i < 16; i++) {
				float vx = float(rand() % 100) / 25.0f - 2;
				float vy = float(rand() % 100) / 25.0f - 3;

				float px = x + offset.x + (rand() % 100) / 100.0f;
				float py = y + (rand() % 100) / 100.0f;

				Particle::addParticle(new Particle(glm::fvec2(px, py), glm::fvec2(vx, vy), 2, 0));
			}
			unsigned char tile = map[x + y * mapWidth].index;
			map[x + y * mapWidth].index = 31;
			prepareOpenGL();
			initOpenGL();
			return tile;
		}
	}
	return 31;
}

std::vector<Bounds> Tilemap::getBBs(glm::fvec2 pos) const {
	std::vector<Bounds> bounds;
	for (int i = (int)(pos.x - offset.x) - 1; i <= (int) (pos.x - offset.x) + 1; i++) {
		for (int j = (int)(pos.y - offset.y) - 1; j <= (int)(pos.y - offset.y) + 1; j++) {
			if (i >= 0 && i < mapWidth && j >= 0 && j < mapHeight) {
				int tile = map[i + j * mapWidth].index;
				if (tile == 31) continue;
				Bounds b = { 0, 0, 1, 1 };
				bounds.push_back({ b.x + float(i) + offset.x, b.y + float(j) + offset.y, b.w, b.h });
			}
		}
	}
	
	return bounds;
}
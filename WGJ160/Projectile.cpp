#include "Projectile.h"
#include "MapGenerator.h"
#include "SoundManager.h"
#include "Scene.h"
#include "Particle.h"

std::vector<Projectile*> Projectile::projs;
Tileset* Projectile::tls;
Mesh Projectile::mesh;

void Projectile::initialize() {
	tls = new Tileset(2, 1, "Textures/bullet.png", GL_TEXTURE7);
	createQuad(mesh, glm::fvec2(0, 0), glm::fvec2(1.0f, 1.0f));
}

void Projectile::drawAll(unsigned int shader) {
	glBindVertexArray(mesh.VAO);
	setUniformInt(shader, "Texture", 7);
	for (Projectile* p : projs) {
		p->draw(shader);
	}
}
void Projectile::draw(unsigned int shader) const {
	setUniformVec2(shader, "offset", position);
	setUniformInt(shader, "tcAsAttribute", 0);
	setUniformVec4(shader, "UtexCoords", tls->getTextureCoords(texIndex));
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Projectile::updateAll(float deltaTime) {
	for (int i = 0; i < projs.size(); i++) {
		projs[i]->update(deltaTime);
		if (projs[i]->dead) {
			delete projs[i];
			projs.erase(projs.begin() + i);
		}
	}
}

void Projectile::update(float deltaTime) {
	position += velocity * deltaTime;
}

void Projectile::destroy() {
	for (Projectile* p : projs) {
		delete p;
	}
	projs.clear();
	delete tls;
}

void Projectile::addProjectile(Projectile* p) {
	projs.push_back(p);
}

Bullet::Bullet(glm::fvec2 pos, glm::fvec2 vel, bool harm_) {
	position = pos;
	velocity = vel;

	harm = harm_;
	texIndex = 0;
}

Bomb::Bomb(glm::fvec2 pos, glm::fvec2 vel) {
	position = pos;
	velocity = vel;

	texIndex = 1;
}

void Bullet::update(float deltaTime) {
	Projectile::update(deltaTime);

	if (harm) {
		glm::fvec2 dir = Scene::player->position - position;
		float l = sqrtf(dir.x * dir.x + dir.y * dir.y);
		if (l < 1) {
			Scene::player->life -= 10;
			dead = 1;

			for (int i = 0; i < 16; i++) {
				Particle::addParticle(new Particle(Scene::player->position, glm::fvec2(float(rand() % 100) / 25 - 2, float(rand() % 100) / 25 - 2), 2, 1));
			}

		}
	} else {
		for (NPC* npc : NPC::npcs) {
			if (npc->dead) continue;
			glm::fvec2 dir = npc->position - position;
			float l = sqrtf(dir.x * dir.x + dir.y * dir.y);
			if (l < 1) {
				npc->dead = true;
				dead = 1;
				Scene::player->life += 10;
				for (int i = 0; i < 150; i++) {
					Particle::addParticle(new Particle(npc->position, glm::fvec2(float(rand() % 100) / 25 - 2, float(rand() % 100) / 25 - 2), 2, 1));
				}
			}
		}
	}

	if (MapGenerator::hasChunk(int(position.x))) {
		Tilemap* map = MapGenerator::getChunk(int(position.x)).map;
		if (map != nullptr) {
			int x = int(position.x - map->offset.x);
			int y = int(position.y);
			if (x >= 0 && x < map->mapWidth && y >= 0 && y < map->mapHeight) {
				if (map->map[x + y * map->mapWidth].index != 31) {
					dead = 1;
					if (map->alterTile(x, y, 3) != 31) {
						SoundManager::playSound("boom");
						Scene::shakeCamera(5);
					} else {
						SoundManager::playSound("hit");
					}
				}
			}
		}
	}
}

void Bomb::update(float deltaTime) {
	velocity.y += 20 * deltaTime;
	if (MapGenerator::hasChunk(int(position.x))) {
		Tilemap* map = MapGenerator::getChunk(int(position.x)).map;
		if (map != nullptr) {

			int x = int(position.x - map->offset.x);
			int y = int(position.y);
			if (x >= 0 && x < map->mapWidth && y >= 0 && y < map->mapHeight) {
				if (map->map[x + y * map->mapWidth].index != 31) {
					dead = 1;
					{
						glm::fvec2 dir = Scene::player->position - position;
						float l = sqrtf(dir.x * dir.x + dir.y * dir.y);
						if (l < 4) {
							dir *= 20 / l;

							Scene::player->velocity = dir;
							Scene::player->velocity.y = -abs(Scene::player->velocity.y);
						}
					}
					for (NPC* npc : NPC::npcs) {
						glm::fvec2 dir = npc->position - position;
						float l = sqrtf(dir.x * dir.x + dir.y * dir.y);
						if (l < 6) {
							if (l < 3) {
								npc->dead = true;
							}
							dir *= 20 / l;

							npc->velocity = dir;
							npc->velocity.y = -abs(npc->velocity.y);
						}
					}
					explode(x, y, map);
				}
			}
		}
	}

	Projectile::update(deltaTime);
}

void Bomb::explode(int x, int y, Tilemap* map) const {
	SoundManager::playSound("bomb");
	Scene::shakeCamera(100);
	for (float i = 0; i < 360; i += 5) {
		float angle = glm::radians(i);
		float force = (rand() % 100) / 20.0f;
		for (float j = 0; j < 20 && force > 0; j += 0.1f) {
			float px = x + cosf(angle) * j;
			float py = y + sinf(angle) * j;
			unsigned char tile = map->alterTile(int(px), int(py), 7);
			if (tile == 31) {
				force -= 0.1f;
			} else {
				switch (int(tile / 4)) {
				case(0):
					force -= 0.7f;
					break;
				case(1):
					force -= 1.9f;
					break;
				case(2):
					force -= 2.3f;
					break;
				case(3):
					force -= 3.5f;
					break;
				}
			}
		}
	}
}
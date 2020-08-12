#include "NPC.h"
#include "SoundManager.h"
#include "MapGenerator.h"
#include "Projectile.h"

Tileset* NPC::tls;
std::vector<NPC*> NPC::npcs;

NPC::NPC(glm::vec2 pos) : w(4), h(2), position(pos), velocity(0, 0) {
	float w2 = w / 2.0f, h2 = h / 2.0f;
	float vertices[] = {
		-w2, -h2, 0, 0,
		-w2,  h2, 0, 1,
		 w2,  h2, 1, 1,
		-w2, -h2, 0, 0,
		 w2,  h2, 1, 1,
		 w2, -h2, 1, 0
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	anim = { 0, 0, 1, 0.1f, 0.0f, 0.0f };
}
NPC::~NPC() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}
void NPC::update(float deltaTime, Tilemap* map) {
	if (map == NULL) {
		dead = 1;
		return;
	}
	anim.currentTime += deltaTime;
	if (anim.currentTime - anim.lastFrameTime > anim.deltaTime) {
		anim.lastFrameTime = anim.currentTime;
		int mod = mods[anim.y];
		anim.x = (anim.x + 1) % mod;
	}
	velocity.x *= pow(0.8f, deltaTime * 100);

	std::vector<Bounds> bounds = map->getBBs(position);
	position.x += velocity.x * deltaTime;
	if (collide(bounds)) position.x -= velocity.x * deltaTime;
	position.y += velocity.y * deltaTime;
	if (collide(bounds)) {
		position.y -= velocity.y * deltaTime;
		velocity.y = 0;
	}
	if (collide(bounds)) {
		position.y-=0.01f;
	}
	if (velocity.y == 0) {
		anim.y = 0;
	}
	velocity.y += 20.0f * deltaTime;

	bool sp = 1;
	if (sp) {
		anim.y = 4;
		if (glfwGetTime() - bLastTime > bDeltaTime) {
			Projectile::addProjectile(new Bullet(glm::fvec2(position), glm::fvec2(20 * (anim.flip ? -1 : 1) + velocity.x, float(rand() % 100) / 25.0f - 2), 1));
			bLastTime = glfwGetTime();
			/*if (!SoundManager::isPlaying("shoot2")) */SoundManager::playSound("shoot2");

			velocity.x += 4 * (anim.flip ? 1 : -1);
		}
	}
	if (velocity.y < 0) {
		if (!sp) anim.y = 2;
	} else if (abs(velocity.x) > 0) {
		if (!sp) anim.y = 1;
	} else {
		if (!sp) anim.y = 0;
	}

	if (position.y > 100) {
		dead = 1;
	}
}
bool NPC::collide(std::vector<Bounds> tiles) const {
	if (position.x < 0) return true;
	Bounds b = { position.x - 0.5f, position.y - 0.3f, 1.0f, 1.2f };
	for (unsigned int i = 0; i < tiles.size(); i++) {
		if (intersects(b, tiles[i])) return true;
	}
	return false;
}
bool NPC::onGround(std::vector<Bounds> tiles) {
	position.y += 0.1f;
	bool og = collide(tiles);
	position.y -= 0.1f;
	return og;
}
void NPC::draw(unsigned int shader) const {
	setUniformVec2(shader, "offset", position.x, position.y);
	setUniformInt(shader, "tcAsAttribute", 0);
	glm::fvec4 tc1 = tls->getTextureCoords(anim.x + 11 * anim.y);
	glm::fvec4 tc2(tc1);
	if (anim.flip) {
		tc2.x = tc1.z;
		tc2.z = tc1.x;
	}
	setUniformInt(shader, "Texture", 5);
	setUniformVec4(shader, "UtexCoords", tc2);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void NPC::init() {
	tls = new Tileset(11, 5, "Textures/enemy.png", GL_TEXTURE5);
}

void NPC::updateAll(float deltaTime) {
	for (int i = 0; i < npcs.size(); i++) {
		npcs[i]->update(deltaTime, MapGenerator::getChunk(npcs[i]->position.x).map);
		if (npcs[i]->dead) {
			delete npcs[i];
			npcs.erase(npcs.begin() + i);
		}
	}
}

void NPC::drawAll(unsigned int shader) {
	for (NPC *npc : npcs) {
		npc->draw(shader);
	}
}

void NPC::destroy() {
	for (NPC* npc : npcs) {
		delete npc;
	}
	npcs.clear();
	delete tls;
}

void NPC::addNPC(NPC* p) {
	npcs.push_back(p);
}

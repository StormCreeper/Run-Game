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

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)nullptr);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	anim = { 0, 0, 1, 0.1f, 0.0f, 0.0f };

	movementVel = 0;
	movementDuration = (rand() % 100) / 20.0f + 1;
	movementBeginning = glfwGetTime();
}
NPC::~NPC() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}
void NPC::update(float deltaTime, glm::fvec2 &playerPos) {
	anim.currentTime += deltaTime;
	if (anim.currentTime - anim.lastFrameTime > anim.deltaTime) {
		anim.lastFrameTime = anim.currentTime;
		int mod = mods[anim.y];
		anim.x = (anim.x + 1) % mod;
	}

	if (glfwGetTime() - movementBeginning > movementDuration) {
		if (movementVel == 0)
			movementVel = (rand() % 2) ? 1 : -1;
		else
			movementVel = 0;
		movementBeginning = glfwGetTime();
		movementDuration = (rand() % 40) / 10.0f + 1;
	} else {
		if (movementVel == 0) {
			anim.y = 0;
		} else {
			anim.y = 1;
			velocity.x += movementVel;
			anim.flip = movementVel < 0;
		}
	}

	velocity.x *= pow(0.8f, deltaTime * 100);

	std::vector<Bounds> bounds = MapGenerator::getBBs(position);
	
	position.x += velocity.x * deltaTime;
	if (collide(bounds)) position.x -= velocity.x * deltaTime;

	position.y += velocity.y * deltaTime;
	if (collide(bounds)) {
		position.y -= velocity.y * deltaTime;
		float vsign = sign(velocity.y);
		while (collide(bounds)) {
			position.y -= 0.01f;
			bounds = MapGenerator::getBBs(position);
		}
		velocity.y = 0;
	}

	velocity.y += 20.0f * deltaTime;
	
	if (movementVel != 0) {
		position.x += movementVel * 0.5f;
		if (collide(bounds)) velocity.y = -4;
		position.x -= movementVel * 0.5f;
	}
	if (sign(playerPos.x - position.x) == (anim.flip ? -1 : 1) && abs(playerPos.x - position.x) < 10 && abs(playerPos.y - position.y) < 3) {
		shooting = true;
	}

	if (shooting) {
		anim.y = 4;
		if (glfwGetTime() - bLastTime > bDeltaTime) {
			Projectile::addProjectile(new Bullet(glm::fvec2(position), glm::fvec2(20 * (anim.flip ? -1 : 1) + velocity.x, float(rand() % 100) / 25.0f - 2), true));
			bLastTime = glfwGetTime();
			SoundManager::playSound("shoot2");

			velocity.x += 5 * (anim.flip ? 1 : -1);

			shooting = false;
		}
	}
	if (position.y > 100) {
		dead = true;
	}
}
bool NPC::collide(std::vector<Bounds> tiles) const {
	if (position.x < 0) return true;
	Bounds b = { position.x - 0.5f, position.y - 0.3f, 1.0f, 1.2f };
	for (auto tile : tiles) {
		if (intersects(b, tile)) return true;
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

void NPC::updateAll(float deltaTime, glm::fvec2 &playerPos) {
	for (unsigned int i = 0; i < npcs.size(); i++) {
		npcs[i]->update(deltaTime, playerPos);
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

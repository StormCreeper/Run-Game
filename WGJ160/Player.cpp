#include "Player.h"
#include "SoundManager.h"
#include "Projectile.h"
#include <random>
#include <time.h>
#include "Scene.h"

Player::Player() : w(4), h(2), position(3, 0), velocity(0, 0), tls(9, 7, "Textures/player.png", GL_TEXTURE1) {
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

	anim = {0, 0, 0, 0.1f, 0.0f, 0.0f};

	bLastTime = glfwGetTime();
	bDeltaTime = 0.1f;

	srand(time(0));
}
Player::~Player() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}
void Player::update(float deltaTime, GLFWwindow* window, Tilemap* map) {
	if (map == nullptr) return;
	anim.currentTime += deltaTime;
	if (anim.currentTime - anim.lastFrameTime > anim.deltaTime) {
		anim.lastFrameTime = anim.currentTime;
		int mod = mods[anim.y];
		anim.x = (anim.x + 1) % mod;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT)) {
		velocity.x -= 150.0f * deltaTime;
		anim.flip = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
		velocity.x += 150.0f * deltaTime;
		anim.flip = 0;
	}

	std::vector<Bounds> bounds = map->getBBs(position);
	velocity.x *= pow(0.85f, deltaTime * 100);
	position.x += velocity.x * deltaTime;
	if(collide(bounds)) position.x -= velocity.x * deltaTime;
	position.y += velocity.y * deltaTime;
	if (collide(bounds)) {
		position.y -= velocity.y * deltaTime;
		velocity.y = 0;
	}
	if (collide(bounds)) {
		position.y-=0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_UP)) {
		if (onGround(bounds)) {
			velocity.y = -11;
			anim.y = 2;
		}
	}
	if (velocity.y == 0) {
		anim.y = 0;
	}
	velocity.y += 20.0f * deltaTime;

	bool sp = glfwGetKey(window, GLFW_KEY_SPACE);
	if (sp) {
		anim.y = 5;
		if (glfwGetTime() - bLastTime > bDeltaTime) {
			Projectile::addProjectile(new Bullet(glm::fvec2(position), glm::fvec2(20 * (anim.flip ? -1 : 1) + velocity.x, float(rand() % 100) / 25.0f - 2), 0));
			bLastTime = glfwGetTime();
			if(!SoundManager::isPlaying("shoot")) SoundManager::playSound("shoot");

			velocity.x += 4 * (anim.flip ? 1 : -1);
		}
	} else {
		SoundManager::stopSound("shoot");
	}
	if (glfwGetKey(window, GLFW_KEY_B)) {
		if (bUnpressed) {
			Projectile::addProjectile(new Bomb(glm::fvec2(position), glm::fvec2(7 * (anim.flip ? -1 : 1), -6) + velocity));
			bUnpressed = 0;
		}
	} else bUnpressed = 1;
	if (velocity.y < 0) {
		if(!sp) anim.y = 2;
	} else if (abs(velocity.x) > 0.01f) {
		if (!sp) anim.y = 1;
	} else {
		if (!sp) anim.y = 0;
	}

	if (position.y > 100) {
		velocity.y = 0;
		position.y = 0;
	}
}
bool Player::collide(std::vector<Bounds> tiles) const {
	if (position.x < 0) return true;
	Bounds b = { position.x - 0.5f, position.y - 0.3f, 1.0f, 1.2f };
	for (unsigned int i = 0; i < tiles.size(); i++) {
		if (intersects(b, tiles[i])) return true;
	}
	return false;
}
bool Player::onGround(std::vector<Bounds> tiles) {
	position.y += 0.1f;
	bool og = collide(tiles);
	position.y -= 0.1f;
	return og;
}
void Player::draw(unsigned int shader) const {
	setUniformVec2(shader, "offset", position.x, position.y);
	setUniformInt(shader, "tcAsAttribute", 0);
	glm::fvec4 tc1 = tls.getTextureCoords(anim.x + 9 * anim.y);
	glm::fvec4 tc2(tc1);
	if (anim.flip) {
		tc2.x = tc1.z;
		tc2.z = tc1.x;
	}
	setUniformInt(shader, "Texture", 1);
	setUniformVec4(shader, "UtexCoords", tc2);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
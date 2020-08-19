#pragma once
#include "utils.h"
#include "Tileset.h"
#include "Tilemap.h"

class Player {
	unsigned int VAO, VBO;
	float w, h;

	double bLastTime;
	double bDeltaTime;

	Animation anim;

	int mods[7] = {
		4, 3, 9, 6, 9, 4, 6
	};

	bool bUnpressed = true;
public:
	glm::fvec2 position;
	glm::fvec2 velocity;

	int life = 100;

	Tileset tls;

	Player();
	~Player();
	void update(float deltaTime, GLFWwindow* window);
	bool collide(std::vector<Bounds> tiles) const;
	bool onGround(std::vector<Bounds> tiles);
	void draw(unsigned int shader) const;
};
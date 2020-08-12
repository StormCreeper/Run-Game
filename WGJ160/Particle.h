#pragma once

#include "utils.h"
#include <vector>
#include "Tileset.h"

class Particle {

	glm::fvec2 position;
	glm::fvec2 velocity;

	float lifetime;

	int texIndex;

	bool harm;

	static Mesh mesh;
	static std::vector<Particle*> particles;
	static Tileset *tls;

public:

	static void init();
	static void updateAll(float deltaTime);
	static void drawAll(unsigned int shader);
	static void destroy();

	static void addParticle(Particle *p);

	Particle(glm::fvec2 pos, glm::fvec2 vel, float lifetime, bool harm_);
	void update(float deltaTime);
	void draw(unsigned int shader) const;
	
};


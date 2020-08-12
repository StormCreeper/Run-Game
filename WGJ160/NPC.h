#pragma once
#include "utils.h"
#include "Tileset.h"
#include "Tilemap.h"

class NPC {
	unsigned int VAO, VBO;
	float w, h;
	
	int mods[5] = {
		4, 3, 9, 11, 4
	};
public:
	static std::vector<NPC*> npcs;
	Animation anim;
	glm::fvec2 position;
	glm::fvec2 velocity;

	float bLastTime;
	float bDeltaTime = 0.4f;

	bool dead = 0;

	NPC(glm::vec2 pos);
	~NPC();
	void update(float deltaTime, Tilemap* map);
	bool collide(std::vector<Bounds> tiles) const;
	bool onGround(std::vector<Bounds> tiles);
	void draw(unsigned int shader) const;

	static void init();
	static void updateAll(float deltaTime);
	static void drawAll(unsigned int shader);
	static void destroy();

	static void addNPC(NPC* p);

	static Tileset* tls;
};
#pragma once
#include "utils.h"
#include <vector>
#include "Tileset.h"
#include "Tilemap.h"

enum class PType {
	Bullet,
	Bomb
};

class Projectile {
protected:
	glm::fvec2 position;
	glm::fvec2 velocity;
	
	int texIndex = 0;
	bool dead = 0;

	virtual void update(float deltaTime);
	void draw(unsigned int shader) const;

	static std::vector<Projectile*> projs;
	static Tileset* tls;
	static Mesh mesh;
public:
	static void initialize();
	static void updateAll(float deltaTime);
	static void drawAll(unsigned int shader);
	static void destroy();

	static void addProjectile(Projectile* p);
};

class Bullet : public Projectile {
	bool harm;
public:
	Bullet(glm::fvec2 pos, glm::fvec2 vel, bool harm_);
	virtual void update(float deltaTime);
};

class Bomb : public Projectile {
public:
	Bomb(glm::fvec2 pos, glm::fvec2 vel);

	void explode(int x, int y, Tilemap* map) const;
	virtual void update(float deltaTime);
};

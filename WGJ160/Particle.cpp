#include "Particle.h"
#include "TextureManager.h"
#include "MapGenerator.h"
#include "Scene.h"

Mesh Particle::mesh = { 0, 0 };
std::vector<Particle*> Particle::particles;
Tileset* Particle::tls;

void Particle::init() {
	createQuad(mesh, glm::fvec2(0, 0), glm::fvec2(0.1f, 0.1f));
	tls = new Tileset(4, 8, "Textures/particles.png", GL_TEXTURE4);
}

void Particle::destroy() {
	for (Particle* p : particles) {
		delete p;
	}
	particles.clear();
	delete tls;
}

void Particle::addParticle(Particle *p) {
	particles.push_back(p);
}

Particle::Particle(glm::fvec2 pos, glm::fvec2 vel, float lt, bool harm_) {
	harm = harm_;
	position = pos;
	velocity = vel;

	lifetime = lt;

	if (harm) texIndex = 16 + rand() % 8;
	else texIndex = rand() % 16;
}

void Particle::updateAll(float deltaTime) {
	for (unsigned int i = 0; i < particles.size(); i++) {
		particles[i]->update(deltaTime);
		if (particles[i]->lifetime < 0) {
			delete particles[i];
			particles.erase(particles.begin() + i);
		}
	}
}

void Particle::update(float deltaTime) {
	velocity.y += 20 * deltaTime;
	position += velocity * deltaTime;

	lifetime -= deltaTime;
}

void Particle::drawAll(unsigned int shader) {
	glBindVertexArray(mesh.VAO);
	setUniformInt(shader, "tcAsAttribute", 0);
	setUniformInt(shader, "Texture", 4);

	for (Particle* p : particles) {
		p->draw(shader);
	}
}

void Particle::draw(unsigned int shader) const {
	setUniformVec2(shader, "offset", position);
	setUniformVec4(shader, "UtexCoords", Particle::tls->getTextureCoords(texIndex));
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

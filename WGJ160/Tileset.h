#pragma once

#include "utils.h"

class Tileset {

	unsigned int texture;
	unsigned int textureWidth;
	unsigned int textureHeight;
public:

	Tileset(unsigned int texW, unsigned int texH, const char* texURL, GLenum texIndex);
	~Tileset();
	glm::fvec4 getTextureCoords(unsigned int textureIndex) const;
	void bindTexture() const;
};


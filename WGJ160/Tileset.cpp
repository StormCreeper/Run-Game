#include "Tileset.h"

Tileset::Tileset(unsigned int texW, unsigned int texH, const char* texURL, GLenum texIndex) {
	glActiveTexture(texIndex);
	texture = loadTexture(texURL);
	textureWidth = texW;
	textureHeight = texH;

	glBindTexture(GL_TEXTURE_2D, texture);
}

Tileset::~Tileset() {
	glDeleteTextures(1, &texture);
}

glm::vec4 Tileset::getTextureCoords(unsigned int textureIndex) const {
	//std::cout << textureWidth << ", " << textureHeight << std::endl;
	glm::ivec2 rawCoords(textureIndex % textureWidth, textureIndex / textureWidth);
	glm::fvec2 normalizedCoords = glm::vec2(rawCoords) / glm::vec2(textureWidth, textureHeight);
	return glm::fvec4(normalizedCoords, normalizedCoords + glm::fvec2(1, 1) / glm::fvec2(textureWidth, textureHeight));
}

void Tileset::bindTexture() const {
	glBindTexture(GL_TEXTURE_2D, texture);
}

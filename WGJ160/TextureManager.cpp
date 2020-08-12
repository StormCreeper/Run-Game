#include "TextureManager.h"
#include <glad/glad.h>
#include "utils.h"

std::map<const char*, unsigned int> TextureManager::textures;

void TextureManager::deleteAllTextures() {
	for (std::pair<const char*, unsigned int> element : textures) {
		glDeleteTextures(1, &element.second);
	}
	textures.clear();
}

void TextureManager::addTexture(const char* src, const char* name) {
	textures[name] = loadTexture(src);
}

unsigned int TextureManager::getTexture(const char* name) {
	return textures[name];
}

void TextureManager::bindTexture(const char* name) {
	glBindTexture(GL_TEXTURE_2D, getTexture(name));
}

#pragma once

#include <map>

class TextureManager {
	static std::map<const char*, unsigned int> textures;

public:
	static void deleteAllTextures();
	static void addTexture(const char* src, const char* name);
	static unsigned int getTexture(const char* name);
	static void bindTexture(const char* name);
};
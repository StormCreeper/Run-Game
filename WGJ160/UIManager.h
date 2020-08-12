#pragma once

#include "utils.h"
#include "Tileset.h"
#include "TextureManager.h"

class UIManager {

	static Mesh UIMesh;

public:
	static void initUI();
	static void deleteUI();
	static void drawUI(unsigned int shader);

	static void testKeys(GLFWwindow* window);
};


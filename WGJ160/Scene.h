#pragma
#include "utils.h"
#include "Tileset.h"
#include "Tilemap.h"
#include "Player.h"
#include "SoundManager.h"
#include "NPC.h"
#include <vector>

class Scene {

	static unsigned int shader;

	static bool unPressed;
	static bool tUnPressed;

public:
	static Player* player;
	static GLFWwindow* window;
	static unsigned int SCR_WIDTH;
	static unsigned int SCR_HEIGHT;

	static void initialize();
	static void primaryThreadLoad();
	static void secondaryThreadLoad();
	static void renderScene(float deltaTime);
	static void shakeCamera(float magnitude);
	static void deleteScene();
	static void processInput();
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow* window, double posx, double posy);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
};
#include "UIManager.h"
#include "SoundManager.h"
#include "TextureManager.h"
#include "Scene.h"

Mesh UIManager::UIMesh;

void UIManager::initUI() {
	createQuad(UIMesh, glm::fvec2(0, 0), glm::fvec2(2, 2));
	glActiveTexture(GL_TEXTURE6);
	TextureManager::addTexture("Textures/UI.png", "UI");
}

void UIManager::deleteUI() {

}

void UIManager::drawUI(unsigned int shader) {
	setUniformInt(shader, "aspect", 0);
	setUniformInt(shader, "tcAsAttribute", 1);
	setUniformFloat(shader, "zoom", 1);
	setUniformVec2(shader, "view", 0, 0);
	setUniformVec2(shader, "offset", 0, 0);

	setUniformInt(shader, "Texture", 6);
	glActiveTexture(GL_TEXTURE6);
	TextureManager::bindTexture("UI");

	glBindVertexArray(UIMesh.VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void UIManager::testKeys(GLFWwindow* window) {
	
}
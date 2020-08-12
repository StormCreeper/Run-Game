#include <iostream>

#include "utils.h"
#include "Scene.h"

int main() {

    std::cout << "Hello there !\nWe did not have time to make a proper UI, so there are the instructions :\nArrow keys to move, space to shoot and b to drop bombs\nyou have to go as far as possible without dying (not implemented yet :) ), and have fun destroying enemies and basically everything else !\nEnjoy !" << std::endl;
    system("pause");
    Scene::initialize();
    
    float lastTime = 0;
    float count = 0;
    float medDelta = 0;
    float deltaTime = 0;
    float currentTime;
    char title[] = "WeeklyGameJam 160 - 0000 FPS";

    while (!glfwWindowShouldClose(Scene::window)) {
        {
            currentTime = (float)glfwGetTime();
            deltaTime = currentTime - lastTime;
            if (deltaTime < 1.0f / 60.0f) continue;
            medDelta += deltaTime;
            if (medDelta < 1.0f) count++;
            else {
                sprintf_s(title, 29, "WeeklyGameJam 160 - %d FPS", (int)(1.0f / medDelta * (count + 1)) + 1);
                glfwSetWindowTitle(Scene::window, title);
                count = 0;
                medDelta = 0;
            }
            lastTime = currentTime;
        }
        glfwPollEvents();
        Scene::renderScene(deltaTime);
        glfwSwapBuffers(Scene::window);
    }
    Scene::deleteScene();

    return 0;
}

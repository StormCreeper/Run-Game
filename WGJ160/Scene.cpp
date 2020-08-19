#include "Scene.h"
#include "UIManager.h"
#include "TextureManager.h"
#include "MapGenerator.h"
#include "Particle.h"
#include "Projectile.h"

unsigned int Scene::SCR_WIDTH(1280);
unsigned int Scene::SCR_HEIGHT(720);

GLFWwindow* Scene::window(nullptr);

Player* Scene::player;

unsigned int Scene::shader(0);

glm::fvec2 camera(0, 0);
glm::fvec2 shake(0, 0);
glm::fvec2 shakeVel(0, 0);

bool Scene::unPressed = true;
bool Scene::tUnPressed = true;

void Scene::initialize() {
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Cool Game", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }
    
    glfwSetFramebufferSizeCallback(Scene::window, framebuffer_size_callback);
    glfwSetCursorPosCallback(Scene::window, (GLFWcursorposfun)mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    glfwSwapInterval(0);

    primaryThreadLoad();

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    shader = createProgram("Shaders/vert.glsl", "Shaders/frag.glsl");

    glClearColor(0.1f, 0.7f, 1.0f, 1.0f);

}
void Scene::primaryThreadLoad() {
    SoundManager::initOpenAL();
    SoundManager::loadSound("Audio/SFX/Shots Mid Speed.wav", "shoot");
    SoundManager::loadSound("Audio/SFX/Shots 2 Low Speed.wav", "shoot2");
    SoundManager::loadSound("Audio/SFX/boom.wav", "boom");
    SoundManager::loadSound("Audio/SFX/bomb.wav", "bomb");
    SoundManager::loadSound("Audio/SFX/hit.wav", "hit");
    SoundManager::loadSound("Audio/Game Jam.wav", "main");
    TextureManager::addTexture("Textures/bullet.png", "bullet");

    SoundManager::changeVolume("main", 0.3f);
    SoundManager::changeVolume("shoot", 1.0f);
    SoundManager::changeVolume("shoot2", 0.2f);

    NPC::init();
    player = new Player();

    UIManager::initUI();
    MapGenerator::initGenerator();
    Particle::init();
    Projectile::initialize();
}
void Scene::secondaryThreadLoad() {

}

void Scene::renderScene(float deltaTime) {
    if(!SoundManager::isPlaying("main")) SoundManager::playSound("main");
    
    processInput();

    float time = float(glfwGetTime());

    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader);
    setUniformVec2(shader, "res", float(SCR_WIDTH), float(SCR_HEIGHT));
    setUniformFloat(shader, "iTime", time);

    setUniformFloat(shader, "zoom", 0.07f);
    setUniformInt(shader, "Texture", 0);
    setUniformInt(shader, "tcAsAttribute", 1);

    setUniformInt(shader, "aspect", 1);

    setUniformFloat(shader, "life", player->life / 100.0f);

    player->update(deltaTime, window);
    camera += (player->position - camera) * 3.0f * deltaTime;

    shakeVel += (-shake) * deltaTime * 1000.0f;
    shake += shakeVel * deltaTime;
    shakeVel *= powf(0.99f, deltaTime * 1000.0f);

    setUniformVec2(shader, "view", camera.x + shake.x, camera.y + shake.y);

    MapGenerator::updateChunks(int(player->position.x));
    MapGenerator::drawMap(shader);
    
    setUniformVec2(shader, "offset", 0, 0);
    
    NPC::updateAll(deltaTime, player->position);

    player->draw(shader);
    NPC::drawAll(shader);
    
    Particle::updateAll(deltaTime);
    Particle::drawAll(shader);

    Projectile::updateAll(deltaTime);
    Projectile::drawAll(shader);

    UIManager::drawUI(shader);

    if (rand() % 100 > 97) player->life--;
}

void Scene::shakeCamera(float magnitude) {
    float angle = float(rand() % 360);
    angle = glm::radians(angle);
    shakeVel.x += magnitude * cosf(angle);
    shakeVel.y += magnitude * sinf(angle);
}

void Scene::deleteScene() {
    glDeleteProgram(shader);
    delete player;

    SoundManager::shutdownOpenAL();
    UIManager::deleteUI();
    MapGenerator::deleteGenerator();
    Particle::destroy();
    Projectile::destroy();

    TextureManager::deleteAllTextures();
    
    NPC::destroy();

    glfwTerminate();
}
void Scene::processInput() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS) {
        if (unPressed) {
            char* vertexShaderSource = readSource("Shaders/vert.glsl");
            char* fragmentShaderSource = readSource("Shaders/frag.glsl");

            int nshader = createProgram(createShader(GL_VERTEX_SHADER, vertexShaderSource), createShader(GL_FRAGMENT_SHADER, fragmentShaderSource));
            if (nshader != 128) {
                glDeleteProgram(shader);
                shader = nshader;
                glUseProgram(shader);
            }
            free(vertexShaderSource);
            free(fragmentShaderSource);
            unPressed = false;
        }
    }
    else {
        unPressed = true;
    }
}

void Scene::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}
void Scene::mouse_callback(GLFWwindow* window, double posx, double posy) {
}
void Scene::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        double xpos = 0;
        double ypos = 0;
        glfwGetCursorPos(window, &xpos, &ypos);
        float sx = float(xpos) / float(SCR_WIDTH) * 2 - 1;
        float sy = float(ypos) / float(SCR_HEIGHT) * 2 - 1;
        
    }
}
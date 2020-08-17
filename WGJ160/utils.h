#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "stb_image.h"

unsigned int createShader(unsigned int shaderType, const char* shaderSource);
unsigned int createProgram(unsigned int vertexShader, unsigned int fragmentShader);
void createProgram(unsigned int* shader, unsigned int vertexShader, unsigned int fragmentShader);
unsigned int createProgram(const char* vname, const char* fname);
char* readSource(const char* filename);
unsigned int loadTexture(const char* filename);
void setUniformVec4(unsigned int shader, const char* name, float x, float y, float z, float w);
void setUniformVec4(unsigned int shader, const char* name, glm::fvec4 vec);
void setUniformVec3(unsigned int shader, const char* name, float x, float y, float z);
void setUniformVec3(unsigned int shader, const char* name, glm::fvec3 vec);
void setUniformVec2(unsigned int shader, const char* name, float x, float y);
void setUniformVec2(unsigned int shader, const char* name, glm::fvec2 vec);
void setUniformVec2i(unsigned int shader, const char* name, int x, int y);
void setUniformVec2i(unsigned int shader, const char* name, glm::ivec2 vec);
void setUniformInt(unsigned int shader, const char* name, int value);
void setUniformFloat(unsigned int shader, const char* name, float value);

float lerp(float a, float b, float c);
float sign(float x);

struct Bounds {
    float x;
    float y;
    float w;
    float h;
};
bool intersects(Bounds r1, Bounds r2);
float modulo(float x, float y);

struct Animation {
	int x;
	int y;
	int flip;
	float deltaTime;
	float lastFrameTime;
	float currentTime;
};

struct Mesh {
	unsigned int VAO;
	unsigned int VBO;

	~Mesh() {
		if(VAO != 0)
			glDeleteVertexArrays(1, &VAO);
		if (VBO != 0)
			glDeleteBuffers(1, &VBO);
	}
};

void createQuad(Mesh & mesh, glm::fvec2 center, glm::fvec2 size);
#include "utils.h"

unsigned int createShader(unsigned int shaderType, const char* shaderSource) {
	unsigned int shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderSource, NULL);
	glCompileShader(shader);

    int success;
    char infoLog[2048];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shader, 2048, NULL, infoLog);
        std::cerr << "Shader compilation error :\n" << infoLog << std::endl;
        return 128;
    }

	return shader;
}
void createProgram(unsigned int* shader, unsigned int vertexShader, unsigned int fragmentShader) {
    *shader = createProgram(vertexShader, fragmentShader);
}

unsigned int createProgram(unsigned int vertexShader, unsigned int fragmentShader) {
    if (vertexShader == 128 || fragmentShader == 128) {
        return 128;
    }
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	glLinkProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}
unsigned int createProgram(const char* vname, const char* fname) {
    char* vertSource = readSource(vname);
    char* fragSource = readSource(fname);

    unsigned int shader = createProgram(createShader(GL_VERTEX_SHADER, vertSource), createShader(GL_FRAGMENT_SHADER, fragSource));

    free(vertSource);
    free(fragSource);

    return shader;
}

char* readSource(const char* filename) {
    char* text = NULL;

    if (filename != NULL) {
        FILE* file;
        fopen_s(&file, filename, "rt");

        if (file != NULL) {
            fseek(file, 0, SEEK_END);
            int count = ftell(file);
            rewind(file);

            if (count > 0) {
                text = (char*)malloc(sizeof(char) * (count + 1));
                count = fread(text, sizeof(char), count, file);
                text[count] = '\0';
            }
            fclose(file);
        }
    }
    else {
        std::cerr << "Nom de fichier invalide";
    }
    return text;
}

unsigned int loadTexture(const char* filename) {
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        //glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture " << filename << std::endl;
        system("PAUSE");
        exit(-1);
    }

    stbi_image_free(data);

    return texture;
}

void setUniformVec4(unsigned int shader, const char* name, float x, float y, float z, float w) {
    unsigned int loc = glGetUniformLocation(shader, name);
    glUniform4f(loc, x, y, z, w);
}
void setUniformVec4(unsigned int shader, const char* name, glm::fvec4 vec) {
    unsigned int loc = glGetUniformLocation(shader, name);
    glUniform4f(loc, vec.x, vec.y, vec.z, vec.w);
}

void setUniformVec3(unsigned int shader, const char* name, float x, float y, float z) {
    unsigned int loc = glGetUniformLocation(shader, name);
    glUniform3f(loc, x, y, z);
}
void setUniformVec3(unsigned int shader, const char* name, glm::fvec3 vec) {
    unsigned int loc = glGetUniformLocation(shader, name);
    glUniform3f(loc, vec.x, vec.y, vec.z);
}

void setUniformVec2(unsigned int shader, const char* name, float x, float y) {
    unsigned int loc = glGetUniformLocation(shader, name);
    glUniform2f(loc, x, y);
}
void setUniformVec2(unsigned int shader, const char* name, glm::fvec2 vec) {
    unsigned int loc = glGetUniformLocation(shader, name);
    glUniform2f(loc, vec.x, vec.y);
}

void setUniformVec2i(unsigned int shader, const char* name, int x, int y) {
    unsigned int loc = glGetUniformLocation(shader, name);
    glUniform2i(loc, x, y);
}
void setUniformVec2i(unsigned int shader, const char* name, glm::ivec2 vec) {
    unsigned int loc = glGetUniformLocation(shader, name);
    glUniform2i(loc, vec.x, vec.y);
}

void setUniformInt(unsigned int shader, const char* name, int value) {
    unsigned int loc = glGetUniformLocation(shader, name);
    glUniform1i(loc, value);
}
void setUniformFloat(unsigned int shader, const char* name, float value) {
    unsigned int loc = glGetUniformLocation(shader, name);
    glUniform1f(loc, value);
}

float lerp(float a, float b, float c) {
    return a + c*(b-a);
}

bool intersects(Bounds r1, Bounds r2) {
    return !(r2.x > r1.x + r1.w || r2.x + r2.w < r1.x || r2.y > r1.y + r1.h || r2.y + r2.h < r1.y);
}

float modulo(float x, float y) {
    x -= y * abs(x / y);
    if (x >= 0.) return (x);
    else return (x + y);
}

void createQuad(Mesh& mesh, glm::fvec2 center, glm::fvec2 size) {
    float w2 = size.x / 2.0f, h2 = size.y / 2.0f;
    float vertices[] = {
        center.x - w2, center.y - h2, 0, 0,
        center.x - w2, center.y + h2, 0, 1,
        center.x + w2, center.y + h2, 1, 1,
        center.x - w2, center.y - h2, 0, 0,
        center.x + w2, center.y + h2, 1, 1,
        center.x + w2, center.y - h2, 1, 0
    };

    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);

    glBindVertexArray(mesh.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
}
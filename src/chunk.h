#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

class Chunk {
public:
    static const int size = 16;
    static const int height = 64;
    int chunkx, chunkz;
    unsigned char blocks[size][height][size];
    unsigned int vao, vbo;
    
    Chunk(int x, int z);
    void draw();

private:
    std::vector<float> vertices;
    
    void generateterrain();
    void addvertex(float x, float y, float z, glm::vec3 color, glm::vec3 normal);
    void addface(const glm::vec3 &position, const glm::vec3 &color, const glm::vec3 &normal);
    void createmesh();
    glm::vec3 getBlockColor(unsigned char blockType);
};
#include "chunk.h"
#include "worldgen.h"

Chunk::Chunk(int x, int z) : chunkx(x), chunkz(z) {
    generateterrain();
    createmesh();
}

void Chunk::draw() {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 9);
}

void Chunk::generateterrain() {
    for (int x = 0; x < size; x++) {
        for (int z = 0; z < size; z++) {
            int worldx = x + chunkx * size;
            int worldz = z + chunkz * size;
            float terrainheight = WorldGen::generateTerrainHeight(worldx, worldz);
            
            for (int y = 0; y < height; y++) {
                blocks[x][y][z] = WorldGen::getBlockType(y, terrainheight);
            }
        }
    }
}

void Chunk::addvertex(float x, float y, float z, glm::vec3 color, glm::vec3 normal) {
    vertices.insert(vertices.end(), {x, y, z});
    vertices.insert(vertices.end(), {color.r, color.g, color.b});
    vertices.insert(vertices.end(), {normal.x, normal.y, normal.z});
}

void Chunk::addface(const glm::vec3 &position, const glm::vec3 &color, const glm::vec3 &normal) {
    const glm::vec3 &p = position;
    const float size = 1.0f;
    
    if (normal == glm::vec3(0, 1, 0)) {
        addvertex(p.x, p.y + size, p.z, color, normal);
        addvertex(p.x + size, p.y + size, p.z, color, normal);
        addvertex(p.x + size, p.y + size, p.z + size, color, normal);
        addvertex(p.x, p.y + size, p.z, color, normal);
        addvertex(p.x + size, p.y + size, p.z + size, color, normal);
        addvertex(p.x, p.y + size, p.z + size, color, normal);
    }
    else if (normal == glm::vec3(0, -1, 0)) {
        addvertex(p.x, p.y, p.z, color, normal);
        addvertex(p.x + size, p.y, p.z, color, normal);
        addvertex(p.x + size, p.y, p.z + size, color, normal);
        addvertex(p.x, p.y, p.z, color, normal);
        addvertex(p.x + size, p.y, p.z + size, color, normal);
        addvertex(p.x, p.y, p.z + size, color, normal);
    }
    else if (normal == glm::vec3(1, 0, 0)) {
        addvertex(p.x + size, p.y, p.z, color, normal);
        addvertex(p.x + size, p.y + size, p.z, color, normal);
        addvertex(p.x + size, p.y + size, p.z + size, color, normal);
        addvertex(p.x + size, p.y, p.z, color, normal);
        addvertex(p.x + size, p.y + size, p.z + size, color, normal);
        addvertex(p.x + size, p.y, p.z + size, color, normal);
    }
    else if (normal == glm::vec3(-1, 0, 0)) {
        addvertex(p.x, p.y, p.z, color, normal);
        addvertex(p.x, p.y + size, p.z, color, normal);
        addvertex(p.x, p.y + size, p.z + size, color, normal);
        addvertex(p.x, p.y, p.z, color, normal);
        addvertex(p.x, p.y + size, p.z + size, color, normal);
        addvertex(p.x, p.y, p.z + size, color, normal);
    }
    else if (normal == glm::vec3(0, 0, 1)) {
        addvertex(p.x, p.y, p.z + size, color, normal);
        addvertex(p.x + size, p.y, p.z + size, color, normal);
        addvertex(p.x + size, p.y + size, p.z + size, color, normal);
        addvertex(p.x, p.y, p.z + size, color, normal);
        addvertex(p.x + size, p.y + size, p.z + size, color, normal);
        addvertex(p.x, p.y + size, p.z + size, color, normal);
    }
    else if (normal == glm::vec3(0, 0, -1)) {
        addvertex(p.x, p.y, p.z, color, normal);
        addvertex(p.x + size, p.y, p.z, color, normal);
        addvertex(p.x + size, p.y + size, p.z, color, normal);
        addvertex(p.x, p.y, p.z, color, normal);
        addvertex(p.x + size, p.y + size, p.z, color, normal);
        addvertex(p.x, p.y + size, p.z, color, normal);
    }
}

glm::vec3 Chunk::getBlockColor(unsigned char blockType) {
    switch (blockType) {
        case 1: return glm::vec3(0.2f, 0.8f, 0.3f);
        case 2: return glm::vec3(0.5f, 0.4f, 0.3f);
        case 3: return glm::vec3(0.4f, 0.4f, 0.4f);
        default: return glm::vec3(1.0f, 1.0f, 1.0f);
    }
}

void Chunk::createmesh() {
    for (int x = 0; x < size; x++) {
        for (int y = 0; y < height; y++) {
            for (int z = 0; z < size; z++) {
                if (blocks[x][y][z] == 0) continue;
                
                glm::vec3 color = getBlockColor(blocks[x][y][z]);
                
                if (y == height - 1 || blocks[x][y + 1][z] == 0)
                    addface(glm::vec3(x, y, z), color, glm::vec3(0, 1, 0));
                if (y == 0 || blocks[x][y - 1][z] == 0)
                    addface(glm::vec3(x, y, z), color, glm::vec3(0, -1, 0));
                if (x == size - 1 || blocks[x + 1][y][z] == 0)
                    addface(glm::vec3(x, y, z), color, glm::vec3(1, 0, 0));
                if (x == 0 || blocks[x - 1][y][z] == 0)
                    addface(glm::vec3(x, y, z), color, glm::vec3(-1, 0, 0));
                if (z == size - 1 || blocks[x][y][z + 1] == 0)
                    addface(glm::vec3(x, y, z), color, glm::vec3(0, 0, 1));
                if (z == 0 || blocks[x][y][z - 1] == 0)
                    addface(glm::vec3(x, y, z), color, glm::vec3(0, 0, -1));
            }
        }
    }
    
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
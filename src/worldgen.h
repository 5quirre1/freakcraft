#pragma once
#include <glm/glm.hpp>
#include <cmath>

namespace WorldGen {
    float generateTerrainHeight(int worldX, int worldZ);
    unsigned char getBlockType(int y, float terrainHeight);
}
#include "worldgen.h"

float WorldGen::generateTerrainHeight(int worldX, int worldZ) {
    float heightbase = 20.0f;
    float noise1 = sin(worldX * 0.1f) * 3.0f;
    float noise2 = cos(worldZ * 0.1f) * 3.0f;
    float noise3 = sin(worldX * 0.05f + worldZ * 0.05f) * 5.0f;
    float terrainheight = heightbase + noise1 + noise2 + noise3;
    return glm::clamp(terrainheight, 1.0f, 59.0f);
}

unsigned char WorldGen::getBlockType(int y, float terrainHeight) {
    if (y < terrainHeight - 8) return 3;
    else if (y < terrainHeight - 3) return 2;
    else if (y < terrainHeight) return 1;
    else return 0;
}
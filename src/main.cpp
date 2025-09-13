#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <memory>

#include "shader.h"
#include "chunk.h"
#include "camera.h"

const unsigned int screenwidth = 1280;
const unsigned int screenheight = 720;

Camera camera;
float deltatime = 0.0f;
float lastframe = 0.0f;

struct ChunkCoord {
    int x, z;
    bool operator==(const ChunkCoord& other) const {
        return x == other.x && z == other.z;
    }
};

struct ChunkCoordHash {
    size_t operator()(const ChunkCoord& coord) const {
        return std::hash<int>()(coord.x) ^ (std::hash<int>()(coord.z) << 1);
    }
};

std::unordered_map<ChunkCoord, std::unique_ptr<Chunk>, ChunkCoordHash> loadedChunks;
std::queue<ChunkCoord> chunksToLoad;
std::queue<ChunkCoord> chunksToUnload;
const int renderDistance = 12;
const int unloadDistance = 16;
const int maxChunksPerFrame = 1;
int chunksProcessedThisFrame = 0;

ChunkCoord lastPlayerChunk = {-9999, -9999};

void framebuffersizecallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mousecallback(GLFWwindow *window, double xpos, double ypos) {
    camera.processMouse(xpos, ypos);
}

ChunkCoord getChunkCoord(glm::vec3 position) {
    return {static_cast<int>(floor(position.x / Chunk::size)), 
            static_cast<int>(floor(position.z / Chunk::size))};
}

float getChunkDistance(const ChunkCoord& a, const ChunkCoord& b) {
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.z - b.z) * (a.z - b.z));
}

void updateChunkLoading() {
    ChunkCoord playerChunk = getChunkCoord(camera.position);
    chunksProcessedThisFrame = 0;
    
    if (playerChunk.x != lastPlayerChunk.x || playerChunk.z != lastPlayerChunk.z) {
        for (int x = playerChunk.x - renderDistance; x <= playerChunk.x + renderDistance; x++) {
            for (int z = playerChunk.z - renderDistance; z <= playerChunk.z + renderDistance; z++) {
                ChunkCoord coord = {x, z};
                float distance = getChunkDistance(playerChunk, coord);
                
                if (distance <= renderDistance && loadedChunks.find(coord) == loadedChunks.end()) {
                    bool alreadyQueued = false;
                    std::queue<ChunkCoord> tempQueue = chunksToLoad;
                    while (!tempQueue.empty()) {
                        if (tempQueue.front().x == coord.x && tempQueue.front().z == coord.z) {
                            alreadyQueued = true;
                            break;
                        }
                        tempQueue.pop();
                    }
                    if (!alreadyQueued) {
                        chunksToLoad.push(coord);
                    }
                }
            }
        }
        
        for (auto it = loadedChunks.begin(); it != loadedChunks.end(); ++it) {
            float distance = getChunkDistance(playerChunk, it->first);
            if (distance > unloadDistance) {
                chunksToUnload.push(it->first);
            }
        }
        
        lastPlayerChunk = playerChunk;
    }
    
    while (!chunksToUnload.empty() && chunksProcessedThisFrame < maxChunksPerFrame) {
        ChunkCoord coord = chunksToUnload.front();
        chunksToUnload.pop();
        loadedChunks.erase(coord);
        chunksProcessedThisFrame++;
    }
    
    while (!chunksToLoad.empty() && chunksProcessedThisFrame < maxChunksPerFrame) {
        ChunkCoord coord = chunksToLoad.front();
        chunksToLoad.pop();
        
        ChunkCoord currentPlayerChunk = getChunkCoord(camera.position);
        float distance = getChunkDistance(currentPlayerChunk, coord);
        
        if (distance <= renderDistance) {
            loadedChunks[coord] = std::make_unique<Chunk>(coord.x, coord.z);
            chunksProcessedThisFrame++;
        }
    }
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow *window = glfwCreateWindow(screenwidth, screenheight, "freakcraft1.0", NULL, NULL);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetCursorPos(window, screenwidth / 2.0f, screenheight / 2.0f);
    glfwSetFramebufferSizeCallback(window, framebuffersizecallback);
    glfwSetCursorPosCallback(window, mousecallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSwapInterval(1);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    Shader shader(vertexshader, fragmentshader);
    shader.use();
    
    glm::vec3 lightpos(50.0f, 100.0f, 50.0f);
    shader.setvec3("lightpos", lightpos);
    
    ChunkCoord initialChunk = getChunkCoord(camera.position);
    for (int x = initialChunk.x - 3; x <= initialChunk.x + 3; x++) {
        for (int z = initialChunk.z - 3; z <= initialChunk.z + 3; z++) {
            ChunkCoord coord = {x, z};
            loadedChunks[coord] = std::make_unique<Chunk>(coord.x, coord.z);
        }
    }
    
    glEnable(GL_DEPTH_TEST);
    
    while (!glfwWindowShouldClose(window)) {
        float currentframe = glfwGetTime();
        deltatime = currentframe - lastframe;
        lastframe = currentframe;
        
        camera.processKeyboard(window, deltatime);
        updateChunkLoading();
        
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 
                                              (float)screenwidth / (float)screenheight, 
                                              0.1f, 1000.0f);
        glm::mat4 view = camera.getViewMatrix();
        
        shader.use();
        shader.setmat4("projection", projection);
        shader.setmat4("view", view);
        shader.setvec3("viewpos", camera.position);
        
        for (auto& pair : loadedChunks) {
            const ChunkCoord& coord = pair.first;
            auto& chunk = pair.second;
            
            glm::mat4 model = glm::translate(glm::mat4(1.0f), 
                                           glm::vec3(coord.x * Chunk::size, 0.0f, 
                                                    coord.z * Chunk::size));
            shader.setmat4("model", model);
            chunk->draw();
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}

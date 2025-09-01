#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>

#include "shader.h"
#include "chunk.h"
#include "camera.h"

const unsigned int screenwidth = 1280;
const unsigned int screenheight = 720;

Camera camera;
float deltatime = 0.0f;
float lastframe = 0.0f;

void framebuffersizecallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mousecallback(GLFWwindow *window, double xpos, double ypos) {
    camera.processMouse(xpos, ypos);
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
    
    std::vector<Chunk> chunks;
    const int renderdistance = 10;
    for (int x = -renderdistance; x <= renderdistance; x++) {
        for (int z = -renderdistance; z <= renderdistance; z++) {
            chunks.emplace_back(x, z);
        }
    }
    
    glEnable(GL_DEPTH_TEST);
    
    while (!glfwWindowShouldClose(window)) {
        float currentframe = glfwGetTime();
        deltatime = currentframe - lastframe;
        lastframe = currentframe;
        
        camera.processKeyboard(window, deltatime);
        
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
        
        for (auto &chunk : chunks) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), 
                                           glm::vec3(chunk.chunkx * Chunk::size, 0.0f, 
                                                    chunk.chunkz * Chunk::size));
            shader.setmat4("model", model);
            chunk.draw();
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}
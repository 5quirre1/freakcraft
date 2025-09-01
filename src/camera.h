#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Camera {
public:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    float yaw;
    float pitch;
    float speed;
    float sensitivity;
    
    Camera();
    void processKeyboard(GLFWwindow* window, float deltaTime);
    void processMouse(double xpos, double ypos);
    glm::mat4 getViewMatrix();
    
private:
    float lastX, lastY;
    bool firstMouse;
    void updateCameraVectors();
};
#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

Camera::Camera() : position(0.0f, 25.0f, 25.0f), front(0.0f, -0.5f, -1.0f), up(0.0f, 1.0f, 0.0f),
                   yaw(-90.0f), pitch(0.0f), speed(15.0f), sensitivity(0.5f),
                   lastX(640.0f), lastY(360.0f), firstMouse(true) {
    updateCameraVectors();
}

void Camera::processKeyboard(GLFWwindow* window, float deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    
    glm::vec3 frontFlat = glm::normalize(glm::vec3(front.x, 0.0f, front.z));
    glm::vec3 rightFlat = glm::normalize(glm::cross(frontFlat, glm::vec3(0.0f, 1.0f, 0.0f)));
    float velocity = speed * deltaTime;
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        position += frontFlat * velocity;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        position -= frontFlat * velocity;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        position += rightFlat * velocity;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        position -= rightFlat * velocity;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        position += glm::vec3(0.0f, 1.0f, 0.0f) * velocity;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        position -= glm::vec3(0.0f, 1.0f, 0.0f) * velocity;
}

void Camera::processMouse(double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    
    yaw += xoffset;
    pitch += yoffset;
    
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
    
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() {
    return glm::lookAt(position, position + front, up);
}

void Camera::updateCameraVectors() {
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(newFront);
}
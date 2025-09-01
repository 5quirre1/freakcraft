#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <iostream>

class Shader {
public:
    unsigned int id;
    
    Shader(const char *vertexsource, const char *fragmentsource);
    void use();
    void setmat4(const std::string &name, const glm::mat4 &mat) const;
    void setvec3(const std::string &name, const glm::vec3 &value) const;

private:
    void checkcompileerrors(unsigned int shader, std::string type);
};

extern const char *vertexshader;
extern const char *fragmentshader;
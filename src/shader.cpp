#include "shader.h"

const char *vertexshader = R"glsl(
#version 330 core
layout (location = 0) in vec3 apos;
layout (location = 1) in vec3 acolor;
layout (location = 2) in vec3 anormal;
out vec3 ourcolor;
out vec3 normal;
out vec3 fragpos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main() {
    gl_Position = projection * view * model * vec4(apos, 1.0f);
    ourcolor = acolor;
    normal = mat3(transpose(inverse(model))) * anormal;
    fragpos = vec3(model * vec4(apos, 1.0f));
}
)glsl";

const char *fragmentshader = R"glsl(
#version 330 core
in vec3 ourcolor;
in vec3 normal;
in vec3 fragpos;
out vec4 fragcolor;
uniform vec3 lightpos;
uniform vec3 viewpos;
void main() {
    float ambientstrength = 0.2f;
    vec3 ambient = ambientstrength * vec3(1.0f);
    vec3 norm = normalize(normal);
    vec3 lightdir = normalize(lightpos - fragpos);
    float diff = max(dot(norm, lightdir), 0.0f);
    vec3 diffuse = diff * vec3(1.0f);
    float specularstrength = 0.5f;
    vec3 viewdir = normalize(viewpos - fragpos);
    vec3 reflectdir = reflect(-lightdir, norm);
    float spec = pow(max(dot(viewdir, reflectdir), 0.0f), 32);
    vec3 specular = specularstrength * spec * vec3(1.0f);
    vec3 result = (ambient + diffuse + specular) * ourcolor;
    fragcolor = vec4(result, 1.0f);
}
)glsl";

Shader::Shader(const char *vertexsource, const char *fragmentsource) {
    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexsource, NULL);
    glCompileShader(vertex);
    checkcompileerrors(vertex, "VERTEX");
    
    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentsource, NULL);
    glCompileShader(fragment);
    checkcompileerrors(fragment, "FRAGMENT");
    
    id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    glLinkProgram(id);
    checkcompileerrors(id, "PROGRAM");
    
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use() {
    glUseProgram(id);
}

void Shader::setmat4(const std::string &name, const glm::mat4 &mat) const {
    glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setvec3(const std::string &name, const glm::vec3 &value) const {
    glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}

void Shader::checkcompileerrors(unsigned int shader, std::string type) {
    int success;
    char infolog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infolog);
            std::cout << "error shader compilation " << type << ": " << infolog << std::endl;
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infolog);
            std::cout << "error shader linking: " << infolog << std::endl;
        }
    }
}
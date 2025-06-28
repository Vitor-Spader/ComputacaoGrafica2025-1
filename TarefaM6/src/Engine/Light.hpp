#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

class Light {
private:
    float ka = 0.2f;
    float kd =0.5f;
    float ks = 0.5f;
    float q = 20.0f;
    std::vector<glm::vec3> lightPosition;

public:
    
    Light(float ka, float kd, float ks, float q);

    void addLightPosition(glm::vec3 position);

    void updateUniformData(GLuint shaderID);
};

#endif

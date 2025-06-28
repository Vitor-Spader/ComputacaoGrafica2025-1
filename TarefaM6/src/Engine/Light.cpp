#include <iostream>
#include <string>
#include <vector>

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

#include "Light.hpp"

Light::Light(float ka, float kd, float ks, float q)
{
    this->ka = ka;
    this->kd = kd;
    this->ks = ks;
    this->q = q;
};

void Light::addLightPosition(glm::vec3 position)
{
    lightPosition.push_back(position);
}

void Light::updateUniformData(GLuint shaderID)
{
    glUniform1f(glGetUniformLocation(shaderID, "ka"), ka);
	glUniform1f(glGetUniformLocation(shaderID, "kd"), kd);
	glUniform1f(glGetUniformLocation(shaderID, "ks"), ks);
	glUniform1f(glGetUniformLocation(shaderID, "q"), q);
	for (int x = 0; x < lightPosition.size(); x++)
    {
		glUniform3f(
            glGetUniformLocation(shaderID, ("light[" + std::to_string(x) + "]").c_str()), 
            lightPosition[x].x,
            lightPosition[x].y,
            lightPosition[x].z
        );
    }
}
#ifndef MESH_HPP
#define MESH_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

class Mesh {
private:
    GLuint VAO;
    glm::vec3 pos;
    glm::vec3 dimensions;
    float angle;
    float vel;
    GLuint drawMode;
    GLuint nVertices;

    void createPointsVAO(std::vector<glm::vec3> points);

public:
    
    Mesh(std::vector<glm::vec3> points, glm::vec3 pos, glm::vec3 dimensions, float angle, float vel, GLuint drawMode);
    void draw(GLuint shaderID);
    void setPos(glm::vec3 pos);
};

#endif

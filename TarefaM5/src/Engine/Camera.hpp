#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

struct windowProperties
{
    GLfloat WIDTH;
    GLfloat HEIGHT;
};

struct mouseProperties
{
    bool first = true;
    float lastX, lastY;
    //para calcular o quanto que o mouse deslocou
    float yaw = -90.0, pitch = 0.0; //rotação em x e y
};

class Camera {
private:
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);//direction
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    float speed = 0.1f;
    float fov = 45.0f; // Angulo de abertura da camera

    mouseProperties mouseControl;

    windowProperties window;
    GLuint shaderID;

    float lastFrame = glfwGetTime();
    void moderateSpeed();

public:
    
    Camera(glm::vec3 cameraPosition, glm::vec3 cameraFront, glm::vec3 cameraUp, float moveSpeed);

    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();
    void updateUniformProperties();
    void setWindowSize(const GLfloat WIDTH, const GLfloat HEIGHT);
    void setShaderID(const GLuint programShaderID);
    GLuint getModelLoc();
    void calculateFov(double yoffset);
    void updateUniformView();

    //mouse
    void calculateCameraPositionByMouse(double xpos, double ypos);
};

#endif

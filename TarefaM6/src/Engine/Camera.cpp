#include "Camera.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraFront, glm::vec3 cameraUp, float moveSpeed)
{

    position = cameraPosition;
    front = cameraFront;
    up = cameraUp;
    speed = moveSpeed;
}

void Camera::moveUp()
{
    moderateSpeed();
    position += speed * front;
}

void Camera::moveDown()
{
    moderateSpeed();
    position -= speed * front;
}

void Camera::moveLeft()
{
    moderateSpeed();
    position -= glm::normalize(glm::cross(front, up)) * speed;
}

void Camera::moveRight()
{
    moderateSpeed();
    position += glm::normalize(glm::cross(front, up)) * speed;
}

void Camera::moderateSpeed()
{
    float currentFrame = glfwGetTime();
    float deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    float cameraSpeed = 2.5f * deltaTime;
}

void Camera::setWindowSize(const GLfloat WIDTH, const GLfloat HEIGHT)
{
    window.WIDTH = WIDTH;
    window.HEIGHT = HEIGHT;
}

void Camera::setShaderID(const GLuint programShaderID)
{
    shaderID = programShaderID;
}

GLuint Camera::getModelLoc()
{
    return glGetUniformLocation(shaderID, "model");
}

void Camera::updateUniformProperties()
{
    // Projeção perspectiva: fov de 45°, aspect ratio da janela
    glm::mat4 projection = glm::perspective(glm::radians(fov), window.WIDTH / window.HEIGHT, 0.1f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    updateUniformView();

	// Matriz de modelo: transformações na geometria (objeto)
	glm::mat4 model = glm::mat4(1); // matriz identidade
	glUniformMatrix4fv(getModelLoc(), 1, GL_FALSE, value_ptr(model));
}

void Camera::updateUniformView()
{
    // View: posiciona a câmera atrás e centralizada
    glm::mat4 view = glm::lookAt(
        position,//glm::vec3(0.0f, 0.0f, 3.0f), // posição da câmera
        position+front,//glm::vec3(0.0f, 0.0f, 0.0f), // olha para a origem
        up//glm::vec3(0.0f, 1.0f, 0.0f)  // vetor "up"
    );
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "view"), 1, GL_FALSE, glm::value_ptr(view));
}

void Camera::calculateCameraPositionByMouse(double xpos, double ypos)
{
    if (mouseControl.first)
	{
		mouseControl.lastX = xpos;
		mouseControl.lastY = ypos;
		mouseControl.first = false;
	}

	float xoffset = xpos - mouseControl.lastX;
	float yoffset = mouseControl.lastY - ypos;
	mouseControl.lastX = xpos;
	mouseControl.lastY = ypos;

	// Suaviza o movimento
	float sensitivity = 0.05;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	mouseControl.yaw += xoffset;
	mouseControl.pitch += yoffset;

	if (mouseControl.pitch > 89.0f)
		mouseControl.pitch = 89.0f;
	if (mouseControl.pitch < -89.0f)
		mouseControl.pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(mouseControl.yaw)) * cos(glm::radians(mouseControl.pitch));
	front.y = sin(glm::radians(mouseControl.pitch));
	front.z = sin(glm::radians(mouseControl.yaw)) * cos(glm::radians(mouseControl.pitch));
    front = glm::normalize(front);

	// Precisamos também atualizar o cameraUp!! Pra isso, usamos o Up do
	// mundo (y), recalculamos Right e depois o Up
	glm::vec3 right = glm::normalize(glm::cross(front, up));
	up = glm::normalize(glm::cross(right, front));
}

void Camera::calculateFov(double yoffset)
{
    if(fov >= 1.0f && fov <= 45.0f)
        fov -= yoffset;
    if(fov <= 1.0f)
        fov = 1.0f;
    if(fov >= 45.0f)
        fov = 45.0f;
}
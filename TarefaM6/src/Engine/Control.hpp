#ifndef CONTROL_HPP
#define CONTROL_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// GLFW
#include <GLFW/glfw3.h>

class Control {
private:
    //default value
    glm::vec3 defaultPosition;
    glm::vec3 defaultRotation;
    float defaultScale;

    glm::vec3 position;
    glm::vec3 rotation;
    float scale;
    float factor;
    float angle;

public:
    
    Control(float x, float y, float z, float moveFactor);

    glm::mat4 getModel();
    void setPosition(glm::vec3 position);
    void setRotateX(int direction);
    void setRotateY(int direction);
    void setRotateZ(int direction);
    void setTranslateX(int direction);
    void setTranslateY(int direction);
    void setTranslateZ(int direction);
    void setScale(int direction);
    void setAngle(float angle);
    void resetMoveControl();
};

#endif

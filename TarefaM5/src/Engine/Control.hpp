#ifndef CONTROL_HPP
#define CONTROL_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// GLFW
#include <GLFW/glfw3.h>

struct coordinate{
        float X;
        float Y;
        float Z;
};

class Control {
private:
    //default value
    coordinate defaultPosition;
    coordinate defaultRotation;
    float defaultScale;

    coordinate position;
    coordinate rotation;
    float scale;
    float factor;

public:
    
    Control(float x, float y, float z, float moveFactor);

    glm::mat4 getModel();
    void setRotateX(int direction);
    void setRotateY(int direction);
    void setRotateZ(int direction);
    void setTranslateX(int direction);
    void setTranslateY(int direction);
    void setTranslateZ(int direction);
    void setScale(int direction);
    void resetMoveControl();
};

#endif

#include "Control.hpp"
#include <iostream>

Control::Control(float x, float y, float z, float moveFactor) {

    defaultPosition.x = x;
    defaultPosition.y = y;
    defaultPosition.z = z;

    position = defaultPosition;
    
    defaultRotation.x = 0.0f;
    defaultRotation.y = 0.0f;
    defaultRotation.z = 0.0f;

    rotation = defaultRotation;
    
    scale = defaultScale = 1.0f;
    factor = moveFactor;

}

glm::mat4 Control::getModel() {

    glm::mat4 model = glm::mat4(1.0f);
    //Deve sempre realizar a translação do modelo
    //Isso para separar as figuras na tela
    model = glm::translate(model, position);
    if (rotation.x != defaultRotation.x || rotation.y != defaultRotation.y || rotation.z != defaultRotation.z )
        model = glm::rotate(model, angle, rotation);

    if (scale != 0)
        model = glm::scale(model, glm::vec3(scale));
    
    return model;
}

void Control::setAngle(float angle)
{
    this->angle = angle;
}

void Control::setRotateX(int direction){
    if (rotation.x != 0)
    {
        //Stop rotate and reset
        rotation.x = defaultRotation.x;
    }
    else
        rotation.x += factor * direction;
}

void Control::setRotateY(int direction){
    if (rotation.y != 0)
    {
        //Stop rotate and reset
        rotation.y = defaultRotation.y;
    }
    else
        rotation.y += factor * direction;
}

void Control::setRotateZ(int direction){
        if (rotation.z != 0)
    {
        //Stop rotate and reset
        rotation.z = defaultRotation.z;
    }
    else
        rotation.z += factor * direction;
}

void Control::setTranslateX(int direction){
    position.x += factor * direction;
}

void Control::setTranslateY(int direction){
    position.y += factor * direction;
}

void Control::setTranslateZ(int direction){ 
    position.z += factor * direction;
}

void Control::setScale(int direction){
    scale = scale + factor * direction;
}

void Control::resetMoveControl(){
    rotation = defaultRotation;
    scale = defaultScale;
}

void Control::setPosition(glm::vec3 position)
{
    this->position.z = position.z;
    this->position.x = position.x;
    this->position.y = position.y;
}
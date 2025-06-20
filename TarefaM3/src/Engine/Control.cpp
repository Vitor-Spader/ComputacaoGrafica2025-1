#include "Control.hpp"
#include <iostream>

Control::Control(float x, float y, float z, float moveFactor) {
    position.X = x;
    position.Y = y;
    position.Z = z;
    rotation.X = 0.0f;
    rotation.Y = 0.0f;
    rotation.Z = 0.0f;
    scale = 1.0f;
    factor = moveFactor;

}

glm::mat4 Control::getModel() {
    //glm::mat4 model = glm::mat4(1.0f);
    float angle = (GLfloat)glfwGetTime();

    glm::mat4 model = glm::mat4(1.0f);
    //Deve sempre realizar a translação do modelo
    //Isso para separar as figuras na tela
    model = glm::translate(model, glm::vec3(position.X, position.Y, position.Z));
    
    if (rotation.X != 0 || rotation.Y != 0 || rotation.Z != 0 )
        model = glm::rotate(model, angle, glm::vec3(rotation.X, rotation.Y, rotation.Z));

    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    if (scale != 0)
        model = glm::scale(model, glm::vec3(scale));
    
        return model;
}

void Control::setRotateX(int direction){
    if (rotation.X != 0)
    {
        //Stop rotate and reset
        rotation.X = 0.0f;
    }
    else
        rotation.X += factor * direction;
}

void Control::setRotateY(int direction){
    if (rotation.Y != 0)
    {
        //Stop rotate and reset
        rotation.Y = 0.0f;
    }
    else
        rotation.Y += factor * direction;
}

void Control::setRotateZ(int direction){
        if (rotation.Z != 0)
    {
        //Stop rotate and reset
        rotation.Z = 0.0f;
    }
    else
        rotation.Z += factor * direction;
}

void Control::setTranslateX(int direction){
    position.X += factor * direction;
}

void Control::setTranslateY(int direction){
    position.Y += factor * direction;
}

void Control::setTranslateZ(int direction){ 
    position.Z += factor * direction;
}

void Control::setScale(int direction){
    float aux = scale + factor * direction;
    scale = aux;
}

void Control::resetMoveControl(){
    rotation.X = 0.0f;
    rotation.Y = 0.0f;
    rotation.Z = 0.0f;
    scale = 1.0f;
}
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

glm::mat4 Control::getModel(glm::mat4& model) {
    //glm::mat4 model = glm::mat4(1.0f);
    float angle = (GLfloat)glfwGetTime();

    //Deve sempre realizar a translação do modelo
    //Isso para separar as figuras na tela
    model = glm::translate(model, glm::vec3(position.X, position.Y, position.Z));
    
    if (rotation.X != 0 || rotation.Y != 0 || rotation.Z != 0 )
        model = glm::rotate(model, angle, glm::vec3(rotation.X, rotation.Y, rotation.Z));
    if (scale != 0)
        model = glm::scale(model, glm::vec3(scale));
    
        return model;
}

void Control::setRotateX(int direction){
    resetMoveControl();
    rotation.X += factor * direction;
}

void Control::setRotateY(int direction){
    resetMoveControl();
    rotation.Y += factor * direction;
}

void Control::setRotateZ(int direction){
    resetMoveControl();
    rotation.Z += factor * direction;
}

void Control::setTranslateX(int direction){
    float aux = position.X + factor * direction;
    resetMoveControl();
    position.X = aux;
}

void Control::setTranslateY(int direction){
    float aux = position.Y + factor * direction;
    resetMoveControl();
    position.Y = aux;
}

void Control::setTranslateZ(int direction){
    float aux = position.Z + factor * direction;
    resetMoveControl();
    position.Z = aux;
}

void Control::setScale(int direction){
    float aux = scale + factor * direction;
    resetMoveControl();
    scale = aux;
}

void Control::resetMoveControl(){
    rotation.X = 0.0f;
    rotation.Y = 0.0f;
    rotation.Z = 0.0f;
    scale = 1.0f;
}
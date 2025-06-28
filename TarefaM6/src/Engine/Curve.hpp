#ifndef CURVE_HPP
#define CURVE_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

//Bezier Curve
class Curve {
private:
    std::vector<glm::vec3> controlPoints; // Pontos de controle da curva
    std::vector<glm::vec3> curvePoints;   // Pontos da curva
    glm::mat4 M;          // Matriz dos coeficientes da curva

public:
    
    Curve(std::vector<glm::vec3> controlPoints, int numPoints);
    std::vector<glm::vec3> getPoints();
    void initializeBernsteinMatrix();
    void generateBezierCurvePoints(int numPoints);

};

#endif

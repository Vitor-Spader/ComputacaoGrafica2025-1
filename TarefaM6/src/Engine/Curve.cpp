#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

#include "Curve.hpp"

Curve::Curve(std::vector<glm::vec3> controlPoints, int numPoints)
{
    this->controlPoints = controlPoints;
    initializeBernsteinMatrix(); // M
    generateBezierCurvePoints(numPoints); // curvePoints
}

std::vector<glm::vec3> Curve::getPoints()
{
    return curvePoints;
}

void Curve::initializeBernsteinMatrix()
{
    this->M[0] = glm::vec4(-1.0f, 3.0f, -3.0f, 1.0f); // Primeira coluna
    this->M[1] = glm::vec4(3.0f, -6.0f, 3.0f, 0.0f);  // Segunda coluna
    this->M[2] = glm::vec4(-3.0f, 3.0f, 0.0f, 0.0f);  // Terceira coluna
    this->M[3] = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);    // Quarta coluna

}

void Curve::generateBezierCurvePoints(int numPoints)
{
    // Calcular os pontos ao longo da curva com base em Bernstein
     // Loop sobre os pontos de controle em grupos de 4

    float piece = 1.0/ (float) numPoints;
    float t;
    for (int i = 0; i < this->controlPoints.size() - 3; i+=3) {
        
        // Gera pontos para o segmento atual
        for (int j = 0; j < numPoints;j++) {
            t = j * piece;
            
            // Vetor t para o polinômio de Bernstein
            glm::vec4 T(t * t * t, t * t, t, 1);
            
            glm::vec3 P0 = this->controlPoints[i];
			glm::vec3 P1 = this->controlPoints[i + 1];
			glm::vec3 P2 = this->controlPoints[i + 2];
			glm::vec3 P3 = this->controlPoints[i + 3];

			glm::mat4x3 G(P0, P1, P2, P3);

            // Calcula o ponto da curva multiplicando tVector, a matriz de Bernstein e os pontos de controle
            glm::vec3 point = G * this->M * T;
            
            this->curvePoints.push_back(point);
        }
    }
}

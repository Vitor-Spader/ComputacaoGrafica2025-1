#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

#include "Mesh.hpp"

Mesh::Mesh(std::vector<glm::vec3> points, glm::vec3 pos, glm::vec3 dimensions, float angle, float vel, GLuint drawMode)
{
    this->pos = pos;
    this->dimensions = dimensions;
    this->angle = angle;
    this->vel = vel;
    this->drawMode = drawMode;
    this->nVertices = points.size();
    createPointsVAO(points);
}

void Mesh::draw(GLuint shaderID)
{
    //Usa o mesmo buffer de geometria para todos os sprites
    glBindVertexArray(this->VAO);

    glm::mat4 model = glm::mat4(1);
    model = glm::translate(model, this->pos);
    model = glm::rotate(model, glm::radians(this->angle), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, this->dimensions);
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glDrawArrays(this->drawMode, 0, this->nVertices);

    glBindVertexArray(0);
}

void Mesh::createPointsVAO(std::vector<glm::vec3> points)
{
	GLuint VBO;
	// Geração do identificador do VBO
	glGenBuffers(1, &VBO);
	// Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, points.size()*sizeof(glm::vec3), points.data(), GL_STATIC_DRAW);

	// Geração do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &this->VAO);
	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
	// e os ponteiros para os atributos
	glBindVertexArray(this->VAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

}

void Mesh::setPos(glm::vec3 pos)
{
    this->pos = pos;
}
/*
 *  Codificado por Rossana Baptista Queiroz
 *  para as disciplinas de Processamento Gráfico/Computação Gráfica - Unisinos
 *  Versão inicial: 07/04/2017
 *  Última atualização: 14/03/2025
 *
 *  Este arquivo contém a função `loadSimpleOBJ`, responsável por carregar arquivos
 *  no formato Wavefront .OBJ e armazenar seus vértices em um VAO para renderização
 *  com OpenGL.
 *
 *  Forma de uso (carregamento de um .obj)
 *  -----------------
 *  ...
 *  int nVertices;
 *  GLuint objVAO = loadSimpleOBJ("../Modelos3D/Cube.obj", nVertices);
 *  ...
 *
 *  Chamada de desenho (Polígono Preenchido - GL_TRIANGLES), no loop do programa:
 *  ----------------------------------------------------------
 *  ...
 *  glBindVertexArray(objVAO);
 *  glDrawArrays(GL_TRIANGLES, 0, nVertices);
 *
 */

 // Cabeçalhos necessários (para esta função), acrescentar ao seu código 
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
 
 
using namespace std;

// GLAD
#include <glad/glad.h>
 
// GLFW
#include <GLFW/glfw3.h>
 
//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Mesh 
{
    GLuint VAO; 

};

int loadSimpleOBJ(string filePATH, int &nVertices);
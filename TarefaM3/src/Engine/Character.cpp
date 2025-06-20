#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

//custom imports
#include "Control.hpp"
#include "Character.hpp"

Character::Character(Control* characterControl, std::string objFilePath, bool hasTexture)
    : control(characterControl), vertexArrayObject(0), textureId(0), hasTextureVAO(false)
{
    vertexArrayObject = loadOBJ(objFilePath);
    if (hasTexture)
    {
        loadTexture(getTextureFromMTL(objFilePath));
    }
    
};

Character::~Character() {
    
    delete control;

    if (vertexArrayObject) {
        glDeleteVertexArrays(1, &vertexArrayObject);
    }
    if (textureId) {
        glDeleteTextures(1, &textureId);
    }
};

GLuint& Character::getVAO()
{
    return vertexArrayObject;
};

GLuint Character::getTextureVAO()
{
    return textureId;
};

void Character::draw(const GLint modelLoc, const GLint textureLoc, const bool drawPoints) const {
    
    // glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(vertexArrayObject);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(control->getModel()));
    //glUniform4f(glGetUniformLocation(modelLoc, "inputColor"), 0.0f, 0.0f, 0.1f, 1.0f);
    
    glUniform1i(textureLoc,0);

    glDrawArrays(GL_TRIANGLES, 0 , nVertices);
    
    if (drawPoints)
        glDrawArrays(GL_POINTS, 0, nVertices);

    glBindVertexArray(0);
};


GLuint Character::loadOBJ(std::string filePATH)
{
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normals;
    std::vector<GLfloat> vBuffer;
    glm::vec3 color = glm::vec3(1.0, 0.0, 0.0);

    std::ifstream arqEntrada(filePATH.c_str());
    if (!arqEntrada.is_open()) 
	{
        std::cerr << "Erro ao tentar ler o arquivo " << filePATH << std::endl;
        return -1;
    }

    std::string line;
    while (getline(arqEntrada, line)) 
	{
        std::istringstream ssline(line);
        std::string word;
        ssline >> word;

        if (word == "v") 
		{
            glm::vec3 vertice;
            ssline >> vertice.x >> vertice.y >> vertice.z;
            vertices.push_back(vertice);
        } 
        else if (word == "vt") 
		{
            glm::vec2 vt;
            ssline >> vt.s >> vt.t;
            vt.t = 1 - vt.t;
            texCoords.push_back(vt);
        } 
        else if (word == "vn") 
		{
            glm::vec3 normal;
            ssline >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        } 
        else if (word == "f")
		 {
            while (ssline >> word) 
			{
                int vi = 0, ti = 0, ni = 0;
                std::istringstream ss(word);
                std::string index;

                if (getline(ss, index, '/')) vi = !index.empty() ? stoi(index) - 1 : 0;
                if (getline(ss, index, '/')) ti = !index.empty() ? stoi(index) - 1 : 0;
                if (getline(ss, index)) ni = !index.empty() ? stoi(index) - 1 : 0;
                
                vBuffer.push_back(vertices[vi].x);
                vBuffer.push_back(vertices[vi].y);
                vBuffer.push_back(vertices[vi].z);
                vBuffer.push_back(color.r);
                vBuffer.push_back(color.g);
                vBuffer.push_back(color.b);
                vBuffer.push_back(texCoords[ti].x);
                vBuffer.push_back(texCoords[ti].y);
            }
        }
    }

    arqEntrada.close();

    std::cout << "Gerando o buffer de geometria..." << std::endl;
    GLuint VBO, VAO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vBuffer.size() * sizeof(GLfloat), vBuffer.data(), GL_STATIC_DRAW);
    
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

	nVertices = vBuffer.size() / 8;  // x, y, z, r, g, b (valores atualmente armazenados por vértice)

    return VAO;
};


void Character::loadTexture(std::string filePath)
{

	// Gera o identificador da textura na memória
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);

	// Ajuste dos parâmetros de wrapping e filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Carregamento da imagem usando a função stbi_load da biblioteca stb_image
	int nrChannels;

	unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

	if (data)
	{
		if (nrChannels == 3) // jpg, bmp
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else // assume que é 4 canais png
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture " << filePath << std::endl;
	}

	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0);
}

std::string Character::getTextureFromMTL(const std::string& objPath) 
{
    namespace fs = std::filesystem;

    // Converte o caminho do obj para path do filesystem
    fs::path objFilePath(objPath);
    
    // Substitui extensão para .mtl
    fs::path mtlPath = objFilePath;
    mtlPath.replace_extension(".mtl");

    // Tenta abrir o arquivo .mtl
    std::ifstream arqEntrada(mtlPath);
    if (!arqEntrada.is_open()) 
    {
        std::cerr << "Erro ao tentar ler o arquivo MTL: " << mtlPath << std::endl;
        return "";
    }

    // Procura pelo map_Kd
    std::string line;
    while (std::getline(arqEntrada, line)) 
    {
        std::istringstream ssline(line);
        std::string word;
        ssline >> word;

        if (word == "map_Kd") 
        {
            std::string textureFile;
            ssline >> textureFile;

            // Caminho completo da textura no mesmo diretório do .obj
            fs::path texturePath = objFilePath.parent_path() / textureFile;
            return texturePath.string();
        }
    }

    std::cerr << "map_Kd não encontrado no arquivo: " << mtlPath << std::endl;
    return "";
}

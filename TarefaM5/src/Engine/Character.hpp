#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include <iostream>
#include <string>
#include <assert.h>
#include <vector>
#include <fstream>
#include <sstream>

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

//custom imports
#include "Control.hpp"

struct LigthProperties{
    float ka;
	float kd = 0.5f;
	float ks;
	float q;
};

class Character {
private:
    GLuint vertexArrayObject;
    GLuint textureId;
    bool hasTextureVAO;
    int nVertices, width, height;
    std::string textureFilePath;
    LigthProperties ligthProperties;

    //Ids
    GLuint textureLoc;
    GLuint modelLoc;
    
    GLuint loadOBJ(std::string filePATH);
    void loadMTLData(const std::string& objPath);
    void loadTexture();

public:
    Control* control;
    Character(Control* characterControl, std::string objFilePath, bool hasTexture);

    GLuint& getVAO();
    GLuint getTextureVAO();
    void initializeUniformProperties(GLuint shaderID);
    void draw(const GLuint shaderID, const bool drawPoints) const;
    ~Character();
};

#endif

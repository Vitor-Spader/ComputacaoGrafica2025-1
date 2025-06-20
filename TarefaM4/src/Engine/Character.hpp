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

class Character {
private:
    GLuint vertexArrayObject;
    GLuint textureId;
    bool hasTextureVAO;
    int nVertices, width, height;
    
    GLuint loadOBJ(std::string filePATH);
    std::string getTextureFromMTL(const std::string& objPath);
    void loadTexture(std::string filePath);

public:
    Control* control;
    Character(Control* characterControl, std::string objFilePath, bool hasTexture);

    GLuint& getVAO();
    GLuint getTextureVAO();

    void draw(const GLint modelLoc, const GLint textureLoc, const bool drawPoints) const;
    ~Character();
};

#endif

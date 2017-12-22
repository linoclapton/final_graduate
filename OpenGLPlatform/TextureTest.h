#pragma once
#include "RenderBase.h"
#include "GLSLProgram.h"

class TextureTest:public RenderBase{
private:
    GLSLProgram glsl;
public:
    GLuint VBO[3];
    GLuint VAO[3];
    GLuint EBO[3];
    GLuint texture;
    GLuint texture1;
    unsigned char* myImg;
    void initial();
    void render();
};


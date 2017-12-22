#pragma once
#include "RenderBase.h"

class FBOModel:public RenderBase{
private:
    GLSLProgram glsl;
    GLuint VBO[3];
    GLuint VAO[3];
    GLuint EBO[3];
    GLuint FBO;
    GLuint RBO;
    GLuint texture;
    GLuint texture1;
    glm::mat4 mv;
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
    float eyeZPosition;
    bool hasLight;
public:
    void initial();
    void render();
    void rotate(glm::mat4 rMat);
};


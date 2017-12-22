#pragma once
#include "GLSLProgram.h"
class RenderBase{
public:
    virtual void initial()=0;
    virtual void render()=0;
    virtual void rotate(glm::mat4){}
};
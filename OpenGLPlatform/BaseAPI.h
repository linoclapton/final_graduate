#pragma once
#include "RenderBase.h"

class BaseAPI:public RenderBase{
private:
public:
    void initial();
    void render();
    void rotate(glm::mat4 rMat);
};
#include "BaseAPI.h"
#include <iostream>

void BaseAPI::initial(){
}

void BaseAPI::render(){
    glColor3f(1.0,1.0,1.0);
    glBegin(GL_LINES);
    glVertex3f(1.0,1.0,0.1);
    glVertex3f(-1.0,-1.0,0.1);
    glEnd();

}

void BaseAPI::rotate(glm::mat4 rMat){
}
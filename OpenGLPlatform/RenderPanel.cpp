#include "RenderPanel.h"
#include <windows.h>
#include <QtOpenGL>
#include <QtGui>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <fstream>
#include <iostream>
#include <soil.h>
#include "FBO.h"
#include "TextureTest.h"
#include "BaseAPI.h"
RenderPanel::RenderPanel(QWidget* parent):QGLWidget(parent){
    model = new TextureTest();//new BaseAPI();//new FBOModel();
    QGLFormat format;
    format.setVersion(4,5);
    format.setProfile(QGLFormat::CoreProfile);
	setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer));
    abs(-1.0);
    //model = new TextureTest();
}

void RenderPanel::initializeGL(){
	GLenum error=glewInit();
    if(error){
        std::cout<<"error"<<std::endl;
    }else{
        std::cout<<"success"<<std::endl;
    }
    model->initial();
    glEnable(GL_BLEND);
    //glEnable(GL_ALPHA_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    //glShadeModel(GL_FLAT);
}
void RenderPanel::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();	
}

void RenderPanel::mouseMoveEvent(QMouseEvent *event)
{
    GLfloat sx = lastPos.x()-width()*0.5;
    GLfloat sy = -lastPos.y()+height()*0.5;
    GLfloat sz = sqrt(diagonal*diagonal-sx*sx-sy*sy);
    GLfloat ex = event->x()-width()*0.5;
    GLfloat ey = -event->y()+height()*0.5;
    GLfloat ez = sqrt(diagonal*diagonal-ex*ex-ey*ey);
    GLfloat dx = GLfloat(event->x() - lastPos.x()) / width();
    GLfloat dy = GLfloat(event->y() - lastPos.y()) / height();
    glm::vec3 line = glm::cross(glm::vec3(sx,sy,sz),glm::vec3(ex,ey,ez));
    GLfloat degree = glm::acos(glm::dot(glm::vec3(ex,ey,ez),glm::vec3(sx,sy,sz))/(diagonal*diagonal));
    model->rotate(glm::rotate(degree, line));
    //model = glm::rotate(glm::radians(-180*dx), glm::vec3(1.0f,0.0f,0.0f))*model;
    //model *= glm::rotate(glm::radians(180*dy), glm::vec3(0.0f,1.0f,0.0f));
    //glsl.setUniform("NormalMatrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
    lastPos = event->pos();
    updateGL();
}

void RenderPanel::resizeGL(int width, int height){
    //static int times = 0;
    //std::cout<<"resize"<<times<<std::endl;
    diagonal = sqrt(width*width+height*height)*0.5;
	glViewport(0, 0, width, height);
}
void RenderPanel::paintGL(){
    //static int times = 0;
    //std::cout<<"paint"<<times<<std::endl;
    //times++;
	glClearColor(0.2f,0.3f,0.3f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
    model->render();
}
void RenderPanel::problem1Init(){
    glsl.create();
    glsl.attachShader(GL_VERTEX_SHADER,"vertex1.glsl");
    glsl.attachShader(GL_FRAGMENT_SHADER,"fragment1.glsl");
    glsl.link();
    glsl.use(0);
    glsl.change(1);
    glsl.create();
    glsl.attachShader(GL_VERTEX_SHADER,"vertex1.glsl");
    glsl.attachShader(GL_FRAGMENT_SHADER,"fragment2.glsl");
    glsl.link();
    glsl.use(0);
    GLfloat vertices[] = {
        0.5f, 0.5f, 0.0f, // Top Right
        0.5f, -0.5f, 0.0f, // Bottom Right
        -0.5f, -0.5f, 0.0f, // Bottom Left
        -0.5f, 0.5f, 0.0f // Top Left
    };
    GLuint indices[] = { // Note that we start from 0!
        0, 1, 3, // First Triangle
    };
    GLuint indices2[]={
        1, 2, 3 // Second Triangle
    };
    glsl.genBuffers(2,VBO);
    glsl.genBuffers(2,EBO);
    glsl.genVertexArrays(2,VAO);
    glBindVertexArray(VAO[0]);
    // 2. Copy our vertices array in a vertex buffer for
    glsl.bindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glsl.bufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 3. Copy our index array in a element buffer for OpenGL
    glsl.bindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
    glsl.bufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // 3. Then set the vertex attributes pointers
    glsl.vertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (GLfloat), (GLvoid*)0);
    glsl.enableVertexAttribArray(0);
    // 4. Unbind VAO (NOT the EBO)
    glBindVertexArray(0);
    glBindVertexArray(VAO[1]);
    // 2. Copy our vertices array in a vertex buffer for
    glsl.bindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glsl.bufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 3. Copy our index array in a element buffer for OpenGL
    glsl.bindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
    glsl.bufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices2), indices2, GL_STATIC_DRAW);
    // 3. Then set the vertex attributes pointers
    glsl.vertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (GLfloat), (GLvoid*)0);
    glsl.enableVertexAttribArray(0);
    // 4. Unbind VAO (NOT the EBO)
    glBindVertexArray(0);
}
void RenderPanel::problem1Render(){
    glsl.use(0);
    glBindVertexArray(VAO[0]);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
    glsl.use(1);
    glBindVertexArray(VAO[1]);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

}
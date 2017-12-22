#include "RenderPanel.h"
#include <windows.h>
#include <QtOpenGL>
#include <QtGui>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <fstream>
#include <iostream>
RenderPanel::RenderPanel(QWidget* parent):QGLWidget(parent){
	setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer));

}

void RenderPanel::initializeGL(){
	GLenum err=glewInit();
    glsl.create();
    glsl.attachShader(GL_VERTEX_SHADER,"vertex1.glsl");
    glsl.attachShader(GL_FRAGMENT_SHADER,"fragment1.glsl");
    glsl.link();
    glsl.use(0);
    GLuint indices[] = { // Note that we start from 0!
        0, 1, 3, // First Triangle
        1, 2, 3 // Second Triangle
    };
    /*GLfloat vertices[] = {
        // Positions // Colors // Texture Coords
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // Top Right
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom Right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // Bottom Left
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f // Top Left
    };*/
    GLfloat vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    //std::cout<<sizeof(points)<<std::endl;
    /*for(int i=0;i<32;i++)
        for(int j=0;j<32;j++)
            for(int k=0;k<32;j++)
            {
                points[i][j][k][0]=i/256.0;
                points[i][j][k][1]=j/256.0;
                points[i][j][k][2]=k/256.0;
            }*/
    //glm::mat4 model;
    //model = glm::translate(model , glm::vec3(0.707, -0.707, 0.0));
    //model = glm::rotate(model , 0.8f, glm::vec3(0.0, 0.0, 1.0));
    //model = glm::translate(model , glm::vec3(0.707, 0.707, 0.0));
    //model = glm::rotate(model , 0.8f, glm::vec3(0.0, 0.0, 1.0));
    //glsl.setUniform("Model",model);
    glsl.genBuffers(2,VBO);
    glsl.genBuffers(2,EBO);
    glsl.genVertexArrays(2,VAO);
    glBindVertexArray(VAO[0]);
    // 2. Copy our vertices array in a vertex buffer for
    glsl.bindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glsl.bufferData(GL_ARRAY_BUFFER, sizeof(vertices),vertices, GL_STATIC_DRAW);
    // 3. Copy our index array in a element buffer for OpenGL
    glsl.bindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
    glsl.bufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // 3. Then set the vertex attributes pointers
    glsl.vertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (GLfloat), (GLvoid*)0);
    glsl.enableVertexAttribArray(0);
    //glsl.vertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof (GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
    //glsl.enableVertexAttribArray(1);
    glsl.vertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof (GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
    glsl.enableVertexAttribArray(2);
    // 4. Unbind VAO (NOT the EBO)
    glBindVertexArray(0);

    glm::mat4 view;
    view = glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    Camera camera;
    float pos[3]={0.0f,0.0f,2.0f};
    float target[3]={0.0f,0.0f,0.0f};
    float up[3]={0.0f,1.0f,0.0f};
    float* viewf = camera.lookAt(pos,target,up);
    model = glm::translate(glm::vec3(-0.5,-0.5,-0.5));
    glsl.setUniform("View",view);
    glsl.setUniform("Model",model);
    int w = width(),h = height();
    glm::mat4 projection = glm::perspective<float>(glm::radians(45.0f), (width()+0.0)/height(), 0.1f, 100.0f);
    glsl.setUniform("Projection",projection);
    glEnable(GL_BLEND);
    //glEnable(GL_ALPHA_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    //glShadeModel(GL_FLAT);
}

void RenderPanel::resizeGL(int width, int height){
	glViewport(0, 0, width, height);
}
void RenderPanel::paintGL(){
	glClearColor(0.2f,0.3f,0.3f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
    glsl.use(1);
    glPointSize(5);
    glColor3f(1.0f,0.0f,0.0f);
    glBegin(GL_POINTS);
    glVertex3f(0.0f,0.0f,0.0f);
    glEnd();
    glsl.use(0);
    //glsl.glActiveTex(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D,texture);
    //model = glm::translate(model , glm::vec3(0.707, -0.707, 0.0));
    //model *= glm::rotate(glm::radians(-35.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    //model *= glm::rotate(glm::radians(35.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    //projection = glm::perspective(glm::radians(45.0f), (float)width() / height(), 0.1f, 100.0f);
    //model = glm::rotate(model , 0.8f, glm::vec3(0.0, 0.0, 1.0));
    // Note that we¡¯re translating the scene in the reverse direction of where we want to move
    //view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    glBindVertexArray(VAO[0]);
    glDrawArrays(GL_TRIANGLES, 0, 256*256);
    //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
void RenderPanel::mousePressEvent(QMouseEvent *event){
    lastPos = event->pos();	
}
void RenderPanel::mouseMoveEvent(QMouseEvent *event){
    GLfloat dx = GLfloat(event->x() - lastPos.x()) / width();
    GLfloat dy = GLfloat(event->y() - lastPos.y()) / height();

    model = glm::rotate(glm::radians(-180*dx), glm::vec3(1.0f,0.0f,0.0f))*model;
    model *= glm::rotate(glm::radians(180*dy), glm::vec3(0.0f,1.0f,0.0f));
    glsl.setUniform("Model",model);
    lastPos = event->pos();
    updateGL();
}
#pragma once
#include "GLSLProgram.h"
#include <QGLWidget>
#include <qlabel.h>
class RenderPanel: public QGLWidget
{
    Q_OBJECT
public:
    RenderPanel(QWidget *parent = 0);
protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
/*    
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);*/
private:
    glm::mat4 model;
    QPoint lastPos;
    GLSLProgram glsl;
    GLuint VAO[3];
    GLuint VBO[3];
    GLuint EBO[3];
    GLuint texture;
    GLuint texture1;
    //GLfloat points[32][32][32][3];
};



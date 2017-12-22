#pragma once
#include "GLSLProgram.h"
#include <QGLWidget>
#include <qlabel.h>
#include "RenderBase.h"
class RenderPanel: public QGLWidget
{
    Q_OBJECT
public:
    RenderPanel(QWidget *parent = 0);
protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    /*void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);*/
private:
    void problem1Init();
    void problem1Render();
    GLSLProgram glsl;
    GLuint VAO[3];
    GLuint VBO[3];
    GLuint EBO[3];
    GLuint texture;
    GLuint texture1;
    QPoint lastPos;
    RenderBase* model;
    float diagonal;
};



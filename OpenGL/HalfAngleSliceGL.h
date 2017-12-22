#pragma once
#include "GLSLProgram.h"
#include <QGLWidget>
#include "GL_Model.h"
#include <QMouseEvent>

class HalfAngleSliceGL: public QGLWidget
{
    Q_OBJECT
public:
    HalfAngleSliceGL(QWidget *parent = 0);
    void reload(QString path);
    ULY::GL_Obj_Model model[10];
protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    GLSLProgram glsl;
    GLuint texture1;
    int w,h;
    unsigned char* img;
    void mouseMoveEvent(QMouseEvent *event);
    /*void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);*/
};
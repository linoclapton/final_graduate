#pragma once
#include "GLSLProgram.h"
#include <QGLWidget>

class GIDVRGL : public QGLWidget
{
    Q_OBJECT
public:
    GIDVRGL(QWidget *parent = 0);
protected:
    void initializeGL();
    void initializeModel();
    void resizeGL(int width, int height);
    void paintGL();
    void vertex(int index);
    void drawCube();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    //void mouseDoubleClickEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
private:
    std::string filename;
    int dimension[3];
    float *udata;
    float color[256][4];
    float opacity[256];
    GLSLProgram glsl;
    GLuint frontFBO;
    GLuint backFBO;
    GLuint textureID2;
    GLuint textureID3;
    int counter;
    int totalTime;
    int lastFPS;
    glm::mat4 mv;
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
    glm::mat4 camera;
    int volumeX,volumeY,volumeZ;
    float spaceX,spaceY,spaceZ;
    int diagonal;
    QPoint	lastPos;
    float ratio[3];
    int currentZ;
    GLuint illumA, illumB;
};
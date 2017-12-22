#pragma once
#include "GLSLProgram.h"
#include "Torus.h"
#include <QGLWidget>
#include <QLabel>
class MyPanel : public QGLWidget
{
	Q_OBJECT

public:
	MyPanel(Torus*,QWidget *parent = 0);
    MyPanel(QWidget *parent=0);
	void changeModel(float value,int index);
	void makeShaderVF();
	void makeShaderVTF();
	void makeShaderVGF();
public slots:
	void changeShader(const QString &str);
	void changeLight(const QString &str);
    void setSpecular(int s);
    void setDiffuse(int d);
    void setAmbient(int a);
signals:
	void setLight(const QString &str);
protected:
	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent *event);
private:
	void draw();
	void initialModel();
	void resetGLSL();
	void saveMVPN();
	void resetMVPN();
	QPoint	lastPos;
	Torus *torus,*torus2;
	boolean isMouseDown;
	GLSLProgram glsl;
	glm::mat4 mv;
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
	float eyeZPosition;
	bool hasLight;
	QLabel *label;
	enum Shader_Stage{VF,VTF,VGF} stage;
    GLuint index;
    GLubyte list[2];
    float ks,kd,ka;
	

};

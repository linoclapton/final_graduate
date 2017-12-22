#include "MyPanel.h"
#include <QtGui>
#include <QtOpenGL>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <QLabel>
#include <iostream>

MyPanel::MyPanel(Torus *torus,QWidget *parent):QGLWidget(parent),torus(torus) {
	setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer));
    torus2 =new Torus(0.7,0.15,3,3);
    ks=kd=ka=0.5;
	isMouseDown = false;
	label = new QLabel();
}

MyPanel::MyPanel(QWidget *parent):QGLWidget(parent)
{
	setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer));
    torus2 =new Torus(0.7,0.15,3,3);
    torus =new Torus(0.7,0.15,60,60);
    ks=kd=ka=0.5;
	isMouseDown = false;
	label = new QLabel();
}
void MyPanel::initializeGL()
{
	GLenum err=glewInit();
	if(err!=GLEW_OK)
	{
		//Problem: glewInit failed, something is seriously wrong.
	}

	glsl.create();
	glsl.attachShader(GL_VERTEX_SHADER, "vertex.glsl");
	glsl.attachShader(GL_FRAGMENT_SHADER, "fragment.glsl");
	glsl.link();
	glsl.use(0);

	stage = VF;
	hasLight = true;
	glsl.setUniform("hasLight",hasLight);

	torus->setRenderMode(GL_TRIANGLES);
	torus->model();
	torus2->setRenderMode(GL_TRIANGLES);
	torus2->model();
	initialModel();
	glEnable(GL_DEPTH_TEST);
	qglClearColor(Qt::black);
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
void MyPanel::resetGLSL(){
	glsl.deleteShaders();
	glsl.deleteProgram();
}
void MyPanel::resetMVPN(){
	glsl.setUniform("Kd", 0.9f, 0.5f, 0.3f);
	glsl.setUniform("Ld", 1.0f, 1.0f, 1.0f);
	glsl.setUniform("Ks", 0.9f, 0.5f, 0.3f);
	glsl.setUniform("Ls", 1.0f, 1.0f, 1.0f);
	glsl.setUniform("EyePosition", 0.0f,0.0f,eyeZPosition);
	glsl.setUniform("Ka", 0.1f, 0.05f, 0.03f);
	glsl.setUniform("La", 1.0f, 1.0f, 1.0f);
	glsl.setUniform("LightPosition", view *glm::vec4(5.0f, 5.0f, 2.0f, 1.0f));
	glsl.setUniform("EyePosition", 0.0f,0.0f,eyeZPosition);
	hasLight?changeLight("light"):changeLight("no light");
	glsl.setUniform("View",view);
	glsl.setUniform("Model", model);
	glsl.setUniform("NormalMatrix",glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
	glsl.setUniform("Projection", projection);
}
void MyPanel::saveMVPN(){

}
void MyPanel::makeShaderVF(){
	if(stage != VF)
	{
		resetGLSL();
		glsl.create();
		glsl.attachShader(GL_VERTEX_SHADER, "vertex.glsl");
		glsl.attachShader(GL_FRAGMENT_SHADER, "fragment.glsl");
		glsl.link();
		glsl.use(0);
		changeLight("light");
		resetMVPN();
		torus->setRenderMode(GL_TRIANGLES);
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		stage = VF;
	}
}
void MyPanel::makeShaderVTF(){
	if(stage !=VTF)
	{
		resetGLSL();
		glsl.create();
		glsl.attachShader(GL_VERTEX_SHADER, "vertex.glsl");
		glsl.attachShader(GL_TESS_CONTROL_SHADER,"tcs.glsl");
		glsl.attachShader(GL_TESS_EVALUATION_SHADER,"tes.glsl");
		glsl.attachShader(GL_FRAGMENT_SHADER, "fragment.glsl");
		glsl.link();
		glsl.use(0);
		changeLight("no light");
		resetMVPN();
		resetMVPN();

		glPatchParameteri(GL_PATCH_VERTICES,3);
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		torus->setRenderMode(GL_PATCHES);
		stage = VTF;
	}
}
void MyPanel::makeShaderVGF(){
	if(stage != VGF)
	{
		resetGLSL();
		glsl.create();
		glsl.attachShader(GL_VERTEX_SHADER, "vertex.glsl");
		glsl.attachShader(GL_GEOMETRY_SHADER, "geo.glsl");
		glsl.attachShader(GL_FRAGMENT_SHADER, "fragment.glsl");
		glsl.link();
		glsl.use(0);
		changeLight("no light");
		resetMVPN();
		torus->setRenderMode(GL_TRIANGLES);
		stage = VGF;
	}
}
void MyPanel::initialModel(){
	model = glm::mat4(1.0f);
	view = glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	projection = glm::perspective(glm::radians(70.0f), (float)4 / 3, 0.3f, 100.0f);
	eyeZPosition = 2.0f;
	glsl.setUniform("Kd", 0.9f, 0.5f, 0.3f);
	glsl.setUniform("Ld", 1.0f, 1.0f, 1.0f);
	glsl.setUniform("Ks", 0.9f, 0.5f, 0.3f);
	glsl.setUniform("Ls", 1.0f, 1.0f, 1.0f);
	glsl.setUniform("EyePosition", 0.0f,0.0f,eyeZPosition);
	glsl.setUniform("Ka", 0.1f, 0.05f, 0.03f);
	glsl.setUniform("La", 1.0f, 1.0f, 1.0f);
	glsl.setUniform("LightPosition", view *glm::vec4(5.0f, 5.0f, 2.0f, 1.0f));
	glsl.setUniform("View",view);
	mv = view * model;
	glsl.setUniform("Model", model);
	glsl.setUniform("NormalMatrix",glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
	glsl.setUniform("Projection", projection);
    index = glGenLists(2);    
    glNewList(index,GL_COMPILE);
    torus->render();
    glEndList();
    model *= glm::rotate(glm::radians(-35.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model *= glm::rotate(glm::radians(35.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    mv = view * model;
    glsl.setUniform("Model", model);
    glsl.setUniform("NormalMatrix",glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
    glNewList(index+1,GL_COMPILE);
    torus2->render();
    glEndList();
    list[0]=0;list[1]=1;
}
void MyPanel::changeLight(const QString &str)
{
	if(str == "light")
		hasLight = true;
	else
		hasLight = false;
	glsl.setUniform("hasLight",hasLight);
	updateGL();
	emit setLight(str);
}

void MyPanel::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat x = GLfloat(width) / height;
	glFrustum(-x, +x, -1.0, +1.0, 4.0, 15.0);
	glMatrixMode(GL_MODELVIEW);
}

void MyPanel::paintGL()
{
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    torus->render();
    //glListBase(index);
    //glCallList(2);
    //glCallLists(2,GL_UNSIGNED_BYTE,list);
}

void MyPanel::changeShader(const QString& shader_stages)
{
	if(shader_stages == "VF")
	{
		makeShaderVF();
	}else if(shader_stages == "VTF")
	{
		makeShaderVTF();
	}else if(shader_stages == "VGF")
	{
		makeShaderVGF();
	}
	updateGL();
}
void MyPanel::mousePressEvent(QMouseEvent *event)
{
	lastPos = event->pos();	
}

void MyPanel::mouseMoveEvent(QMouseEvent *event)
{
	GLfloat dx = GLfloat(event->x() - lastPos.x()) / width();
	GLfloat dy = GLfloat(event->y() - lastPos.y()) / height();

	model = glm::rotate(glm::radians(-180*dx), glm::vec3(1.0f, 0.0f, 0.0f))*model;
	model *= glm::rotate(glm::radians(180*dy), glm::vec3(0.0f,1.0f,0.0f));
	glsl.setUniform("Model",model);
	mv = view * model;
	glsl.setUniform("NormalMatrix",
		glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
	lastPos = event->pos();
	updateGL();
}

void MyPanel::mouseDoubleClickEvent(QMouseEvent *event)
{
	
}
void MyPanel::changeModel(float value,int index)
{
	torus->setData(value,index);
	updateGL();
}
void MyPanel::wheelEvent(QWheelEvent *event)
{
	float eyePosition[3];
	glsl.getUniform("EyePosition",eyePosition);
	float dist = (float)event->delta()/1440;	
	eyePosition[2] = eyePosition[2] - dist;	
	if(eyePosition[2]>6)
		eyePosition[2] = 6;
	if(eyePosition[2]<1)
		eyePosition[2] = 1;
	eyeZPosition = eyePosition[2];
	view = glm::lookAt(glm::vec3(0.0f, 0.0f, eyePosition[2]), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));	
	mv = view * model;
	glsl.setUniform("EyePosition", 0.0f,0.0f,eyePosition[2]);
	glsl.setUniform("View",view);
	glsl.setUniform("NormalMatrix",glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
	updateGL();
}

void MyPanel::setDiffuse(int d){
    kd = d/100.0f;
	glsl.setUniform("Kd", kd,kd,kd);
    updateGL();
}

void MyPanel::setAmbient(int a){
    ka = a/100.0f;
	glsl.setUniform("Ka", ka,ka,ka);
    updateGL();
}

void MyPanel::setSpecular(int s){
    ks = s/100.0f;
	glsl.setUniform("Ks", ks,ks,ks);
    updateGL();

}
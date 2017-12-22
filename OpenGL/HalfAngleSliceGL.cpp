#include "HalfAngleSliceGL.h"
#include <soil.h>
#include <glm/gtc/type_ptr.hpp>

HalfAngleSliceGL::HalfAngleSliceGL(QWidget *parent):QGLWidget(parent){
}

void HalfAngleSliceGL::initializeGL() {
    GLenum err = glewInit();
    /*glsl.create();
    glsl.attachShader(GL_VERTEX_SHADER, "vertex_texture.glsl");
    glsl.printLog();
    glsl.attachShader(GL_FRAGMENT_SHADER, "frag_texture.glsl");
    glsl.printLog();
    glsl.link();
    glsl.use(0);
    glsl.glActiveTex (GL_TEXTURE1);
    glGenTextures(1,&texture1);
    glBindTexture(GL_TEXTURE_2D,texture1);
    w = 0;h = 0;
    img = SOIL_load_image("awesomeface.png",&w,&h,0,SOIL_LOAD_RGBA);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,w,h,0,GL_RGBA,GL_UNSIGNED_BYTE,img);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glsl.generateMipmap(GL_TEXTURE_2D);
    //glGenerateMipmap(GL_TEXTURE_2D);
    glsl.setUniform("texture1",1);
    glsl.setUniform("imageRes",w,h,0);
    SOIL_free_image_data(img);
    glEnable(GL_BLEND);
    //glEnable(GL_ALPHA_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glsl.change(1);
    glsl.create();
    glsl.attachShader(GL_VERTEX_SHADER, "vertex_texture.glsl");
    glsl.printLog();
    glsl.attachShader(GL_FRAGMENT_SHADER, "frag_pick.glsl");
    glsl.printLog();
    glsl.link();
    glsl.setUniform("posPick",-1,-1,0);
    glsl.use(0);
    if(err!=GLEW_OK){

    }*/
    GLfloat light_position[] = {0.0,0.0,-2.0,1.0};
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_2D,0);
    model[0].init("C:/data/obj/rock/","rock.obj");
    //model.init("C:/data/obj/","bumpy.obj");
    //model.init("C:/data/obj/","cube.obj");
    //model.init("C:/data/obj/","281.obj");
    model[1].init("C:/data/obj/cow/","cow.obj");
    //model[2].init("C:/data/obj/ct_scanner/","ct_scanner.obj");

    //model.init("C:/data/obj/Fidget_Spinner/","Fidget_Spinner.obj");

    const float *pSource = (const float*)glm::value_ptr(model[0].m_model);
    glMultMatrixf(pSource);
}

void HalfAngleSliceGL::mouseMoveEvent(QMouseEvent *event){
    /*glsl.use(1);
    glsl.setUniform("posPick",event->pos().x(),height()-event->pos().y(),0);
    update();*/
}

void HalfAngleSliceGL::reload(QString path){
    
    glsl.use(0);
    glsl.glActiveTex (GL_TEXTURE1);
    //glsl.deleteTex(texture1);
    //glGenTextures(1,&texture1);
    QImage image;
    image.load(path);
    img = image.bits();//SOIL_load_image(path.toUtf8(),&w,&h,0,SOIL_LOAD_RGBA);
    w = image.width();
    h = image.height();
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,image.width(),image.height(),0,GL_RGBA,GL_UNSIGNED_BYTE,img);
    glsl.setUniform("imageRes",w,h,0);
    //SOIL_free_image_data(img);
}


void HalfAngleSliceGL::resizeGL(int width, int height)
{
    //glsl.use(0);
	glViewport(0, 0, width, height);
	//glFrustum(-1.0, +1.0, -1.0, +1.0, -2.0, 15.0);
    //glsl.setUniform("resolution",width,height,0);
	/*glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat x = GLfloat(width) / height;
	glMatrixMode(GL_MODELVIEW);
    */
}

void HalfAngleSliceGL::paintGL(){
    glClearColor(0.0f,0.0f,0.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::mat4 rotate = glm::rotate(glm::radians(-10.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    const float *pSource = (const float*)glm::value_ptr(rotate);
    glMultMatrixf(pSource);
    model[0].render();
    model[1].render();
    //model[3].render();
    /*glColor3f(1.0,1.0,0.0);
    glsl.use(0);
    glBegin(GL_POLYGON);
    glVertex3f(-1.0,1.0,0.0);
    glVertex3f(1.0,1.0,0.0);
    glVertex3f(1.0,-1.0,0.0);
    glVertex3f(-1.0,-1.0,0.0);
    glEnd();

    glsl.use(1);
    glBegin(GL_POLYGON);
    glVertex3f(-1.0,1.0,0.0);
    glVertex3f(1.0,1.0,0.0);
    glVertex3f(1.0,-1.0,0.0);
    glVertex3f(-1.0,-1.0,0.0);
    glEnd();*/
}
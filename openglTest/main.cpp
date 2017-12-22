// linkGLlibrary.cpp : Defines the entry point for the console application.
//
#include<gl/glew.h>
#include<gl/glut.h>
#include<gl/glu.h>
#include <GL/GL.h>
//#include<windows.h>
void renderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glBegin(GL_TRIANGLES);
    glVertex3f(-0.5,-0.5,0.0);
    glVertex3f(0.5,0.0,0.0);
    glVertex3f(0.0,0.5,0.0);
    glEnd();
    glutSwapBuffers();
}

int main(int argc,char* argv[])
{
    glewInit();
    glutInit(&argc, (char**) argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(320,320);
    glutCreateWindow("Hello OpenGL");
    glutDisplayFunc(renderScene);
    glutMainLoop();//enters the GLUT event processing loop.
    return 0;
}


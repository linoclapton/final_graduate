#include "GIDVRGL.h"
#include <fstream>
#include <iostream>
#include <QMouseEvent>
#include "ULYMath.h"
#include "ULY2DIO.h"
#include "mat2bmp.h"

using namespace std;

GIDVRGL::GIDVRGL(QWidget *parent):QGLWidget(parent) {
    filename = "head.bin";
    std::ifstream fin;
    //fin.open("tooth.bin",std::ios::binary);
    fin.open(filename,std::ios::binary);
    int maxAndMin[2];
    fin.read((char*)maxAndMin,2*4);
    fin.read((char*)dimension,3*4);    
    //dimension[2] >>= 1;
    int w = dimension[2],h=dimension[1],d=dimension[0];
    cout<<d<<' '<<h<<' '<<w<<endl;
    udata = new float[w*h*d];
    float datawidth = maxAndMin[0]-maxAndMin[1];
    fin.read((char *)udata,w*h*d*4);
    for(int i=0;i<w;i++){
        for(int j=0;j<h;j++)
            for(int k= 0;k<d;k++)
                if(j<h)
                    udata[i*h*d+j*d+k] = (udata[i*h*d+j*d+k]-maxAndMin[1])/datawidth;
                else
                    udata[i*h*d+j*d+k] = 0.0;
    }
    std::ofstream fout;
    fout.open("head2_float.bin",std::ios::binary);
    fout.write((char*)udata,w*h*d*4);
    fout.close();
    fin.close();
    fin.open("color_IPSVI.txt");
    float r1,g1,b1;
    //cout<<"---------------------------color---------------------------------"<<endl;
    for(int i=0;i<256;i++)
    {
        fin>>r1>>g1>>b1;
        //cout<<r1<<' '<<g1<<' '<<b1<<endl;
        color[i][0]=r1;//*0.00392;
        color[i][1]=g1;//*0.00392;
        color[i][2]=b1;//*0.00392;
    }
    fin.close();
    fin.open("opacity_IPSVI.txt");
    //cout<<"---------------------------opacity---------------------------------"<<endl;
    for(int i=0;i<256;i++)
    { 
        fin>>opacity[i];
        //cout<<opacity[i]<<endl;
        //opacity[i] = ULY::clamp<float>(1.0f*i*0.0039f,0.01f,0.8f);
        color[i][3] = 1.0f;//opacity[i]*256;
    }
    opacity[0] =0.0f;
}

void GIDVRGL::initializeGL(){
    GLenum err = glewInit();
    if(err!=GLEW_OK){

    }
    int major,minor;
    glsl.getConstant(GL_MAJOR_VERSION,&major);
    glsl.getConstant(GL_MINOR_VERSION,&minor);
    cout<<"opengl version:"<<major<<"."<<minor<<endl;
    glsl.create();
    glsl.attachShader(GL_VERTEX_SHADER, "IPSVI_vertex.glsl");
    glsl.printLog();
    glsl.attachShader(GL_FRAGMENT_SHADER, "IPSVI_fragment.glsl");
    glsl.printLog();
    glsl.link();
    glsl.use(0);
    glsl.initTextShader();

    qglClearColor(Qt::black);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEPTH_CLAMP);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    glOrtho(-1.0,1.0,-1.0,1.0,-5,8);

    lastFPS = 60.0;
    counter = 0;
    totalTime = 0;

    glsl.genFramebuffers(1,&frontFBO);
    glsl.bindFramebuffer(frontFBO,1024,1024,1);
    GLuint renderTex;
    glGenTextures(1, &renderTex);
    glsl.glActiveTex(GL_TEXTURE3); // Use texture unit 3
    glBindTexture(GL_TEXTURE_2D, renderTex);
    // glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE,4,GL_RGBA,1024,1024,NULL);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA16,1024,1024,0,GL_RGBA,GL_FLOAT,NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glsl.framebufferTexture2D(renderTex);
    GLuint depthBuf;
    glsl.genRenderbuffers(depthBuf);
    glsl.bindRenderbuffer(depthBuf);
    glsl.renderbufferStorage(1024);
    glsl.framebufferRenderbuffer(depthBuf);
    // Set the target for the fragment shader outputs
    GLenum drawBufs[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, drawBufs);

    glsl.genFramebuffers(1,&backFBO);
    glsl.bindFramebuffer(backFBO,1024,1024,1);
    GLuint renderTexBack;
    glGenTextures(1, &renderTexBack);
    glsl.glActiveTex(GL_TEXTURE4); // Use texture unit 4
    glBindTexture(GL_TEXTURE_2D, renderTexBack);
    //glTexImage2DMultisample(GL_TEXTURE_2D,4,GL_RGBA,1024,1024,NULL);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA16,1024,1024,0,GL_RGBA,GL_FLOAT,NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glsl.framebufferTexture2D(renderTexBack);
    GLuint depthBufBack;
    glsl.genRenderbuffers(depthBufBack);
    glsl.bindRenderbuffer(depthBufBack);
    glsl.renderbufferStorage(1024);
    glsl.framebufferRenderbuffer(depthBufBack);
    // Set the target for the fragment shader outputs
    GLenum drawBufsBack[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, drawBufsBack);

    // Unbind the framebuffer, and revert to default framebuffer
    //glsl.bindFramebuffer(0,1024,1024,1);
    //127
    int w=dimension[2],h=dimension[1],d=dimension[0];

    glsl.glActiveTex(GL_TEXTURE9);
    GLuint textureID1;
    glGenTextures(1, &textureID1);
    glBindTexture(GL_TEXTURE_3D, textureID1);
    //glTexStorage3D(GL_TEXTURE_3D, 0, GL_R16, 512, 512, 175);
    glTexImage3D(GL_TEXTURE_3D, 0,GL_R32F,d,h,w,0, GL_RED,GL_FLOAT,udata);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    delete[] udata;
    //glGenerateMipmapEXT( GL_TEXTURE_3D );
    GLint loc = glsl.getUniformLocation("volume_tex");
    glsl.setUniform(loc, 9);
    //glBindImageTexture(0,textureID1, 0,GL_TRUE, 0, GL_READ_ONLY, GL_R32F);
    glsl.glActiveTex(GL_TEXTURE1);
    glGenTextures(1, &textureID2);
    glBindTexture(GL_TEXTURE_1D, textureID2);
    //glTexStorage3D(GL_TEXTURE_3D, 0, GL_R16, 512, 512, 175);
    glTexImage1D(GL_TEXTURE_1D, 0,GL_RGBA16,256,0, GL_RGBA,GL_FLOAT,color);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    loc = glsl.getUniformLocation("color_tex");
    glsl.setUniform(loc, 1);
    //glActiveTexture(GL_TEXTURE2);
    glsl.glActiveTex(GL_TEXTURE2);
    glGenTextures(1, &textureID3);
    glBindTexture(GL_TEXTURE_1D, textureID3);
    //glTexStorage3D(GL_TEXTURE_3D, 0, GL_R16, 512, 512, 175);
    glTexImage1D(GL_TEXTURE_1D, 0,GL_R32F,256,0, GL_RED,GL_FLOAT,opacity);
    //glActiveTextureARB(GL_TEXTURE0);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    loc = glsl.getUniformLocation("opacity_tex");
    glsl.setUniform(loc, 2);

    //char* c = new char[512*512*4];
    //memset(c,255,512*512*4);
    glGenTextures(1, &illumA);
    glsl.glActiveTex(GL_TEXTURE5); 
    glBindTexture(GL_TEXTURE_2D, illumA);
    //glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,512,512,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA32F,512,512,0,GL_RGBA,GL_FLOAT,NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    loc = glsl.getUniformLocation("illumA");
    glsl.setUniform(loc, 5);
    //glBindImageTexture(5, illumA, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8UI);
    glBindImageTexture(5, illumA, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);

    glsl.getInfo(GL_MAX_VERTEX_ATTRIBS);
    glsl.getInfo(GL_DEPTH_CLEAR_VALUE);
    glsl.getInfo(GL_COLOR_CLEAR_VALUE);
    glsl.getInfo(GL_STENCIL_CLEAR_VALUE);
    glsl.getInfo(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX);
    glsl.getInfo(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX);
    //delete[] c;

    /*glGenTextures(1, &illumB);
    glsl.glActiveTex(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, illumB);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,512,512,0,GL_RGBA,GL_FLOAT,NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    loc = glsl.getUniformLocation("illumB");
    glsl.setUniform(loc, 6);*/

    initializeModel();
}

void GIDVRGL::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);
	glFrustum(-1.0, +1.0, -1.0, +1.0, -2.0, 15.0);
    diagonal = sqrt(width*width+height*height)*0.5;

    float* c = new float[width*height*4];
    for(int i=0;i<width*height;i++){
        c[i*4+0] = 0.0f;
        c[i*4+1] = 0.0f;
        c[i*4+2] = 0.0f;
        c[i*4+3] = 1.0f;
    }
    glsl.glActiveTex(GL_TEXTURE5); 
    glBindTexture(GL_TEXTURE_2D, illumA);
    //glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,width,height,0,GL_RGBA,GL_FLOAT,NULL);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA32F,width,height,0,GL_RGBA,GL_FLOAT,c);
    delete[] c;
}

void GIDVRGL::paintGL(){
        glsl.use(0);
        glsl.bindFramebuffer(0,1024,1024,1);
        LARGE_INTEGER nStartCounter,nFrequency;
        QueryPerformanceCounter(&nStartCounter);
        QueryPerformanceFrequency(&nFrequency);
        LARGE_INTEGER nStopCounter;
        //Draw basic tips
        glClearColor(0.0f,0.0f,0.0f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glsl.setUniform("drawAxis",true);
        glLineWidth(5);
        glsl.setUniform("axisColor",1.0,0.0,0.0,0.8);
        glBegin(GL_LINES);
        glVertex3f(0.0,0.0,0.0);
        glVertex3f(0.5,0.0,0.0);
        glEnd();
        glsl.setUniform("axisColor",1.0f,0.0f,0.0f,0.8f);
        glBegin(GL_POLYGON);
        glVertex3f(0.0,0.0,0.0);
        glVertex3f(0.02,0.0,0.0);
        glVertex3f(0.02,0.02,0.0);
        glVertex3f(0.0,0.02,0.0);
        glEnd();
        glsl.setUniform("axisColor",0.0,1.0,0.0,1.0);
        glBegin(GL_LINES);
        glVertex3f(0.0,0.0,0.0);
        glVertex3f(0.0,0.5,0.0);
        glEnd();
        glsl.setUniform("axisColor",0.0,0.0,1.0,1.0);
        glBegin(GL_LINES);
        glVertex3f(0.0,0.0,0.0);
        glVertex3f(0.0,0.0,0.5);
        glEnd();
        glsl.setUniform("axisColor",1.0,1.0,0.0,1.0);
        glPointSize(5);
        glBegin(GL_POINTS);
        glVertex3f(-0.5,0.5,0.5);
        glVertex3f(0.5,-0.5,0.5);
        glVertex3f(0.5,0.5,-0.5);
        glVertex3f(-0.5,-0.5,0.5);
        glVertex3f(-0.5,0.5,-0.5);
        glVertex3f(0.5,-0.5,-0.5);
        glVertex3f(-0.5,-0.5,-0.5);
        glVertex3f(0.5,0.5,0.5);
        glEnd();
        glLineWidth(1);
        glsl.setUniform("drawAxis",false);
        glsl.setUniform("preRender",true);
        glsl.bindFramebuffer(backFBO,1024,1024,1);
        glClearColor(0.0f,0.0f,0.0f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);
        glViewport(0,0,1024,1024);
        drawCube(); 
        glsl.bindFramebuffer(frontFBO,1024,1024,1);
        glClearColor(0.0f,0.0f,0.0f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_BACK);
        drawCube(); 
        int loc = glsl.getUniformLocation("front_tex");
        glsl.setUniform(loc, 3);
        loc = glsl.getUniformLocation("back_tex");
        glsl.setUniform(loc, 4);
        glsl.bindFramebuffer(0,1024,1024,1);
        glViewport(0,0,this->width(),this->height());
        //Draw Volume
        glsl.setUniform("preRender",false);
        int maxStep = 10;
        float ratio = 2.0/maxStep;
        for(int i=0;i<width();i++){
            /*glBegin(GL_QUADS);
            glVertex3f(1.0,-1.0,0.0);
            glVertex3f(1.0,1.0,0.0);
            glVertex3f(-1.0,1.0,0.0);
            glVertex3f(-1.0,-1.0,0.0);
            glEnd();*/
            /*glBegin(GL_LINES);
            glVertex3f((i)*ratio-1.0,-1.0,0.0);
            glVertex3f((i)*ratio-1.0,1.0,0.0);*/

            glBegin(GL_QUADS);
            glVertex3f((i+1)*ratio-1.0,-1.0,0.0);
            glVertex3f((i+1)*ratio-1.0,1.0,0.0);
            glVertex3f(i*ratio-1.0,1.0,0.0);
            glVertex3f(i*ratio-1.0,-1.0,0.0);
            glEnd();
        }
        glFinish();
        QueryPerformanceCounter(&nStopCounter);
        int nTime = nFrequency.QuadPart/(nStopCounter.QuadPart - nStartCounter.QuadPart);
        if(counter==32){
            lastFPS = totalTime>>5;
            totalTime = 0;
            counter = 0;
        }else{
            counter++;
            totalTime += nTime;
        }
        QString s("fps:");
        s = s+QString::number(lastFPS);
        glsl.renderText(s.toStdString(),30,560,0.5,glm::vec3(0.0,1.0,0.0));
        //drawString(s.toStdString().c_str());
}


void GIDVRGL::vertex(int index){
    static float a = 0.48;
    static float v[8][3]={{a,-a,a},{a,a,a},{-a,a,a},{-a,-a,a},
    {a,-a,-a},{a,a,-a},{-a,a,-a},{-a,-a,-a}
    };
    static float c[8][3]={{a*2,-a*2,a*2},{a*2,a*2,a*2},{-a*2,a*2,a*2},{-a*2,-a*2,a*2},
    {a*2,-a*2,-a*2},{a*2,a*2,-a*2},{-a*2,a*2,-a*2},{-a*2,-a*2,-a*2}
    };
    //glColor3fv(c[index]);
    glVertex3fv(v[index]);
}

void GIDVRGL::drawCube(){

    glBegin(GL_QUADS);

    vertex(0);
    vertex(1);
    vertex(2);
    vertex(3);

    vertex(0);
    vertex(4);
    vertex(5);
    vertex(1);

    vertex(4);
    vertex(7);
    vertex(6);
    vertex(5);

    vertex(3);
    vertex(2);
    vertex(6);
    vertex(7);

    vertex(1);
    vertex(5);
    vertex(6);
    vertex(2);

    vertex(4);
    vertex(0);
    vertex(3);
    vertex(7);
    glEnd();
}

void GIDVRGL::initializeModel(){
    float space[3];
    space[0]=dimension[0];//0.595703f*dimension[0];
    space[1]=dimension[1];//0.595703f*dimension[1];
    space[2]=dimension[2];//0.330017f*dimension[2];
    float mx = space[0];
    if(mx<space[1]) mx = space[1];
    if(mx<space[2]) mx = space[2];
    for(int i=0;i<3;i++)
        ratio[i] = mx/space[i];
    //ratio[i] = dimension[2]/dimension[i];
    currentZ = 0;
    spaceX = 1.0 / dimension[0];
    spaceY = 1.0 / dimension[1];
    spaceZ = 1.0 / dimension[2];
    model = glm::mat4(1.0f);
    camera = glm::mat4(1.0f);
    model *= glm::rotate(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    //model *= glm::rotate(glm::radians(35.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    view = glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    projection = glm::mat4(1.0f);//glm::perspective(glm::radians(70.0f), (float)4 / 3, 0.3f, 100.0f);
    glsl.setUniform("Camera",camera);
    glsl.setUniform("steps",512.0f);
    //glsl.setUniform("Kd", 0.9f, 0.5f, 0.3f);
    glsl.setUniform("Kd", 0.5f, 0.5f, 0.5f);
    glsl.setUniform("Ld", 1.0f, 1.0f, 1.0f);
    glsl.setUniform("Ks", 0.8f, 0.8f, 0.8f);
    glsl.setUniform("Ls", 1.0f, 1.0f, 1.0f);
    glsl.setUniform("EyePosition", -2.0f,-2.0f,-2.0f);
    glsl.setUniform("Ka", 0.2f, 0.2f, 0.2f);
    glsl.setUniform("La", 1.0f, 1.0f, 1.0f);
    //glsl.setUniform("LightPosition", glm::vec4(0.707106,0,0.707106,0));//view *glm::vec4(5.0f, 5.0f, 2.0f, 1.0f));
    glsl.setUniform("LightPosition", glm::vec4(-1.0,-1.0,-1.0,1.0));//view *glm::vec4(5.0f, 5.0f, 2.0f, 1.0f));
    glsl.setUniform("View",view);
    mv = view * model;
    glsl.setUniform("Model", model);
    glsl.setUniform("NormalMatrix",glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
    glsl.setUniform("Projection", projection);
    glsl.setUniform("WindowMin",0.0f);
    glsl.setUniform("WindowMax",1.0f);
    glsl.setUniform("WindowWidth",1.00f);
    glsl.setUniform("spaceX",spaceX); 
    glsl.setUniform("spaceY",spaceY); 
    glsl.setUniform("spaceZ",spaceZ); 
    glsl.setUniform("ratioX",ratio[0]);
    glsl.setUniform("ratioY",ratio[1]);
    glsl.setUniform("ratioZ",ratio[2]);
    glsl.setUniform("drawImage",true);
}


void GIDVRGL::mousePressEvent(QMouseEvent *event){
    if(event->button()==Qt::RightButton){
        static bool common = true;
        common =  !common;
        glsl.setUniform("drawImage",common);
        /*int w ,h ;
        w = width();
        h = height();
        char* pixel = new char[w*h*4];
        memset(pixel,0,w*h*4);
        glsl.use(0);
        glsl.glActiveTex(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, illumA);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixel);
        //glGetTextureImage(GL_TEXTURE_2D, 0, GL_RGBA8, GL_UNSIGNED_BYTE,512*512, pixel);
        WriteBitmapFile("test.bmp",w,h,(unsigned char*)pixel);  
        delete[] pixel;
        return;*/
    }
    lastPos = event->pos();	
    glsl.use(0);
    glsl.setUniform("steps",128.0f);
}
void GIDVRGL::mouseMoveEvent(QMouseEvent *event){
    GLfloat sx = lastPos.x()-this->width()*0.5;
    GLfloat sy = -lastPos.y()+this->height()*0.5;
    GLfloat sz = sqrt(diagonal*diagonal-sx*sx-sy*sy);
    GLfloat ex = event->x()-this->width()*0.5;
    GLfloat ey = -event->y()+this->height()*0.5;
    GLfloat ez = sqrt(diagonal*diagonal-ex*ex-ey*ey);
    if((ex<width()*0.5&&ex>-width()*0.5)&&(ey<height()*0.5&&ey>-height()*0.5)){
        GLfloat dx = GLfloat(event->x() - lastPos.x()) / width();
        GLfloat dy = GLfloat(event->y() - lastPos.y()) / height();
        glm::vec3 line = glm::cross(glm::vec3(sx,sy,sz),glm::vec3(ex,ey,ez));
        GLfloat degree = glm::acos(glm::dot(glm::vec3(ex,ey,ez),glm::vec3(sx,sy,sz))/(diagonal*diagonal));
        model = glm::rotate(degree, line)*model;
        //model = glm::rotate(glm::radians(-180*dx), glm::vec3(1.0f,0.0f,0.0f))*model;
        //model *= glm::rotate(glm::radians(180*dy), glm::vec3(0.0f,1.0f,0.0f));
        glsl.setUniform("Model",model);
        
        //std::cout<<"==================================================="<<std::endl;
        //std::cout<<sx<<' '<<sy<<' '<<ex<<' '<<ey<<' '<<std::endl;
        /*for(int i=0;i<4;i++){
            for(int j=0;j<4;j++)
                std::cout<<model[i][j]<<' ';
            std::cout<<std::endl;
        }*/

        mv = view * model;
        //glsl.setUniform("NormalMatrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
        lastPos = event->pos();
        updateGL();
    }
            
}
void GIDVRGL::mouseReleaseEvent(QMouseEvent *event){
    glsl.use(0);
    glsl.setUniform("steps",512.0f);
    updateGL();
}

void GIDVRGL::wheelEvent(QWheelEvent *event){
    float dist;
    glsl.use(0);
    //float eyePosition[3];
    //glsl.getUniform("EyePosition",eyePosition);
    dist = -event->delta()/1440.0;	
    currentZ += dist;
    camera *= glm::scale(glm::vec3(1+dist,1+dist,1+dist)) ; 
    glsl.setUniform("Camera",camera);
    /*eyePosition[2] = eyePosition[2] - dist;	
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
    */
    updateGL();
}
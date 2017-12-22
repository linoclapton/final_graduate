#include <iostream>
#include <fstream>
#include <gl/glew.h>
#include <gl/glut.h>
#include "GPUConfig.h"
using namespace std;
int queryNVidiaCardMemory()
{
    __try
    {
        int iVal = 0;
        glGetIntegerv( GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &iVal );
        return iVal;
    }
    __except(1) { }
    return 0;
}
int queryAmdCardMemory()
{
    __try
    {
        int iVal = 0;
        glGetIntegerv( GL_TEXTURE_FREE_MEMORY_ATI, &iVal );
        return iVal;
    }
    __except(1)
    {
    }
    return 0;
}
int queryVideoCardMemory( GPUTYPE type )
{
    __try
    {
        switch(type){
        case NVIDIA: return queryNVidiaCardMemory();
        case AMD: return queryAmdCardMemory();
        case UNKNOWN: return 0;
        }
    }
    __except(1) {}
    return -1;
}


int GPUInfo(int argc,char* argv[]){
    glutInitDisplayMode(GLUT_DEPTH | GLUT_SINGLE | GLUT_RGB);  
    glutInit(&argc,argv);  
    GPUConfig gpuConfig;
    glutCreateWindow("GPUInfo"); 
    ofstream out("info.txt");
    GLenum err=glewInit();
    if(err!=GLEW_OK)
    {
        out<<"gpu init failed"<<endl;
    }
    const char* venderName = (char*)glGetString(GL_VENDOR); 
    const char* renderName = (char*)glGetString(GL_RENDERER);
    std::string gpuType(venderName);
    std::string gpuRender(renderName);
    std::size_t found; 
    std::string type;
    if((found = gpuType.find("NVIDIA"))!=std::string::npos){
        gpuConfig.hasGPU = true;
        gpuConfig.gpuType = NVIDIA;
        type = "NVIDIA";
    }else if((found =gpuRender.find("AMD"))!=std::string::npos){
        gpuConfig.hasGPU = true;
        gpuConfig.gpuType = AMD;
        type = "AMD";
    }else{
        gpuConfig.hasGPU = false;
        gpuConfig.gpuType = UNKNOWN;
        type = "UNKNOWN or Intel";
    }
    int major,minor;
    glGetIntegerv(GL_MAJOR_VERSION,&major); 
    gpuConfig.openglMajorVersion = major;
    glGetIntegerv(GL_MINOR_VERSION,&minor); 
    gpuConfig.openglMinorVersion = minor;
    gpuConfig.memorySize = queryVideoCardMemory(gpuConfig.gpuType)/1024;
    string volumRender =gpuConfig.canVolumeRender()?"true":"false";
    string anyCut      =gpuConfig.canRectSelect()?"true":"false";
    out<<"Vendor        : "<<venderName<<std::endl;
    out<<"Render        : "<<renderName<<std::endl;
    out<<"Opengl Version: "<<major<<'.'<<minor<<std::endl;
    out<<"Verify Type   : "<<type.c_str()<<endl;
    out<<"GPU MEMORY    : "<<gpuConfig.memorySize<<"MB"<<std::endl;
    out<<"Volume Render : "<<volumRender.c_str()<<endl;
    out<<"Any cut       : "<<anyCut.c_str()<<endl;
    out.close();
    return 0;
}
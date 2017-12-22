#pragma once
enum GPUTYPE{NVIDIA,AMD,INTEL,UNKNOWN};
class GPUConfig{
public:
    GPUTYPE gpuType;
    int memorySize;
    int openglMajorVersion;
    int openglMinorVersion;
    bool hasGPU;
    GPUConfig():hasGPU(false),memorySize(0){}
    bool canVolumeRender(){
        if(memorySize<800&&hasGPU) 
            return false;
        return true;
    }
    bool canRectSelect(){
        if(openglMajorVersion<4)
            return false;
        else if(openglMinorVersion<3)
            return false;
        return true;
    }
};

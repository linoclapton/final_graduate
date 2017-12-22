#pragma once
enum PointType{
    Front,
    Back
};
class MarkPoint{
public:
    MarkPoint(float mx=0.0f,float my=0.0f,PointType mtype=Front,float mr=0.1f):
        x(mx),y(my),type(mtype),r(mr) { }
    float x;
    float y;
    PointType type;
    float r;
};

//#include "MyWindow.h"
#include "GradientBasedStart.h"
#include "HalfAngleSlice.h"
#include "IPSVI.h"
#include "GIDVR.h"
#include <QtWidgets/QApplication>
#include <iostream>
#include <fstream>
#include "TorusWindow.h"
#include "ULYMath.h"
#include "svpng.h"
#include "ULY2DIO.h"
#include "Eigen/Core"
void test_rgb(void) {
   unsigned char rgb[ 512 * 256 * 4], *p = rgb;
   unsigned x, y;
   FILE *fp = fopen("rgb.png", "wb");
   for (y = 0; y < 512; y++)
       for (x = 0; x < 256; x++) {
           *p++ = (unsigned char)x%256;    /* R */
           *p++ = (unsigned char)y%256;    /* G */
           *p++ = 128;                 /* B */
           *p++ = 255;                 /* B */
       }
   ULY::Pixel pixel;
   pixel.width = 512;
   pixel.height = 256;
   pixel.img = (char*)(&rgb[0]);
   pixel.writeBMP("rgb.bmp");
   svpng(fp, 512, 256, rgb, 1);
   fclose(fp);
}
const string shader = GLSL(
    varying vec4 sPos;       
    uniform bool DrawSelectRect;
    void main(void)
    {
        if(DrawSelectRect){
            gl_Position = gl_Vertex;
        }else{
            sPos			= gl_ModelViewProjectionMatrix * gl_Vertex;
            gl_Position		= ftransform() ;
            gl_TexCoord[0]	= gl_MultiTexCoord1;
        }
    }
);

int main(int argc, char *argv[]) {
   Eigen::initParallel();
   QApplication a(argc, argv);
   enum WindowType{GradientBase,Torus,HalfAngle,IPS,GI,others};
   //test_rgb();
   WindowType type;
   cout<<shader<<endl;
   type = GradientBase;
   QMainWindow* w;
   switch(type){
   case GradientBase:
       GradientBasedStart();
       break;
   case Torus:
       w = new TorusWindow;
       w->show();
       break;
   case HalfAngle:
       w = new HalfAngleSlice; 
       w->show();
       break;
   case IPS:
       w = new IPSVI;
       w->show();
       break;
   case GI:
       w = new GIDVR;
       w->show();
       break;
    Default:
       break;
   }
   return a.exec();
}


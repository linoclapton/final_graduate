#include <iostream>
#include <fstream>

#include "ULYBase.h"
#include "RTLYWorld.h"
#include "RTLYCamera.h"
#include "ULYMath.h"

using namespace RTLY;
//using namespace ULY;

int main(){

   World w; 
   w.build();
   w.camera->render(w);
   //w.renderScene();
   w.outputPPMImage("result2.ppm");
   return 0;
}


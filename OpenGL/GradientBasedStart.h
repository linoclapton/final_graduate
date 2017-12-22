#pragma once

#include "Present.h"
#include <QtWidgets/QApplication>
#include <iostream>
#include <fstream>
#include <QPushButton>
#include "ULYBase.h"
#include "RunGuard.h"
#include "graphcut.h"
#include "ULYBitTools.h"
#include "ULY2DIO.h"
#include "ULYMath.h"
#include "TorusWindow.h"

void GradientBasedStart()
{
    /*int a=-1,b=0x80000000;
    ULY::BitTools::show_bytes<int>(a);
    if(!ULY::BitTools::tadd_ok(a,b))
        printf("error\n");
    printf("%f\n",ULY::invSqrt(16.0f));
    //ULY::Pixel p;
    //p.readFile("1.bmp");
    //drawPixelOnDesktop(p);
    //p.writeBMP("test.bmp");
    return 0;*/
    ULY::Clock clock;
    clock.start();
    RunGuard guard( "OpenGL" );
    if ( !guard.tryToRun() )
        return;
    Present* myWindow = new Present;
    myWindow->show();
    myWindow->readin();
    myWindow->_applyTFButton->click();
    clock.end("total");
}

#include "MySlider.h"
#include <QLabel>
#include <String>
#include <sstream>
void MySlider::change(int newValue){
	if(index>0&&index<3)	
		canvas->changeModel((float)newValue/100.0f,index);
	else if(index<5)
		canvas->changeModel(newValue,index);
}
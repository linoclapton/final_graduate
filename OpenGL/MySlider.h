#pragma  once

#include <QSlider>
#include <QLabel>
#include "MyPanel.h"
class MySlider:public QSlider{
	Q_OBJECT
public:
	MySlider(QWidget *parent=0,MyPanel* panel=0,int index=0):QSlider(parent){
		canvas = panel;
		this->index = index;
		this->setOrientation(Qt::Horizontal);
	}
    void setPanel(MyPanel *panel){
        canvas = panel;
    }
    void setIndex(int index){
        this->index = index;
    }
public slots:
	void change(int);
private:
	QLabel label;
	MyPanel* canvas;
	int index;
};
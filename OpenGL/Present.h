#ifndef PRESENT_H
#define PRESENT_H

#include <QtWidgets/QMainWindow>
#include "ui_present.h"
#include "TransferFunction1DWidget.h"
#include <QColorDialog>
class Present : public QMainWindow
{
    Q_OBJECT

public:
    Present(QWidget *parent = 0);
    ~Present();
    void closeEvent(QCloseEvent * event);
    void readin();
    QPushButton *_applyTFButton;
private:
    Ui::PresentClass ui;
    QSlider *minSlider;
    QSlider *maxSlider;
    QLabel *minLabel;
    QLabel *maxLabel;
    QLabel *canvas;
    TransferFunction1DWidget* tf1DWidget;
    float ambient[3],diffuse[3],specular[3];
    float min;
    float max;
    int AZ;
    int BZ;
    QColor backColor;
    QColor foreColor;
    QColorDialog *backDialog;
    QColorDialog *foreDialog;
private slots: 
    void save();
    void changeMin(int value);
    void changeMax(int value);
    void changeMinZ(int value);
    void changeMaxZ(int value);
    void changeAmbientR(int value);
    void changeAmbientG(int value);
    void changeAmbientB(int value);
    void changeDiffuseR(int value);
    void changeDiffuseG(int value);
    void changeDiffuseB(int value);
    void changeSpecularR(int value);
    void changeSpecularG(int value);
    void changeSpecularB(int value);
    void changeLight(float*,float*,float*);
    void updateTex();
    void updateTF();
    void changeForeColor();
    void changeBackColor();
    void changeMotionStatus(int state);
    void changeSlider(float min,float max);
	void changeType(int);

};

#endif // PRESENT_H

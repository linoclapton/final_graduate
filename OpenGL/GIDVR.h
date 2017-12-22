#pragma once
#include "ui_FGI_DVR.h"

class GIDVR: public QMainWindow{
    Q_OBJECT
public: 
     GIDVR(QWidget* parent=0);
    ~GIDVR();
private:
    Ui::GIDVR ui;
};
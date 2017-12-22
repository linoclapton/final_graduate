#pragma once
#include "ui_IPSVI.h"

class IPSVI: public QMainWindow{
    Q_OBJECT

public: 
    IPSVI(QWidget* parent=0);
    ~IPSVI();
private:
    Ui::IPSVI ui;
};
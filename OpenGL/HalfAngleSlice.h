#pragma once

#include "ui_halfangleslice.h"
#include <QDirModel>
#include <QKeyEvent>

class HalfAngleSlice : public QMainWindow{
    Q_OBJECT

public: 
    HalfAngleSlice(QWidget* parent=0);
    ~HalfAngleSlice();
protected:
    void keyPressEvent(QKeyEvent *event);
private:
    Ui::MainWindow ui;
    QDirModel *model;
};
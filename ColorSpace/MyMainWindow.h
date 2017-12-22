#ifndef MYMAINWINDOW_H
#define MYMAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include "ui_mymainwindow.h"

class MyMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MyMainWindow(QWidget *parent = 0);
    ~MyMainWindow();

private:
    Ui::MyMainWindowClass ui;
};

#endif // MYMAINWINDOW_H

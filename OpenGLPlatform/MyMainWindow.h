#ifndef MYMAINWINDOW_H
#define MYMAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QResizeEvent>
#include "ui_mymainwindow.h"

class MyMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MyMainWindow(QWidget *parent = 0);
    ~MyMainWindow();

    void resizeEvent(QResizeEvent *event){QMainWindow::resizeEvent(event);ui.openGLWidget->resize(event->size().width(),event->size().height());}
private:
    Ui::MyMainWindowClass ui;
};

#endif // MYMAINWINDOW_H

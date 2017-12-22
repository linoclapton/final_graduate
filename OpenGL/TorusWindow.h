#pragma once
#include <QMainWindow>
#include <QLabel>
#include "MyPanel.h"
#include "ui_TorusWidget.h"

class TorusWindow : public QMainWindow
{
	Q_OBJECT

public:
	TorusWindow(QWidget *parent=0, Qt::WindowFlags flags=0);
		
	
private:
    Ui::TorusWindow ui;
	QWidget* centralWidget;
	MyPanel* canvas;
};
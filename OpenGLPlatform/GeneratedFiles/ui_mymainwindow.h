/********************************************************************************
** Form generated from reading UI file 'mymainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MYMAINWINDOW_H
#define UI_MYMAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>
#include "RenderPanel.h"

QT_BEGIN_NAMESPACE

class Ui_MyMainWindowClass
{
public:
    QWidget *centralWidget;
    RenderPanel *openGLWidget;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MyMainWindowClass)
    {
        if (MyMainWindowClass->objectName().isEmpty())
            MyMainWindowClass->setObjectName(QStringLiteral("MyMainWindowClass"));
        MyMainWindowClass->resize(814, 636);
        centralWidget = new QWidget(MyMainWindowClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        openGLWidget = new RenderPanel(centralWidget);
        openGLWidget->setObjectName(QStringLiteral("openGLWidget"));
        openGLWidget->setGeometry(QRect(0, 0, 811, 621));
        MyMainWindowClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MyMainWindowClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 814, 23));
        MyMainWindowClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MyMainWindowClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MyMainWindowClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MyMainWindowClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MyMainWindowClass->setStatusBar(statusBar);

        retranslateUi(MyMainWindowClass);

        QMetaObject::connectSlotsByName(MyMainWindowClass);
    } // setupUi

    void retranslateUi(QMainWindow *MyMainWindowClass)
    {
        MyMainWindowClass->setWindowTitle(QApplication::translate("MyMainWindowClass", "MyMainWindow", 0));
    } // retranslateUi

};

namespace Ui {
    class MyMainWindowClass: public Ui_MyMainWindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MYMAINWINDOW_H

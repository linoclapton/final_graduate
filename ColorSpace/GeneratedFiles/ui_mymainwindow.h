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
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "RenderPanel.h"

QT_BEGIN_NAMESPACE

class Ui_MyMainWindowClass
{
public:
    QWidget *centralWidget;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    RenderPanel *openGLWidget;

    void setupUi(QMainWindow *MyMainWindowClass)
    {
        if (MyMainWindowClass->objectName().isEmpty())
            MyMainWindowClass->setObjectName(QStringLiteral("MyMainWindowClass"));
        MyMainWindowClass->resize(1049, 680);
        centralWidget = new QWidget(MyMainWindowClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayoutWidget = new QWidget(centralWidget);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(10, -10, 1051, 691));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        openGLWidget = new RenderPanel(centralWidget);
        openGLWidget->setObjectName(QStringLiteral("openGLWidget"));
        openGLWidget->setGeometry(QRect(0, -10, 1049, 689));
        MyMainWindowClass->setCentralWidget(centralWidget);

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

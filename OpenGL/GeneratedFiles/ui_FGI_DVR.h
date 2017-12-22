/********************************************************************************
** Form generated from reading UI file 'FGI_DVR.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FGI_DVR_H
#define UI_FGI_DVR_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>
#include <gidvrgl.h>

QT_BEGIN_NAMESPACE

class Ui_GIDVR
{
public:
    QAction *actionOpen;
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    GIDVRGL *openGLWidget;
    QMenuBar *menubar;
    QMenu *menu;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *GIDVR)
    {
        if (GIDVR->objectName().isEmpty())
            GIDVR->setObjectName(QStringLiteral("GIDVR"));
        GIDVR->resize(966, 665);
        actionOpen = new QAction(GIDVR);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        centralwidget = new QWidget(GIDVR);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        openGLWidget = new GIDVRGL(centralwidget);
        openGLWidget->setObjectName(QStringLiteral("openGLWidget"));

        horizontalLayout->addWidget(openGLWidget);

        GIDVR->setCentralWidget(centralwidget);
        menubar = new QMenuBar(GIDVR);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 966, 23));
        menu = new QMenu(menubar);
        menu->setObjectName(QStringLiteral("menu"));
        GIDVR->setMenuBar(menubar);
        statusbar = new QStatusBar(GIDVR);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        statusbar->setStyleSheet(QStringLiteral("background-color:rgb(0,0,0);color:rgb(255,255,255);"));
        GIDVR->setStatusBar(statusbar);

        menubar->addAction(menu->menuAction());
        menu->addAction(actionOpen);

        retranslateUi(GIDVR);

        QMetaObject::connectSlotsByName(GIDVR);
    } // setupUi

    void retranslateUi(QMainWindow *GIDVR)
    {
        GIDVR->setWindowTitle(QApplication::translate("GIDVR", "MainWindow", 0));
        actionOpen->setText(QApplication::translate("GIDVR", "Open", 0));
        menu->setTitle(QApplication::translate("GIDVR", "File", 0));
    } // retranslateUi

};

namespace Ui {
    class GIDVR: public Ui_GIDVR {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FGI_DVR_H

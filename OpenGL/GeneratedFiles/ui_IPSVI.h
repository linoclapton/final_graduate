/********************************************************************************
** Form generated from reading UI file 'IPSVI.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_IPSVI_H
#define UI_IPSVI_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>
#include "IPSVIGL.h"

QT_BEGIN_NAMESPACE

class Ui_IPSVI
{
public:
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    IPSVIGL *openGLWidget;

    void setupUi(QMainWindow *IPSVI)
    {
        if (IPSVI->objectName().isEmpty())
            IPSVI->setObjectName(QStringLiteral("IPSVI"));
        IPSVI->resize(969, 960);
        centralwidget = new QWidget(IPSVI);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        openGLWidget = new IPSVIGL(centralwidget);
        openGLWidget->setObjectName(QStringLiteral("openGLWidget"));

        horizontalLayout->addWidget(openGLWidget);

        IPSVI->setCentralWidget(centralwidget);

        retranslateUi(IPSVI);

        QMetaObject::connectSlotsByName(IPSVI);
    } // setupUi

    void retranslateUi(QMainWindow *IPSVI)
    {
        IPSVI->setWindowTitle(QApplication::translate("IPSVI", "MainWindow", 0));
    } // retranslateUi

};

namespace Ui {
    class IPSVI: public Ui_IPSVI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_IPSVI_H

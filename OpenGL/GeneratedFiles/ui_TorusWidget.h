/********************************************************************************
** Form generated from reading UI file 'TorusWidget.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TORUSWIDGET_H
#define UI_TORUSWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "MyPanel.h"
#include "MySlider.h"

QT_BEGIN_NAMESPACE

class Ui_TorusWindow
{
public:
    MyPanel *canvas;
    QSplitter *splitter;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout_5;
    QComboBox *shader_stage;
    QComboBox *light;
    QWidget *layoutWidget1;
    QHBoxLayout *horizontalLayout;
    QSpinBox *spinBox_outer;
    MySlider *slider_outer;
    QWidget *layoutWidget2;
    QHBoxLayout *horizontalLayout_2;
    QSpinBox *spinBox_inner;
    MySlider *slider_inner;
    QWidget *layoutWidget3;
    QHBoxLayout *horizontalLayout_3;
    QSpinBox *spinBox_rings;
    MySlider *slider_rings;
    QWidget *layoutWidget4;
    QHBoxLayout *horizontalLayout_4;
    QSpinBox *spinBox_slices;
    MySlider *slider_slices;
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QSlider *slider_specular;
    QSlider *slider_diffuse;
    QSlider *slider_ambient;

    void setupUi(QWidget *TorusWindow)
    {
        if (TorusWindow->objectName().isEmpty())
            TorusWindow->setObjectName(QStringLiteral("TorusWindow"));
        TorusWindow->resize(1300, 900);
        canvas = new MyPanel(TorusWindow);
        canvas->setObjectName(QStringLiteral("canvas"));
        canvas->setGeometry(QRect(-10, 0, 1150, 900));
        splitter = new QSplitter(TorusWindow);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setGeometry(QRect(1150, 50, 132, 551));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(10);
        sizePolicy.setHeightForWidth(splitter->sizePolicy().hasHeightForWidth());
        splitter->setSizePolicy(sizePolicy);
        splitter->setOrientation(Qt::Vertical);
        layoutWidget = new QWidget(splitter);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        horizontalLayout_5 = new QHBoxLayout(layoutWidget);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        horizontalLayout_5->setContentsMargins(0, 0, 0, 0);
        shader_stage = new QComboBox(layoutWidget);
        shader_stage->setObjectName(QStringLiteral("shader_stage"));

        horizontalLayout_5->addWidget(shader_stage);

        light = new QComboBox(layoutWidget);
        light->setObjectName(QStringLiteral("light"));

        horizontalLayout_5->addWidget(light);

        splitter->addWidget(layoutWidget);
        layoutWidget1 = new QWidget(splitter);
        layoutWidget1->setObjectName(QStringLiteral("layoutWidget1"));
        horizontalLayout = new QHBoxLayout(layoutWidget1);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        spinBox_outer = new QSpinBox(layoutWidget1);
        spinBox_outer->setObjectName(QStringLiteral("spinBox_outer"));

        horizontalLayout->addWidget(spinBox_outer);

        slider_outer = new MySlider(layoutWidget1);
        slider_outer->setObjectName(QStringLiteral("slider_outer"));
        slider_outer->setOrientation(Qt::Horizontal);

        horizontalLayout->addWidget(slider_outer);

        splitter->addWidget(layoutWidget1);
        layoutWidget2 = new QWidget(splitter);
        layoutWidget2->setObjectName(QStringLiteral("layoutWidget2"));
        horizontalLayout_2 = new QHBoxLayout(layoutWidget2);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        spinBox_inner = new QSpinBox(layoutWidget2);
        spinBox_inner->setObjectName(QStringLiteral("spinBox_inner"));

        horizontalLayout_2->addWidget(spinBox_inner);

        slider_inner = new MySlider(layoutWidget2);
        slider_inner->setObjectName(QStringLiteral("slider_inner"));
        slider_inner->setOrientation(Qt::Horizontal);

        horizontalLayout_2->addWidget(slider_inner);

        splitter->addWidget(layoutWidget2);
        layoutWidget3 = new QWidget(splitter);
        layoutWidget3->setObjectName(QStringLiteral("layoutWidget3"));
        horizontalLayout_3 = new QHBoxLayout(layoutWidget3);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        spinBox_rings = new QSpinBox(layoutWidget3);
        spinBox_rings->setObjectName(QStringLiteral("spinBox_rings"));

        horizontalLayout_3->addWidget(spinBox_rings);

        slider_rings = new MySlider(layoutWidget3);
        slider_rings->setObjectName(QStringLiteral("slider_rings"));
        slider_rings->setOrientation(Qt::Horizontal);

        horizontalLayout_3->addWidget(slider_rings);

        splitter->addWidget(layoutWidget3);
        layoutWidget4 = new QWidget(splitter);
        layoutWidget4->setObjectName(QStringLiteral("layoutWidget4"));
        horizontalLayout_4 = new QHBoxLayout(layoutWidget4);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        spinBox_slices = new QSpinBox(layoutWidget4);
        spinBox_slices->setObjectName(QStringLiteral("spinBox_slices"));

        horizontalLayout_4->addWidget(spinBox_slices);

        slider_slices = new MySlider(layoutWidget4);
        slider_slices->setObjectName(QStringLiteral("slider_slices"));
        slider_slices->setOrientation(Qt::Horizontal);

        horizontalLayout_4->addWidget(slider_slices);

        splitter->addWidget(layoutWidget4);
        widget = new QWidget(TorusWindow);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setGeometry(QRect(1150, 620, 131, 151));
        verticalLayout = new QVBoxLayout(widget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        slider_specular = new QSlider(widget);
        slider_specular->setObjectName(QStringLiteral("slider_specular"));
        slider_specular->setMinimum(0);
        slider_specular->setMaximum(100);
        slider_specular->setOrientation(Qt::Horizontal);

        verticalLayout->addWidget(slider_specular);

        slider_diffuse = new QSlider(widget);
        slider_diffuse->setObjectName(QStringLiteral("slider_diffuse"));
        slider_diffuse->setMinimum(0);
        slider_diffuse->setMaximum(100);
        slider_diffuse->setValue(0);
        slider_diffuse->setOrientation(Qt::Horizontal);

        verticalLayout->addWidget(slider_diffuse);

        slider_ambient = new QSlider(widget);
        slider_ambient->setObjectName(QStringLiteral("slider_ambient"));
        slider_ambient->setMinimum(0);
        slider_ambient->setMaximum(100);
        slider_ambient->setOrientation(Qt::Horizontal);

        verticalLayout->addWidget(slider_ambient);


        retranslateUi(TorusWindow);

        QMetaObject::connectSlotsByName(TorusWindow);
    } // setupUi

    void retranslateUi(QWidget *TorusWindow)
    {
        TorusWindow->setWindowTitle(QApplication::translate("TorusWindow", "Form", 0));
        shader_stage->clear();
        shader_stage->insertItems(0, QStringList()
         << QApplication::translate("TorusWindow", "VF", 0)
         << QApplication::translate("TorusWindow", "VTF", 0)
         << QApplication::translate("TorusWindow", "VGF", 0)
        );
        light->clear();
        light->insertItems(0, QStringList()
         << QApplication::translate("TorusWindow", "light", 0)
         << QApplication::translate("TorusWindow", "no light", 0)
        );
    } // retranslateUi

};

namespace Ui {
    class TorusWindow: public Ui_TorusWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TORUSWIDGET_H

/********************************************************************************
** Form generated from reading UI file 'present.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PRESENT_H
#define UI_PRESENT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>
#include "VolumeRenderPanel.h"

QT_BEGIN_NAMESPACE

class Ui_PresentClass
{
public:
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout_2;
    QHBoxLayout *horizontalLayout;
    VolumeRenderPanel *openGLWidget;
    QGridLayout *gridLayout;
    QLabel *minLabel;
    QPushButton *resetButton;
    QSlider *minSlider;
    QLabel *zALabel;
    QLabel *maxLabel;
    QSlider *maxSlider;
    QSlider *zBSlider;
    QLabel *zBLabel;
    QSlider *zASlider;
    QSlider *ambient_r;
    QPushButton *saveButton;
    QSlider *ambient_g;
    QSlider *ambient_b;
    QPushButton *button_undoclip;
    QSlider *diffuse_r;
    QSlider *diffuse_g;
    QSlider *diffuse_b;
    QSlider *specular_r;
    QSlider *specular_g;
    QSlider *specular_b;
    QPushButton *applyTFButton;
    QLabel *label;
    QPushButton *button_foreground;
    QCheckBox *checkBox_foreground;
    QLabel *label_2;
    QPushButton *button_background;
    QCheckBox *checkBox_background;
    QPushButton *button_cleargraphcut;
    QLabel *tfPanel;
    QComboBox *comboBox_type;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *PresentClass)
    {
        if (PresentClass->objectName().isEmpty())
            PresentClass->setObjectName(QStringLiteral("PresentClass"));
        PresentClass->resize(1190, 1037);
        centralWidget = new QWidget(PresentClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        horizontalLayout_2 = new QHBoxLayout(centralWidget);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        openGLWidget = new VolumeRenderPanel(centralWidget);
        openGLWidget->setObjectName(QStringLiteral("openGLWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(openGLWidget->sizePolicy().hasHeightForWidth());
        openGLWidget->setSizePolicy(sizePolicy);
        openGLWidget->setMinimumSize(QSize(960, 960));
        openGLWidget->setFocusPolicy(Qt::ClickFocus);

        horizontalLayout->addWidget(openGLWidget);

        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setSizeConstraint(QLayout::SetMinimumSize);
        gridLayout->setHorizontalSpacing(0);
        minLabel = new QLabel(centralWidget);
        minLabel->setObjectName(QStringLiteral("minLabel"));

        gridLayout->addWidget(minLabel, 0, 0, 1, 1);

        resetButton = new QPushButton(centralWidget);
        resetButton->setObjectName(QStringLiteral("resetButton"));

        gridLayout->addWidget(resetButton, 0, 3, 2, 1);

        minSlider = new QSlider(centralWidget);
        minSlider->setObjectName(QStringLiteral("minSlider"));
        minSlider->setMaximum(1000);
        minSlider->setValue(100);
        minSlider->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(minSlider, 1, 0, 2, 2);

        zALabel = new QLabel(centralWidget);
        zALabel->setObjectName(QStringLiteral("zALabel"));

        gridLayout->addWidget(zALabel, 2, 3, 1, 1);

        maxLabel = new QLabel(centralWidget);
        maxLabel->setObjectName(QStringLiteral("maxLabel"));

        gridLayout->addWidget(maxLabel, 3, 0, 1, 1);

        maxSlider = new QSlider(centralWidget);
        maxSlider->setObjectName(QStringLiteral("maxSlider"));
        maxSlider->setMinimum(0);
        maxSlider->setMaximum(1000);
        maxSlider->setValue(850);
        maxSlider->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(maxSlider, 4, 0, 1, 2);

        zBSlider = new QSlider(centralWidget);
        zBSlider->setObjectName(QStringLiteral("zBSlider"));
        zBSlider->setMinimum(0);
        zBSlider->setMaximum(161);
        zBSlider->setValue(1);
        zBSlider->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(zBSlider, 4, 3, 1, 1);

        zBLabel = new QLabel(centralWidget);
        zBLabel->setObjectName(QStringLiteral("zBLabel"));

        gridLayout->addWidget(zBLabel, 5, 3, 1, 1);

        zASlider = new QSlider(centralWidget);
        zASlider->setObjectName(QStringLiteral("zASlider"));
        zASlider->setMaximum(174);
        zASlider->setValue(0);
        zASlider->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(zASlider, 6, 3, 1, 1);

        ambient_r = new QSlider(centralWidget);
        ambient_r->setObjectName(QStringLiteral("ambient_r"));
        ambient_r->setMaximum(100);
        ambient_r->setValue(70);
        ambient_r->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(ambient_r, 7, 0, 1, 2);

        saveButton = new QPushButton(centralWidget);
        saveButton->setObjectName(QStringLiteral("saveButton"));

        gridLayout->addWidget(saveButton, 7, 3, 2, 1);

        ambient_g = new QSlider(centralWidget);
        ambient_g->setObjectName(QStringLiteral("ambient_g"));
        ambient_g->setMaximum(100);
        ambient_g->setValue(70);
        ambient_g->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(ambient_g, 8, 0, 1, 2);

        ambient_b = new QSlider(centralWidget);
        ambient_b->setObjectName(QStringLiteral("ambient_b"));
        ambient_b->setMaximum(100);
        ambient_b->setValue(70);
        ambient_b->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(ambient_b, 9, 0, 1, 2);

        button_undoclip = new QPushButton(centralWidget);
        button_undoclip->setObjectName(QStringLiteral("button_undoclip"));

        gridLayout->addWidget(button_undoclip, 9, 3, 1, 1);

        diffuse_r = new QSlider(centralWidget);
        diffuse_r->setObjectName(QStringLiteral("diffuse_r"));
        diffuse_r->setMaximum(100);
        diffuse_r->setValue(20);
        diffuse_r->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(diffuse_r, 10, 0, 1, 2);

        diffuse_g = new QSlider(centralWidget);
        diffuse_g->setObjectName(QStringLiteral("diffuse_g"));
        diffuse_g->setMaximum(100);
        diffuse_g->setValue(20);
        diffuse_g->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(diffuse_g, 11, 0, 1, 2);

        diffuse_b = new QSlider(centralWidget);
        diffuse_b->setObjectName(QStringLiteral("diffuse_b"));
        diffuse_b->setMaximum(100);
        diffuse_b->setValue(20);
        diffuse_b->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(diffuse_b, 12, 0, 1, 2);

        specular_r = new QSlider(centralWidget);
        specular_r->setObjectName(QStringLiteral("specular_r"));
        specular_r->setMaximum(100);
        specular_r->setValue(40);
        specular_r->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(specular_r, 13, 0, 1, 2);

        specular_g = new QSlider(centralWidget);
        specular_g->setObjectName(QStringLiteral("specular_g"));
        specular_g->setMaximum(100);
        specular_g->setValue(40);
        specular_g->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(specular_g, 14, 0, 1, 2);

        specular_b = new QSlider(centralWidget);
        specular_b->setObjectName(QStringLiteral("specular_b"));
        specular_b->setMaximum(100);
        specular_b->setValue(40);
        specular_b->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(specular_b, 15, 0, 1, 2);

        applyTFButton = new QPushButton(centralWidget);
        applyTFButton->setObjectName(QStringLiteral("applyTFButton"));

        gridLayout->addWidget(applyTFButton, 16, 0, 1, 2);

        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 17, 0, 1, 1);

        button_foreground = new QPushButton(centralWidget);
        button_foreground->setObjectName(QStringLiteral("button_foreground"));
        button_foreground->setStyleSheet(QStringLiteral("background:rgb(0,255,0);"));

        gridLayout->addWidget(button_foreground, 17, 1, 1, 1);

        checkBox_foreground = new QCheckBox(centralWidget);
        checkBox_foreground->setObjectName(QStringLiteral("checkBox_foreground"));

        gridLayout->addWidget(checkBox_foreground, 17, 2, 1, 1);

        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 18, 0, 1, 1);

        button_background = new QPushButton(centralWidget);
        button_background->setObjectName(QStringLiteral("button_background"));
        button_background->setStyleSheet(QStringLiteral("background:rgb(0,0,255);"));

        gridLayout->addWidget(button_background, 18, 1, 1, 1);

        checkBox_background = new QCheckBox(centralWidget);
        checkBox_background->setObjectName(QStringLiteral("checkBox_background"));

        gridLayout->addWidget(checkBox_background, 18, 2, 1, 1);

        button_cleargraphcut = new QPushButton(centralWidget);
        button_cleargraphcut->setObjectName(QStringLiteral("button_cleargraphcut"));

        gridLayout->addWidget(button_cleargraphcut, 19, 0, 1, 2);

        tfPanel = new QLabel(centralWidget);
        tfPanel->setObjectName(QStringLiteral("tfPanel"));

        gridLayout->addWidget(tfPanel, 20, 0, 1, 4);

        comboBox_type = new QComboBox(centralWidget);
        comboBox_type->setObjectName(QStringLiteral("comboBox_type"));

        gridLayout->addWidget(comboBox_type, 10, 3, 1, 1);


        horizontalLayout->addLayout(gridLayout);


        horizontalLayout_2->addLayout(horizontalLayout);

        PresentClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(PresentClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1190, 23));
        PresentClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(PresentClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        PresentClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(PresentClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        PresentClass->setStatusBar(statusBar);

        retranslateUi(PresentClass);

        QMetaObject::connectSlotsByName(PresentClass);
    } // setupUi

    void retranslateUi(QMainWindow *PresentClass)
    {
        PresentClass->setWindowTitle(QApplication::translate("PresentClass", "Present", 0));
        minLabel->setText(QApplication::translate("PresentClass", "0", 0));
        resetButton->setText(QApplication::translate("PresentClass", "reset", 0));
        zALabel->setText(QApplication::translate("PresentClass", "0", 0));
        maxLabel->setText(QApplication::translate("PresentClass", "1", 0));
        zBLabel->setText(QApplication::translate("PresentClass", "1", 0));
        saveButton->setText(QApplication::translate("PresentClass", "saveGradient", 0));
        button_undoclip->setText(QApplication::translate("PresentClass", "undoClip", 0));
        applyTFButton->setText(QApplication::translate("PresentClass", "apply", 0));
        label->setText(QApplication::translate("PresentClass", "foreground", 0));
        button_foreground->setText(QString());
        checkBox_foreground->setText(QString());
        label_2->setText(QApplication::translate("PresentClass", "background", 0));
        button_background->setText(QString());
        checkBox_background->setText(QString());
        button_cleargraphcut->setText(QApplication::translate("PresentClass", "clear", 0));
        tfPanel->setText(QString());
        comboBox_type->clear();
        comboBox_type->insertItems(0, QStringList()
         << QApplication::translate("PresentClass", "1\347\261\273", 0)
         << QApplication::translate("PresentClass", "2\347\261\273", 0)
         << QApplication::translate("PresentClass", "3\347\261\273", 0)
         << QApplication::translate("PresentClass", "4\347\261\273", 0)
        );
    } // retranslateUi

};

namespace Ui {
    class PresentClass: public Ui_PresentClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PRESENT_H

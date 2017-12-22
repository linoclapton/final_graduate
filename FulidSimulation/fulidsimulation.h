#ifndef FULIDSIMULATION_H
#define FULIDSIMULATION_H

#include <QtWidgets/QMainWindow>
#include "ui_fulidsimulation.h"

class FulidSimulation : public QMainWindow
{
    Q_OBJECT

public:
    FulidSimulation(QWidget *parent = 0);
    ~FulidSimulation();

private:
    Ui::FulidSimulationClass ui;
};

#endif // FULIDSIMULATION_H

#include "fulidsimulation.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FulidSimulation w;
    w.show();
    return a.exec();
}

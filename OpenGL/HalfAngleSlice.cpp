#include "HalfAngleSlice.h"
#include <iostream>

HalfAngleSlice::HalfAngleSlice(QWidget *parent):QMainWindow(parent){
    ui.setupUi(this);
    model = new QDirModel();  
    model->setReadOnly(false); 
    model->setSorting(QDir::DirsFirst | QDir::IgnoreCase | QDir::Name); 

    ui.treeView->setModel(model);  
    ui.treeView->setRootIndex(model->index("c:/users/ly/pictures"));  
    //ui.treeView->setVisible(false);
    //connect(ui.treeView,SIGNAL(mouseDoubleClickEvent()))
}

void HalfAngleSlice::keyPressEvent(QKeyEvent *event){
    if(event->key()==Qt::Key_Return){
        ui.openGLWidget->update();
    /*  std::cout<<model->filePath(ui.treeView->currentIndex()).toStdString()<<std::endl;
        ui.openGLWidget->reload(model->filePath(ui.treeView->currentIndex()));
        ui.openGLWidget->update();
        */
    }
    
}

HalfAngleSlice::~HalfAngleSlice(){

}
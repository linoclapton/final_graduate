#include "Present.h"
#include <fstream>

Present::Present(QWidget *parent)
    : QMainWindow(parent)
{
    QGLFormat format;
    format.setVersion(4,3);
    format.setProfile(QGLFormat::CoreProfile);
    ui.setupUi(this);
    tf1DWidget = new TransferFunction1DWidget();
    tf1DWidget->show();
    //ui.openGLWidget->canvas = ui.canvas;
    minSlider = ui.minSlider;
    maxSlider = ui.maxSlider;
    minLabel = ui.minLabel;
    maxLabel = ui.maxLabel;
    ambient[0]=0.7f; ambient[1]=0.7f; ambient[2]=0.7f;
    diffuse[0]=0.2f; diffuse[1]=0.2f; diffuse[2]=0.2f;
    specular[0]=0.4f; specular[1]=0.4f; specular[2]=0.4f;
    _applyTFButton = ui.applyTFButton;
    min = 0;
    max = 1;
    AZ = 0;
    BZ = 1;
    foreColor = QColor(0,255,0);
    backColor = QColor(0,0,255);
    ui.openGLWidget->setGraphCutColor(0,backColor.redF(),backColor.greenF(),backColor.blueF());
    ui.openGLWidget->setGraphCutColor(1,foreColor.redF(),foreColor.greenF(),foreColor.blueF());

    connect(ui.ambient_r,SIGNAL(valueChanged(int)),this,SLOT(changeAmbientR(int)));
    connect(ui.ambient_g,SIGNAL(valueChanged(int)),this,SLOT(changeAmbientG(int)));
    connect(ui.ambient_b,SIGNAL(valueChanged(int)),this,SLOT(changeAmbientB(int)));
    connect(ui.diffuse_r,SIGNAL(valueChanged(int)),this,SLOT(changeDiffuseR(int)));
    connect(ui.diffuse_g,SIGNAL(valueChanged(int)),this,SLOT(changeDiffuseG(int)));
    connect(ui.diffuse_b,SIGNAL(valueChanged(int)),this,SLOT(changeDiffuseB(int)));
    connect(ui.specular_r,SIGNAL(valueChanged(int)),this,SLOT(changeSpecularR(int)));
    connect(ui.specular_g,SIGNAL(valueChanged(int)),this,SLOT(changeSpecularG(int)));
    connect(ui.specular_b,SIGNAL(valueChanged(int)),this,SLOT(changeSpecularB(int)));
    connect(minSlider,SIGNAL(valueChanged(int)),this,SLOT(changeMin(int)));
    connect(maxSlider,SIGNAL(valueChanged(int)),this,SLOT(changeMax(int)));
    connect(ui.zASlider,SIGNAL(valueChanged(int)),this,SLOT(changeMinZ(int)));
    connect(ui.zBSlider,SIGNAL(valueChanged(int)),this,SLOT(changeMaxZ(int)));
    connect(ui.saveButton,SIGNAL(clicked()),this,SLOT(save()));
    connect(ui.resetButton,SIGNAL(clicked()),this,SLOT(updateTex()));
    connect(tf1DWidget,SIGNAL(ValueChanged()),this,SLOT(updateTF()));
    connect(ui.applyTFButton,SIGNAL(clicked()),this,SLOT(updateTF()));
    connect(ui.openGLWidget,SIGNAL(changeSlider(float,float)),this,SLOT(changeSlider(float,float)));
    connect(ui.openGLWidget,SIGNAL(changeLight(float*,float*,float*)),this,SLOT(changeLight(float*,float*,float*)));
    connect(ui.button_foreground,SIGNAL(clicked()),this,SLOT(changeForeColor()));
    connect(ui.button_background,SIGNAL(clicked()),this,SLOT(changeBackColor()));
    connect(ui.checkBox_foreground,SIGNAL(stateChanged(int)),this,SLOT(changeMotionStatus(int)));
    connect(ui.checkBox_background,SIGNAL(stateChanged(int)),this,SLOT(changeMotionStatus(int)));
    connect(ui.button_cleargraphcut,SIGNAL(clicked()),ui.openGLWidget,SLOT(clearPoints()));
    connect(ui.button_undoclip,SIGNAL(clicked()),ui.openGLWidget,SLOT(undoClip()));
}
void Present::changeLight(float* am,float* diff,float* spec){
    for(int i=0;i<3;i++){
        ambient[i] = am[i];
        diffuse[i] = diff[i];
        specular[i] = spec[i];
    }
    ui.ambient_r->setValue(am[0]*100);
    ui.ambient_g->setValue(am[1]*100);
    ui.ambient_b->setValue(am[2]*100);
    ui.diffuse_r->setValue(diff[0]*100);
    ui.diffuse_g->setValue(diff[1]*100);
    ui.diffuse_b->setValue(diff[2]*100);
    ui.specular_r->setValue(spec[0]*100);
    ui.specular_g->setValue(spec[1]*100);
    ui.specular_b->setValue(spec[2]*100);
    /*changeAmbientG(am[1]*100);
    changeAmbientB(am[2]*100);
    changeDiffuseR(diff[0]*100);
    changeDiffuseG(diff[1]*100);
    changeDiffuseB(diff[2]*100);
    changeSpecularR(spec[0]*100);
    changeSpecularG(spec[1]*100);
    changeSpecularB(spec[2]*100);*/
}
void Present::closeEvent(QCloseEvent* event){
    std::ofstream out("config.txt");
    out<<*(ui.openGLWidget);
    out<<*(tf1DWidget);
    tf1DWidget->close();
}
void Present::readin(){
    std::ifstream in("config.txt");
    in>>*(ui.openGLWidget);
    in>>*(tf1DWidget);
    ui.openGLWidget->updateUniform();
    ui.openGLWidget->updateGL();
}
void Present::save(){
    ui.openGLWidget->saveGradient();
}
void Present::changeMax(int value){
    maxLabel->setText(QString::number(value/1000.0f));
    max = value/1000.0f;
    ui.openGLWidget->changeWindowFilter(min,max);
    ui.openGLWidget->updateGL();
}
void Present::changeMin(int value){
    minLabel->setText(QString::number(value/1000.0f));
    min = value/1000.0f;
    ui.openGLWidget->changeWindowFilter(min,max);
    ui.openGLWidget->updateGL();
}
void Present::changeMaxZ(int value){
    ui.zBLabel->setText(QString::number(value));
    BZ = value;
    ui.openGLWidget->changeSliceZ(AZ,BZ);
    ui.openGLWidget->updateGL();
}
void Present::changeMinZ(int value){
    ui.zALabel->setText(QString::number(value));
    AZ = value;
    ui.openGLWidget->changeSliceZ(AZ,BZ);
    ui.openGLWidget->updateGL();
}
void Present::changeAmbientR(int value){
    ambient[0] = value/100.0f;
    ui.openGLWidget->updateLight(ambient,diffuse,specular);
    ui.openGLWidget->updateGL();
}
void Present::changeAmbientG(int value){
    ambient[1] = value/100.0f;
    ui.openGLWidget->updateLight(ambient,diffuse,specular);
    ui.openGLWidget->updateGL();
}
void Present::changeAmbientB(int value){
    ambient[2] = value/100.0f;
    ui.openGLWidget->updateLight(ambient,diffuse,specular);
    ui.openGLWidget->updateGL();
}
void Present::changeDiffuseR(int value){
    diffuse[0] = value/100.0f;
    ui.openGLWidget->updateLight(ambient,diffuse,specular);
    ui.openGLWidget->updateGL();
}
void Present::changeDiffuseG(int value){
    diffuse[1] = value/100.0f;
    ui.openGLWidget->updateLight(ambient,diffuse,specular);
    ui.openGLWidget->updateGL();
}
void Present::changeDiffuseB(int value){
    diffuse[2] = value/100.0f;
    ui.openGLWidget->updateLight(ambient,diffuse,specular);
    ui.openGLWidget->updateGL();
}
void Present::changeSpecularR(int value){
    specular[0] = value/100.0f;
    ui.openGLWidget->updateLight(ambient,diffuse,specular);
    ui.openGLWidget->updateGL();
}
void Present::changeSpecularG(int value){
    specular[1] = value/100.0f;
    ui.openGLWidget->updateLight(ambient,diffuse,specular);
    ui.openGLWidget->updateGL();
}
void Present::changeSpecularB(int value){
    specular[2] = value/100.0f;
    ui.openGLWidget->updateLight(ambient,diffuse,specular);
    ui.openGLWidget->updateGL();
}
void Present::updateTex(){
    ui.openGLWidget->resetModel();
    ui.openGLWidget->updateGL();
}
void Present::updateTF(){
    std::vector<float> tf;
    tf1DWidget->GetTfColorAndAlpha(256,tf);
    ui.openGLWidget->updateTF(tf);
    ui.openGLWidget->updateGL();
}
void Present::changeSlider(float min,float max){
    minSlider->setValue(min*1000);
    maxSlider->setValue(max*1000);
}
Present::~Present()
{

}
void Present::changeBackColor(){
    int r,g,b;
    backDialog = new QColorDialog(backColor);
    backDialog->setModal(true);
    backColor = backDialog->getColor(backColor);
    backColor.getRgb(&r,&g,&b);
    QString s = "background-color: rgb("+QString::number(r)+","+QString::number(g)+","+QString::number(b)+");";
    ui.button_background->setStyleSheet(s);
    ui.openGLWidget->setGraphCutColor(0,backColor.redF(),backColor.greenF(),backColor.blueF());
}

void Present::changeForeColor(){
    int r,g,b;
    foreDialog = new QColorDialog(foreColor);
    foreDialog->setModal(true);
    foreColor = foreDialog->getColor(foreColor);
    foreColor.getRgb(&r,&g,&b);
    QString s = "background-color: rgb("+QString::number(r)+","+QString::number(g)+","+QString::number(b)+");";
    ui.button_foreground->setStyleSheet(s);
    ui.openGLWidget->setGraphCutColor(1,foreColor.redF(),foreColor.greenF(),foreColor.blueF());
}


void Present::changeMotionStatus(int state){
    if(state!=0){
        ui.checkBox_background->setChecked(false);
        ui.checkBox_foreground->setChecked(false);
        QCheckBox* box = dynamic_cast<QCheckBox*> (this->sender());
        box->setChecked(true);
        if(ui.checkBox_foreground->isChecked()){
            ui.openGLWidget->setLeftMotionType(PaintFront);
        }else{
            ui.openGLWidget->setLeftMotionType(PaintBack);
        }
    }
    else{
        ui.openGLWidget->setLeftMotionType(Rotate);
    }
}
#include "TorusWindow.h"
#include "MySlider.h"
#include <QSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QComboBox>
TorusWindow::TorusWindow(QWidget *parent, Qt::WindowFlags flags)
	: QMainWindow(parent, flags){
		this->setWindowTitle("Lino Presentation");
        ui.setupUi(this);
        ui.spinBox_outer->setRange(0,100);
        ui.slider_outer->setRange(0,100);
        ui.slider_outer->setValue(70);
        ui.spinBox_outer->setValue(70);
        ui.slider_outer->setIndex(1);
        ui.slider_outer->setPanel(ui.canvas);
        ui.spinBox_inner->setRange(0,100);
        ui.slider_inner->setRange(0,100);
        ui.slider_inner->setValue(15);
        ui.spinBox_inner->setValue(15);
        ui.slider_inner->setIndex(2);
        ui.slider_inner->setPanel(ui.canvas);
        ui.spinBox_rings->setRange(1,100);
        ui.slider_rings->setRange(1,100);
        ui.slider_rings->setValue(60);
        ui.spinBox_rings->setValue(60);
        ui.slider_rings->setPanel(ui.canvas);
        ui.slider_rings->setIndex(3);
        ui.spinBox_slices->setRange(1,100);
        ui.slider_slices->setRange(1,100);
        ui.slider_slices->setValue(60);
        ui.spinBox_slices->setValue(60);
        ui.slider_slices->setPanel(ui.canvas);
        ui.slider_slices->setIndex(4);
        QObject::connect(ui.spinBox_slices,SIGNAL(valueChanged(int)),ui.slider_slices,SLOT(setValue(int)));
        QObject::connect(ui.slider_slices,SIGNAL(valueChanged(int)),ui.spinBox_slices,SLOT(setValue(int)));
        QObject::connect(ui.slider_slices,SIGNAL(valueChanged(int)),ui.slider_slices,SLOT(change(int)));
        QObject::connect(ui.spinBox_rings,SIGNAL(valueChanged(int)),ui.slider_rings,SLOT(setValue(int)));
        QObject::connect(ui.slider_rings,SIGNAL(valueChanged(int)),ui.spinBox_rings,SLOT(setValue(int)));
        QObject::connect(ui.slider_rings,SIGNAL(valueChanged(int)),ui.slider_rings,SLOT(change(int)));
        QObject::connect(ui.shader_stage, SIGNAL(currentIndexChanged(const QString &)), ui.canvas, SLOT(changeShader(const QString &)));  
		QObject::connect(ui.light, SIGNAL(currentIndexChanged(const QString &)), ui.canvas, SLOT(changeLight(const QString &)));  
		QObject::connect(ui.light, SIGNAL(currentIndexChanged(const QString &)), ui.canvas, SLOT(changeLight(const QString &)));  
		QObject::connect(ui.canvas, SIGNAL(setLight(const QString &)),ui.light,SLOT(setCurrentText(const QString &)));
        QObject::connect(ui.spinBox_outer,SIGNAL(valueChanged(int)),ui.slider_outer,SLOT(setValue(int)));
        QObject::connect(ui.slider_outer,SIGNAL(valueChanged(int)),ui.spinBox_outer,SLOT(setValue(int)));
        QObject::connect(ui.slider_outer,SIGNAL(valueChanged(int)),ui.slider_outer,SLOT(change(int)));
        QObject::connect(ui.spinBox_inner,SIGNAL(valueChanged(int)),ui.slider_inner,SLOT(setValue(int)));
        QObject::connect(ui.slider_inner,SIGNAL(valueChanged(int)),ui.spinBox_inner,SLOT(setValue(int)));
        QObject::connect(ui.slider_inner,SIGNAL(valueChanged(int)),ui.slider_inner,SLOT(change(int)));
        QObject::connect(ui.slider_specular,SIGNAL(valueChanged(int)),ui.canvas,SLOT(setSpecular(int)));
        QObject::connect(ui.slider_diffuse,SIGNAL(valueChanged(int)),ui.canvas,SLOT(setDiffuse(int)));
        QObject::connect(ui.slider_ambient,SIGNAL(valueChanged(int)),ui.canvas,SLOT(setAmbient(int)));
		/*centralWidget = new QWidget;
		QHBoxLayout *mainLayout = new QHBoxLayout;
		QVBoxLayout *leftLayout = new QVBoxLayout;
		QGridLayout *rightLayout = new QGridLayout;
		mainLayout->setMargin(0);
		rightLayout->setMargin(0);
		Torus *torus =new Torus(0.7,0.15,60,60);
		canvas = new MyPanel(torus);
		leftLayout->addWidget(canvas);

		canvas->setFixedSize(1150,900);

		QComboBox *shader_stage = new QComboBox;
		shader_stage->addItem("VF");
		shader_stage->addItem("VTF");
		shader_stage->addItem("VGF");
		rightLayout->addWidget(shader_stage,0,0);
		QComboBox *light = new QComboBox;
		light->addItem("light");
		light->addItem("no light");
		QObject::connect(shader_stage, SIGNAL(currentIndexChanged(const QString &)), canvas, SLOT(changeShader(const QString &)));  
		QObject::connect(light, SIGNAL(currentIndexChanged(const QString &)), canvas, SLOT(changeLight(const QString &)));  
		QObject::connect(canvas, SIGNAL(setLight(const QString &)),light,SLOT(setCurrentText(const QString &)));
		rightLayout->addWidget(light,0,1);

		MySlider *slider_outer = new MySlider(0,canvas,1);
		QSpinBox *spinBox_outer = new QSpinBox;
		spinBox_outer->setRange(0,100);
		slider_outer->setRange(0,100);
		slider_outer->setValue(70);
		spinBox_outer->setValue(70);
		QObject::connect(spinBox_outer,SIGNAL(valueChanged(int)),slider_outer,SLOT(setValue(int)));
		QObject::connect(slider_outer,SIGNAL(valueChanged(int)),spinBox_outer,SLOT(setValue(int)));
		QObject::connect(slider_outer,SIGNAL(valueChanged(int)),slider_outer,SLOT(change(int)));
		rightLayout->addWidget(spinBox_outer,1,0);
		rightLayout->addWidget(slider_outer,1,1);


		MySlider *slider_inner = new MySlider(0,canvas,2);
		QSpinBox *spinBox_inner = new QSpinBox;
		spinBox_inner->setRange(0,100);
		slider_inner->setRange(0,100);
		slider_inner->setValue(15);
		spinBox_inner->setValue(15);
		QObject::connect(spinBox_inner,SIGNAL(valueChanged(int)),slider_inner,SLOT(setValue(int)));
		QObject::connect(slider_inner,SIGNAL(valueChanged(int)),spinBox_inner,SLOT(setValue(int)));
		QObject::connect(slider_inner,SIGNAL(valueChanged(int)),slider_inner,SLOT(change(int)));
		rightLayout->addWidget(spinBox_inner);
		rightLayout->addWidget(slider_inner);

		MySlider *slider_rings = new MySlider(0,canvas,3);
		QSpinBox *spinBox_rings = new QSpinBox;
		spinBox_rings->setRange(1,100);
		slider_rings->setRange(1,100);
		slider_rings->setValue(60);
		spinBox_rings->setValue(60);
		connect(spinBox_rings,SIGNAL(valueChanged(int)),slider_rings,SLOT(setValue(int)));
		connect(slider_rings,SIGNAL(valueChanged(int)),spinBox_rings,SLOT(setValue(int)));
		connect(slider_rings,SIGNAL(valueChanged(int)),slider_rings,SLOT(change(int)));
		rightLayout->addWidget(spinBox_rings);
		rightLayout->addWidget(slider_rings);


		MySlider *slider_slices = new MySlider(0,canvas,4);
		QSpinBox *spinBox_slices = new QSpinBox;
		spinBox_slices->setRange(1,100);
		slider_slices->setRange(1,100);
		slider_slices->setValue(60);
		spinBox_slices->setValue(60);
		connect(spinBox_slices,SIGNAL(valueChanged(int)),slider_slices,SLOT(setValue(int)));
		connect(slider_slices,SIGNAL(valueChanged(int)),spinBox_slices,SLOT(setValue(int)));
		connect(slider_slices,SIGNAL(valueChanged(int)),slider_slices,SLOT(change(int)));
		rightLayout->addWidget(spinBox_slices);
		rightLayout->addWidget(slider_slices);

		mainLayout->addLayout(leftLayout);
		mainLayout->addLayout(rightLayout);
		this->setCentralWidget(centralWidget);
		centralWidget->setLayout(mainLayout);*/
}
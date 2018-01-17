#pragma once
#include "GLSLProgram.h"
#include <QGLWidget>
#include <qlabel.h>
#include "ULYBase.h"
#include <Serializable.h>
#include "AVSTable2D.h"
#include "MarkPoint.h"
#include "SLIC.h"
#include "GL_Model.h"
enum MotionType{
    NoMotion,
    Rotate,
    PaintBack,
    PaintFront,
    ClipVolume,
    Scale,
};
enum RenderType{

};
class VolumeRenderPanel: public QGLWidget,Serializable
{
    Q_OBJECT
public:
    VolumeRenderPanel(QWidget *parent = 0);
    void changeWindowFilter(float min,float max);
    void changeSliceZ(int a,int b);
    void saveGradient();
    void updateTex();
    void updateTF(std::vector<float> &tf);
    void updateLight(float* a,float *d,float *s);
    friend ofstream& operator<<(ofstream&,VolumeRenderPanel&);
    friend ifstream& operator>>(ifstream&,VolumeRenderPanel&);
    void updateUniform();
    QLabel *canvas;
    std::vector<MarkPoint> points;
    void setLeftMotionType(MotionType type);
    void setGraphCutColor(int type,float r,float g,float b);
    void saveScreenShot();
    float *upWindScheme(int,int,int);
    float *upWindScheme2(int,int,int);
    float index(int x, int y, int z);
    float getOpacity(float g);
	void recomputeLV();
public:
	static const int max_label = 4;
	QPolygonF polygons[max_label];
	QVector<QColor> qcolors[max_label];
    float am[max_label][3],diff[max_label][3],spec[max_label][3];
    float min[max_label],max[max_label];
	int current_type;
signals:
    void changeSlider(float min,float max);
    void changeLight(float*,float*,float*);
public slots:
    void clearPoints();
    void resetModel();
    void undoClip();
	void changeClassType(int index);
private slots:
    void rotateBack();
protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
private:
    void initialModel();
    void initialComputeShader();
    void bindData();
    void createPIT();
    float clamp(float value,float min,float max);
    void resetMVPN();
    void drawCube(int type = 0 );
    void clipData();
    inline bool exclude(float x,float y,float z,float x1,float y1,float a,float b,float c);
    void drawString(const char*);
    void vertex(int index, int type = 0);
    glm::vec4 slerp(glm::vec4,glm::vec4,float);
    glm::vec4 mat2q(glm::mat4);
    glm::mat4 q2mat(glm::vec4);
    glm::vec4 rotateStart;
    glm::vec4 rotateStop;
    QPoint	lastPos;
    QPoint  posA,posB;
    ULY::GL_Obj_Model obj;
    float x1,x2,y1,y2;
    bool drawSelect;
    bool drawSelectRect;   
    bool isMouseDown;
    bool leftDown,rightDown;
    GLSLProgram glsl;
    glm::mat4 mv;
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
    glm::mat4 camera;
    GLuint texture;
    float currentZ;
    float eyeZPosition;
    float eyePosition[3];
    bool hasLight;
    bool isRotating;
    MotionType leftMotionType,rightMotionType,wheelMotionType,backupState[3];
    enum Shader_Stage{VF,VTF,VGF} stage;
    GLuint textureID2;
    GLuint textureID3;
    /*static const int SLICEX = 256,SLICEY= 256,SLICEZ=161;
    ushort data[SLICEZ][SLICEY][SLICEX];
    #define FILENAME "tooth.bin"*/
    string filename;
    int dimension[3];
    float mx , mi;
    //static const int SLICEX=512,SLICEY=512,SLICEZ=175;
    //int SLICEX,SLICEY,SLICEZ;
    unsigned char *dataFlag;//[SLICEZ][SLICEY][SLICEX];
    float *udata;//[SLICEZ][SLICEY][SLICEX];
    float *ulabel;
    float screenPixels[960][960][4];
    GLfloat lookupImg[256*256*4];
    float maxValue,minValue;
    float color[256][4];
    float color_texs[4][256][4];
    int gradientGrayData[256][256];
    int volumeX,volumeY,volumeZ;
    float spaceX,spaceY,spaceZ;
    float opacity[256*max_label];
    float histgram[256];
    ULY::Clock clock;
    GLuint FBO;
    GLuint frontFBO;
    GLuint backFBO;
    GLuint graphCutFBO;
    float currentID;
    int counter;
    int totalTime;
    int lastFPS;
    int w,h;
    string tab[19];
    AVSTable2D asTable[19];
    int sliceA,sliceB;
    float minVoxel,maxVoxel;
    float minGradient,maxGradient;
    GLuint VAO,VBO,EBO;
    float backColor[3],frontColor[3];
    float ratio[3];
    int diagonal;
    bool graphCut;
    QTimer* rotateTimer;
    int rotateBackTimes;
    glm::mat4 rotateBackModel;
    GLuint labelFlagTex;
    float labelFlag[1024];
    SLIC slic;
    GLfloat radius;
    bool drawPolygon;
    int** label;
	GLuint proba_tex[max_label];
	unsigned char * probability[max_label];
	float scatter[max_label], sharp[max_label];
};


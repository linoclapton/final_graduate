#include "VolumeRenderPanel.h"
#include <windows.h>
#include <QtOpenGL>
#include <QtGui>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <fstream>
#include <iostream>
#include "GL_Model.h"
//#include "mat2bmp.h"
#include <ctime>
#include <Eigen/SparseCore>
#include <Eigen/Sparse>
#include <Eigen/SparseCholesky>
#include <Eigen/SparseLU> 
#include <Eigen/SparseQR>
#include <Eigen/IterativeLinearSolvers>
#include <vector>
#include <omp.h>
bool Scat; 
typedef Eigen::SparseMatrix<double> SpMat; // declares a column-major sparse matrix type of double
typedef Eigen::Triplet<double> T;
static const int DIM = 2;
extern "C" 
float* gc( float*,int,int,int);

float VolumeRenderPanel::index(int x, int y, int z) {
    return udata[DIM*x*dimension[1]*dimension[0]+DIM*y*dimension[0]+DIM*z];
}


float VolumeRenderPanel::getOpacity(float g){
   return  opacity[int(g*256)];
}

float* VolumeRenderPanel::upWindScheme(int Nx, int Ny, int Nz) {
    clock.start();
    omp_set_num_threads(8);
    Eigen::setNbThreads(8);
    int n = Eigen::nbThreads();
    cout<<"n:"<< n <<endl;
    float* p = new float[Nx*Ny*Nz]; 
    int total = Nx*Ny*Nz;
    float l[] = {0.573,0.573,0.573};
   for( int i = 0 ; i < total ; ++i)
        p[i] = 0.0;
    for( int t = 0 ; t < 5; t++) {
        for( int i = 0 ; i < Nx ; i++ ){
            for( int j = 0; j < Ny; j++ ){
                for( int k = 0; k < Nz; k++ ){
                    if(i == 0 || j == 0 || k == 0) {
                        p[i*Ny*Nz+j*Nz+k] = (1.0-getOpacity(index(i,j,k)))*
                            (0.5*l[0]*(i==0?1.0:p[(i-1)*Ny*Nz+j*Nz+k]-p[i*Ny*Nz+j*Nz+k])+
                             0.5*l[1]*(j==0?1.0:p[i*Ny*Nz+(j-1)*Nz+k]-p[i*Ny*Nz+j*Nz+k])+
                             0.5*l[2]*(k==0?1.0:p[i*Ny*Nz+j*Nz+k-1]-p[i*Ny*Nz+j*Nz+k])+p[i*Ny*Nz+j*Nz+k]);
                    }else {
                        p[i*Ny*Nz+j*Nz+k] = (1.0-getOpacity(index(i,j,k)))*
                            (0.5*l[0]*(p[(i-1)*Ny*Nz+j*Nz+k]-p[i*Ny*Nz+j*Nz+k])+
                             0.5*l[1]*(p[i*Ny*Nz+(j-1)*Nz+k]-p[i*Ny*Nz+j*Nz+k])+
                             0.5*l[2]*(p[i*Ny*Nz+j*Nz+k-1]-p[i*Ny*Nz+j*Nz+k])+p[i*Ny*Nz+j*Nz+k]);
                    }
                }
            }
        }
    }
    clock.end("begin prepare");
    int slice = Ny*Nz;
    SpMat A(total,total);
    //A.reserve(Eigen::VectorXi::Constant(total,6));
    cout<<total<<endl;
    std::vector<T> tripletList;
    clock.start();
    Eigen::VectorXd b(total);    
    for(int i = 0 ; i < total ; ++i) {
        if(i-Ny*Nz+1>0){
            tripletList.push_back(T(i,i-slice,-1)); 
            //A.insert(i,i-slice)=-1;
        }
        if(i-Ny+1>0){
            tripletList.push_back(T(i,i-Ny,-1)); 
            //A.insert(i,i-Ny)=-1;
        }
        if(i > 0){
            tripletList.push_back(T(i,i-1,-1)); 
            //A.insert(i,i-1)=-1;
        }
        tripletList.push_back(T(i,i,6)); 
        //A.insert(i,i)=6;
        if(i+1 < total){
            tripletList.push_back(T(i,i+1,-1)); 
            //A.insert(i,i+1)=-1;
        }
        if(i+Ny < total){
            tripletList.push_back(T(i,i+Ny,-1)); 
            //A.insert(i,i+Ny)=-1;
        }
        if(i+slice < total){
            tripletList.push_back(T(i,i+slice,-1)); 
            //A.insert(i,i+slice)=-1;
        }
        b[i] = p[i];
    }
    clock.end("prepare");
    clock.start();
    A.setFromTriplets(tripletList.begin(), tripletList.end());
    clock.end("begin insert Matrix");
    Eigen::ConjugateGradient<SpMat, Eigen::Lower> cg;
    clock.start();
    cg.compute(A);
    clock.end("compute Matrix");
    clock.start();
    Eigen::VectorXd x = cg.solve(b); 
    clock.end("solve Matrix");
    for( int i = 0;  i< total ; ++i ) 
        p[i] = x[i];
    ULY::write("real",(char*)p,total*4); 
    //ULY::read("real",(char*)p,total*4); 
    return p;
}

float* VolumeRenderPanel::upWindScheme2(int Nx, int Ny, int Nz) {
	float* p = new float[Nx*Ny*Nz];
	int total = Nx*Ny*Nz;
	float l[] = { 0.577,0.577,0.577};
	for (int i = 0; i < total; ++i)
		p[i] = 0.0;
	for (int i = 0; i < Nx; i++) {
		for (int t = 0; t < 5; t++) {
			for (int j = 0; j < Ny; j++) {
				for (int k = 0; k < Nz; k++) {
					if (i == 0 || j == 0 || k == 0) {
						p[i*Ny*Nz + j*Nz + k] = (1.0 - getOpacity(index(i, j, k)))*
							0.5*(l[0] * (i == 0 ? 1.0 : p[(i - 1)*Ny*Nz + j*Nz + k] - p[i*Ny*Nz + j*Nz + k]) +
								l[1] * (j == 0 ? 1.0 : p[i*Ny*Nz + (j - 1)*Nz + k] - p[i*Ny*Nz + j*Nz + k]) +
								l[2] * (k == 0 ? 1.0 : p[i*Ny*Nz + j*Nz + k - 1] - p[i*Ny*Nz + j*Nz + k]) + 2*p[i*Ny*Nz + j*Nz + k]);
					}
					else if (i == Nx - 1 || j == Ny - 1 || k == Nz - 1) {
						p[i*Ny*Nz + j*Nz + k] = 0.0;
					}
					else {
						p[i*Ny*Nz + j*Nz + k] = (1.0 - getOpacity(index(i, j, k)))*
							0.5*(l[0] * (p[(i - 1)*Ny*Nz + j*Nz + k] - p[i*Ny*Nz + j*Nz + k]) +
								l[1] * (p[i*Ny*Nz + (j - 1)*Nz + k] - p[i*Ny*Nz + j*Nz + k]) +
								l[2] * (p[i*Ny*Nz + j*Nz + k - 1] - p[i*Ny*Nz + j*Nz + k]) + 2*p[i*Ny*Nz + j*Nz + k]);
					}
					if (p[i*Ny*Nz + j*Nz + k] > 1.0)
						p[i*Ny*Nz + j*Nz + k] = (1.0 - getOpacity(index(i, j, k)));
					if (p[i*Ny*Nz + j*Nz + k] < 0.01)
						p[i*Ny*Nz + j*Nz + k] = 0.0;
				}
			}
		}
	}
	float *lv = gc(p, Nx, Ny, Nz);
	float mx = lv[0];
	float mn = lv[0];
	float mx2 = p[0];
	float mn2 = p[0];
	for (int i = 0; i < Nx*Ny*Nz; i++) {
		if (lv[i] < mn) mn = lv[i];
		if (lv[i] > mx) mx = lv[i];
		if (p[i] > mx2) mx2 = p[i];
		if (p[i] < mn2) mn2 = p[i];
	}
	cout <<"data max and min:"<< mx << ' ' << mn << ' '<<mx2<<' '<<mn2 << endl;
	return lv;
}

//graphCutTex 长宽需与屏幕大小一致
VolumeRenderPanel::VolumeRenderPanel(QWidget* parent):QGLWidget(parent){
    //filename = "kidney2.data";
    filename = "foot.bin";
    //filename = "buckyball.bin";
    //filename = "manix.bin";
    std::ifstream fin;
    //fin.open("tooth.bin",std::ios::binary);
    fin.open(filename,std::ios::binary);
    int maxAndMin[2];
    fin.read((char*)maxAndMin,2*4);
    fin.read((char*)dimension,3*4);    
    //dimension[2] >>= 1;
    int w = dimension[2],h = dimension[1],d = dimension[0];
    cout<<maxAndMin[0]<<' '<<maxAndMin[1]<<endl;
    cout<<d<<' '<<h<<' '<<w<<endl;
    udata = new float[w*h*d];
    float datawidth = maxAndMin[0]-maxAndMin[1];
    dataFlag = new unsigned char[w*h*d];
    fin.read((char *)udata,w*h*d*4);
    for(int i=0;i<w*h*d;i++){
        udata[i] = (udata[i]-maxAndMin[1])/datawidth;
    }
    /*for(int i=0;i<w;i++){
        for(int j=0;j<h;j++)
            for(int k= 0;k<d;k++)
                if(i<w-50)
                    udata[i*h*d+j*d+k] = (udata[i*h*d+j*d+k]-maxAndMin[1])/datawidth;
                else
                    udata[i*h*d+j*d+k] = 0.0;
    }*/
    eyePosition[0] = eyePosition[1] = eyePosition[2] = 0.0;
    fin.close();
    unsigned int** supervoxel = new unsigned int*[w];
    label = new int*[w];
    cout<<maxAndMin[0]<<' '<<maxAndMin[1]<<endl;
    for(int i=0;i<w;i++){
        supervoxel[i] = new unsigned int[h*d];
        label[i] = new int[h*d]; 
        memset(label[i],0,h*d*4);
    }
    unsigned int tmp;
    for(int i=0;i<w;i++)
        for(int j=0;j<h*d;j++){
            tmp =  udata[i*h*d+j]*255;
            supervoxel[i][j] =0xFF000000+tmp<<16+tmp<<8+tmp;
        }
    double compatness = 20.0;
    int numberlabels = 1000;
    int supervoxelsize = w*h*d/numberlabels+1;//w*h*d;
    clock.start();
    //slic.DoSupervoxelSegmentation(supervoxel,d,h,w,label,numberlabels,supervoxelsize,compatness);
    clock.end("super voxel");
    
    /*std::ofstream fout;
    fout.open("label.bin",std::ios::binary);
    for(int i=0;i<w;i++)
        fout.write((char *)label[i],h*d*4);
    fout.close();*/
    fin.open("label.bin",std::ios::binary);
    for(int i=0;i<w;i++){
        //fin.read((char *)label[i],h*d*4);
    }
    fin.close();
    mx = 0.0,mi=10000.0;
    for(int i=0;i<w;i++)
        for(int j=0;j<h*d;j++){
            if(mx<label[i][j])
                mx = label[i][j];
            if(mi>label[i][j])
                mi = label[i][j];
        }
    cout<<"maxlabel:"<<mx<<' '<<mi<<endl;
    /*ulabel = new float[w*h*d];
    float labelwidth = mx-mi;
    for(int i=0;i<w;i++)
        for(int j=0;j<h*d;j++){
            ulabel[i*h*d+j] = label[i][j]/labelwidth;
        }*/
    for(int i=0;i<1024;i++)
        labelFlag[i] = 0.5f;
    for(int i=18;i<19;i++)
        labelFlag[i] = 0.5f;

    /*fin.open("head.bin",std::ios::binary);
    fin.read((char*)dimension,3*4);    
    int* newSuper = new int[dimension[0]*dimension[1]*dimension[2]];
    fin.read((char *)newSuper,dimension[2]*dimension[1]*dimension[0]*4);
    fin.close();
    float* newSuper2 =new float[dimension[0]*dimension[1]*dimension[2]];
    for(int i=0;i<dimension[2]*dimension[1]*dimension[0];i++)
                newSuper2[i] = newSuper[i]/10000.0f;
    std::ofstream fout;
    fout.open("newhead.bin",std::ios::binary);
    maxAndMin[0] = 0.0f;maxAndMin[1] = 1.0f;
    fout.write((char*)maxAndMin,2*4);
    fout.write((char*)dimension,3*4);
    fout.write((char *)udata,dimension[0]*dimension[1]*dimension[2]*4);
    fout.close();*/
    mx = 0.0,mi=10000.0;
    for(int i=0;i<dimension[0]*dimension[1]*dimension[2];i++)
    {
        if(mx<udata[i])
            mx = udata[i];
        if(mi>udata[i])
            mi = udata[i];
    }
    cout<<"max value:"<<mx<<' '<<mi<<endl;
    float space[3];
    space[0]=dimension[0];//0.595703f*dimension[0];
    space[1]=dimension[1];//0.595703f*dimension[1];
    space[2]=dimension[2];//0.330017f*dimension[2];
    mx = space[0];
    if(mx<space[1]) mx = space[1];
    if(mx<space[2]) mx = space[2];
    for(int i=0;i<3;i++)
        ratio[i] = mx/space[i];
        //ratio[i] = dimension[2]/dimension[i];
    spaceX = 1.0/dimension[0];
    spaceY = 1.0/dimension[1];
    spaceZ = 1.0/dimension[2];
    currentID = 0;
	setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer));
	isMouseDown = false;
    currentZ = 0;
    leftMotionType = Rotate;
    rightMotionType = ClipVolume;
    wheelMotionType = Scale;
    rotateBackTimes = 30;
    graphCut = false;
    memset(dataFlag,0,w*h*d);
    /*for(int i=0;i<SLICEX;i++)
        for(int j=0;j<SLICEY;j++)
            {
                gradientGrayData[i][j]=0;
                for(int k=0;k<SLICEZ;k++)
                    dataFlag[k][j][i] = 0;
            }*/
   /* for(int i=0;i<256;i++)
        for(int j=0;j<256;j++)
            for(int k=0;i<161;k++)
                value[i][j][k] = 0.0f;
                */
    for(int i=0;i<256;i++)
        histgram[i] = 0;
    leftDown = false;
    rightDown = false;
    min=0;
    max=1;
    sliceA = 0;
    sliceB = 1;
    drawSelect = false;
    drawSelectRect = false;
    counter = 0;
    totalTime = 0;
    isRotating = false;
    lastFPS = 60.0;
    rotateTimer = new QTimer(this);
    connect(rotateTimer,SIGNAL(timeout()),this,SLOT(rotateBack()));
    fin.open("color2.txt");
    int r1,g1,b1;
    for(int i=0;i<256;i++)
    {
        fin>>r1>>g1>>b1;
        color[i][0]=r1;//*0.00392;
        color[i][1]=g1;//*0.00392;
        color[i][2]=b1;//*0.00392;
    }
    fin.close();
    fin.open("opacity_IPSVI.txt");
    for(int i=0;i<256;i++)
    { 
        fin>>opacity[i];
        //opacity[i] = clamp(1.0f*i*0.0039f,0.01f,0.8f);
        color[i][3] = 1.0f;//opacity[i]*256;
    }
    //opacity[0] =0.0f;

    fin.close();
    double r=0.,g=0.,b=0.,a=0.; 
    float rcol, gcol, bcol, acol;
    double rInt[256],gInt[256],bInt[256],aInt[256];
    int smin,smax;
    double factor; int lookupindex = 0;
    rInt[0] = 0.;gInt[0] = 0.;bInt[0] = 0.;aInt[0] = 0.;float tauc;
    // compute integral functions
    for (int i=1;i<256;i++) {
        tauc = (color[(i-1)][3]+color[i][3])/2.;
        r = r + (color[(i-1)][0]+color[i][0])/2.*tauc/255.;
        g = g + (color[(i-1)][1]+color[i][1])/2.*tauc/255.;
        b = b + (color[(i-1)][2]+color[i][2])/2.*tauc/255.;
        a = a + tauc;
        rInt[i] = r;gInt[i] = g;bInt[i] = b;aInt[i] = a; }
    // compute look-up table from integral functions
    for (int sb=0;sb<256;sb++)
        for (int sf=0;sf<256;sf++) {
            if (sb < sf) { smin = sb;smax = sf; }
            else { smin = sf;smax = sb; }
            if (smax != smin) {
                factor = 1. / (double)(smax - smin);
                rcol = (rInt[smax] - rInt[smin]) * factor;
                gcol = (gInt[smax] - gInt[smin]) * factor;
                bcol = (bInt[smax] - bInt[smin]) * factor;
                acol = 256.*
                    (1.-exp(-(aInt[smax]-aInt[smin])*factor/255.));
            } else {
                factor = 1. / 255.;
                rcol = color[smin][0]*color[smin][3]*factor;
                gcol = color[smin][1]*color[smin][3]*factor;
                bcol = color[smin][2]*color[smin][3]*factor;
                acol = (1.-exp(-color[smin][3]*1./255.))*256.; }
            lookupImg[lookupindex++] = clamp(rcol*0.0039,0,1);
            lookupImg[lookupindex++] = clamp(gcol*0.0039,0,1);
            lookupImg[lookupindex++] = clamp(bcol*0.0039,0,1);
            lookupImg[lookupindex++] = clamp(acol*0.0039,0,1);
            }
        tab[0]  = "asTable/AVSTable2D_a09_g-09.tab";
        tab[1]  = "asTable/AVSTable2D_a09_g-08.tab";
        tab[2]  = "asTable/AVSTable2D_a09_g-07.tab";
        tab[3]  = "asTable/AVSTable2D_a09_g-06.tab";
        tab[4]  = "asTable/AVSTable2D_a09_g-05.tab";
        tab[5]  = "asTable/AVSTable2D_a09_g-04.tab";
        tab[6]  = "asTable/AVSTable2D_a09_g-03.tab";
        tab[7]  = "asTable/AVSTable2D_a09_g-02.tab";
        tab[8]  = "asTable/AVSTable2D_a09_g-01.tab";
        tab[9]  = "asTable/AVSTable2D_a09_g00.tab";
        tab[10] = "asTable/AVSTable2D_a09_g01.tab";
        tab[11] = "asTable/AVSTable2D_a09_g02.tab";
        tab[12] = "asTable/AVSTable2D_a09_g03.tab";
        tab[13] = "asTable/AVSTable2D_a09_g04.tab";
        tab[14] = "asTable/AVSTable2D_a09_g05.tab";
        tab[15] = "asTable/AVSTable2D_a09_g06.tab";
        tab[16] = "asTable/AVSTable2D_a09_g07.tab";
        tab[17] = "asTable/AVSTable2D_a09_g08.tab";
        tab[18] = "asTable/AVSTable2D_a09_g09.tab";
    for(int i=0;i<19;i++)
        asTable[i].loadFromFile(tab[i]);
    radius = 0.03;
}

void VolumeRenderPanel::initializeGL(){
	GLenum err=glewInit();
	if(err!=GLEW_OK)
	{
		//Problem: glewInit failed, something is seriously wrong.
	}
	glsl.create();
	glsl.attachShader(GL_VERTEX_SHADER, "volume_vertex.glsl");
    glsl.printLog();
	glsl.attachShader(GL_FRAGMENT_SHADER, "volume_fragment.glsl");
    glsl.printLog();
	glsl.link();
	glsl.use(0);
    //glsl.initTextShader();

    glsl.getInfo(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX);
	initialModel();
	hasLight = true;
    //torus->setRenderMode(GL_TRIANGLES);
	//torus->model();
    glsl.setUniform("hasLight",hasLight);
    glsl.setUniform("preRender",false);
    glsl.setUniform("DrawSelectRect",false);
    float color[6]={1.0,0.0,0.0,0.5,0.5,0.0};
    glsl.setUniform("colors",2,color);
    //此处4.3s
    updateTex();
    createPIT();
    //bindData();
	qglClearColor(Qt::black);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
    glEnable(ANTIALIASED_QUALITY);
    glEnable(GL_DEPTH_CLAMP);
	glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    
    glOrtho(-1.0,1.0,-1.0,1.0,-5,8);
    
    glsl.change(1);
	glsl.create();
	glsl.attachShader(GL_COMPUTE_SHADER, "compute.glsl");
    glsl.printLog();
	glsl.link();
	glsl.use(1);
    initialComputeShader();
	glsl.use(0);

    glsl.change(2);
    glsl.create();
    glsl.attachShader(GL_VERTEX_SHADER,"fbo_vertex.glsl");
    glsl.attachShader(GL_FRAGMENT_SHADER,"fbo_fragment.glsl");
    glsl.link();
    glsl.printLog();
    glsl.use(2);
    glsl.genBuffers(1,&VBO);
    glsl.genBuffers(1,&EBO);
    glsl.genVertexArrays(1,&VAO);
    GLfloat board[] = {-1.0,1.0,0.0,  1.0,1.0,0.0, 1.0,-1.0,0.0, -1.0,-1.0,0.0};
    GLuint board_indice[] = {0,2,1,0,3,2};
    glsl.bindVertexArray(VAO);
    glsl.bindBuffer(GL_ARRAY_BUFFER, VBO);
    glsl.bufferData(GL_ARRAY_BUFFER, sizeof(board),board, GL_STATIC_DRAW);
    glsl.bindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glsl.bufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(board_indice),board_indice, GL_STATIC_DRAW);
    glsl.vertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glsl.enableVertexAttribArray(0);
    glsl.glActiveTex(GL_TEXTURE1);
    glsl.setUniform("tex",1);
    glsl.bindVertexArray(0);


    glsl.genFramebuffers(1,&FBO);
    glsl.bindFramebuffer(FBO,512,512,1);
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glsl.glActiveTex(GL_TEXTURE1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_FLOAT, NULL); 
    glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex, 0);
    GLuint depthBuf;
    glsl.genRenderbuffers(depthBuf);
    glsl.bindRenderbuffer(depthBuf);
    glsl.renderbufferStorage(1024);
    glsl.framebufferRenderbuffer(depthBuf);
    GLuint attachments[1] = {GL_COLOR_ATTACHMENT0};
    glsl.drawBuffers(1,  attachments);

    glsl.genFramebuffers(1,&graphCutFBO);
    glsl.bindFramebuffer(graphCutFBO,1024,1024,1);
    GLuint renderTex;
    glGenTextures(1, &renderTex);
    glsl.glActiveTex(GL_TEXTURE11); // Use texture unit 3
    glBindTexture(GL_TEXTURE_2D, renderTex);
    // glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE,4,GL_RGBA,1024,1024,NULL);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA16,960,960,0,GL_RGBA,GL_FLOAT,NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glsl.framebufferTexture2D(renderTex);
    glsl.genRenderbuffers(depthBuf);
    glsl.bindRenderbuffer(depthBuf);
    glsl.renderbufferStorage(1024);
    glsl.framebufferRenderbuffer(depthBuf);
    GLenum drawBufs[] = {GL_COLOR_ATTACHMENT0};
    glsl.drawBuffers(1, drawBufs);
    glsl.bindFramebuffer(0,1024,1024,1);
    glsl.use(0);

    // Set the target for the fragment shader outputs

    glsl.getInfo(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX);
    glsl.change(3);
    glsl.create();
    glsl.attachShader(GL_VERTEX_SHADER,"vertex_plane.glsl");
    glsl.attachShader(GL_FRAGMENT_SHADER,"fragment_plane.glsl");
    glsl.link();
    glsl.printLog();
    glsl.use(0);
    /*glsl.use(-1);
    glsl.glActiveTex(GL_TEXTURE0);
    glEnable(GL_DEPTH);
    glEnable(GL_TEXTURE_2D);
    GLfloat light_position[] = {0.0,0.0,2.0,1.0};
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    obj.init("C:/data/obj/cow/","cow.obj");*/
}
void VolumeRenderPanel::updateTex(){
    /*
    glsl.glActiveTex(GL_TEXTURE3); 
    glBindTexture(GL_TEXTURE_1D_ARRAY,texture);
    float samplerColor[18]={1.0,0.0,0.0,0.0,0.0,1.0,
                            1.0,1.0,1.0,0.0,1.0,1.0,
                            1.0,1.0,0.0,1.0,0.0,0.0
                            };
    std::cout<<"here"<<std::endl;
    glTexImage2D(GL_TEXTURE_1D_ARRAY, 0, GL_RGB, 2, 3,0, GL_RGB,GL_FLOAT,samplerColor);
    glTexParameteri(GL_TEXTURE_1D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D_ARRAY,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D_ARRAY,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    updateGL();
    */

    //glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    clock.start();
    glsl.genFramebuffers(1,&frontFBO);
    glsl.bindFramebuffer(frontFBO,1024,1024,1);
    GLuint renderTex;
    glGenTextures(1, &renderTex);
    glsl.glActiveTex(GL_TEXTURE3); // Use texture unit 3
    glBindTexture(GL_TEXTURE_2D, renderTex);
   // glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE,4,GL_RGBA,1024,1024,NULL);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA16,1024,1024,0,GL_RGBA,GL_FLOAT,NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glsl.framebufferTexture2D(renderTex);
    GLuint depthBuf;
    glsl.genRenderbuffers(depthBuf);
    glsl.bindRenderbuffer(depthBuf);
    glsl.renderbufferStorage(1024);
    glsl.framebufferRenderbuffer(depthBuf);
    // Set the target for the fragment shader outputs
    GLenum drawBufs[] = {GL_COLOR_ATTACHMENT0};
    glsl.drawBuffers(1, drawBufs);

    glsl.genFramebuffers(1,&backFBO);
    glsl.bindFramebuffer(backFBO,1024,1024,1);
    GLuint renderTexBack;
    glGenTextures(1, &renderTexBack);
    glsl.glActiveTex(GL_TEXTURE4); // Use texture unit 4
    glBindTexture(GL_TEXTURE_2D, renderTexBack);
    //glTexImage2DMultisample(GL_TEXTURE_2D,4,GL_RGBA,1024,1024,NULL);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA16,1024,1024,0,GL_RGBA,GL_FLOAT,NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glsl.framebufferTexture2D(renderTexBack);
    GLuint depthBufBack;
    glsl.genRenderbuffers(depthBufBack);
    glsl.bindRenderbuffer(depthBufBack);
    glsl.renderbufferStorage(1024);
    glsl.framebufferRenderbuffer(depthBufBack);
    // Set the target for the fragment shader outputs
    GLenum drawBufsBack[] = {GL_COLOR_ATTACHMENT0};
    glsl.drawBuffers(1, drawBufsBack);

    // Unbind the framebuffer, and revert to default framebuffer
    glsl.bindFramebuffer(0,1024,1024,1);
    //127
    int w=dimension[2],h=dimension[1],d=dimension[0];
    //fin.open("tooth.bin",std::ios::binary);
    //short min = 32767,max = -32768;
    /*ushort min=15536,max=0;
    for(int i=0;i<d;i++)
        for(int j=0;j<h;j++)
            for(int k=0;k<w;k++)
            {
               // data[k][j][i] = 100+(i*10)%300;
                if(min>data[k][j][i])min=data[k][j][i];
                if(max<data[k][j][i])max=data[k][j][i];
            }
    minVoxel = min;
    maxVoxel = max;
    
    for(int i=0;i<d;i++)
        for(int j=0;j<h;j++)
            for(int k=0;k<w;k++)
            {
                udata[k][j][i] = (data[k][j][i]-min+0.0)/(max-min);
            }

    std::ofstream fout;
    fout.open("tooth.bin",std::ios::binary);
        
    fout.write((char *)udata,w*h*d*4);
    fout.close();

    float tmp;
    int x,y;
    minGradient = 65536;
    maxGradient = 0;*/
/*    for(int k=0;k<w;k++)
        for(int j=0;j<h;j++)
            for(int i=0;i<d;i++)
            {
                if(i==0||i==d-1||j==0||j==d-1||k==0||k==w-1)
                { value[k][j][i]=0;continue;}
                tmp = ((data[k-1][j][i]-data[k+1][j][i])*(data[k-1][j][i]-data[k+1][j][i])
                  +(data[k][j-1][i]-data[k][j+1][i])*(data[k][j-1][i]-data[k][j+1][i])
                   +(data[k][j][i-1]-data[k][j][i+1])*(data[k][j][i-1]-data[k][j][i+1]));
               value[k][j][i] = sqrt(tmp)/2/maxVoxel;
               if(value[k][j][i]>maxGradient) 
                   maxGradient = value[k][j][i];
               if(value[k][j][i]<minGradient)
                   minGradient = value[k][j][i];
            }*/
    //bindData();
    //182
    // 
    glsl.glActiveTex(GL_TEXTURE9);
    GLuint textureID1;
    glGenTextures(1, &textureID1);
    glBindTexture(GL_TEXTURE_3D, textureID1);
    //glTexStorage3D(GL_TEXTURE_3D, 0, GL_R16, 512, 512, 175);
    
    glsl.texImage3D(GL_TEXTURE_3D, 0,GL_R32F,d,h,w,0, GL_RED,GL_FLOAT,udata);
    //glTexImage3D(GL_TEXTURE_3D, 0,GL_R32F,d/2,h/2,w/2,0, GL_RED,GL_FLOAT,lv);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    //glGenerateMipmapEXT( GL_TEXTURE_3D );
    GLint loc = glsl.getUniformLocation("volume_tex");
    glsl.setUniform(loc, 9);
    //glBindImageTexture(0,textureID1, 0,GL_TRUE, 0, GL_READ_ONLY, GL_R32F);
    glsl.glActiveTex(GL_TEXTURE1);
    glGenTextures(1, &textureID2);
    glBindTexture(GL_TEXTURE_1D, textureID2);
    //glTexStorage3D(GL_TEXTURE_3D, 0, GL_R16, 512, 512, 175);
    glTexImage1D(GL_TEXTURE_1D, 0,GL_RGBA16,256,0, GL_RGBA,GL_FLOAT,color);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    loc = glsl.getUniformLocation("color_tex");
    glsl.setUniform(loc, 1);
    //glActiveTexture(GL_TEXTURE2);
    glsl.glActiveTex(GL_TEXTURE2);
    glGenTextures(1, &textureID3);
    glBindTexture(GL_TEXTURE_1D, textureID3);
    //glTexStorage3D(GL_TEXTURE_3D, 0, GL_R16, 512, 512, 175);
    glTexImage1D(GL_TEXTURE_1D, 0,GL_R32F,256,0, GL_RED,GL_FLOAT,opacity);
    //glActiveTextureARB(GL_TEXTURE0);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    loc = glsl.getUniformLocation("opacity_tex");
    glsl.setUniform(loc, 2);
    GLuint asTexture;
    glsl.glActiveTex(GL_TEXTURE5);
    glGenTextures(1, &asTexture);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureID3);
    glsl.texImage3D(GL_TEXTURE_2D_ARRAY,0,GL_RED,256,256,19,0,GL_RED,GL_FLOAT,NULL);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    for(int i=0;i<19;i++)
        glsl.texSubImage3D(GL_TEXTURE_2D_ARRAY,0,0,0,i,256,256,1,GL_RED,GL_FLOAT,asTable[i].getRadianceDataPtr());
    loc = glsl.getUniformLocation("asTable");
    glsl.setUniform(loc, 5);
    /*GLuint frontTex;
    GLfloat whiteTex[] = { 1, 1, 1, 1 };
    glsl.glActiveTex(GL_TEXTURE5);
    glGenTextures(1, &frontTex);
    glBindTexture(GL_TEXTURE_2D,frontTex);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA16,1,1,0,GL_RGBA, GL_FLOAT,whiteTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    GLuint backTex;
    glsl.glActiveTex(GL_TEXTURE6);
    glGenTextures(1, &backTex);
    glBindTexture(GL_TEXTURE_2D,backTex);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA16,1,1,0,GL_RGBA, GL_FLOAT,whiteTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);*/
    float* lv = 0;
    if(Scat)
        lv = upWindScheme2(dimension[2]/DIM,dimension[1]/DIM,dimension[0]/DIM);
    glsl.glActiveTex(GL_TEXTURE6);
    GLuint textureID6;
    glGenTextures(1, &textureID6);
    glBindTexture(GL_TEXTURE_3D, textureID6);
    //glTexStorage3D(GL_TEXTURE_3D, 0, GL_R16, 512, 512, 175);
    glsl.texImage3D(GL_TEXTURE_3D, 0,GL_R32F,d/DIM,h/DIM,w/DIM,0, GL_RED,GL_FLOAT, lv);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    //glGenerateMipmapEXT( GL_TEXTURE_3D );
    loc = glsl.getUniformLocation("volume_label");
    glsl.setUniform(loc, 6);
    delete[] lv;

    GLuint gradientGrayTex;
    glsl.glActiveTex(GL_TEXTURE7);
    glGenTextures(1, &gradientGrayTex);
    glBindTexture(GL_TEXTURE_2D,gradientGrayTex);
    //glTexStorage3D(GL_TEXTURE_3D, 0, GL_R16, 512, 512, 175);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RED,256,256,0, GL_RED,GL_FLOAT,gradientGrayData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    loc = glsl.getUniformLocation("gradient_gray_opacity");
    glsl.setUniform(loc, 7);

    glsl.glActiveTex(GL_TEXTURE8);
    GLuint textureID8;
    glGenTextures(1, &textureID8);
    glBindTexture(GL_TEXTURE_3D, textureID8);
    //glTexStorage3D(GL_TEXTURE_3D, 0, GL_R16, 512, 512, 175);
    glsl.texImage3D(GL_TEXTURE_3D, 0,GL_RED,d,h,w,0, GL_RED,GL_UNSIGNED_BYTE,dataFlag);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    loc = glsl.getUniformLocation("volume_flag");
    glsl.setUniform(loc, 8);
    //glBindImageTexture(0,textureID8, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
	glsl.bindImageTexture(0, textureID8, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R8UI);
    //updateGL();
    glsl.glActiveTex(GL_TEXTURE12);
    glGenTextures(1, &labelFlagTex);
    glBindTexture(GL_TEXTURE_1D,labelFlagTex);
    //glTexStorage3D(GL_TEXTURE_3D, 0, GL_R16, 512, 512, 175);
    glTexImage1D(GL_TEXTURE_1D, 0,GL_R32F,850,0, GL_RED,GL_FLOAT,labelFlag);
    //glActiveTextureARB(GL_TEXTURE0);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    loc = glsl.getUniformLocation("label_flag");
    glsl.setUniform(loc, 12);

    glsl.glActiveTex(GL_TEXTURE11);
    glsl.setUniform("graphCutTex",11);
    clock.end("bind texture");
}
void VolumeRenderPanel::initialComputeShader(){
    glsl.setUniform("volumeZ",dimension[2]); 
    glsl.setUniform("spaceX",spaceX); 
    glsl.setUniform("spaceY",spaceY); 
    glsl.setUniform("spaceZ",spaceZ); 
    glsl.setUniform("currentID",currentID);
    //GLuint loc = glsl.mglGetUniformLocation("image_data");
    //glsl.setUniform(loc, 0);
    GLuint loc = glsl.getUniformLocation("image_output");
    glsl.setUniform(loc, 8);
}
//discard
void VolumeRenderPanel::bindData(){
    //std::fstream out("g.txt",std::ios::out);
    /*
    int w=SLICEZ,h=SLICEY,d=SLICEX;
    int tw=256,th=256;
    int ca[17]={0},cb[17]={0};
    int x,y;
    float v;
    if(max==min)
        max += 0.0001;
    if(sliceA==0)
        sliceA++;
    if(sliceA==sliceB)
        sliceB++;
    for(int i=0;i<tw;i++)
        for(int j=0;j<th;j++){
               gradientGrayData[i][j] = 0;
        }
    maxValue = 0;
    for(int i=sliceA;i<sliceB;i++)
        for(int j=1;j<h-1;j++)
            for(int k=1;k<w-1;k++)
            {
               if(value[k][j][i]>maxValue) 
                   maxValue = value[k][j][i];
            }

    for(int k=1;k<w-1;k++)
        for(int j=1;j<h-1;j++)
            for(int i=sliceA;i<sliceB;i++)
            {
               v = value[k][j][i]/maxValue;
               if(udata[k][j][i]>=min&&udata[k][j][i]<=max){
                   //jv =(v-min)/(max-min);
                   x = v*tw;
                   y = (udata[k][j][i]-min)/(max-min)*th;
                   if(x>tw)
                       x =tw-1;
                   if(y>th)
                       y =th-1;
                   gradientGrayData[x][y] += 1;
               }
            }

    for(int i=0;i<256;i++)
        histgram[i] = 0;
    int id;
    int highest = 0;
    int secondhighest;
    for(int k=0;k<w;k++)
        for(int j=0;j<h;j++)
            for(int i=0;i<d;i++)
            {
                id = udata[k][j][i]*255;
                histgram[id]++;
                if(histgram[id]>highest&&id!=0)
                    highest = histgram[id];
            }
    int a=-1,b=-1,c=-1;
    for(int i=1;i<256;i++){
        if(a==-1||histgram[a]<histgram[i])
            a = i;
        else if(b==-1||histgram[b]<histgram[i])
        {
            if(histgram[i]>histgram[i-1]&&histgram[i]>histgram[i+1])
                b = i;
        }
        else if(c==-1||histgram[c]<histgram[i]&&histgram[i]>histgram[i+1])
        {   
            if(histgram[i]>histgram[i-1])
                c = i;
        }
    } 
    QImage image(tw,th,QImage::Format_RGB888);
    int hight;
    int pixel;
    for(int i=0;i<tw;i++)
        for(int j=0;j<th;j++){
            //out<<gradientGrayData[i][j]<<std::endl;

           // pixel = gradientGrayData[i][j];
          //  if(pixel>5&&pixel<100)
          //      pixel+=100;
          //  image.setPixel(j,tw-i-1,qRgb(pixel,0,0));
          //  image.setPixel(i,j,qRgb(0,0,0));
        }
    int pixelColor = 0;
    for(int i=1;i<tw;i++){
        hight = histgram[i]/highest*255.0;
        if(i==a||i==b||i==c)
            pixelColor = 255;
        else
            pixelColor = 0;
        for(int j=255-hight;j<255;j++)
        image.setPixel(i,j,qRgb(255,pixelColor,0));
    }
        //out.close();
    canvas->setPixmap(QPixmap::fromImage(image));
    */
}
void VolumeRenderPanel::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);
    this->w = width;
    this->h = height;
    diagonal = sqrt(width*width+height*height)*0.5;
	glFrustum(-1.0, +1.0, -1.0, +1.0, -2.0, 15.0);
	/*glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat x = GLfloat(width) / height;
	glMatrixMode(GL_MODELVIEW);
    */
}
void VolumeRenderPanel::vertex(int index,int type){
    static float a = 0.49;
    static float v[8][3]={{a,-a,a},{a,a,a},{-a,a,a},{-a,-a,a},
                   {a,-a,-a},{a,a,-a},{-a,a,-a},{-a,-a,-a}
                  };
    static float b = 0.01;
    static float v2[8][3]={{b,-b,b},{b,b,b},{-b,b,b},{-b,-b,b},
                   {b,-b,-b},{b,b,-b},{-b,b,-b},{-b,-b,-b}
                  };
    static float c[8][3]={{a*2,-a*2,a*2},{a*2,a*2,a*2},{-a*2,a*2,a*2},{-a*2,-a*2,a*2},
                   {a*2,-a*2,-a*2},{a*2,a*2,-a*2},{-a*2,a*2,-a*2},{-a*2,-a*2,-a*2}
                  };
    //glColor3fv(c[index]);
    if( type == 0 )
        glVertex3fv(v[index]);
    else if( type == 1 ){
        float t[3] = {v2[index][0]+eyePosition[0],v2[index][1]+eyePosition[1],v2[index][2]+eyePosition[2]};
        glVertex3fv(t);
    }
}

void VolumeRenderPanel::drawCube(int type){
    
    glBegin(GL_QUADS);
    
    vertex(0,type);
    vertex(1,type);
    vertex(2,type);
    vertex(3,type);

    vertex(0,type);
    vertex(4,type);
    vertex(5,type);
    vertex(1,type);

    vertex(4,type);
    vertex(7,type);
    vertex(6,type);
    vertex(5,type);

    vertex(3,type);
    vertex(2,type);
    vertex(6,type);
    vertex(7,type);

    vertex(1,type);
    vertex(5,type);
    vertex(6,type);
    vertex(2,type);

    vertex(4,type);
    vertex(0,type);
    vertex(3,type);
    vertex(7,type);
    glEnd();
}
void VolumeRenderPanel::paintGL()
{
    //glClearColor(0.0f,0.0f,0.0f,1.0f);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if(!graphCut){
            isRotating=false;
            glsl.use(0);
            glsl.setUniform("graphCut",graphCut);
            glsl.setUniform("EyePosition",0.0f,0.0f,-1.0f);
            if(isRotating){
                glsl.bindFramebuffer(FBO,512,512,1);
                glViewport(0,0,512,512);
            }else{
                glsl.bindFramebuffer(0,1024,1024,1);
            }
            LARGE_INTEGER nStartCounter,nFrequency;
            QueryPerformanceCounter(&nStartCounter);
            QueryPerformanceFrequency(&nFrequency);
            LARGE_INTEGER nStopCounter;
            //Draw basic tips
            glClearColor(0.0f,0.0f,0.0f,1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            /*glsl.setUniform("drawAxis",true);
            glLineWidth(5);
            glsl.setUniform("axisColor",1.0,0.0,0.0,0.8);
            glBegin(GL_LINES);
            glVertex3f(0.0,0.0,0.0);
            glVertex3f(0.5,0.0,0.0);
            glEnd();
            glsl.setUniform("axisColor",1.0f,0.0f,0.0f,0.8f);
            glBegin(GL_POLYGON);
            glVertex3f(0.0,0.0,0.0);
            glVertex3f(0.02,0.0,0.0);
            glVertex3f(0.02,0.02,0.0);
            glVertex3f(0.0,0.02,0.0);
            glEnd();
            glsl.setUniform("axisColor",0.0,1.0,0.0,1.0);
            glBegin(GL_LINES);
            glVertex3f(0.0,0.0,0.0);
            glVertex3f(0.0,0.5,0.0);
            glEnd();
            glsl.setUniform("axisColor",0.0,0.0,1.0,1.0);
            glBegin(GL_LINES);
            glVertex3f(0.0,0.0,0.0);
            glVertex3f(0.0,0.0,0.5);
            glEnd();
            glsl.setUniform("axisColor",1.0,1.0,0.0,1.0);
            glPointSize(5);
            glBegin(GL_POINTS);
            glVertex3f(-0.5,0.5,0.5);
            glVertex3f(0.5,-0.5,0.5);
            glVertex3f(0.5,0.5,-0.5);
            glVertex3f(-0.5,-0.5,0.5);
            glVertex3f(-0.5,0.5,-0.5);
            glVertex3f(0.5,-0.5,-0.5);
            glVertex3f(-0.5,-0.5,-0.5);
            glVertex3f(0.5,0.5,0.5);
            glEnd();*/
            glLineWidth(1);
            glsl.setUniform("drawAxis",false);
            glsl.setUniform("preRender",true);
            glsl.bindFramebuffer(backFBO,1024,1024,1);
            glClearColor(0.0f,0.0f,0.0f,1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glCullFace(GL_FRONT);
            glViewport(0,0,1024,1024);
            drawCube(); 
            glsl.bindFramebuffer(frontFBO,1024,1024,1);
            glClearColor(0.0f,0.0f,0.0f,1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glCullFace(GL_BACK);
            drawCube(); 
            int loc = glsl.getUniformLocation("front_tex");
            glsl.setUniform(loc, 3);
            loc = glsl.getUniformLocation("back_tex");
            glsl.setUniform(loc, 4);
            if(isRotating){
                glsl.bindFramebuffer(FBO,512,512,1);
                glViewport(0,0,512,512);
            }else{
                glsl.bindFramebuffer(0,1024,1024,1);
                glViewport(0,0,w,h);
            }
            glCullFace(GL_BACK);
            //Draw Volume
            glsl.setUniform("preRender",false);
            glBegin(GL_QUADS);
            glVertex3f(1.0,-1.0,0.0);
            glVertex3f(1.0,1.0,0.0);
            glVertex3f(-1.0,1.0,0.0);
            glVertex3f(-1.0,-1.0,0.0);
            glEnd();
            //Draw Select Rect
            if(drawSelectRect){
                glsl.setUniform("DrawSelectRect",true);
                x1 = (2*posA.x()-width()+0.0)/width();
                x2 = (2*posB.x()-width()+0.0)/width();
                y1 = -(2*posA.y()-height()+0.0)/height(); 
                y2 = -(2*posB.y()-height()+0.0)/height(); 
                glLineWidth(1);
                glsl.setUniform("DrawSelectRectLine",true);
                glBegin(GL_LINE_LOOP);
                glVertex3f(x1,y1,0.0);
                glVertex3f(x1,y2,0.0);
                glVertex3f(x2,y2,0.0);
                glVertex3f(x2,y1,0.0);
                glEnd();
                glsl.setUniform("DrawSelectRectLine",false);
                glLineWidth(1);
                glBegin(GL_QUADS);
                glVertex3f(x1,y1,0.0);
                glVertex3f(x1,y2,0.0);
                glVertex3f(x2,y2,0.0);
                glVertex3f(x2,y1,0.0);
                glEnd();
                glsl.setUniform("DrawSelectRect",false);
            }

            glFinish();
            QueryPerformanceCounter(&nStopCounter);
            int nTime = nFrequency.QuadPart/(nStopCounter.QuadPart - nStartCounter.QuadPart);
            if(counter==32){
                lastFPS = totalTime>>5;
                totalTime = 0;
                counter = 0;
            }else{
                counter++;
                totalTime += nTime;
            }
            QString s("fps:");
            s = s+QString::number(lastFPS);
            //glsl.renderText(s.toStdString(),30,560,0.5,glm::vec3(0.0,1.0,0.0));
            //drawString(s.toStdString().c_str());
            if(isRotating){
                glsl.bindFramebuffer(0,512,512,1);
                glViewport(0,0,w,h);
                glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
                glsl.use(2);
                glsl.bindVertexArray(VAO);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glsl.use(0);
            }
            glsl.use(3);
            int count = 32;
            float theta = 3.1415926*2/count;
            if(drawPolygon)
            for(int j=points.size()-1;j>-1;j--){
                if(points[j].type==Front)
                    glsl.setUniform("outputColor",frontColor[0],frontColor[1],frontColor[2]);
                else
                    glsl.setUniform("outputColor",backColor[0],backColor[1],backColor[2]);
                glBegin(GL_POLYGON);
                for(int i=count;i>0;i--)
                    glVertex3f(points[j].x+sin(theta*i)*radius,points[j].y+cos(theta*i)*radius,0.0f);
                glEnd();
            }
            glsl.use(0);
        }else{
            glsl.use(3);
            glsl.bindFramebuffer(graphCutFBO);
            glClearColor(0.0f,0.0f,0.0f,1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            int count = 32;
            float theta = 3.1415926*2/count;
            for(int j=points.size()-1;j>-1;j--){
                if(points[j].type==Front)
                    glsl.setUniform("outputColor",0.9,0.9,0.9);
                else
                    glsl.setUniform("outputColor",0.5,0.5,0.5);
                glBegin(GL_POLYGON);
                for(int i=count;i>0;i--)
                    glVertex3f(points[j].x+sin(theta*i)*radius,points[j].y+cos(theta*i)*radius,0.0f);
                glEnd();
            }

            glsl.use(0);
            glsl.setUniform("graphCut",graphCut);
            //glsl.setUniform("drawAxis",false);
            //glsl.setUniform("DrawSelectRect",false);
            //glsl.setUniform("preRender",false);
            glsl.bindFramebuffer(0);
            glClearColor(0.0f,0.0f,0.0f,1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glBegin(GL_QUADS);
            glVertex3f(1.0,-1.0,0.0);
            glVertex3f(1.0,1.0,0.0);
            glVertex3f(-1.0,1.0,0.0);
            glVertex3f(-1.0,-1.0,0.0);
            glEnd();
            glFinish();
            glReadPixels(0,0,w,h,GL_RGBA,GL_FLOAT,screenPixels);
            int labelcount[850];
            for(int i=0;i<850;i++){
                labelcount[i] = 0;
            }
            float x,y,z;
            float ratioR[3]={1.0/ratio[0],1.0/ratio[1],1.0/ratio[2]};
            int id;
            int totalCount = 0;
            int totalType = 0;
            int px,py,pz;
            for(int i=0;i<960;i++)
                for(int j=0;j<960;j++)
                {
                    x = screenPixels[i][j][0];
                    y = screenPixels[i][j][1];
                    z = screenPixels[i][j][2];
                    if(x>0.0f||y>0.0f||z>0.0f){
                        px = int(z*dimension[2])<dimension[2]?int(z*dimension[2]):dimension[2]-1;
                        py = int(y*dimension[1])<dimension[1]?int(y*dimension[1]):dimension[1]-1;
                        pz = int(x*dimension[0])<dimension[0]?int(x*dimension[0]):dimension[0]-1;
                        //x = (x-0.5)*ratioR[0]+0.5;
                        //y = (y-0.5)*ratioR[1]+0.5;
                        //z = (z-0.5)*ratioR[2]+0.5;
                        //id = int(850*ulabel[int(z*dimension[2]*dimension[1]*dimension[0]+y*dimension[1]*dimension[0]+x*dimension[0])]);
                        id = label[px][py*dimension[0]+pz];
                        //cout<<y*dimension[1]*dimension[0]<<' '<<x*dimension[0]<<endl;
                        //if(labelFlag[id]){
                        //    cout<<id<<endl;
                        //}
                        totalCount++;
                        if(labelcount[id]==0)
                            totalType++;
                        labelcount[id]++;
                    }
                }
            cout<<"type:"<<totalType<<' '<<totalCount<<endl;
            for(int i=0;i<850;i++){
                cout<<labelcount[i]<<' ';
                if(i%50==49)
                    cout<<endl;
            }
            int averagecount;
            if(totalType<2){
                totalType=1;
                averagecount=1;
            }else{
             averagecount = totalCount/totalType;
            }
            for(int i=0;i<850;i++){
                if(labelcount[i]>=averagecount)
                    labelFlag[i] = 0.5f;
                else
                    labelFlag[i] = 0.0f;
            }
            cout<<"========================="<<endl;
            glsl.glActiveTex(GL_TEXTURE12);
            glBindTexture(GL_TEXTURE_1D,labelFlagTex);
            //glTexStorage3D(GL_TEXTURE_3D, 0, GL_R16, 512, 512, 175);
            glTexImage1D(GL_TEXTURE_1D, 0,GL_R32F,850,0, GL_RED,GL_FLOAT,labelFlag);
            graphCut = false;
            paintGL();
        }
    /*glsl.use(-1);
    glm::mat4 rotate = glm::rotate(glm::radians(-10.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    const float *pSource = (const float*)glm::value_ptr(rotate);
    glMultMatrixf(pSource);
        obj.render();*/
        //glsl.use(0);
    //std::cout<<1/nTime<<std::endl;     
	//torus->render();
}
void VolumeRenderPanel::drawString(const char* str){
        static int isFirstCall =  1;  
        static GLuint lists;  

        if (isFirstCall) {  
            isFirstCall = 0;  
            // 申请MAX_CHAR个连续的显示列表编号  
            lists = glGenLists(128);  
            // 把每个字符的绘制命令都装到对应的显示列表中  
            wglUseFontBitmaps(wglGetCurrentDC(), 0, 127, lists);  
        }  
        // 调用每个字符对应的显示列表，绘制每个字符  
        for (; *str != '\0'; ++str) {  
            glCallList(lists + *str);  
        }  
}
void VolumeRenderPanel::saveGradient(){
    
    std::fstream out("g.csv",std::ios::out);
    for(int i=0;i<256;i++)
    {
        for(int j=0;j<256;j++)
        out<<gradientGrayData[i][j]<<",";
    out<<std::endl;
    }
    out.close();
}
bool VolumeRenderPanel::exclude(float x,float y,float z,float x1,float y1,float a,float b,float c){
    glm::vec4 rotateCoord(x-0.5,y-0.5,z-0.5,1.0);
    //glm::vec4 rotateCoord(x,y,z,1.0);
    rotateCoord = model*rotateCoord;
    float result = a*(rotateCoord.x-x1)+b*(rotateCoord.y-y1);
    if(result>0)
        return true;
    return false;
}
void VolumeRenderPanel::clipData(){
    glsl.use(1);
    glsl.setUniform("Model",model);
    glsl.setUniform("undo",false);
    glsl.setUniform("currentID",currentID);
    glsl.setUniform("posA",x1/camera[0][0],y1/camera[0][0],0);
    glsl.setUniform("posB",x2/camera[0][0],y2/camera[0][0],0);
    glsl.dispatchCompute((dimension[0]+31)/32,(dimension[1]+31)/32,(dimension[2]+31)/32);
    glsl.use(0);
    /*int x,y,z;
    float ax=x1/camera[0][0],ay=y1/camera[0][0],bx=x2/camera[0][0],by=y2/camera[0][0];
    for(int i=0;i<SLICEZ;i++)
        for(int j=0;j<SLICEY;j++)
            for(int k=0;k<SLICEX;k++){
                if(udata[i][j][k]<=0||dataFlag[i][j][k]!=0)
                    continue;
                if(exclude(k*spaceX,j*spaceY,i*spaceZ,ax,0.0,-1.0,0.0,0.0))
                    continue;
                if(exclude(k*spaceX,j*spaceY,i*spaceZ,bx,0.0,1.0,0.0,0.0))
                    continue;
                if(exclude(k*spaceX,j*spaceY,i*spaceZ,0.0,ay,0.0,1.0,0.0))
                    continue;
                if(exclude(k*spaceX,j*spaceY,i*spaceZ,0.0,by,0.0,-1.0,0.0))
                    continue;
                   dataFlag[i][j][k] = currentID;
            }*/
}
void VolumeRenderPanel::undoClip(){
    glsl.use(1);
    glsl.setUniform("currentID",currentID);
    glsl.setUniform("undo",true);
    glsl.dispatchCompute((dimension[0]+31)/32,(dimension[1]+31)/32,(dimension[2]+31)/32);
    //glDispatchCompute(8,8,6);
    glsl.use(0);
    if(currentID>0){
        currentID--;
    }
    update();
}

void VolumeRenderPanel::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        switch(leftMotionType){
        case Rotate:
            lastPos = event->pos();	
            leftDown = true;
            isRotating = true;
            glsl.use(0);
            glsl.setUniform("steps",128.0f);
            break;
        case PaintBack:
            lastPos = event->pos();
            points.push_back(MarkPoint(2.0*lastPos.x()/w-1.0,1.0-2.0*lastPos.y()/h,Back));
            leftDown = true;
            break;
        case PaintFront:
            lastPos = event->pos();
            points.push_back(MarkPoint(2.0*lastPos.x()/w-1.0,1.0-2.0*lastPos.y()/h,Front));
            leftDown = true;
            break;
        case NoMotion:
            break;
        default:
            break;
        }
    }
    else if(event->button()==Qt::RightButton){
        switch(rightMotionType){
        case ClipVolume:
            posA = event->pos();
            drawSelectRect = true;
            rightDown = true;
            break;
        case NoMotion:
            break;
        }
    }
}
void VolumeRenderPanel::mouseReleaseEvent(QMouseEvent *event){
    if(event->button()==Qt::LeftButton){
        switch(leftMotionType){
        case Rotate:
            leftDown = false;
            isRotating = false;
            glsl.use(0);
            glsl.setUniform("steps",512.0f);
            updateGL();
            break;
        default:
            break;
        }
    }else if(event->button()==Qt::RightButton){
        switch(rightMotionType){
        case ClipVolume:
            drawSelectRect = false;
            rightDown = false;
            //glsl.setUniform("selectA",x1,y1,0.1f);
            //glsl.setUniform("selectB",x2,y2,0.1f);
            //float b[3] = {x2,y2,0.1};
            //glsl.setUniform("selectB",3,b);
            //std::cout<<x1<<" "<<y1<<" "<<x2<<" "<<y2<<std::endl;
            currentID += 1.0f;
            clipData();

            //glsl.glActiveTex(GL_TEXTURE8);
            //glTexImage3D(GL_TEXTURE_3D, 0,GL_RED,SLICEX,SLICEY,SLICEZ,0, GL_RED,GL_FLOAT,dataFlag);
            drawSelect = true;
            glsl.setUniform("DrawSelect",drawSelect);
            updateGL();
            break;
        default:
            break;
        }
    }else{
        graphCut = !graphCut;
        //drawSelect = !drawSelect;
        /*if(currentID>0)
        {for(int i=0;i<SLICEZ;i++)
            for(int j=0;j<SLICEY;j++)
                for(int k=0;k<SLICEX;k++){
                       if(dataFlag[i][j][k] == currentID)
                           dataFlag[i][j][k] = 0;
                }
        currentID-=1.0f;
        }*/
        //glsl.glActiveTex(GL_TEXTURE8);
        //glTextureSubImage3D(GL_TEXTURE_3D, 0,0,0,0,SLICEX,SLICEY,SLICEZ, GL_RED,GL_FLOAT,dataFlag);
        //glBindImageTexture(0,text8, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
        //glsl.setUniform("DrawSelect",drawSelect);
        updateGL();
    }
}
void VolumeRenderPanel::mouseMoveEvent(QMouseEvent *event)
{
    using namespace std;
    if(leftDown){
        GLfloat sx = lastPos.x()-w*0.5;
        GLfloat sy = -lastPos.y()+h*0.5;
        GLfloat sz = sqrt(diagonal*diagonal-sx*sx-sy*sy);
        GLfloat ex = event->x()-w*0.5;
        GLfloat ey = -event->y()+h*0.5;
        GLfloat ez = sqrt(diagonal*diagonal-ex*ex-ey*ey);
        switch(leftMotionType){
        case Rotate:
            if((ex<w*0.5&&ex>-w*0.5)&&(ey<h*0.5&&ey>-h*0.5)){
                GLfloat dx = GLfloat(event->x() - lastPos.x()) / width();
                GLfloat dy = GLfloat(event->y() - lastPos.y()) / height();
                glm::vec3 line = glm::cross(glm::vec3(sx,sy,sz),glm::vec3(ex,ey,ez));
                GLfloat degree = glm::acos(glm::dot(glm::vec3(ex,ey,ez),glm::vec3(sx,sy,sz))/(diagonal*diagonal));
                model = glm::rotate(degree, line)*model;
                //model = glm::rotate(glm::radians(-180*dx), glm::vec3(1.0f,0.0f,0.0f))*model;
                //model *= glm::rotate(glm::radians(180*dy), glm::vec3(0.0f,1.0f,0.0f));
                glsl.setUniform("Model",model);
                
                //std::cout<<"==================================================="<<std::endl;
                //std::cout<<sx<<' '<<sy<<' '<<ex<<' '<<ey<<' '<<std::endl;
                /*for(int i=0;i<4;i++){
                    for(int j=0;j<4;j++)
                        std::cout<<model[i][j]<<' ';
                    std::cout<<std::endl;
                }*/

                mv = view * model;
                //glsl.setUniform("NormalMatrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
                lastPos = event->pos();
                updateGL();
            }
            break;
        case PaintBack:
            points.push_back(MarkPoint(2.0*event->x()/w-1.0,1.0-2.0*event->y()/h,Back));
            updateGL();
            break;    
        case PaintFront:
            points.push_back(MarkPoint(2.0*event->x()/w-1.0,1.0-2.0*event->y()/h,Front));
            updateGL();
            break;    
        case NoMotion:
            break;
        }
    }
    else if(rightDown){
        switch(rightMotionType){
        case ClipVolume:
            posB = event->pos();
            updateGL();
            break;
        default:
            break;
        }
    }
}
void VolumeRenderPanel::createPIT(){
        // create texture
        GLuint preintName;
        glsl.glActiveTex(GL_TEXTURE10);
        glGenTextures(1,&preintName);
        glBindTexture(GL_TEXTURE_2D,preintName);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0,
            GL_RGBA,GL_FLOAT, &lookupImg);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        GLint loc = glsl.getUniformLocation("preIntegerationcolor");
        glsl.setUniform(loc, 10);
}
void VolumeRenderPanel::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton){
        QPoint posA = event->pos();
        float x = (2*posA.x()-width()+0.0)/width();
        float y = -(2*posA.y()-height()+0.0)/height(); 
        std::cout<<x*0.5<<" "<<y*0.5<<std::endl;
        glsl.setUniform("selectA",x,y,0.1f);
        glsl.setUniform("GetPos",true);
        updateGL();
        updateGL();
        float pixel[4]={100,255,255,0};
        //glReadPixels((x1+1)/2*width(),(y1+1)/2*height(),1,1,GL_RGBA,GL_FLOAT,pixel);
        glReadPixels(posA.x()-1,height()-posA.y()-1,1,1,GL_RGBA,GL_FLOAT,pixel);
        for(int i=0;i<4;i++)
            std::cout<<pixel[i]<<" ";
        std::cout<<std::endl;
        glsl.setUniform("GetPos",false);
        updateGL();
    }
}
void VolumeRenderPanel::wheelEvent(QWheelEvent *event){
    float dist;
    switch(wheelMotionType){
    case Scale:
        glsl.use(0);
        //float eyePosition[3];
        //glsl.getUniform("EyePosition",eyePosition);
        dist = -event->delta()/1440.0;	
        currentZ += dist;
        camera *= glm::scale(glm::vec3(1+dist,1+dist,1+dist)) ; 
        glsl.setUniform("Camera",camera);
        /*eyePosition[2] = eyePosition[2] - dist;	
        if(eyePosition[2]>6)
            eyePosition[2] = 6;
        if(eyePosition[2]<1)
            eyePosition[2] = 1;
        eyeZPosition = eyePosition[2];
        view = glm::lookAt(glm::vec3(0.0f, 0.0f, eyePosition[2]), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));	
        mv = view * model;
        glsl.setUniform("EyePosition", 0.0f,0.0f,eyePosition[2]);
        glsl.setUniform("View",view);
        glsl.setUniform("NormalMatrix",glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
        */
        updateGL();
        break;
    default:
        break;
    }
}
void VolumeRenderPanel::changeWindowFilter(float min,float max){
    glsl.use(0);
    glsl.setUniform("WindowMax",max);
    glsl.setUniform("WindowMin",min);
    glsl.setUniform("WindowWidth",max-min);
    this->min = min;
    this->max = max;
    //bindData();
}
void VolumeRenderPanel::changeSliceZ(int a,int b){
    glsl.use(0);
    //sliceA = a;
    //sliceB = b;
    //bindData();
    //updateGL();
    glsl.setUniform("slice",a);
}
void VolumeRenderPanel::resetMVPN(){
	glsl.setUniform("Kd", 0.9f, 0.5f, 0.3f);
	glsl.setUniform("Ld", 0.2f, 0.2f, 0.2f);
	glsl.setUniform("Ks", 0.9f, 0.5f, 0.3f);
	glsl.setUniform("Ls", 0.4f, 0.4f, 0.4f);
	glsl.setUniform("EyePosition", 0.0f,0.0f,eyeZPosition);
	glsl.setUniform("Ka", 0.1f, 0.05f, 0.03f);
	glsl.setUniform("La", 0.7f, 0.7f, 0.7f);
	glsl.setUniform("LightPosition", view *glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f));
	glsl.setUniform("View",view);
	glsl.setUniform("Model", model);
	glsl.setUniform("NormalMatrix",glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
	glsl.setUniform("Projection", projection);
}
void VolumeRenderPanel::initialModel(){
	model = glm::mat4(1.0f);
    camera = glm::mat4(1.0f);
	model *= glm::rotate(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	//model *= glm::rotate(glm::radians(35.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	view = glm::mat4(1.0f);// glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	projection = glm::mat4(1.0f);//glm::perspective(glm::radians(70.0f), (float)4 / 3, 0.3f, 100.0f);
	eyeZPosition = 0.0f;
    glsl.setUniform("Camera",camera);
    glsl.setUniform("steps",512.0f);
	//glsl.setUniform("Kd", 0.9f, 0.5f, 0.3f);
	glsl.setUniform("Kd", 0.6f, 0.6f, 0.6f);
	glsl.setUniform("Ld", 0.2f, 0.2f, 0.2f);
	glsl.setUniform("Ks", 0.2f, 0.2f, 0.2f);
	glsl.setUniform("Ls", 0.4f, 0.4f, 0.4f);
	glsl.setUniform("EyePosition", -2.0f,-2.0f,-2.0f);
	glsl.setUniform("Ka", 1.0f, 1.0f, 1.0f);
	glsl.setUniform("La", 0.7f, 0.7f, 0.7f);
	glsl.setUniform("LightPosition", glm::vec4(-1.0,-1.0,-1.0,1.0));//view *glm::vec4(5.0f, 5.0f, 2.0f, 1.0f));
	//glsl.setUniform("LightPosition", glm::vec4(-1.0,-1.0,-1.0,1.0));//view *glm::vec4(5.0f, 5.0f, 2.0f, 1.0f));
	glsl.setUniform("View",view);
	mv = view * model;
	glsl.setUniform("Model", model);
	glsl.setUniform("NormalMatrix",glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
	glsl.setUniform("Projection", projection);
    min = 0.10f;
    max = 0.85f;
    glsl.setUniform("WindowMin",0.10f);
    glsl.setUniform("WindowMax",0.85f);
    glsl.setUniform("WindowWidth",0.75f);
    glsl.setUniform("spaceX",spaceX); 
    glsl.setUniform("spaceY",spaceY); 
    glsl.setUniform("spaceZ",spaceZ); 
    glsl.setUniform("ratioX",ratio[0]);
    glsl.setUniform("ratioY",ratio[1]);
    glsl.setUniform("ratioZ",ratio[2]);
}

float VolumeRenderPanel::clamp(float value,float min,float max){
    if(value<min)
        return min;
    if(value>max)
        return max;
    return value;
}

void VolumeRenderPanel::updateLight(float ambient[],float diffuse[],float specular[]){
    for(int i=0;i<3;i++){
        am[i] = ambient[i];
        diff[i] = diffuse[i];
        spec[i] = specular[i];
    }
	glsl.setUniform("La", ambient[0],ambient[1],ambient[2]);
	glsl.setUniform("Ld", diffuse[0],diffuse[1],diffuse[2]); 
	glsl.setUniform("Ls", specular[0],specular[1],specular[2]); 
	//glsl.setUniform("LightPosition", specular[0],specular[1],specular[2]); 

}

void VolumeRenderPanel::updateTF(std::vector<float> &tf){
	clock.start();
    ofstream fout("color_IPSVI.txt");
    ofstream fout2("opacity_IPSVI.txt");
    for(int i=0;i<256;i++)
    {
        color[i][0] = tf[i*4];
        color[i][1] = tf[i*4+1];
        color[i][2] = tf[i*4+2];
        color[i][3] = tf[i*4+3];
        opacity[i]  = tf[i*4+3];
        fout<<color[i][0]<<' '<<color[i][1]<<' '<<color[i][2]<<endl;
        fout2<<opacity[i]<<endl;
    }
    fout.close();
    fout2.close();
    glsl.glActiveTex(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_1D, textureID2);
    glTexImage1D(GL_TEXTURE_1D, 0,GL_RGBA,256,0, GL_RGBA,GL_FLOAT,color);
    glsl.glActiveTex(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_1D, textureID3);
    glTexImage1D(GL_TEXTURE_1D, 0,GL_RED,256,0, GL_RED,GL_FLOAT,opacity);
    if(Scat){
		clock.start();
        float* lv = upWindScheme2(dimension[2]/DIM,dimension[1]/DIM,dimension[0]/DIM);
		clock.end("upwind time:");
        glsl.glActiveTex(GL_TEXTURE6);
        glsl.texImage3D(GL_TEXTURE_3D, 0,GL_R32F,dimension[0]/DIM,dimension[1]/DIM,dimension[2]/DIM,0, GL_RED,GL_FLOAT, lv);
        delete[] lv;
    }
    glsl.glActiveTex(GL_TEXTURE0);
	clock.end("total update:");
    updateGL();
}

ofstream& operator<<(ofstream& out,VolumeRenderPanel &data){
    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            out<<data.model[i][j]<<' ';
    out<<'\n';
    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            out<<data.camera[i][j]<<' ';
    out<<'\n';
    out<<data.min<<' '<<data.max<<' ';
    out<<'\n';
    for(int i=0;i<3;i++)
        out<<data.am[i]<<' '<<data.diff[i]<<' '<<data.spec[i]<<' ';
    out<<'\n';
    return out;
}

ifstream& operator>>(ifstream& in,VolumeRenderPanel &data){
    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            in>>data.model[i][j];
    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            in>>data.camera[i][j];
    in>>data.min>>data.max;
    for(int i=0;i<3;i++)
        in>>data.am[i]>>data.diff[i]>>data.spec[i];
    return in;
}

void VolumeRenderPanel::updateUniform(){
    glsl.setUniform("Model",model);
    glsl.setUniform("Camera",camera);
    emit changeSlider(min,max);
    emit changeLight(am,diff,spec);
    //changeWindowFilter(min,max);
   
}

void VolumeRenderPanel::setLeftMotionType(MotionType type){
    this->leftMotionType = type;
    if(type==Rotate)
        drawPolygon = false;
    else
        drawPolygon = true;
    update();
}

void VolumeRenderPanel::setGraphCutColor(int type, float r,float g,float b){
    if(type==0){
        backColor[0] = r;
        backColor[1] = g;
        backColor[2] = b;
    }else if(type == 1){
        frontColor[0] = r;
        frontColor[1] = g;
        frontColor[2] = b;
    }
}

void VolumeRenderPanel::clearPoints(){
    points.clear();
    for(int i=0;i<850;i++)
        labelFlag[i] = 1.0f;
    glsl.glActiveTex(GL_TEXTURE12);
    glBindTexture(GL_TEXTURE_1D,labelFlagTex);
    //glTexStorage3D(GL_TEXTURE_3D, 0, GL_R16, 512, 512, 175);
    glTexImage1D(GL_TEXTURE_1D, 0,GL_R32F,850,0, GL_RED,GL_FLOAT,labelFlag);
    updateGL();
}
void VolumeRenderPanel::resetModel(){
    backupState[0] = leftMotionType;    
    backupState[1] = rightMotionType;    
    backupState[2] = wheelMotionType;
    leftMotionType = rightMotionType = wheelMotionType = NoMotion;
    rotateTimer->start(20);  
    rotateBackTimes = 10;
    rotateStart = mat2q(model);
    rotateStop = mat2q(glm::rotate(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
    camera = glm::mat4(1.0f);
    glsl.setUniform("Camera",camera);
}
void VolumeRenderPanel::rotateBack(){
    if(rotateBackTimes>0){
        rotateBackTimes--;
        model = q2mat(slerp(rotateStart,rotateStop,(10.0-rotateBackTimes)*0.1));
        glsl.setUniform("Model",model);
        updateGL();
    }else{
        leftMotionType = backupState[0];     
        rightMotionType = backupState[1];
        wheelMotionType = backupState[2];
        rotateTimer->stop();
    }
}
glm::vec4 VolumeRenderPanel::mat2q(glm::mat4 mat){
    float tr = mat[0][0] + mat[1][1] + mat[2][2];
    glm::vec4 q;
    if (tr > 0) { 
        float S = sqrt(tr+1.0) * 2; // S=4*q.w 
        q.w = 0.25 * S;
        q.x = (mat[2][1] - mat[1][2]) / S;
        q.y = (mat[0][2] - mat[2][0]) / S; 
        q.z = (mat[1][0] - mat[0][1]) / S; 
    } else if ((mat[0][0] > mat[1][1])&(mat[0][0] > mat[2][2])) { 
        float S = sqrt(1.0 + mat[0][0] - mat[1][1] - mat[2][2]) * 2; // S=4*q.x 
        q.w = (mat[2][1] - mat[1][2]) / S;
        q.x = 0.25 * S;
        q.y = (mat[0][1] + mat[1][0]) / S; 
        q.z = (mat[0][2] + mat[2][0]) / S; 
    } else if (mat[1][1] > mat[2][2]) { 
        float S = sqrt(1.0 + mat[1][1] - mat[0][0] - mat[2][2]) * 2; // S=4*q.y
        q.w = (mat[0][2] - mat[2][0]) / S;
        q.x = (mat[0][1] + mat[1][0]) / S; 
        q.y = 0.25 * S;
        q.z = (mat[1][2] + mat[2][1]) / S; 
    } else { 
        float S = sqrt(1.0 + mat[2][2] - mat[0][0] - mat[1][1]) * 2; // S=4*q.z
        q.w = (mat[1][0] - mat[0][1]) / S;
        q.x = (mat[0][2] + mat[2][0]) / S;
        q.y = (mat[1][2] + mat[2][1]) / S;
        q.z = 0.25 * S;
    }
    return q;
}
glm::mat4 VolumeRenderPanel::q2mat(glm::vec4 q){
    glm::mat4 m;
    m[0][0] = 1 - 2*q.y*q.y - 2*q.z*q.z;
    m[0][1] = 2*q.x*q.y - 2*q.z*q.w;
    m[0][2] = 2*q.x*q.z + 2*q.y*q.w;
    m[1][0] = 2*q.x*q.y + 2*q.z*q.w;
    m[1][1] = 1 - 2*q.x*q.x - 2*q.z*q.z;
    m[1][2] = 2*q.y*q.z - 2*q.x*q.w;
    m[2][0] = 2*q.x*q.z - 2*q.y*q.w;
    m[2][1] = 2*q.y*q.z + 2*q.x*q.w;
    m[2][2] = 1 - 2*q.x*q.x - 2*q.y*q.y;
    m[3][3] = 1;
    return m;
}
glm::vec4 VolumeRenderPanel::slerp(glm::vec4 qa, glm::vec4 qb, float t) {
    // quaternion to return
    glm::vec4 qm(0.0,0.0,0.0,0.0);
    // Calculate angle between them.
    float cosHalfTheta = qa.w * qb.w + qa.x * qb.x + qa.y * qb.y + qa.z * qb.z;
    // if qa=qb or qa=-qb then theta = 0 and we can return qa
    if (abs(cosHalfTheta) >= 1.0){
        qm.w = qa.w;qm.x = qa.x;qm.y = qa.y;qm.z = qa.z;
        return qm;
    }
    // Calculate temporary values.
    float halfTheta = acos(cosHalfTheta);
    float sinHalfTheta = sqrt(1.0 - cosHalfTheta*cosHalfTheta);
    // if theta = 180 degrees then result is not fully defined
    // we could rotate around any axis normal to qa or qb
    if (fabs(sinHalfTheta) < 0.001){ // fabs is floating point absolute
        qm.w = (qa.w * 0.5 + qb.w * 0.5);
        qm.x = (qa.x * 0.5 + qb.x * 0.5);
        qm.y = (qa.y * 0.5 + qb.y * 0.5);
        qm.z = (qa.z * 0.5 + qb.z * 0.5);
        return qm;
    }
    float ratioA = sin((1 - t) * halfTheta) / sinHalfTheta;
    float ratioB = sin(t * halfTheta) / sinHalfTheta; 
    //calculate Quaternion.
    qm.w = (qa.w * ratioA + qb.w * ratioB);
    qm.x = (qa.x * ratioA + qb.x * ratioB);
    qm.y = (qa.y * ratioA + qb.y * ratioB);
    qm.z = (qa.z * ratioA + qb.z * ratioB);
    return qm;
}
void VolumeRenderPanel::saveScreenShot()  
{  
    int clnWidth,clnHeight; //client width and height  
    static void * screenData;  
    RECT rc;  
    int len = 960 * 960 * 3;  
    screenData = malloc(len);  
    memset(screenData,0,len);  
    glReadPixels(0, 0, 960, 960, GL_RGB, GL_UNSIGNED_BYTE, screenData);  

    //生成文件名字符串，以时间命名  
    time_t tm = 0;  
    tm = time(NULL);  
    char lpstrFilename[256] = {0};  
    sprintf_s(lpstrFilename,sizeof(lpstrFilename),"%d.bmp",tm);  

    //WriteBitmapFile(lpstrFilename,960,960,(unsigned char*)screenData);  
    //释放内存  
    free(screenData);  
}

void VolumeRenderPanel::keyPressEvent(QKeyEvent *event){
    if(event->key()==Qt::Key_1){
        Scat = false;
        glsl.setUniform("Scat",Scat);
        cout<<"scat false"<<endl;
    }
    else if(event->key()==Qt::Key_2){
        Scat = true;
        glsl.setUniform("Scat", Scat);
        cout<<"scat true"<<endl;
    }
    updateGL();
   /* if(event->key()==Qt::Key_A){
        eyePosition[0]-=0.001;
    }else if(event->key() == Qt::Key_D){
        eyePosition[0]+=0.001;
    }else if(event->key() == Qt::Key_W){
        eyePosition[1]-=0.001;
    }else if(event->key() == Qt::Key_X){
        eyePosition[1]+=0.001;
    }else if(event->key() == Qt::Key_S && (event->modifiers() & Qt::ShiftModifier)){
        eyePosition[2]+=0.001;
    }
    else if(event->key() == Qt::Key_S){
        eyePosition[2]-=0.001;
    }else if(event->key() == Qt::Key_Return){
        resetModel();
    }
    cout<<view[3][0]<<' '<<view[3][1]<<' '<<view[3][2]<<endl;
	view = glm::mat4(1.0f);//glm::perspective(glm::radians(70.0f), (float)4 / 3, 0.3f, 100.0f);
    view[3][0] = -eyePosition[0];
    view[3][1] = -eyePosition[1];
    view[3][2] = -eyePosition[2];
    glsl.setUniform("EyePosition",eyePosition);
    glsl.setUniform("LightPosition",glm::vec4(eyePosition[0],eyePosition[1],eyePosition[2],1.0));
    glsl.setUniform("View",view);
    updateGL();*/
}
/***************************************/
/*       @author   : Li Yan            */
/*       @version  : 139               */
/*       @created  : 2016-11-22 17:08  */
/*       @modified : 2016-11-22 21:02  */
/***************************************/
#include <iostream>
#include <fstream>

/*#include "ULYBase.h"
#include "RTLYWorld.h"
#include "RTLYCamera.h"
#include "ULYMath.h"*/

//using namespace RTLY;
//using namespace ULY;
using namespace std;

int main(){
    cout<<"here0"<<endl;
    ifstream fin("manix1.bin",ios::binary);
    ofstream fout("manix.bin",ios::binary);
    short data[512];
    float data2[512];
    int mi = 999;
    int mx = -3095;
    int m[] = {3095,-1000};
    int dim[] =  {512,256,460};
    fout.write((char*)m,2*4);
    fout.write((char*)dim,3*4);
    for(int i=0;i<460;i++){
        for(int j=0;j<256;j++){
            //if(j==256)
            //cout<<i<<' '<<j<<endl;
            fin.read((char*)data,512*4);
            for(int k=0;k<512;k++){
                data2[k] = data[k];
                //if(j==256){
                //cout<<data2[k]<<"\t";
                //if(k%14==13)
                //    cout<<endl;
                if( mi > data[k])
                    mi = data[k];
                if( mx < data[k])
                    mx = data[k];
            }
            fout.write((char*)data2,dim[0]*4);
    //      if(j==256){
    //      system("pause");
    //      system("cls");
    //      }
        }
    }
        cout<<"min and max:"<<mi<<' '<<mx<<endl;
    fin.close();
    fout.close();

   /*World w; 
   w.build();
   w.camera->render(w);
   //w.renderScene();
   w.outputPPMImage("result2.ppm");*/
   return 0;
}

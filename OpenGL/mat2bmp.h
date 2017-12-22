#ifndef MAT2BMP
#define MAT2BMP

/*typedef struct {  
    DWORD  biSize;        // BITMAPINFOHEADER结构体的字节大小  
    LONG   biWidth;       //图像的宽度  
    LONG   biHeight;       //图像的高度  
    WORD   biPlanes;       //目标设备平面数目，必须为1  
    WORD   biBitCount;       //每个像素的颜色位数,可取1,4,8,16,24,32  
    DWORD  biCompression;     //压缩类型，我们主要是关注没有压缩的，即BI_RGB  
    DWORD  biSizeImage;     //图像数据的字节大小（不包括两个头部）  
    LONG   biXPelsPerMeter;   
    LONG   biYPelsPerMeter;  
    DWORD  biClrUsed;  
    DWORD  biClrImportant;  
} BITMAPINFOHEADER;*/  



//WriteBitmapFile
//根据bitmapData的（RGB）数据，保存bitmap
//filename是要保存到物理硬盘的文件名（包括路径）
/*BOOL WriteBitmapFile(char * filename,int width,int height,unsigned char * bitmapData)
{
    //填充BITMAPFILEHEADER
    BITMAPFILEHEADER bitmapFileHeader;
    memset(&bitmapFileHeader,0,sizeof(BITMAPFILEHEADER));
    bitmapFileHeader.bfSize = sizeof(BITMAPFILEHEADER);
    bitmapFileHeader.bfType = 0x4d42;	//BM
    bitmapFileHeader.bfOffBits =sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    //填充BITMAPINFOHEADER
    BITMAPINFOHEADER bitmapInfoHeader;
    memset(&bitmapInfoHeader,0,sizeof(BITMAPINFOHEADER));
    bitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapInfoHeader.biWidth = width;
    bitmapInfoHeader.biHeight = height;
    bitmapInfoHeader.biPlanes = 1;
    bitmapInfoHeader.biBitCount = 24;
    bitmapInfoHeader.biCompression = BI_RGB;
    bitmapInfoHeader.biSizeImage = width * abs(height) * 3;

    //////////////////////////////////////////////////////////////////////////
    FILE * filePtr;			//连接要保存的bitmap文件用
    unsigned char tempRGB;	//临时色素
    int imageIdx;

    //交换R、B的像素位置,bitmap的文件放置的是BGR,内存的是RGB
    for (imageIdx = 0;imageIdx < bitmapInfoHeader.biSizeImage;imageIdx +=3)
    {
        tempRGB = bitmapData[imageIdx];
        bitmapData[imageIdx] = bitmapData[imageIdx + 2];
        bitmapData[imageIdx + 2] = tempRGB;
    }

    filePtr = fopen(filename,"wb");
    if (NULL == filePtr)
    {
        return FALSE;
    }

    fwrite(&bitmapFileHeader,sizeof(BITMAPFILEHEADER),1,filePtr);

    fwrite(&bitmapInfoHeader,sizeof(BITMAPINFOHEADER),1,filePtr);

    fwrite(bitmapData,bitmapInfoHeader.biSizeImage,1,filePtr);

    fclose(filePtr);
    return TRUE;
}*/
#endif

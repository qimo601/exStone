/**
Author:
Liqun Liu (lliu329@uwo.ca)                                   
Danfeng Chen (dchen285@uwo.ca)  

The code is for research use only.
Details about the code please refer to README.txt
**/

#ifndef APPDATA_H
#define APPDATA_H

#include "Data3D.h"
#include "Image3D.h"
#include "common.h"
#include "MyBasic.h"
#include "vtkImageData.h"
#include "vtkSmartPointer.h"
#include <vector>

using namespace std;

class AppData
{
public:
    AppData();
    void loadImage(vtkImageData* image,MyBasic::Range3D imgBox);
	vtkSmartPointer<vtkImageData> getLabelMap();

    Image3D<short> image;
    Data3D<LABEL> label;
    Data3D<LABEL> seeds;
    //mask with user seeds
    Data3D<LABEL> mask;
    MyBasic::Range3D tightBox;
	MyBasic::Range3D shifttightBox;
    MyBasic::Index3D imgStart;
	MyBasic::Index3D wholeRange;
	MyBasic::Index3D star_first;
    MyBasic::Index3D star_last;
	MyBasic::Index3D shiftstar_first;
    MyBasic::Index3D shiftstar_last;
    Config cfg;


};

#endif // APPDATA_H

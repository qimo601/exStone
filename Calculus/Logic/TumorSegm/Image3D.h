/**
Author:
Liqun Liu (lliu329@uwo.ca)                                   
Danfeng Chen (dchen285@uwo.ca)  

The code is for research use only.
Details about the code please refer to README.txt
**/

#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <fstream> // For reading and saving files
#include "common.h"
#include "MyBasic.h"
#include "Data3D.h"
#include "smart_assert.h"
#include "Image2D.h"

using std::string;
using MyBasic::Range3D;
using MyBasic::Range;
using MyBasic::Index3D;

//To do------------------------------------------------------------------
// 1) Think up some usage of ROI

template<typename T>
class Image3D : public Data3D<T>
{
public:
    Image3D(int s=0,int r=0, int c=0);
    Image3D(MyBasic::Size3D _size);
    Image3D(const Data3D<T>& src);

    template<typename T2>
    Image3D(const Data3D<T2>& src);

    void load_info(const string& filename);
   // void load(const string& filename);

    void save_info(const string& filename) const;
   // void save(const string& filename) const;

    Data3D<T> dX() const;
    Data3D<T> dY() const;
    Data3D<T> dZ() const;

    Image2D<T> getSlice(int slice) const;

    inline int getWidth() const;
    inline int getHeight() const;
    inline int getLength() const;

private:
    Data3D<T> dI(DIRECTION d) const;

public:
    double distR;
    double distC;
    double distS;
};


template<typename T>
Image3D<T>::Image3D(int s, int r, int c) : Data3D<T>(s,r,c)
{
    distR = 1;
    distC = 1;
    distS = 1;
}

template<typename T>
Image3D<T>::Image3D(MyBasic::Size3D _size) : Data3D<T>(_size)
{
    distR = 1;
    distC = 1;
    distS = 1;
}

template<typename T>
Image3D<T>::Image3D(const Data3D<T>& src): Data3D<T>(src)
{
    distR = 1;
    distC = 1;
    distS = 1;
}

template<typename T> template<typename T2>
Image3D<T>::Image3D(const Data3D<T2>& src): Data3D<T>(src)
{
    distR = 1;
    distC = 1;
    distS = 1;
}

//overwrite load_info to load the distance information
template<typename T>
void Image3D<T>::load_info(const string& filename)
{
    string info_file = filename + "_info.txt";

    // open file
    ifstream fin( info_file.c_str() );
    smart_assert(fin.is_open(),"The info file is not found");

    // size of the data
    int sli,row, col;
    fin >> sli;
    fin >> row;
    fin >> col;
    fin.ignore(255, '\n');
    //spacing information
    fin >> distS;
    fin >> distR;
    fin >> distC;

    fin.close();

    this->resize(sli,row,col);
}
/*
template<typename T>
void Image3D<T>::load(const std::string& filename)
{
    //load dimension information and allocate the memory first
   // load_info(filename);
    Data3D<T>::load(filename);
}
*/
//overwrite the save_info function to save the distance information
template<typename T>
void Image3D<T>::save_info(const string& filename) const
{
    smart_assert(!filename.empty(),"No file for written");

    string info_file = filename + "_info.txt";
    FILE* pFile = fopen(info_file.c_str(),"wt");
    smart_assert(pFile,"File error");

    fprintf(pFile,"%d %d %d\n",this->getNumSli(),this->getNumRow(),this->getNumCol());
    fprintf(pFile,"%lf %lf %lf\n",distS,distR,distC);
    fclose(pFile);
}

template<typename T>
int Image3D<T>::getLength() const
{
   return this->getNumSli();
}

template<typename T>
int Image3D<T>::getWidth() const
{
    return this->getNumCol();
}

template<typename T>
int Image3D<T>::getHeight() const
{
    return this->getNumRow();
}

template<typename T>
Image2D<T> Image3D<T>::getSlice(int slice) const
{
    Image2D<T> img_slice(Data3D<T>::getSlice(slice));
    img_slice.distR = distR;
    img_slice.distC = distC;

    return img_slice;
}

template<typename T>
Data3D<T> Image3D<T>::dI(DIRECTION d) const
{
    smart_assert(d<=2, "Input direction does not work for 3D image");

    Data3D<T> gd(this->getNumSli(),this->getNumRow(),this->getNumCol());

    Range sli,row, col;
    int shift[3]= {0};
    if(d==ROW)
        {
            sli=Range(0,this->getNumSli()-1);
            row=Range(0,this->getNumRow()-1);
            col=Range(0,this->getNumCol()-2);
            shift[2] = 1;
        }
    else if(d==COLUMN)
        {
            sli=Range(0,this->getNumSli()-1);
            row = Range(0,this->getNumRow()-2);
            col = Range(0,this->getNumCol()-1);
            shift[1] = 1;
        }
    else
        {
            sli=Range(0,this->getNumSli()-2);
            row=Range(0,this->getNumRow()-1);
            col=Range(0,this->getNumCol()-1);
            shift[0] = 1;

        }
    for(int s=sli.start; s<=sli.end; s++)
    {

       for(int r=row.start; r<= row.end; r++)
        {
            for(int c = col.start; c<=col.end; c++)
                {
                    gd.at(s,r,c) = this->at(s,r,c)-this->at(s+shift[0],r+shift[1],c+shift[2]);
                }
        }
    }

    return gd;
}

template<typename T>
Data3D<T> Image3D<T>::dZ() const
{
    return dI(SLICE);
}

template<typename T>
Data3D<T> Image3D<T>::dX() const
{
    return dI(COLUMN);
}

template<typename T>
Data3D<T> Image3D<T>::dY() const
{
    return dI(ROW);
}


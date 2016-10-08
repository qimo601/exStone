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
#include "Data2D.h"
#include "smart_assert.h"

using std::string;
using MyBasic::Range2D;
using MyBasic::Range;
using MyBasic::Index2D;

//To do------------------------------------------------------------------
// 1) Think up some usage of ROI

template<typename T>
class Image2D : public Data2D<T>
{
public:
    Image2D(int r=0, int c=0);
    Image2D(const Data2D<T>& src);

    template<typename T2>
    Image2D(const Data2D<T2>& src);

    //void load(const string& filename);
    void load_info(const string& filename);
    //void save(const string& filename) const;
    void save_info(const string& filename) const;

    Data2D<T> dX() const;
    Data2D<T> dY() const;

    inline int getWidth() const;
    inline int getHeight() const;

    Image2D<T> getImageROI(const MyBasic::Range2D& r) const;

    inline double getVariance() const;

private:
    void computeVariance() const;
    Data2D<T> dI(DIRECTION d) const;

public:
    double distR;
    double distC;
private:
    mutable double variance;
};


template<typename T>
Image2D<T>::Image2D(int r, int c) : Data2D<T>(r,c)
{
    distR = 1;
    distC = 1;
    variance = -1;
}

template<typename T>
Image2D<T>::Image2D(const Data2D<T>& src): Data2D<T>(src)
{
    distR = 1;
    distC = 1;
    variance = -1;
}

template<typename T> template< typename T2>
Image2D<T>::Image2D(const Data2D<T2>& src):Data2D<T>(src)
{
    distR = 1;
    distC = 1;
    variance = -1;
}

//overwrite load_info to load the distance information
template<typename T>
void Image2D<T>::load_info(const string& filename)
{
    string info_file = filename + "_info.txt";

    // open file
    ifstream fin( info_file.c_str() );
    smart_assert(fin.is_open(),"The info file is not found");

    // size of the data
    int row, col;
    fin >> row;
    fin >> col;
    fin.ignore(255, '\n');
    //spacing information
    fin >> distR;
    fin >> distC;

    fin.close();

    this->resize(row,col);
}

//overwrite the save_info function to save the distance information
template<typename T>
void Image2D<T>::save_info(const string& filename) const
{
    smart_assert(!filename.empty(),"No file for written");

    string info_file = filename + "_info.txt";
    FILE* pFile = fopen(info_file.c_str(),"wt");
    smart_assert(pFile,"File error");

    ofstream fout( info_file.c_str() );
    fout << this->getNumRow() << " ";
    fout << this->getNumCol() << "\n";

    //write spacing information
    fout << distR << " ";
    fout << distC << " ";

    fout.close();
}

template<typename T>
int Image2D<T>::getWidth() const
{
    return this->getNumCol();
}

template<typename T>
int Image2D<T>::getHeight() const
{
    return this->getNumRow();
}


template<typename T>
inline double Image2D<T>::getVariance() const
{
    if(variance<0) this->computeVariance();

    return this->variance;
}

template<typename T>
void Image2D<T>::computeVariance() const
{
    double DEFAULT_VARIANCE =  0.0001;
    double v = 0.0;
    int total = 0;
//	MyBasic::Range2D r(0,this->getNumRow()-2,0,this->getNumCol()-2);
    MyBasic::Range row(0,this->getNumRow()-2);
    MyBasic::Range col(0,this->getNumCol()-2);

    for(int r=row.start; r<=row.end; r++)
        {
            for(int c=col.start; c<=col.end; c++)
                {
                    v=v+abs(this->at(r,c)-this->at(r+1,c))+abs(this->at(r,c)-this->at(r,c+1));
                    total=total+2;
                }
        }

    if (v/total < DEFAULT_VARIANCE ) this->variance = DEFAULT_VARIANCE;
    else this->variance = v/total;
}

template<typename T>
Data2D<T> Image2D<T>::dI(DIRECTION d) const
{
    smart_assert(d<=1, "Input direction does not work for 2D image");

    Data2D<T> gd(this->getNumRow(),this->getNumCol());

    Range row, col;
    int shift[2]= {0};
    if(d==ROW)
        {
            row=Range(0,this->getNumRow()-1);
            col=Range(0,this->getNumCol()-2);
            shift[1] = 1;
        }
    else
        {
            row = Range(0,this->getNumRow()-2);
            col = Range(0,this->getNumCol()-1);
            shift[0] = 1;
        }

    for(int r=row.start; r<= row.end; r++)
        {
            for(int c = col.start; c<=col.end; c++)
                {
                    gd.at(r,c) = this->at(r,c)-this->at(r+shift[0],c+shift[1]);
                }
        }

    return gd;
}

template<typename T>
Data2D<T> Image2D<T>::dX() const
{
    return dI(COLUMN);
}

template<typename T>
Data2D<T> Image2D<T>::dY() const
{
    return dI(ROW);
}



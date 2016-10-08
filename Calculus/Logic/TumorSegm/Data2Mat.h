/**
Author:
Liqun Liu (lliu329@uwo.ca)                                   
Danfeng Chen (dchen285@uwo.ca)  

The code is for research use only.
Details about the code please refer to README.txt
**/

#ifndef DATA2MAT_H_INCLUDED
#define DATA2MAT_H_INCLUDED

#include <opencv2/core/core.hpp>
#include "Data2D.h"
#include "MyMath.h"
// functions for converting Data2D, Data3D or c style memory to opencv showable image

//To do: add convertor to Data3D
namespace TypeCast
{

template<typename type, typename T>
Data2D<type> convert(const Data2D<T>& src);

template<typename type, typename T>
Data2D<type> convert(const Data2D<T>& src);

template<typename T>
cv::Mat toGrayscale(const T* data, unsigned int _row, unsigned int _col, bool is_normalize = true)
{
    cv::Mat result(_row,_col,CV_8UC1);
  //  result = cv::Scalar(255);
    if(is_normalize)
        {
            long long unsigned int count = _row*_col;
            double min_value = min(data,count);
            double max_value = max(data,count);
            double min_max_diff = max_value -min_value;
            //to avoid divided by 0
            if(min_max_diff==0) min_max_diff = 1;

            for(int r=0; r<_row; r++)
                {
                    for(int c=0; c<_col; c++)
                        {
                            result.at<uchar>(r,c) = 255*(data[r*_col+c]-min_value)/min_max_diff;
                        }
                }

        }
    else
        {
            for(int r=0; r<_row; r++)
                {
                    for(int c=0; c<_col; c++)
                        {
                            result.at<uchar>(r,c) = data[r*_col+c];
                        }
                }
        }

    return result;

}

//template<typename T>
//cv::Mat toGrayscale(const T* data, int _slice, int _row, int _col, bool is_normalize = true);

//convert to RGB image
template<typename T>
cv::Mat toRGB(const Data2D<T>& input_data, bool is_normalize = true);

template<typename T>
cv::Mat toRGB(const T* data, int _row, int _col, bool is_normalize = true);

template<typename T>
cv::Mat toRGB(const T* data, int _slice, int _row, int _col, bool is_normalize = true);

//convert to grayscale image
template<typename T>
cv::Mat toGrayscale(const Data2D<T>& input_data, bool is_normalize = true)
{
    return toGrayscale(input_data.getData(),input_data.getNumRow(),input_data.getNumCol(),is_normalize);
}

template<typename T>
cv::Mat toGrayscale(const Data3D<T>& input_data, bool is_normalize = true)
{
    int nRow = input_data.getNumRow();
    int nCol = input_data.getNumCol();
    int nSli = input_data.getNumSli();
    int mat_nRow = nRow;
    int mat_nCol = nSli*2-1 + nSli*nCol;

    Data2D<T> tmp(mat_nRow,mat_nCol,input_data.getMax());
    MyBasic::Range2D rsrc(0,nRow-1,0,nCol-1);
    MyBasic::Range2D rdst(0,nRow-1,0,nCol-1);
    MyBasic::Index2D p(0,nCol+2);

    for(int s=0;s<nSli;s++)
    {
       tmp.copyFrom(input_data.getSlice(s),rsrc,rdst);
       rdst += p;
    }

    return toGrayscale(tmp,is_normalize);
}

template<typename T>
cv::Mat toRGB(const Data2D<T>& input_data, bool is_normalize)
{
    return toRGB(input_data.getData(),input_data.getNumRow(),input_data.getNumCol(),is_normalize);
}

template<typename T>
cv::Mat toRGB(const T* data, int _row, int _col, bool is_normalize)
{
    cv::Mat result(_row,_col,CV_8UC3);
  //  result = cv::Scalar(255);
    if(is_normalize)
        {
            long long unsigned int count = _row*_col;
            double min_value = min(data,count);
            double max_value = max(data,count);
            double min_max_diff = max_value -min_value;
            //to avoid divided by 0
            if(min_max_diff==0) min_max_diff = 1;

            for(int r=0; r<_row; r++)
                {
                    for(int c=0; c<_col; c++)
                        {
                            uchar pixel_value = 255*(data[r*_col+c]-min_value)/min_max_diff;
                            result.at<uchar>(r,c) = cv::Vec3b(pixel_value,pixel_value,pixel_value);
                        }
                }

        }
    else
        {
            for(int r=0; r<_row; r++)
                {
                    for(int c=0; c<_col; c++)
                        {
                            uchar pixel_value = data[r*_col+c];
                            result.at<uchar>(r,c) = cv::Vec3b(pixel_value,pixel_value,pixel_value);
                        }
                }
        }

    return result;
}

}

#endif // DATA2MAT_H_INCLUDED

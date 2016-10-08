/**
Author:
Liqun Liu (lliu329@uwo.ca)                                   
Danfeng Chen (dchen285@uwo.ca)  

The code is for research use only.
Details about the code please refer to README.txt
**/

#ifndef IMAGEPROCESSING_H_INCLUDED
#define IMAGEPROCESSING_H_INCLUDED

#include "Data2D.h"
#include "MyMath.h"
namespace ImageProcessing
{


//-----------------------------------------------------------------------------------//
//                  declarations
//-----------------------------------------------------------------------------------//

////////////////////////////////////////////////////////////
//    range normalization
//data: to be normalized matrix
//lrb:  lower range boundary
//hrb:  higher range boundary
////////////////////////////////////////////////////////////
template<typename T>
void normalize(Data2D<T>& data, T lrb, T hrb);
template<typename T>
void normalize(Data3D<T>& data, T lrb, T hrb);
template<typename indexType, typename intensityType>
Data2D<intensityType> index2intensity(const Data2D<indexType>& index,const vector<intensityType>& map);
vector<double> hist(const Data2D<int>& bin_id, const Data2D<LABEL>& mask, int bin_num, int indicator = 1);
vector<double> hist(const Data3D<int>& bin_id, const Data3D<LABEL>& mask, int bin_num, int indicator = 1);
////////////////////////////////////////////////////////////
//           translation mode
//KEEP: keep the whole image
//crop: crop the part of image which has been shifted out
////////////////////////////////////////////////////////////
enum TRANSLATE_MODE
{
    FILL = 0, CROP
};
//translate the image, pa
template<typename T>
Data2D<T> translate(const Data2D<T>& _img, const MyBasic::Index2D& t, TRANSLATE_MODE);
//pad the image with 4 band size
template<typename T>
Data2D<T> pad(const Data2D<T>& _img,unsigned int left, unsigned int right,
                                    unsigned int top, unsigned int bottom);

//************************************************************************************//
//************************* image analysis functions *********************************//
//************************************************************************************//

//total = bwarea(BW) estimates the area of the objects in binary image BW.
//total is a scalar whose value corresponds roughly to the total number of
//on pixels in the image, but might not be exactly the same because different
// patterns of pixels are weighted differently.
template<typename T>
unsigned int  bwarea(const Data2D<T>& bw);
template<typename T>
unsigned int perimeter(const Data2D<T>& bw);
template<typename T>
unsigned int perimeter(const Data2D<T>& bw, uchar connectivity);
template<typename T>
MyBasic::Index2D centroid(const Data2D<T>& bw);
/*
template<typename T>
std::vector<MyBasic::Index2D> bwperim(const Data2D<T>& bw,uchar connectivity);
template<typename T>
std::vector<MyBasic::Index3D> bwperim(const Data3D<T>& bw,uchar connectivity);
*/

template<typename T>
Data2D<bool> bwperim(const Data2D<T>& bw,uchar connectivity=4);
template<typename T>
Data2D<bool> bwperim4(const Data2D<T>& bw);
template<typename T>
Data2D<bool> bwperim8(const Data2D<T>& bw);
template<typename T>
Data3D<bool> bwperim(const Data3D<T>& bw,uchar connectivity=4);
template<typename T>
Data3D<bool> bwperim4(const Data3D<T>& bw);
template<typename T>
Data3D<bool> bwperim6(const Data3D<T>& bw);
template<typename T>
Data3D<bool> bwperim8(const Data3D<T>& bw);
template<typename T>
Data3D<bool> bwperim18(const Data3D<T>& bw);
template<typename T>
Data3D<bool> bwperim26(const Data3D<T>& bw);

template<typename T>
vector<MyBasic::Index2D> boundaryPoints(const Data2D<T>& bw, uchar connectivity);
template<typename T>
double symetricness(const Data2D<T>& bw, const MyBasic::Index2D& center);
template<typename T>
double distanceStd2Center(const Data2D<T>& bw, const MyBasic::Index2D& center,uchar connectivity=4);

}; //declaration of namespace ImageProcessing ends here

namespace IP = ImageProcessing;

//---------------------------------------------------------------------------------------------//
//                                     implementations
//--------------------------------------------------------------------------------------------//
template<typename T>
void ImageProcessing::normalize(Data2D<T>& data, T lrb, T hrb)
{
   T vmin = min(data);
   T vmax = max(data);
   T irange = vmax - vmin; //input range
   //avoid divided by 0
   if(irange==0) irange = 1;
   T orange = hrb-lrb;  //output range

    for(int i=0; i<data.getNumData(); i++)
        {
            data.at(i)= orange*(data.at(i)-vmin)/irange + lrb ;
        }
}

template<typename T>
void ImageProcessing::normalize(Data3D<T>& data, T lrb, T hrb)
{
   T vmin = min(data);
   T vmax = max(data);
   T irange = vmax - vmin; //input range
   //avoid divided by 0
   if(irange==0) irange = 1;
   T orange = hrb-lrb;  //output range

    for(int i=0; i<data.getNumData(); i++)
        {
            data.at(i)= orange*(data.at(i)-vmin)/irange + lrb ;
        }
}

template<typename indexType, typename intensityType>
Data2D<intensityType> ImageProcessing::index2intensity(const Data2D<indexType>& index,const vector<intensityType>& map)
{
    Data2D<intensityType> result(index.getSize());
    for(int r=0;r<index.getNumRow();r++)
    {
       for(int c=0;c<index.getNumCol();c++)
       {
          result.at(r,c) = map[index.at(r,c)];
       }
    }

    return result;
}

/*
//translate the image, pa
template<typename T>
Data2D<T> ImageProcessing::translate(const Data2D<T>& _img, const MyBasic::Index2D& t, TRANSLATE_MODE)
{
    switch(TRANSLATE_MODE)
        {
        case FILL:
            unsigned int left, right, top, bottom;
            left = t.col>0? t.col : 0;
            right = t.col>0? 0 : t.col;
            top = t.row>0? t.row : 0;
            bottom = t.row>0? 0 : t.row;
            return pad(_img,left,right,top,bottom);
        case CROP:
            MyBasic::Range2D roi;
            MyBasic::Size2D size_new = _img.getSize() - abs(t);
            roi.row.start = t.row>0? 0 : -t.row;
            roi.row.end = t.row>0? size_new.row-1-t.row : size_new.row-1;
            roi.col.start = t.col>0? 0 : -t.col;
            roi.col.end = t.col>0? size_new.col-1-t.col : size_new.col-1;
            return translate(_img(roi),t,FILL);
        }
}
*/
//pad the image with 4 band size

template<typename T>
Data2D<T> ImageProcessing::pad(const Data2D<T>& _img,unsigned int left,
              unsigned int right, unsigned int top, unsigned int bottom)
{
    unsigned int width = _img.getNumCol();
    unsigned int height = _img.getNumRow();
    unsigned int owidth = width + left + right;
    unsigned int oheight = height + top + bottom;

    Data2D<T> result(oheight,owidth);
    result.copyFrom(_img,_img.getRange(),MyBasic::Range2D(top,top+height-1,left,left+width-1));

    return result;
}

template<typename T>
unsigned int ImageProcessing::bwarea(const Data2D<T>& bw)
{
    int nRow = bw.getNumRow();
    int nCol = bw.getNumCol();

    unsigned int  result=0;
    for(int r=0; r<nRow; r++)
        {
            for(int c=0; c<nCol; c++)
                {
                    if(bw.at(r,c)!=0) result +=1;
                }
        }

    return result;
}


template<typename T>
unsigned int  ImageProcessing::perimeter(const Data2D<T>& bw, uchar connectivity)
{
    return bwarea(bwperim(bw,connectivity));
}

template<typename T>
MyBasic::Index2D ImageProcessing::centroid(const Data2D<T>& bw)
{
     long long  ct_r=0, ct_c=0;
     long long  count=0;
     for(int r=0;r<bw.getNumRow();r++)
     {
        for(int c=0;c<bw.getNumCol();c++)
        {
            if(bw.at(r,c))
            {
               ct_r += r;
               ct_c += c;
               count++;
            }
        }
    }
    //if no pixels of object, return the image center
    if(count==0) return bw.getSize()/2;
    else return MyBasic::Index2D(ct_r/count,ct_c/count);
}

//connectivity: 4, 8
template<typename T>
Data2D<bool> ImageProcessing::bwperim(const Data2D<T>& bw,uchar connectivity)
{
     switch(connectivity)
     {
        case 4:
        return bwperim4(bw);
        case 8:
        return bwperim8(bw);
     }
}

template<typename T>
Data2D<bool> ImageProcessing::bwperim4(const Data2D<T>& bw)
{
     Data2D<bool> result(bw.getSize());

     for(int r=0;r<bw.getNumRow();r++)
     {
        for(int c=0;c<bw.getNumCol();c++)
        {
           if(bw.at(r,c)==0) continue;
           if( (bw.inRange(r,c+1)&&bw.at(r,c+1)==0) ||
               (bw.inRange(r,c-1)&&bw.at(r,c-1)==0) ||
               (bw.inRange(r+1,c)&&bw.at(r+1,c)==0) ||
               (bw.inRange(r-1,c)&&bw.at(r-1,c)==0) )
               {
                  result.at(r,c)=true;
               }
        }

     }

     return result;
}

template<typename T>
Data2D<bool> ImageProcessing::bwperim8(const Data2D<T>& bw)
{
     Data2D<bool> result(bw.getSize());

     for(int r=0;r<bw.getNumRow();r++)
     {
        for(int c=0;c<bw.getNumCol();c++)
        {
           if(bw.at(r,c)==0) continue;
           if( (bw.inRange(r,c+1)&&bw.at(r,c+1)==0) ||
               (bw.inRange(r,c-1)&&bw.at(r,c-1)==0) ||
               (bw.inRange(r+1,c)&&bw.at(r+1,c)==0) ||
               (bw.inRange(r-1,c)&&bw.at(r-1,c)==0) ||
               (bw.inRange(r+1,c+1)&&bw.at(r+1,c+1)==0) ||
               (bw.inRange(r-1,c-1)&&bw.at(r-1,c-1)==0) ||
               (bw.inRange(r+1,c-1)&&bw.at(r+1,c-1)==0) ||
               (bw.inRange(r-1,c+1)&&bw.at(r-1,c+1)==0) )
               {
                  result.at(r,c)=true;
               }
        }

     }

     return result;
}

template<typename T>
Data3D<bool> ImageProcessing::bwperim(const Data3D<T>& bw,uchar connectivity)
{
     switch(connectivity)
     {
        case 4:
        return bwperim4(bw);
        case 6:
        return bwperim6(bw);
        case 8:
        return bwperim8(bw);
        case 18:
        return bwperim18(bw);
        case 26:
        return bwperim26(bw);
     }
}

//2D connectivity on each slice, no 3D connectivity
template<typename T>
Data3D<bool> ImageProcessing::bwperim4(const Data3D<T>& bw)
{
    Data3D<bool> result(bw.getSize());
    for(int s=0;s<bw.getNumSli();s++)
    {
       result.setSlice(s,bwperim4(bw.getSlice(s)));
    }
    return result;
}

template<typename T>
Data3D<bool> ImageProcessing::bwperim6(const Data3D<T>& bw)
{
    Data3D<bool> result(bw.getSize());

    for(int s=0;s<bw.getNumSli();s++)
    {
       for(int r=0;r<bw.getNumRow();r++)
       {
          for(int c=0;c<bw.getNumCol();c++)
          {
            if(bw.at(s,r,c)==0) continue;
            if( (bw.inRange(s,r,c+1)&&bw.at(s,r,c+1)==0) ||
                (bw.inRange(s,r,c-1)&&bw.at(s,r,c-1)==0) ||
                (bw.inRange(s,r+1,c)&&bw.at(s,r+1,c)==0) ||
                (bw.inRange(s,r-1,c)&&bw.at(s,r-1,c)==0) ||
                (bw.inRange(s-1,r,c)&&bw.at(s-1,r,c)==0) ||
                (bw.inRange(s+1,r,c)&&bw.at(s+1,r,c)==0) )
                {
                  result.at(s,r,c) = true;
                }
          }
       }
    }

    return result;
}

////2D connectivity on each slice, no 3D connectivity
template<typename T>
Data3D<bool> ImageProcessing::bwperim8(const Data3D<T>& bw)
{
    Data3D<bool> result(bw.getSize());
    for(int s=0;s<bw.getNumSli();s++)
    {
       result.setSlice(s,bwperim8(bw.getSlice(s)));
    }
    return result;
}


template<typename T>
Data3D<bool> ImageProcessing::bwperim18(const Data3D<T>& bw)
{
    smart_assert(false,"please implement bwperim18 in ImageProcessing.h");
    Data3D<bool> result;
    return result;
}

template<typename T>
Data3D<bool> bwperim26(const Data3D<T>& bw)
{
    smart_assert(false,"please implement bwperim26 in ImageProcessing.h");
    Data3D<bool> result;
    return result;
}

//used for compactness measure, can be removed from image processing
//if it's more appropriate
template<typename T>
double ImageProcessing::symetricness(const Data2D<T>& bw, const MyBasic::Index2D& center)
{
     Data2D<bool> rot_bw(bw.getSize());
     for(int r=0;r<bw.getNumRow();r++)
     {
        for(int c=0;c<bw.getNumCol();c++)
        {
           if(bw.at(r,c)==0) continue;
           MyBasic::Index2D cur_point(r,c);
           MyBasic::Index2D rot_point;
           rot_point = center;
           rot_point +=center;
           rot_point -= cur_point;
         //  rot_point = center + (center - cur_point);
           if(bw.inRange(rot_point)) rot_bw.at(rot_point) = 1;
        }
     }

     long long intersection =  bwarea(rot_bw&bw);
     double result = double(intersection)/bwarea(bw);

     return result;
}

template<typename T>
vector<MyBasic::Index2D> ImageProcessing::boundaryPoints(const Data2D<T>& bw, uchar connectivity)
{
    vector<MyBasic::Index2D> bdpoint;
    Data2D<bool> perim = bwperim(bw,connectivity);
    for(int r=0;r<perim.getNumRow();r++)
    {
       for(int c=0;c<perim.getNumCol();c++)
       {
          if(perim.at(r,c)) bdpoint.push_back(MyBasic::Index2D(r,c));
       }
    }

    return bdpoint;
}

template<typename T>
double ImageProcessing::distanceStd2Center(const Data2D<T>& bw, const MyBasic::Index2D& center, uchar connectivity)
{
     vector<MyBasic::Index2D> bdpoint = boundaryPoints(bw,connectivity);
     bdpoint = bdpoint - center;
     vector<double> dist(bdpoint.size());
     for(int i=0;i<bdpoint.size();i++)
     {
        dist[i] = bdpoint[i].norm();
     }

     return standardDeviation<double>(dist);
}


#endif // IMAGEPROCESSING_H_INCLUDED

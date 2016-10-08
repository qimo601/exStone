/**
Author:
Liqun Liu (lliu329@uwo.ca)                                   
Danfeng Chen (dchen285@uwo.ca)  

The code is for research use only.
Details about the code please refer to README.txt
**/

#ifndef SMOOTHTERM_H_INCLUDED
#define SMOOTHTERM_H_INCLUDED

#include "common.h"
#include "Data2D.h"
#include <math.h>
#include <vector>
using namespace std;

namespace lliu
{
double computeNLinkCost(double diff,double variance);

template<typename T>
pair<Data2D<double>, Data2D<double> > ComputeSmoothCostWhiteToDark(Data2D<T>& image,double dist,double variance, DIRECTION dir)
{
    pair<Data2D<double>, Data2D<double> > sm;
    sm.first = Data2D<double>(image.getSize());
    sm.second = Data2D<double>(image.getSize());

    MyBasic::Range2D roi;
    MyBasic::Index2D shift;
    switch(dir)
        {
        case ROW:
            roi = Range2D(0,image.getNumRow()-1,0,image.getNumCol()-2);
            shift = MyBasic::Index2D(0,1);
            break;
        case COLUMN:
            roi = Range2D(0,image.getNumRow()-2,0,image.getNumCol()-1);
            shift =  MyBasic::Index2D(1,0);
            break;
        }

    double default_weight = 1/dist;

    for(int r=roi.start.row; r<=roi.end.row; r++)
        {
            for(int c=roi.start.col; c<=roi.end.col; c++)
                {
                    MyBasic::Index2D point1(r,c);
                    MyBasic::Index2D point2 = point1+shift;

                    double diff = image.at(point1) - image.at(point2);
                    double w = computeNLinkCost(diff,variance);

                    if(diff>0)
                        {
                            sm.first.at(point1) = w/dist;
                            sm.second.at(point2) = default_weight;
                        }
                    else
                        {
                            sm.first.at(point1) = default_weight;
                            sm.second.at(point2) = w/dist;
                        }
                }
        }

    return sm;
}
};

#endif // SMOOTHTERM_H_INCLUDED

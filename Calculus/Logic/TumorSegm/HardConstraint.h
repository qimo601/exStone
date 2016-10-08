/**
Author:
Liqun Liu (lliu329@uwo.ca)                                   
Danfeng Chen (dchen285@uwo.ca)  

The code is for research use only.
Details about the code please refer to README.txt
**/

#ifndef HARDCONSTRAINT_H_INCLUDED
#define HARDCONSTRAINT_H_INCLUDED

#include "MyBasic.h"
#include <vector>
using std::vector;

namespace lliu
{
using namespace MyBasic;
vector<MyBasic::Index2D> getPointsWithinCircle(const vector<Index2D>& seeds, int radius);
vector<MyBasic::Index2D> getPointsWithinCircle(const vector<Index3D>& seeds, int radius);


vector<MyBasic::Index2D> getBorderPoints(const Size2D& ImgSize, int bandWidth);
vector<MyBasic::Index2D> getBorderPoints(const Size3D& ImgSize, int bandWidth);


};

#endif // HARDCONSTRAINT_H_INCLUDED

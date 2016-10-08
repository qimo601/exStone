/**
Author:
Liqun Liu (lliu329@uwo.ca)                                   
Danfeng Chen (dchen285@uwo.ca)  

The code is for research use only.
Details about the code please refer to README.txt
**/

#ifndef STARSHAPE_H_INCLUDED
#define STARSHAPE_H_INCLUDED

#include "GraphLinkCost.h"


SparseNLink starshape2D(const MyBasic::Size2D& dim, const MyBasic::Index2D& seed);
SparseNLink starshape3D(const MyBasic::Size3D& dim, const MyBasic::Index3D& _seed);


#endif // STARSHAPE_H_INCLUDED

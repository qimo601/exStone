/**
Author:
Liqun Liu (lliu329@uwo.ca)                                   
Danfeng Chen (dchen285@uwo.ca)  

The code is for research use only.
Details about the code please refer to README.txt
**/

#pragma once
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include <string.h>

typedef unsigned int uint32;
typedef int node_id;
typedef unsigned short uint16;
typedef unsigned char uchar;

const double HDHUGE=10000.0;
const double STAR_HUGE=HDHUGE/2;

enum DIRECTION
{
    ROW=0,COLUMN,SLICE
};

enum LABEL
{
    BACKGROUND=0,FOREGROUND,UNKNOWN
};

struct Config
{
    Config& operator =(const Config& cfg)
    {
        memcpy(this,&cfg,sizeof(Config));
        return *this;
    }
    bool starshape2;
    bool starshape3;
    bool dataterm;
    bool smoothterm;
    bool hardConstraint;
    bool white_to_dark;
    int bin_num;
    int min_area;
    int max_area;
    int seed_radius;
};



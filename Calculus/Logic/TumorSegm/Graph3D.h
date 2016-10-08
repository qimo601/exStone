/**
Author:
Liqun Liu (lliu329@uwo.ca)                                   
Danfeng Chen (dchen285@uwo.ca)  

The code is for research use only.
Details about the code please refer to README.txt
**/

#ifndef GRAPH3D_H_INCLUDED
#define GRAPH3D_H_INCLUDED

#pragma once

#include "BK/graph.h"
#include "common.h"
#include "MyBasic.h"
#include "Data3D.h"
#include "MyMath.h"
#include <vector>
using std::vector;

class Graph3D : public Graph<double,double,double>
{

public:
    Graph3D(int _sli,int _row, int _col);
    Graph3D(MyBasic::Size3D _size);
    virtual ~Graph3D();

    void addSmoothCost(const Data3D<double>& cap,const Data3D<double>& rev_cap, DIRECTION d);
    void addSmoothCost(const Data3D<double>& cap,const Data3D<double>& rev_cap, const MyBasic::Index3D& shift);
    //sparse smooth cost
    void addSmoothCost(const vector<int>& id1, const vector<int>& id2,const vector<double>& cap,const vector<double>& rev_cap);

    void addDataCost(const vector<int>& id,const vector<double>& cost0, const vector<double>& cost1);
    void addDataCost(const Data3D<double>& datacost, LABEL label);


    Data3D<LABEL> get_labeling();
    LABEL what_segment(const Index3D& i);
    double maxflow();

    inline int getNumSli() const;
    inline int getNumRow() const ;
    inline int getNumCol() const ;
    inline MyBasic::Size3D getSize() const ;

private:
    inline int Index2Id(const MyBasic::Index3D& i) const;
    inline int Index2Id(int s,int r, int c) const;
private:
    int nSli;
    int nRow;
    int nCol;
    double flow;
    bool done;
};


int Graph3D::Index2Id(const Index3D& i) const
{
    return Index2Id(i.sli,i.row,i.col);
}

int Graph3D::Index2Id(int s,int r, int c) const
{
    return s*nRow*nCol+r*nCol + c;
}

int Graph3D:: getNumSli() const
{
    return nSli;
}

int Graph3D:: getNumRow() const
{
    return nRow;
}


int Graph3D:: getNumCol() const
{
    return nCol;
}

MyBasic::Size3D Graph3D::getSize() const
{
    return MyBasic::Size3D(nSli,nRow,nCol);
}







#endif // GRAPH3D_H_INCLUDED

/**
Author:
Liqun Liu (lliu329@uwo.ca)                                   
Danfeng Chen (dchen285@uwo.ca)  

The code is for research use only.
Details about the code please refer to README.txt
**/

#pragma once

#include "BK/graph.h"
#include "common.h"
#include "MyBasic.h"
#include "Data2D.h"
#include "MyMath.h"
#include <vector>
using std::vector;

class Graph2D : public Graph<double,double,double>
{
    /*
    	using Graph<double,double,double>::add_edge;
    	using Graph<double,double,double>::add_tweights;
        using Graph<double,double,double>::what_segment;
    */
public:
    Graph2D(int _row, int _col);
    virtual ~Graph2D();

 //   inline void add_edge(const int& i, const int& j, const double& cap, const double& rev_cap);
  //  inline void add_tweights(const int& i, const double& fgcost, const double& bkcost);

    void addSmoothCost(const Data2D<double>& cap,const Data2D<double>& rev_cap, DIRECTION d);
    void addSmoothCost(const Data2D<double>& cap,const Data2D<double>& rev_cap, const MyBasic::Index2D& shift);
    //sparse smooth cost
    void addSmoothCost(const vector<int>& id1, const vector<int>& id2,const vector<double>& cap,const vector<double>& rev_cap);

    void addDataCost(const vector<int>& id,const vector<double>& cost0, const vector<double>& cost1);
    void addDataCost(const Data2D<double>& datacost, LABEL label);


    Data2D<LABEL> get_labeling();
    LABEL what_segment(const Index2D& i);
    double maxflow();

    inline int getNumRow() const ;
    inline int getNumCol() const ;
    inline MyBasic::Size2D getSize() const ;

private:
    inline int Index2Id(const MyBasic::Index2D& i) const;
    inline int Index2Id(int r, int c) const;
private:
    int nRow;
    int nCol;
    double flow;
    bool done;
};


int Graph2D::Index2Id(const Index2D& i) const
{
    return Index2Id(i.row,i.col);
}

int Graph2D::Index2Id(int r, int c) const
{
    return r*nCol + c;
}

int Graph2D:: getNumRow() const
{
    return nRow;
}


int Graph2D:: getNumCol() const
{
    return nCol;
}

MyBasic::Size2D Graph2D::getSize() const
{
    return MyBasic::Size2D(nRow,nCol);
}






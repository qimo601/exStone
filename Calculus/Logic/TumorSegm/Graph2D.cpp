#include "Graph2D.h"
#include "BK/graph.h"

//implementation of non-inline functions of class Graph2D
Graph2D::Graph2D(int _row, int _col):Graph<double,double,double>(_row*_col,8*_row*_col),nRow(_row),nCol(_col)
{
    Graph<double,double,double>::add_node(_row*_col);
    done = false;
}

Graph2D::~Graph2D()
{

}

//add smooth cost with direction
void Graph2D::addSmoothCost(const Data2D<double>& cap, const Data2D<double>& rev_cap, DIRECTION d)
{
    switch(d)
        {
        case ROW:
            this->addSmoothCost(cap,rev_cap,MyBasic::Index2D(0,1));
            break;
        case COLUMN:
            this->addSmoothCost(cap,rev_cap,MyBasic::Index2D(1,0));
            break;
        case UNKNOWN:
            break;
        }
}


//add regular smooth cost
void Graph2D::addSmoothCost(const Data2D<double>& cap,const Data2D<double>& rev_cap, const Index2D& shift)
{
    Range2D roi(0,nRow-1-shift.row, 0, nCol-1-shift.col);

    for(int r=roi.start.row; r<=roi.end.row; r++)
        {
            for(int c=roi.start.col; c<=roi.end.col; c++)
                {
                    MyBasic::Index2D point1(r,c);
                    MyBasic::Index2D point2 = point1 + shift;
                    double vcap = cap.at(point1);
                    double vrev_cap = rev_cap.at(point2);
                    add_edge(Index2Id(point1),Index2Id(point2),cap.at(point1),rev_cap.at(point2));
                }
        }
}

//add sparse smooth cost
void Graph2D::addSmoothCost(const vector<int>& id1, const vector<int>& id2,const vector<double>& cap,const vector<double>& rev_cap)
{
    int num = id1.size();
    for(int i=0; i<num; i++)
        {
            add_edge(id1[i], id2[i], cap[i], rev_cap[i]);
        }
}

//add sparse data cost
void Graph2D::addDataCost(const vector<int>& id,const vector<double>& fgcost, const vector<double>& bkcost)
{
    int num = id.size();
    for(int i=0; i<num; i++)
        {
            add_tweights(id[i], bkcost[i],fgcost[i]);
        }
}

//add regular(grid) data cost
void Graph2D::addDataCost(const Data2D<double>& datacost, LABEL label)
{
    for(int r=0; r<nRow; r++)
        {
            for(int c=0; c<nCol; c++)
                {
                    MyBasic::Index2D point(r,c);
                    switch(label)
                        {
                        case FOREGROUND:
                            add_tweights(Index2Id(point),0,datacost.at(point));
                            break;
                        case BACKGROUND:
                            add_tweights(Index2Id(point),datacost.at(point),0);
                            break;
                        case UNKNOWN:
                            break;
                        }
                }
        }
}


Data2D<LABEL> Graph2D::get_labeling()// const
{
    if(!done) return Data2D<LABEL>();
    Data2D<LABEL> label(nRow,nCol);
    for(int r=0; r<nRow; r++)
        {
            for(int c=0; c<nCol; c++)
                {
                    label.at(r,c) = what_segment(Index2D(r,c));
                }
        }
    return label;
}

LABEL Graph2D::what_segment(const Index2D& i)// const
{
    if(Graph<double,double,double>::what_segment(Index2Id(i))==Graph<double,double,double>::SOURCE) return FOREGROUND;
    else return BACKGROUND;
}



double Graph2D::maxflow()
{
    flow = Graph<double,double,double>::maxflow();
    done = true;
    return flow;
}


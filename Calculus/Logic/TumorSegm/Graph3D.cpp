#include "Graph3D.h"
#include "BK/graph.h"

//implementation of non-inline functions of class Graph3D
Graph3D::Graph3D(int _sli,int _row, int _col):Graph<double,double,double>(_sli*_row*_col,8*_sli*_row*_col),nSli(_sli),nRow(_row),nCol(_col)
{
    Graph<double,double,double>::add_node(_sli*_row*_col);
    done = false;
}

Graph3D::Graph3D(MyBasic::Size3D _size)
:Graph<double,double,double>(_size.nSli*_size.nRow*_size.nCol,8*_size.nSli*_size.nRow*_size.nCol),
nSli(_size.nSli),nRow(_size.nRow),nCol(_size.nCol)
{
    Graph<double,double,double>::add_node(_size.nSli*_size.nRow*_size.nCol);
    done = false;
}

Graph3D::~Graph3D()
{

}

//add smooth cost with direction
void Graph3D::addSmoothCost(const Data3D<double>& cap, const Data3D<double>& rev_cap, DIRECTION d)
{
    switch(d)
        {
        case SLICE:
            this->addSmoothCost(cap,rev_cap,MyBasic::Index3D(1,0,0));
            break;
        case ROW:
            this->addSmoothCost(cap,rev_cap,MyBasic::Index3D(0,0,1));
            break;
        case COLUMN:
            this->addSmoothCost(cap,rev_cap,MyBasic::Index3D(0,1,0));
            break;
        }
}


//add regular smooth cost
void Graph3D::addSmoothCost(const Data3D<double>& cap,const Data3D<double>& rev_cap, const Index3D& shift)
{
    Range row(0,nRow-1-shift.row);
    Range col(0,nCol-1-shift.col);
    Range sli(0,nSli-1-shift.sli);
    for(int s=sli.start; s<=sli.end; s++)
    {

     for(int r=row.start; r<=row.end; r++)
        {
            for(int c=col.start; c<=col.end; c++)
                {
                    MyBasic::Index3D point1(s,r,c);
                    MyBasic::Index3D point2 = point1 + shift;

                    add_edge(Index2Id(point1),Index2Id(point2),cap.at(point1),rev_cap.at(point2));
                }
        }
    }
}

//add sparse smooth cost
void Graph3D::addSmoothCost(const vector<int>& id1, const vector<int>& id2,const vector<double>& cap,const vector<double>& rev_cap)
{
    int num = id1.size();
	cout<<"Begin addSmoothCost!"<<endl;
    for(int i=0; i<num; i++)
        {
//			cout<<i<<endl;
            add_edge(id1[i], id2[i], cap[i], rev_cap[i]);
        }
	cout<<"Done addSmoothCost!"<<endl;
}

//add sparse data cost
void Graph3D::addDataCost(const vector<int>& id,const vector<double>& fgcost, const vector<double>& bkcost)
{
    int num = id.size();
    for(int i=0; i<num; i++)
        {
            add_tweights(id[i],bkcost[i],fgcost[i]);
        }
}

//add regular(grid) data cost
void Graph3D::addDataCost(const Data3D<double>& datacost, LABEL label)
{
    for(int s=0;s<nSli; s++)
    {

    for(int r=0; r<nRow; r++)
        {
            for(int c=0; c<nCol; c++)
                {
                    MyBasic::Index3D point(s,r,c);
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
}


Data3D<LABEL> Graph3D::get_labeling()// const
{
    if(!done) return Data3D<LABEL>();
    Data3D<LABEL> label(nSli,nRow,nCol);
    for(int s=0; s<nSli; s++)
    {

    for(int r=0; r<nRow; r++)
        {
            for(int c=0; c<nCol; c++)
                {
                    label.at(s,r,c) = what_segment(Index3D(s,r,c));
                }
        }
    }
    return label;
}

LABEL Graph3D::what_segment(const Index3D& i)// const
{
    if(Graph<double,double,double>::what_segment(Index2Id(i))==Graph<double,double,double>::SOURCE) return FOREGROUND;
    else return BACKGROUND;
}



double Graph3D::maxflow()
{
    flow = Graph<double,double,double>::maxflow();
    done = true;
    return flow;
}


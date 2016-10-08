#include "HardConstraint.h"
#include "Data2D.h"

namespace lliu
{
vector<MyBasic::Index2D> getPointsWithinCircle(const vector<Index2D>& seeds,int radius)
{

    int diameter=1+2*radius;
    Data2D<int> circleMask(diameter,diameter);
    int ct[2]= {radius,radius};

    //generate the circular kernel
    for(int i=0; i<diameter; i++)
        {
            for(int j=0; j<diameter; j++)
                {
                    if((i-ct[0])*(i-ct[0])+(j-ct[1])*(j-ct[1])<=radius*radius)
                        {
                            circleMask.at(i,j)=1;
                        }
                }
        }

    // set mask near seeds to value mask_val
    vector<MyBasic::Index2D> points;

    for(int index_seeds=0; index_seeds<seeds.size(); index_seeds++)
        {
            int ctr=seeds[index_seeds].row;
            int ctc=seeds[index_seeds].col;
            for(int i=-radius; i<=radius; i++)
                {
                    for(int j=-radius; j<=radius; j++)
                        {
                            if(circleMask.at(radius+i,radius+j)==1)
                                {
                                    int r=ctr+i;
                                    int c=ctc+j;

                                    points.push_back(MyBasic::Index2D(r,c));
                                }
                        }
                }
        }

    return points;
}

vector<MyBasic::Index2D> getPointsWithinCircle(const vector<Index3D>& seeds, int radius)
{
    //To do
    return vector<MyBasic::Index2D>();
}

vector<MyBasic::Index2D> getBorderPoints(const Size2D& imgSize, int bandWidth)
{
    //use the code of leetcode problem spiral matrix
    int cur_row = imgSize.nRow;
    int cur_col = imgSize.nCol;
    int step = 1;
    int r=0, c=-1; //current position
    vector<MyBasic::Index2D> points;

    while(cur_row!=imgSize.nRow-bandWidth && cur_col!=imgSize.nCol-bandWidth)
        {
            //walk a row
            for(int n=0; n<cur_col; n++)
                {
                    c = c+step;
                    points.push_back(MyBasic::Index2D(r,c));
                }
            cur_row--;

            //walk a col
            for(int n=0; n<cur_row; n++)
                {
                    r = r+step;
                    points.push_back(MyBasic::Index2D(r,c));
                }
            cur_col--;

            step = -step;
        }

    return points;

}

}


/**
Author:
Liqun Liu (lliu329@uwo.ca)                                   
Danfeng Chen (dchen285@uwo.ca)  

The code is for research use only.
Details about the code please refer to README.txt
**/

#ifndef ADAPTIVESEGMENT2D_H_INCLUDED
#define ADAPTIVESEGMENT2D_H_INCLUDED

#include "Graph2D.h"
#include "Image2D.h"
#include "Data2D.h"
#include "GraphLinkCost.h"

class AdaptiveSegment2D
{
    //Configuration settings
    //To do : compactness measurement, can be passed as a function
friend class AdaptiveSegment3D;

public:
    AdaptiveSegment2D(Image2D<short> &_image, Data2D<LABEL> &_mask, MyBasic::Range2D _seg_region = MyBasic::Range2D::all());
    virtual ~AdaptiveSegment2D();

    //the interface for the algorithm
    //execute binary search segmentation
    void execute();
    //segment with a give lambda
    void segment(double lambda);

    //settings setters
    void configure(const Config& _cfg);
    //adding seeds
    void addSeed(MyBasic::Index2D p, LABEL l);
    void addStar(MyBasic::Index2D p);
    void setTumorCenter(MyBasic::Index2D p);

    //getters
    inline Data2D<LABEL> getLabeling() const
    {
        return labeling;
    }

    //average of smoothcost at all direction
    Data2D<double> getSmoothterm() const;
    //diff between fgcost and bkcost
    Data2D<double> getDataterm() const;

    inline double getLambda() const
    {
        return bestLambda;
    }
    /*
    //unary cost for given label
    Data2D<double> getDataCost(LABEL l) const;
    pair<Data2D<double>,Data2D<double> > getSmoothCost(DIRECTION d) const;
    const SparseNLink& getSparseNLink() const;
    const SparseTLink& getSparseTLink() const;
    */

private:
    //core functions for adaptive segmentation
    //search for legal interval of lambdas
    void binarySearchForRange(double &lambda_left, double &lambda_right);
    void searchForBestLambda(double lambda_left, double lambda_right, int smp_num);
    void prepareData();
    void computeSmoothterm();
    void computeDataterm();
    void computeSparseDataCost();
    void computeStarshape();
    void computeHardConstraint();
    void hardConstraintStarSeeds();
    void hardConstraintBorder();
    void hardConstraintBackground();
private:
    //settings for the algorithm
    Config cfg;
    //inputs for the algorithm
    Image2D<short>& image;
    Data2D<LABEL>& mask;

    MyBasic::Range2D seg_region;
    vector<MyBasic::Index2D> fgseeds;
    vector<MyBasic::Index2D> bkseeds;
    vector<MyBasic::Index2D> starcenter;
    MyBasic::Index2D tumor_center;
    //flag for data
    bool readyToSegment;
    /////////////////////////////////////////////////////////////////////////////
    // below are intermediate result for constructing the graph
    // size could be different with the original image and mask
    ////////////////////////////////////////////////////////////////////////////
    //segmentation result
    Data2D<LABEL> labeling;
    Data2D<LABEL> labeling_roi;
    Image2D<short> img_roi;

    //cap, rev_cap for NLink at row/column direction
    pair<Data2D<double>,Data2D<double> > smoothCostR;
    pair<Data2D<double>,Data2D<double> > smoothCostC;
    //store the smooth cost with lambda = 1
    pair<Data2D<double>,Data2D<double> > tmpSmoothCostR;
    pair<Data2D<double>,Data2D<double> > tmpSmoothCostC;

    // data cost on a grid
    Data2D<double> fgcost;
    Data2D<double> bkcost;

    //smoothterm for irregular edges generated from star shape constraint
    SparseNLink smoothCostExtra;
    SparseTLink dataCostExtra;
    //result for parameter searching while segmentation

    //the result for parameter searching
    double bestLambda;

};


#endif // ADAPTIVESEGMENT2D_H_INCLUDED

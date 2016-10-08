/**
Author:
Liqun Liu (lliu329@uwo.ca)                                   
Danfeng Chen (dchen285@uwo.ca)  

The code is for research use only.
Details about the code please refer to README.txt
**/

#ifndef ADAPTIVESEGMENT3D_H_INCLUDED
#define ADAPTIVESEGMENT3D_H_INCLUDED

//To do
//decide which information to be stored
//dataterm, smoothterm, 1st segmentation, 2nd segmentation

//adaptive segmentation for 3D volumn, internally use AdaptiveSegment2D

#include "Smoothterm.h"
#include "Data3D.h"
#include "Image3D.h"
#include "GraphLinkCost.h"
#include "Graph3D.h"

//middle slice
class AdaptiveSegment3D
{
public:
    AdaptiveSegment3D(Image3D<short>& _image, Data3D<LABEL>& _mask, int _mid_slice,MyBasic::Range3D _seg_region = MyBasic::Range3D::all());
    virtual ~AdaptiveSegment3D();
    ///////////////////////////////////////////////////////////////////////
    //functions for segmentation setttings
    //////////////////////////////////////////////////////////////////////
    void configure(Config _cfg);
    void addStar2D(MyBasic::Index3D p);
    void addStar3D(MyBasic::Index3D p);
    void addTumorCenter(MyBasic::Index3D p);
    void setSeeds(const Data3D<LABEL>& _seeds);
    void setAreaRatio(const vector<double>& min_ratio,const vector<double>& max_ratio);
	void setConfig(bool star2D,bool star3D);
    /////////////////////////////////////////////////////////////////////
    //functions for segmentation
    /////////////////////////////////////////////////////////////////////
    //segment the whole volume, 3D segmentation (with 3D star shape constraint)
    void segment();
    //resegment after first round segmentation(either 2D or 3D)
    void resegment(const Data3D<LABEL>* new_seeds);
    //////////////////////////////////////////////////////////////////////
    //getters
    /////////////////////////////////////////////////////////////////////
    Data3D<LABEL> getLabeling() const;
    vector<double> getLambda() const;
  //  void setSegRegion(MyBasic::Range3D roi);
 //   void setSideStars(MyBasic::Index3D _s1, MyBasic::Index3D _s2);
 //   void setTightBox(MyBasic::Range3D _range);

private:
    //flip the label of the give pixel to opposite label
    void flipSeed(MyBasic::Index3D p);
    void removeSeed(MyBasic::Index3D p);
    void addSeed(MyBasic::Index3D p, LABEL l);
    void setSeed(MyBasic::Index3D p, LABEL l);
    //search for best lambda for each slice, internally use 2D adaptive segmentation (2D segmentation)
    void ParameterSearch();
    //adaptive segmentation on a single slice with give configuration
    void ParameterSearchOnSlice(int ind,Config cfg);
    void computeStarShape();
    int index2Id(const Index3D& i) const;
    int index2Id(int s, int r, int c) const;
     //////////////////////////////////////////////////////////////
     //member variables
     //////////////////////////////////////////////////////////////
private:
    Data3D<double> fgcost;
    Data3D<double> bkcost;

    pair<Data3D<double>,Data3D<double> > smoothCostR; //size equal to the graph
    pair<Data3D<double>,Data3D<double> > smoothCostC;

    SparseNLink extraSmoothCost; //id in terms of size of graph
    SparseTLink extraDataCost;

    Image3D<short>& image;
    Data3D<LABEL>& mask;
    Data3D<LABEL> seeds;

    Range3D seg_region;
    Config cfg;
    int mid_slice;

    //star shape center and tumor center
    vector<MyBasic::Index2D> tumor_centers;
    vector<vector<MyBasic::Index2D> > stars2D;
 //   vector<vector<MyBasic::Index2D> > fgseeds;
  //  vector<vector<MyBasic::Index2D> > bkseeds;
    vector<MyBasic::Index3D> stars3D;

    vector<double> max_area_ratio;
    vector<double> min_area_ratio;

    unsigned int num_iteration;
    //graph for 3D segmentation
    Graph3D* m_pGraph;

    //result for segmentation
    Data3D<LABEL> labeling;
    //best lambda for each slice
    vector<double> bestLambda;
};

#endif // AdaptiveSegment3D_H_INCLUDED

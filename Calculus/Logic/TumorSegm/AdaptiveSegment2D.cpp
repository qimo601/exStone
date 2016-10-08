#include "AdaptiveSegment2D.h"
#include "Smoothterm.h"
#include "MyMath.h"
#include "HardConstraint.h"
#include "ImageProcessing.h"
#include "StarShape.h"
#include "common.h"
#include <iostream>
using namespace std;

//#include "Viewer.h"

AdaptiveSegment2D::AdaptiveSegment2D(Image2D<short>& _image, Data2D<LABEL>& _mask, MyBasic::Range2D _seg_region)
:image(_image),mask(_mask)
{
  //  _image.getSize().print();

    if(_seg_region==MyBasic::Range2D::all())
    {
       seg_region =_image.getRange();
    }
    else if(!_image.inRange(seg_region))
    {
       printf("The input segmentation region is not in Range, segmentation region is set to be the whole image\n");
       seg_region =_image.getRange();
    }
    else
    {
       seg_region = _seg_region;
    }

    //To do: replace setting the cfg in the constructor by reading cfg file from outside
    //and set cfg using function configure
    //set cfg to default values
    cfg.starshape2 = true;
    cfg.starshape3 = false;  //2D segmentation, no 3D starshape
    cfg.smoothterm = true;
    cfg.dataterm = true;
    cfg.hardConstraint =true;
    cfg.white_to_dark = true;
    cfg.bin_num = 190;
    cfg.max_area = IP::bwarea<LABEL>(mask);
    cfg.min_area = cfg.max_area/3;
    cfg.seed_radius = 2;

    //set tumor center
    tumor_center = (seg_region.start+seg_region.end)/2;

    readyToSegment = false;

    labeling = Data2D<LABEL>(_image.getSize());
//	bestLambda=10;
}

AdaptiveSegment2D::~AdaptiveSegment2D()
{

}


void AdaptiveSegment2D::execute()
{	
    if(!readyToSegment)
        {
            prepareData();
            readyToSegment = true;
        }
    double lambda_left=0.1, lambda_right=20;
    binarySearchForRange(lambda_left,lambda_right);
    searchForBestLambda(lambda_left,lambda_right,8);
//	cout<<"!!!!!!!!!!AdaptiveSegment2D::execute()"<<endl;
    segment(bestLambda);
}

void AdaptiveSegment2D::configure(const Config& _cfg)
{
    memcpy(&cfg,&_cfg,sizeof(Config));
}

/*
void AdaptiveSegment2D::setSegRegion(const MyBasic::Range2D& _roi)
{
    if(_roi==MyBasic::Range2D::all())
        {
            seg_region = MyBasic::Range2D(0,image.getNumRow()-1,0,image.getNumCol()-1);
        }
    else seg_region = _roi;
}
*/

void AdaptiveSegment2D::addSeed(MyBasic::Index2D p, LABEL l)
{
     MyBasic::Index2D shift(0,0); // = seg_region.start;
     switch(l)
     {
        case FOREGROUND:
            fgseeds.push_back(p-shift);
            break;
        case BACKGROUND:
            bkseeds.push_back(p-shift);
            break;
     }
}

void AdaptiveSegment2D::addStar(MyBasic::Index2D p)
{
    //To do: add some check
    MyBasic::Index2D shift(0,0); // = seg_region.start;
    starcenter.push_back(p-shift);
}

void AdaptiveSegment2D::setTumorCenter(MyBasic::Index2D p)
{
    MyBasic::Index2D shift(0,0); // = seg_region.start;
    if(seg_region.inRange(p))
    {
    tumor_center = p-shift;
    }

    else
    {
     printf("The input tumor center is not in the legal range, image center is used\n");
     tumor_center = (seg_region.start + seg_region.end)/2;
     tumor_center -= shift;
    }
}


/*
Data2D<double> AdaptiveSegment2D::getSmoothterm() const
{
    //To do
    Data2D<double> sm(seg_region.getSize());
    sm = smoothCostR.first + smoothCostR.second + smoothCostC.first + smoothCostC.second;
    sm/=4;
    return sm;
}

Data2D<double> AdaptiveSegment2D::getDataterm() const
{
    return fgcost-bkcost;
}

Data2D<double> AdaptiveSegment2D::getDataCost(LABEL l) const
{
   switch(l)
   {
     case FOREGROUND:
        return fgcost;
        break;
     case BACKGROUND:
        return bkcost;
        break;
   }
}

pair<Data2D<double>,Data2D<double> > AdaptiveSegment2D::getSmoothCost(DIRECTION d) const
{
   switch(d)
   {
      case ROW:
          return smoothCostR;
      case COLUMN:
          return smoothCostC;
   }
}

const SparseNLink& AdaptiveSegment2D::getSparseNLink() const
{
   return smoothCostExtra;
}

const SparseTLink& AdaptiveSegment2D::getSparseTLink() const
{
   return dataCostExtra;
}
*/
void AdaptiveSegment2D::binarySearchForRange(double &lambda_left, double &lambda_right)
{
 //   double lambda_left=0.1, lambda_right=20;
    int iter_times = 0;
    int max_times = 10;
    double lambda_middle;
  //  cout<<"min area "<<cfg.min_area<<" max area "<<cfg.max_area<<endl;
    //step 1: search for lambda_middle
    while(iter_times<max_times)
        {
            lambda_middle = (lambda_left+lambda_right)/2;
            segment(lambda_middle);
            int area = IP::bwarea(labeling);

      //      cout<<"lambda middle "<<lambda_middle<<" area "<<area<<endl;

            if( area>=cfg.min_area && area<=cfg.max_area) break;
            if(area<cfg.min_area)
                {
                    lambda_right = lambda_middle;
                }
            else
                {
                    lambda_left = lambda_middle;
                }


        iter_times++;
        }


    //lambda_middle is only a lambda gives the legal area
    //lambda_middle is not the best lambda

    //second step: get the legal interval
    //shrink lambda_left and lambda_right towards lambda_middle
    //until the results of lambda_left and lambda_right are within legal range
    //step 2: search for lambda_left
    iter_times = 0;
    while(iter_times<max_times)
        {
	//		cout<<iter_times<<"/"<<max_times<<"  binarySearchForRange/lambda_left"<<endl;
            segment(lambda_left);
            int area = IP::bwarea(labeling);

      //      cout<<"lambda left "<<lambda_left<<" area "<<area<<endl;

            if(area>=cfg.min_area && area<=cfg.max_area) break;
            if(area<cfg.min_area)
            {
               printf("The minimum lambda produces a segmentation less than min area\n");
               break;
            }


            lambda_left = (lambda_left+lambda_middle)/2;

            iter_times++;
        }


    iter_times = 0;
    while(iter_times<max_times)
        {
	//		cout<<iter_times<<"/"<<max_times<<"  binarySearchForRange/lambda_right"<<endl;
            iter_times++;
            segment(lambda_right);
            int area = IP::bwarea(labeling);

          //  cout<<"lambda right "<<lambda_right<<" area "<<area<<endl;

            if(area>=cfg.min_area && area<=cfg.max_area) break;
            if(area>cfg.max_area)
            {
               printf("The maximum lambda produces a segmentation greater than max area\n");
               break;
            }
            lambda_right = (lambda_right+lambda_middle)/2;
        }

    //debug
  //  cout<<"lambda left "<<lambda_left<<" lambda middle "<<lambda_middle<<" lambda right "<<lambda_right<<endl;
}

//To do: compactness
void AdaptiveSegment2D::searchForBestLambda(double lambda_left, double lambda_right, int smp_num)
{
    //third step: sample smp_num values from [lambda_left, lambda_right]
    //find the one with the best compactness
//    tumor_center.print();
 //   image.getSize().print();
    Data3D<LABEL> result(smp_num,image.getHeight(),image.getWidth());

    using namespace std;
//	cout<<"lambda_left:"<<lambda_left<<"               lambda_right:"<<lambda_right<<endl;
    double dl = (lambda_right-lambda_left)/smp_num;
	bestLambda=lambda_right;
    vector<pair<double,double> > cmpt_lambda;
    for(int i=0; i<smp_num; i++)
        {
            double lambda = lambda_left + i*dl;
	//		cout<<i<<"/"<<smp_num<<"  AdaptiveSegment2D::searchForBestLambda(double lambda_left, double lambda_right, int smp_num)"<<endl;
            segment(lambda);
            // Viewer::show("label",labeling);
            double compactness =-IP::distanceStd2Center(labeling,tumor_center);
            cmpt_lambda.push_back(pair<double,double>(compactness, lambda));

            result.setSlice(i,labeling);
    //        cout<<"lambda "<<lambda<<" compactness "<<compactness<<endl;
        }

    double bestCompactness = cmpt_lambda[0].first;
    for(int i=1;i<smp_num;i++)
    {
        if(cmpt_lambda[i].first>bestCompactness)
        {
           bestCompactness = cmpt_lambda[i].first;
           bestLambda = cmpt_lambda[i].second;
//		   cout<<"best lambda "<<bestLambda<<endl;
        }
    }
    
  //  Viewer::show("lambda result",result);


}

void AdaptiveSegment2D::prepareData()
{
/*
    MyBasic::Size2D s = seg_region.getSize();
    img_roi.resize(s.nRow,s.nCol);
    img_roi.copyFrom(image,seg_region,img_roi.getRange());
*/
    img_roi = image;
    if(cfg.smoothterm) this->computeSmoothterm();
    if(cfg.dataterm) this->computeDataterm();
    this->hardConstraintBackground();
    if(cfg.starshape2)  this->computeStarshape();
    if(cfg.hardConstraint) this->hardConstraintStarSeeds();
    this->hardConstraintBorder();

    this->computeSparseDataCost();
}

void AdaptiveSegment2D::segment(double lambda)
{
//	cout<<"segment(bestLambda):"<<lambda<<endl;
    if(!readyToSegment)
        {
            prepareData();
            readyToSegment = true;
        }

    //build graph and cut
    Graph2D g(img_roi.getNumRow(),img_roi.getNumCol());
    if(cfg.dataterm)
    {
       g.addDataCost(fgcost,FOREGROUND);
       g.addDataCost(bkcost,BACKGROUND);
    }
       g.addDataCost(dataCostExtra.id,dataCostExtra.fgcost,dataCostExtra.bkcost);
//	   cout<<"adaptiveSegment2D segment done add data cost"<<endl;

    if(cfg.smoothterm)
    {
       //scale smooth term
       smoothCostR.first = tmpSmoothCostR.first*lambda;
       smoothCostR.second = tmpSmoothCostR.second*lambda;
       smoothCostC.first = tmpSmoothCostC.first*lambda;
       smoothCostC.second = tmpSmoothCostC.second*lambda;
       g.addSmoothCost(smoothCostR.first,smoothCostR.second,ROW);
       g.addSmoothCost(smoothCostC.first,smoothCostC.second,COLUMN);
    }
    g.addSmoothCost(smoothCostExtra.id1,smoothCostExtra.id2,smoothCostExtra.cap,smoothCostExtra.rev_cap);
//	cout<<"adaptiveSegment2D segment done add smooth cost"<<endl;

    g.maxflow();

    labeling = g.get_labeling();
    /*
    labeling_roi = g.get_labeling();
    MyBasic::Range2D rsrc = labeling_roi.getRange();
    MyBasic::Range2D rdst = seg_region;
    labeling.copyFrom(labeling_roi,rsrc,seg_region);
    */
}

//called only before segmentation in preparedData()
void AdaptiveSegment2D::computeSmoothterm()
{
    Image2D<short> crop_image = image(seg_region);
    double variance = crop_image.getVariance();
    //printf("distR %lf distC %lf\n",image.distR,image.distC);
    tmpSmoothCostR = lliu::ComputeSmoothCostWhiteToDark(img_roi,image.distR,variance,ROW);
    double smin = tmpSmoothCostR.first.getMin();
    double smax = tmpSmoothCostR.first.getMax();
    smin = tmpSmoothCostR.second.getMin();
    smax = tmpSmoothCostR.second.getMax();
    tmpSmoothCostC = lliu::ComputeSmoothCostWhiteToDark(img_roi,image.distC,variance,COLUMN);
    smin = tmpSmoothCostC.first.getMin();
    smax = tmpSmoothCostC.first.getMax();
    smin = tmpSmoothCostC.second.getMin();
    smax = tmpSmoothCostC.second.getMax();
}

//called only before segmentation in preparedData()
void AdaptiveSegment2D::computeDataterm()
{
    double const SMALLCONST = 0.001;
    Data2D<int> bin_id = image;
    IP::normalize(bin_id,0,cfg.bin_num-1);
    bin_id = floor(bin_id);

    vector<double> fghist = IP::hist(bin_id,mask,cfg.bin_num,1);
    double sm = sum(fghist);
    vector<double> fgprob = fghist/sum(fghist);
    vector<double> bkhist = IP::hist(bin_id,mask,cfg.bin_num,0);
    vector<double> bkprob = bkhist/sum(bkhist);
    Data2D<int> bin_id_roi = bin_id; //bin_id(seg_region);

    vector<double> fglikelihood = -log(fgprob + SMALLCONST);
    vector<double> bklikelihood = -log(bkprob + SMALLCONST);
    fgcost = IP::index2intensity(bin_id_roi,fglikelihood);
    bkcost = IP::index2intensity(bin_id_roi,bklikelihood);

}

//called only before segmentation in preparedData()
void AdaptiveSegment2D::computeStarshape()
{
    for(int i=0;i<starcenter.size();i++)
    {
       //SparseNLink star = starshape2D(seg_region.getSize(),starcenter[i]);
       SparseNLink star = starshape2D(img_roi.getSize(),starcenter[i]);
       smoothCostExtra.id1.insert(smoothCostExtra.id1.end(),star.id1.begin(),star.id1.end());
       smoothCostExtra.id2.insert(smoothCostExtra.id2.end(),star.id2.begin(),star.id2.end());
       smoothCostExtra.cap.insert(smoothCostExtra.cap.end(),star.cap.begin(),star.cap.end());
       smoothCostExtra.rev_cap.insert(smoothCostExtra.rev_cap.end(),star.rev_cap.begin(),star.rev_cap.end());
    }

}

//called only before segmentation in preparedData()
void AdaptiveSegment2D::computeSparseDataCost()
{
    //hard constraint
    //hard constraint for foreground
    //int width = seg_region.getWidth();
    int width = image.getWidth();
    for(int i=0; i<fgseeds.size(); i++)
        {
            dataCostExtra.fgcost.push_back(0);
            dataCostExtra.bkcost.push_back(HDHUGE);
            dataCostExtra.id.push_back(fgseeds[i].row*width+fgseeds[i].col);
        }

    for(int i=0; i<bkseeds.size(); i++)
        {
            dataCostExtra.fgcost.push_back(HDHUGE);
            dataCostExtra.bkcost.push_back(0);
            dataCostExtra.id.push_back(bkseeds[i].row*width+bkseeds[i].col);
        }

}

//called only before segmentation in preparedData()
void AdaptiveSegment2D::hardConstraintStarSeeds()
{
    //the index of star centers is the index relative to the origin of the img_roi
    vector<MyBasic::Index2D> points = lliu::getPointsWithinCircle(starcenter,cfg.seed_radius);
    for(int i=0;i<points.size();i++)
    {
        if(img_roi.inRange(points[i])) fgseeds.push_back(points[i]);
    }
 //   fgseeds.insert(fgseeds.end(),points.begin(),points.end());
}

//called only before segmentation in prepareData()
void AdaptiveSegment2D::hardConstraintBorder()
{
    vector<MyBasic::Index2D> points = lliu::getBorderPoints(img_roi.getSize(),1);
    bkseeds.insert(bkseeds.end(),points.begin(),points.end());
}

void AdaptiveSegment2D::hardConstraintBackground()
{
    for(int r=0;r<image.getHeight();r++)
    {
       for(int c=0;c<image.getWidth();c++)
       {
          MyBasic::Index2D p(r,c);
          if(!seg_region.inRange(p))
          {
             fgcost.at(r,c) += HDHUGE;
          }
       }
    }

}


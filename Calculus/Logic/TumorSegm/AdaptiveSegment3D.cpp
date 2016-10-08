#include "AdaptiveSegment3D.h"
#include "AdaptiveSegment2D.h"
#include "Graph3D.h"
#include "ImageProcessing.h"
#include "StarShape.h"
#include "common.h"

double decrease_area_min[7]={0.5117,0.6762,0.8207,0.8698,0.8453,0.7268,0.5069};
//To do: check seg_region
AdaptiveSegment3D::AdaptiveSegment3D(Image3D<short>& _image, Data3D<LABEL>& _mask,int _mid_slice, MyBasic::Range3D _seg_region)
:image(_image),mask(_mask),mid_slice(_mid_slice)
{
    labeling = Data3D<LABEL>(_image.getSize());
    bestLambda = vector<double>(_image.getLength());
 //   fgseeds.resize(_image.getLength());
  //  bkseeds.resize(_image.getLength());
    stars2D.resize(_image.getLength());
    tumor_centers.resize(_image.getLength());
   fgcost.resize(image.getSize());
   bkcost.resize(image.getSize());
   smoothCostR.first.resize(image.getSize());
   smoothCostR.second.resize(image.getSize());
   smoothCostC.first.resize(image.getSize());
   smoothCostC.second.resize(image.getSize());

   if(_seg_region==MyBasic::Range3D::all() || !image.inRange(_seg_region))
   {
       seg_region = image.getRange();
   }
   else seg_region = _seg_region;

   m_pGraph = NULL;
   seeds.resize(_image.getSize());
   seeds.setAll(UNKNOWN);
   num_iteration=0;
}

AdaptiveSegment3D::~AdaptiveSegment3D()
{
   delete m_pGraph;
}
void AdaptiveSegment3D::configure(Config _cfg)
{
    memcpy(&cfg,&_cfg,sizeof(Config));
}

void AdaptiveSegment3D::addStar2D(MyBasic::Index3D p)
{
     if(seg_region.inRange(p)) stars2D[p.sli].push_back(MyBasic::Index2D(p.row,p.col));
}

void AdaptiveSegment3D::addStar3D(MyBasic::Index3D p)
{
     if(seg_region.inRange(p)) stars3D.push_back(p);
}
//one tumor center per slice
void AdaptiveSegment3D::addTumorCenter(MyBasic::Index3D p)
{
     if(seg_region.inRange(p)) tumor_centers[p.sli]=MyBasic::Index2D(p.row,p.col);
}

//add a new seed
void AdaptiveSegment3D::addSeed(MyBasic::Index3D p, LABEL l)
{
     seeds.at(p) = l;
     extraDataCost.id.push_back(index2Id(p));
     switch(l)
     {
         case FOREGROUND:
              extraDataCost.fgcost.push_back(0);
              extraDataCost.bkcost.push_back(HDHUGE);
              break;
        case BACKGROUND:
              extraDataCost.fgcost.push_back(HDHUGE);
              extraDataCost.bkcost.push_back(0);
              break;
     }
/*
     if(seg_region.inRange(p))
     {
        if(l==FOREGROUND) fgseeds[p.sli].push_back(MyBasic::Index2D(p.row,p.col));
        else if(l==BACKGROUND) bkseeds[p.sli].push_back(MyBasic::Index2D(p.row,p.col));
     }
     */
}

void AdaptiveSegment3D::flipSeed(MyBasic::Index3D p)
{
  //replace the next line with a warning message if you don't want to be terminated
 //   smart_assert(seeds.at(p)!=UNKNOWN,"Can not flip pixel without hard constraint label");
     switch(seeds.at(p))
     {
        //flip FOREGROUND seed to BACKGROUND seed
        case FOREGROUND:
             extraDataCost.id.push_back(index2Id(p));
             extraDataCost.fgcost.push_back(HDHUGE);
             extraDataCost.bkcost.push_back(-HDHUGE);  //combine former bkcost and add to zero
             seeds.at(p) = BACKGROUND;
             break;
        //flip BACKGROUND seed to FOREGROUND seed
        case BACKGROUND:
             extraDataCost.id.push_back(index2Id(p));
             extraDataCost.fgcost.push_back(-HDHUGE);
             extraDataCost.bkcost.push_back(HDHUGE);
             seeds.at(p) = FOREGROUND;
             break;
     }
}

void AdaptiveSegment3D::removeSeed(MyBasic::Index3D p)
{
    switch(seeds.at(p))
    {
       case FOREGROUND:
             extraDataCost.id.push_back(index2Id(p));
             extraDataCost.fgcost.push_back(0);
             extraDataCost.bkcost.push_back(-HDHUGE);  //combine former bkcost and add to zero
             break;
       case BACKGROUND:
             extraDataCost.id.push_back(index2Id(p));
             extraDataCost.fgcost.push_back(-HDHUGE);
             extraDataCost.bkcost.push_back(0);  //combine former bkcost and add to zero
             break;
    }

    seeds.at(p) = UNKNOWN;

}

void AdaptiveSegment3D::setSeed(MyBasic::Index3D p, LABEL l)
{
     if(l==UNKNOWN)
     {
        removeSeed(p);
        return;
     }
    else if(seeds.at(p)==UNKNOWN)
    {
        addSeed(p,l);
    }
    else if(seeds.at(p)!=l)
    {
        flipSeed(p);
    }
}

void AdaptiveSegment3D::setSeeds(const Data3D<LABEL>& _seeds)
{
    //first round: seeds used for 2d segmentation(parameter search)
    if(num_iteration==0)
    {
       seeds.copyFrom(_seeds,seg_region,seg_region);
       return;
    }

    //second round, update sparse data cost
    extraDataCost.id.clear();
    extraDataCost.fgcost.clear();
    extraDataCost.bkcost.clear();
  //  HDHUGE = HDHUGE*2;

     for(int s=seg_region.start.sli; s<=seg_region.end.sli; s++)
     {
         for(int r=seg_region.start.row; r<=seg_region.end.row; r++)
         {
            for(int c=seg_region.start.col; c<=seg_region.end.col; c++)
            {
               setSeed(MyBasic::Index3D(s,r,c),_seeds.at(s,r,c));
            }
         }
     }
}

void AdaptiveSegment3D::setAreaRatio(const vector<double>& min_ratio,const vector<double>& max_ratio)
{
    min_area_ratio = min_ratio;
    max_area_ratio = max_ratio;
}

void AdaptiveSegment3D::setConfig(bool star2D,bool star3D)
{
	cfg.starshape3=star3D;
	cfg.starshape2=star2D;
}

Data3D<LABEL> AdaptiveSegment3D::getLabeling() const
{
   return labeling;
}

vector<double> AdaptiveSegment3D::getLambda() const
{
    return bestLambda;
}

int AdaptiveSegment3D::index2Id(const Index3D& i) const
{
    return index2Id(i.sli,i.row,i.col);
}

int AdaptiveSegment3D::index2Id(int s,int r, int c) const
{
    return s*seg_region.getHeight()*seg_region.getWidth()+r*seg_region.getWidth() + c;
}
/*
//To do: set in the constructor
void AdaptiveSegment3D::setSegRegion(MyBasic::Range3D roi)
{
   seg_region = roi;
   labeling_roi = Data3D<LABEL>(roi.getSize());
   fgcost.resize(seg_region.getSize());
   bkcost.resize(seg_region.getSize());
   smoothCostR.first.resize(seg_region.getSize());
   smoothCostR.second.resize(seg_region.getSize());
   smoothCostC.first.resize(seg_region.getSize());
   smoothCostC.second.resize(seg_region.getSize());
}
*/
/*
void AdaptiveSegment3D::setTightBox(MyBasic::Range3D _range)
{
   tightBox = _range;
   Index3D star_middle = Index3D(tightBox.start.sli, (tightBox.row.start+tightBox.row.end)/2,
                                 (tightBox.col.start+tightBox.col.end)/2);
   stars[tightBox.start.sli] = star_middle;
}

void AdaptiveSegment3D::setSideStars(MyBasic::Index3D _s1, MyBasic::Index3D _s2)
{
   //initialize seeds
   star_first = _s1;
   star_last = _s2;
   Index3D star_middle = Index3D(tightBox.start.sli, (tightBox.row.start+tightBox.row.end)/2,
                                 (tightBox.col.start+tightBox.col.end)/2);

   stars.front() = star_first;
   stars.back() = star_last;

   int dr = (star_middle.row - star_first.row)/(star_middle.sli-star_first.sli);
   int dc = (star_middle.col - star_first.col)/(star_middle.sli-star_first.sli);
   for(int i=1;i<star_middle.sli;i++)
   {
       stars[i] = star_first + Index3D(i,dr*i,dc*i);
   }

   dr = (star_middle.row - star_last.row)/(star_middle.sli-star_first.sli);
   dc = (star_middle.col - star_last.col)/(star_middle.sli-star_first.sli);
   for(int i=star_last.sli-1;i>star_middle.sli;i--)
   {
       stars[i] = star_last + Index3D(i-star_last.sli,dr*(i-star_last.sli),dc*(i-star_last.sli));
   }
}
*/
void AdaptiveSegment3D::ParameterSearch()
{
    ParameterSearchOnSlice(mid_slice,cfg);

    int ratio_bin_num = max_area_ratio.size();
   //search on left slices
   for(int i=mid_slice-1;i>=0;i--)
   {
     mask.setSlice(i,labeling.getSlice(i+1));
     int area = IP::bwarea(labeling.getSlice(i+1));
     cfg.min_area = min_area_ratio[(ratio_bin_num-1)*i/(image.getLength()-1)]*area;
     cfg.max_area = max_area_ratio[(ratio_bin_num-1)*i/(image.getLength()-1)]*area;
     //roi, seeds, starshape center
     ParameterSearchOnSlice(i,cfg);
   //  printf("%d ,%d\n",i,area);
   }
   //search on right slices
   for(int i=mid_slice+1;i<=seg_region.end.sli;i++)
   {
     mask.setSlice(i,labeling.getSlice(i-1));
     int area = IP::bwarea(labeling.getSlice(i-1));
   //  printf("%d ,%d\n",i,area);
     cfg.min_area = min_area_ratio[(ratio_bin_num-1)*(i-1)/(image.getLength()-1)]*area;
     cfg.max_area = max_area_ratio[(ratio_bin_num-1)*(i-1)/(image.getLength()-1)]*area;
     //roi, seeds, starshape center
     ParameterSearchOnSlice(i,cfg);
  //   printf("%d ,%d\n",i,area);
   }

}

void AdaptiveSegment3D::ParameterSearchOnSlice(int ind,Config cfg)
{
	cout<<"Search parameters on "<<ind<<" slice."<<endl;
   Image2D<short> img_slice = image.getSlice(ind);
   Data2D<LABEL> mask_slice = mask.getSlice(ind);
  // Viewer::show("img_slice",img_slice);
  // Viewer::show("mask_slice",mask_slice);
   MyBasic::Range2D seg_region_slice(seg_region.start.row,seg_region.end.row,seg_region.start.col,seg_region.end.col);
   AdaptiveSegment2D seg(img_slice,mask_slice,seg_region_slice);
   seg.configure(cfg);
//   seg.setSegRegion(MyBasic::Range2D(seg_region.row,seg_region.col)); //where to get this information
 //  seg.addStar(MyBasic::Index2D(stars[ind].row,stars[ind].col));  //one star center
 //  seg.setTumorCenter(tumor_centers[ind]);
   for(int i=0;i<stars2D[ind].size();i++)
   {
       seg.addStar(stars2D[ind][i]);
   }
   seg.setTumorCenter(tumor_centers[ind]);

   for(int r=seg_region.start.row;r<=seg_region.end.row;r++)
   {
      for(int c=seg_region.start.col;c<=seg_region.end.col;c++)
      {
         if(seeds.at(ind,r,c)!=UNKNOWN)
            seg.addSeed(MyBasic::Index2D(r,c),seeds.at(ind,r,c));
      }
   }
   /*
   for(int i=0;i<fgseeds[ind].size();i++)
   {
       seg.addSeed(fgseeds[ind][i],FOREGROUND);
   }
   for(int i=0;i<bkseeds[ind].size();i++)
   {
       seg.addSeed(bkseeds[ind][i],BACKGROUND);
   }
   */
   seg.execute();

   //get lambda
   bestLambda[mid_slice] = seg.getLambda(); //get best lambda
   //get labeling
   labeling.setSlice(ind,seg.getLabeling());  //get labeling of the original image size
   /*
   MyBasic::Range3D roi = seg_region;
   roi.setSli(Range(ind,ind));
   labeling.copyFrom(labeling_roi,labeling_roi.getRange(),roi); */
   //get data cost
   if(cfg.dataterm)
   {
     fgcost.setSlice(ind,seg.fgcost);  //get data cost of the seg_region/graph size
     bkcost.setSlice(ind,seg.bkcost);
   }
   //get smooth cost
   if(cfg.smoothterm)
   {
     pair<Data2D<double>, Data2D<double> > smoothCostR2D = seg.smoothCostR;  //pair cap rev_cap
     smoothCostR.first.setSlice(ind,smoothCostR2D.first);
     smoothCostR.second.setSlice(ind,smoothCostR2D.second);
     pair<Data2D<double>, Data2D<double> > smoothCostC2D = seg.smoothCostC;  //pair cap rev_cap
     smoothCostC.first.setSlice(ind,smoothCostC2D.first);
     smoothCostC.second.setSlice(ind,smoothCostC2D.second);
   }

   //get sparse data cost and smooth cost
   int id_shift =  ind*image.getHeight()*image.getWidth();
   SparseTLink st = seg.dataCostExtra; //get sparse smooth cost of the seg_region/graph size
   for(unsigned int i=0; i<st.id.size();i++)
   {
      st.id[i] += id_shift;
   }

   SparseNLink sn = seg.smoothCostExtra; //
   //change the id
    for(unsigned int i=0; i<sn.id1.size();i++)
   {
      sn.id1[i] += id_shift;
      sn.id2[i] += id_shift;
   }

   extraDataCost.id.insert(extraDataCost.id.end(),st.id.begin(),st.id.end()); //size of graph, in terms of id
   extraDataCost.fgcost.insert(extraDataCost.fgcost.end(),st.fgcost.begin(),st.fgcost.end());
   extraDataCost.bkcost.insert(extraDataCost.bkcost.end(),st.bkcost.begin(),st.bkcost.end());
   extraSmoothCost.id1.insert(extraSmoothCost.id1.end(),sn.id1.begin(),sn.id1.end()); //
   extraSmoothCost.id2.insert(extraSmoothCost.id2.end(),sn.id2.begin(),sn.id2.end());
   extraSmoothCost.cap.insert(extraSmoothCost.cap.end(),sn.cap.begin(),sn.cap.end());
   extraSmoothCost.rev_cap.insert(extraSmoothCost.rev_cap.end(),sn.rev_cap.begin(),sn.rev_cap.end());
}

void AdaptiveSegment3D::segment()
{
    //build graph and cut
   // Graph3D g(img_roi.getNumSli(),img_roi.getNumRow(),img_roi.getNumCol());
  // cout<<" before compute star shape"<<endl;
	cout<<"Begin adaptive segment3D"<<endl;
    ParameterSearch();
    if(!cfg.starshape3) return;
    computeStarShape();
    m_pGraph = new Graph3D(image.getSize());

    if(cfg.dataterm)
    {
       m_pGraph->addDataCost(fgcost,FOREGROUND);
       m_pGraph->addDataCost(bkcost,BACKGROUND);
    }
	cout<<"Done add dataterm  1"<<endl;
    m_pGraph->addDataCost(extraDataCost.id,extraDataCost.fgcost,extraDataCost.bkcost);
	cout<<"Done add extra dataterm  2"<<endl;
    if(cfg.smoothterm)
    {
       m_pGraph->addSmoothCost(smoothCostR.first,smoothCostR.second,ROW);
       m_pGraph->addSmoothCost(smoothCostC.first,smoothCostC.second,COLUMN);
    }
	cout<<"Done add smoothterm  3"<<endl;
	cout<<extraSmoothCost.id1.size()<<";"<<extraSmoothCost.id2.size()
		<<extraSmoothCost.cap.size()<<extraSmoothCost.rev_cap.size()<<endl;
//	return;
    m_pGraph->addSmoothCost(extraSmoothCost.id1,extraSmoothCost.id2,extraSmoothCost.cap,extraSmoothCost.rev_cap);
	cout<<"Done add extra smoothterm  4"<<endl;
 //cout<<"star3d size "<<extraSmoothCost.id1.size()<<endl;
//	cout<<" Begin maxflow "<<endl;
    double flow = m_pGraph->maxflow();
    cout<<" Got flow  5"<<flow<<endl;
    labeling = m_pGraph->get_labeling();

    num_iteration++;
	cout<<"Num_iteration:"<<num_iteration<<endl;
}

//resegment with new user input seeds
void AdaptiveSegment3D::resegment(const Data3D<LABEL>* new_seeds)
{
    smart_assert(num_iteration!=0,"please run segment() first");

    if(new_seeds==NULL)
    {
       cout<<"no new user seeds, nothing is done"<<endl;
       return;
    }

    setSeeds(*new_seeds);

    int node_num = m_pGraph->getNumSli()*m_pGraph->getNumRow()*m_pGraph->getNumCol();

    for(int i=0;i<extraDataCost.id.size();i++)
         m_pGraph->mark_node(extraDataCost.id[i]);
    //change hard contraint


    m_pGraph->addDataCost(extraDataCost.id,extraDataCost.fgcost,extraDataCost.bkcost);

    double flow = m_pGraph->maxflow();

    labeling = m_pGraph->get_labeling();

    num_iteration++;
}

void AdaptiveSegment3D::computeStarShape()
{
    for(int i=0;i<stars3D.size();i++)
    {
       SparseNLink star = starshape3D(image.getSize(),stars3D[i]);
       extraSmoothCost.id1.insert(extraSmoothCost.id1.end(),star.id1.begin(),star.id1.end());
       extraSmoothCost.id2.insert(extraSmoothCost.id2.end(),star.id2.begin(),star.id2.end());
       extraSmoothCost.cap.insert(extraSmoothCost.cap.end(),star.cap.begin(),star.cap.end());
       extraSmoothCost.rev_cap.insert(extraSmoothCost.rev_cap.end(),star.rev_cap.begin(),star.rev_cap.end());
    }
}


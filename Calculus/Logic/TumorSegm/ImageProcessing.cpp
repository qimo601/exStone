#include "ImageProcessing.h"

    vector<double> ImageProcessing::hist(const Data2D<int>& bin_id, const Data2D<LABEL>& mask, int bin_num, int indicator )
    {
        vector<double> vhist(bin_num,0);
        smart_assert(bin_id.getSize()==mask.getSize(),"size not equal");

        for(int i=0;i<bin_id.getNumData();i++)
        {
            if(mask.at(i)==indicator) vhist[bin_id.at(i)] = vhist[bin_id.at(i)]+1;
        }

        return vhist;
    }

    vector<double> ImageProcessing::hist(const Data3D<int>& bin_id, const Data3D<LABEL>& mask, int bin_num, int indicator )
    {
        return hist(bin_id.getMat(),mask.getMat(),bin_num,indicator);
    }

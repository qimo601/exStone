#include "Smoothterm.h"

namespace lliu
{
double computeNLinkCost(double diff,double variance)
{
    return exp(-(diff*diff)/(variance*variance));
}
}

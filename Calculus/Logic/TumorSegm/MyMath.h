/**
Author:
Liqun Liu (lliu329@uwo.ca)                                   
Danfeng Chen (dchen285@uwo.ca)  

The code is for research use only.
Details about the code please refer to README.txt
**/

#ifndef MYMATH_H_INCLUDED
#define MYMATH_H_INCLUDED

#include <vector>
#include "Data2D.h"
#include "Data3D.h"
#include "MyBasic.h"
#include "VectOperation.h"
#include <math.h>
///////////////////////////////////////////////////////
// Global operators for Data2D and Kernel2D objects //
///////////////////////////////////////////////////////

using namespace std;

//To do
MyBasic::Size2D abs(const MyBasic::Size2D& s);
MyBasic::Index2D abs(const MyBasic::Index2D& i);

template<typename T>
Data2D<T> abs(const Data2D<T>& a);

template<typename T>
Data3D<T> abs(const Data3D<T>& a);

template<typename T>
double standardDeviation(const vector<T>& a);

template<typename T>
double mean(const vector<T>& a);

template<typename T>
double norm(const vector<T>& a);

template<typename T>
vector<double> log(const vector<T>& op);

template<typename T>
vector<T> floor(const vector<T>& op);

template<typename T>
Data2D<T> floor(const Data2D<T>& op);

template<typename T>
Data2D<T> operator &(const Data2D<T>& op1, const Data2D<T>& op2);

template<typename T>
Data3D<T> operator &(const Data3D<T>& op1, const Data3D<T>& op2);

template<typename T>
double sum(const Data2D<T>& a);

template<typename T>
double sum(const Data3D<T>& a);

template<typename T>
double sum(const T* a, long long unsigned int count);

template<typename T>
T sum(const vector<T>& a);

template<typename T>
T min(const T* a, long long unsigned int count);
template<typename T>
T min(const Data2D<T>& a);
template<typename T>
T min(const Data3D<T>& a);

template<typename T>
T max(const T* a, long long unsigned int count);
template<typename T>
T max(const Data2D<T>& a);
template<typename T>
T max(const Data3D<T>& a);

template<typename T>
Data2D<T> abs(const Data2D<T>& a)
{
    Data2D<T> result(a.getSize());
    for(int i=0;i<a.getNumData();i++)
    {
        result.at(i) = abs(a.at(i));
    }

    return result;
}

template<typename T>
Data3D<T> abs(const Data3D<T>& a)
{
    Data3D<T> result(a.getSize());
    for(int i=0;i<a.getNumData();i++)
    {
        result.at(i) = abs(a.at(i));
    }

    return result;
}

template<typename T>
void divide(vector<T>& data, T dividor);


template<typename T>
double standardDeviation(const vector<T>& a)
{
    double avg = mean(a);
    vector<double> diff(a);
    diff = diff - avg;
    return norm(diff);
}

template<typename T>
double mean(const vector<T>& a)
{
   double total =  sum(a);
   return total/a.size();
}

template<typename T>
double norm(const vector<T>& a)
{
   double total = 0;
   for(int i=0;i<a.size();i++)
   {
      total += a[i]*a[i];
   }

   return sqrt(total);
}

template<typename T>
vector<double> log(const vector<T>& op)
{
   vector<double> result(op.size());
   int s = op.size();
   for(int i=0;i<op.size();i++)
   {
    //  T j = op[i];
      result[i] = log(op[i]);
   }

   return result;
}

template<typename T>
vector<T> floor(const vector<T>& op)
{
   vector<double> result;
   for(int i=0;i<op.size();i++)
   {
      result[i] = floor(op[i]);
   }

   return result;
}

template<typename T>
Data2D<T> floor(const Data2D<T>& op)
{
    Data2D<T> result(op.getSize());
    for(int r=0;r<op.getNumRow();r++)
    {
       for(int c=0;c<op.getNumCol();c++)
       {
          result.at(r,c) = floor((double)op.at(r,c));
       }
    }

    return result;
}

template<typename T1,typename T2>
Data2D<bool> operator &(const Data2D<T1>& op1, const Data2D<T2>& op2)
{
    smart_assert(op1.getSize() == op2.getSize(), "The two mats does not match in size");
    Data2D<bool> result(op1.getSize());
    for(unsigned int id=0; id<op1.getNumData();id++)
    {
       if(op1.at(id) && op2.at(id)) result.at(id) = 1;
    }

    return result;
}

template<typename T>
Data3D<T> operator &(const Data3D<T>& op1, const Data3D<T>& op2)
{
    smart_assert(op1.getSize() == op2.getSize(), "The two mats does not match in size");
    Data2D<T> result(op1.getSize());
    for(unsigned int id=0; id<op1.getNumData();id++)
    {
       if(op1.at(id) && op2.at(id)) result.at(id) = 1;
    }

    return result;
}

template<typename T>
double sum(const Data2D<T>& a)
{
    return a.getSum();
}

template<typename T>
double sum(const Data3D<T>& a)
{
    return a.getSum();
}

template<typename T>
double sum(const T* a, long long unsigned int count)
{
    double result = 0.0;
    for(long long unsigned int i=0; i<count; i++)
        {
            result += a[i];
        }
    return result;
}

template<typename T>
T min(const T* a, long long unsigned int count)
{
    T result = a[0];
    for(long long unsigned int i=1; i<count; i++)
        {
            result = a[i]<result?a[i]:result;
        }
    return result;
}

template<typename T>
T min(const Data2D<T>& a)
{
   return a.getMin();
}
template<typename T>
T min(const Data3D<T>& a)
{
   return a.getMin();
}

template<typename T>
T max(const T* a, long long unsigned int count)
{
    T result = a[0];
    for(long long unsigned int i=1; i<count; i++)
        {
            result = a[i]>result?a[i]:result;
        }
    return result;
}

template<typename T>
T max(const Data2D<T>& a)
{
   return a.getMax();
}

template<typename T>
T max(const Data3D<T>& a)
{
   return a.getMax();
}

template<typename T>
T sum(const vector<T>& a)
{
    return std::accumulate(a.begin(),a.end(),0);
}

template<typename T>
void divide(vector<T>& data, T dividor)
{
    assert(dividor!=0);
    for(int i=0; i<data.size(); i++)
        {
            data[i] = data[i]/dividor;
        }
}

#endif // MYMATH_H_INCLUDED

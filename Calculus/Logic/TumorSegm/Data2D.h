/**
Author:
Liqun Liu (lliu329@uwo.ca)                                   
Danfeng Chen (dchen285@uwo.ca)  

The code is for research use only.
Details about the code please refer to README.txt
**/

#ifndef DATA2D_H_INCLUDED
#define DATA2D_H_INCLUDED

/* This file is for 2D matrix, and it supports random access,
   copy data patches/blocks with bounding box or mask, min and max
   functions
*/


#include "MyBasic.h"
#include "smart_assert.h"
#include <string.h>
#include <stdio.h>
#include <fstream>

using MyBasic::Range2D;
using MyBasic::Index2D;
using MyBasic::Size2D;
using MyBasic::Range;

template<typename T>
class Data2D
{

public:
    //////////////////////////////////////////////////////////////////////////////////
    //constructor and destructor
    //////////////////////////////////////////////////////////////////////////////////

    //rewrite default constructor
    Data2D();
    Data2D(int row, int col, T val= T());
    //edit on May 29, 2014
    Data2D(int row, int col, const T* arr);
    //constructor with single input parameter, use explicit
    explicit Data2D(MyBasic::Size2D size, T val = T());
    //copy constrcutor
    Data2D(const Data2D<T>& src);

    template<typename T2>
    Data2D(const Data2D<T2>& src);
    virtual ~Data2D(void) ;

    //////////////////////////////////////////////////////////////////////////////////
    //print the matrix
    //////////////////////////////////////////////////////////////////////////////////
    void print(void) const;
    //load and save
    virtual void save_info(const std::string& filename) const;
    virtual void save(const std::string& filename) const;
    virtual void load_info(const std::string& filename);
    virtual void load(const std::string& filename);

    //////////////////////////////////////////////////////////////////////////////////
    //get operations
    //////////////////////////////////////////////////////////////////////////////////
    inline T& at(const int row,const int col);
    inline T& at(const MyBasic::Index2D& p) ;
    inline T& at(const int id) ;
	inline const T& at(const int row, const int col) const;
	inline const T& at(const MyBasic::Index2D& p) const;
	inline const T& at(const int id) const;

    //////////////////////////////////////////////////////////////////////////////////
    //set operation for a patch of data
    //////////////////////////////////////////////////////////////////////////////////
    void set(const Range& range_row, const Range& range_col, T val);
    void set(const Range2D& range, T val) ;
    //set data using a mask
    void set(const Data2D<bool> &mask, T val);
    //set all data
    void setAll(T val);

    //////////////////////////////////////////////////////////////////////////////////
    //comparison
    //////////////////////////////////////////////////////////////////////////////////
    bool isEqualTo(const Data2D<T>& src) const ;
    // const T* getData() const { return m_pData; }
    const T* getData() const;

    //////////////////////////////////////////////////////////////////////////////////
    //copy data
    //////////////////////////////////////////////////////////////////////////////////
    ////copy whole data to object
    void copyFrom(const Data2D<T> &src);
    //copy data using rects
    void copyFrom(const Data2D<T> &src,const Range2D &rsrc, const Range2D &rdst) ;
    //copy data using a mask
    void copyFrom(const Data2D<T> &src,const Data2D<bool> &mask);  //does not check memory overlap

    ////copy whole data to object
    template<typename T2>
    void copyFrom(const Data2D<T2> &src);
    //copy data using rects
    template<typename T2>
    void copyFrom(const Data2D<T2> &src,const Range2D &rsrc, const Range2D &rdst);
    //copy data using a mask
    template<typename T2>
    void copyFrom(const Data2D<T2> &src,const Data2D<bool> &mask);  //does not check memory overlap

    //////////////////////////////////////////////////////////////////////////////////
    //min and max
    //////////////////////////////////////////////////////////////////////////////////
    T getMin() const;
    T getMax() const;
    double getSum() const;

    //////////////////////////////////////////////////////////////////////////////////
    //check the input index is legal or not
    //////////////////////////////////////////////////////////////////////////////////
    inline bool inRange(const Index2D& p) const ;
    inline bool inRange(const Range2D& r) const;
    inline bool inRange(int row, int col) const;
    inline bool inRange(int id) const;

    //////////////////////////////////////////////////////////////////////////////////
    //tranforms between id and point
    //////////////////////////////////////////////////////////////////////////////////
    inline int getID(int row,int col) const ;
    inline int getID(MyBasic::Index2D& p) const;

    //////////////////////////////////////////////////////////////////////////////////
    //get data information
    //////////////////////////////////////////////////////////////////////////////////
    inline bool isEmpty() const;
    //the 3D size and number of data contained in the object
    inline MyBasic::Size2D getSize() const ;
    inline MyBasic::Range2D getRange() const;
    inline unsigned int getNumCol() const;
    inline unsigned int getNumRow() const;
    inline unsigned int getNumData() const;
    inline unsigned int getCapacity() const;

    void resize(int row, int col);
    void clear();

    //////////////////////////////////////////////////////////////////////////////////
    //operator overloading
    //////////////////////////////////////////////////////////////////////////////////

    //Data2D<T>& operator = (Data2D<T>& src);
    Data2D<T>& operator = (const Data2D<T>& src);
  //  Data2D<T>& operator = (Data2D<T> src);

    template<typename T2>
    Data2D<T>& operator = (const Data2D<T2>& src);
    // basic arithmetic operators
    Data2D<T>& operator+=(const Data2D<T>& arg); // adding another Data2D
    Data2D<T>& operator-=(const Data2D<T>& arg); // subtracting another Data2D
    Data2D<T>& operator+=(const T& val); // adding a constant to each element of the table
    Data2D<T>& operator-=(const T& val); // subtracting a constant from each element of the table
    Data2D<T>& operator*=(const double& s); // multiplication by a constant scalar
    Data2D<T>& operator/=(const double& s); //division by a constant scalar
    Data2D<T> operator()(const MyBasic::Range2D& ROI) const;

protected:
    unsigned int m_nCol;
    unsigned int m_nRow;
    unsigned int m_capacity;
    unsigned int m_numData;
    T* m_pData;
};

template<typename T>
Data2D<T>::Data2D()
{
    m_nRow=0;
    m_nCol=0;
    m_numData =0;
    m_capacity =0;
    m_pData = NULL;
}

template<typename T>
Data2D<T>::Data2D(int row, int col, T val)
{
    m_nRow=0;
    m_nCol=0;
    m_numData =0;
    m_capacity =0;
    m_pData = NULL;
    resize(row,col);
    setAll(val);
}

//edit on May 29, 2014
template<typename T>
Data2D<T>::Data2D(int row, int col, const T* arr)
{
    m_nRow=0;
    m_nCol=0;
    m_numData =0;
    m_capacity =0;
    m_pData = NULL;
    resize(row,col);
    memcpy(m_pData,arr, row*col*sizeof(T));
}

//constructor with single input parameter, use explicit
template<typename T>
Data2D<T>::Data2D(MyBasic::Size2D size, T val)
{
    m_nRow=0;
    m_nCol=0;
    m_numData =0;
    m_capacity =0;
    m_pData = NULL;
    resize(size.nRow,size.nCol);
    setAll(val);
}

//copy constrcutor
template<typename T>
Data2D<T>::Data2D(const Data2D<T>& src)
{
    m_nRow=0;
    m_nCol=0;
    m_numData =0;
    m_capacity =0;
    m_pData = NULL;
    copyFrom(src);

}

template<typename T> template<typename T2>
Data2D<T>::Data2D(const Data2D<T2>& src)
{
    m_nRow=0;
    m_nCol=0;
    m_numData =0;
    m_capacity =0;
    m_pData = NULL;
    copyFrom(src);

}


template<typename T>
Data2D<T>::~Data2D(void)
{
    delete[] m_pData;
    m_pData = NULL;
}

//////////////////////////////////////////////////////////////////////////////////
//print the matrix
//////////////////////////////////////////////////////////////////////////////////
template<typename T>
void Data2D<T>::print(void) const
{
    cout<<"The matrix is---->"<<endl;
    for(int y=0; y<m_nRow; y++)
        {
            for(int x=0; x<m_nCol; x++)
                cout<<m_pData[y*m_nCol+x]<<' ';
            cout<<endl;
        }
}

template<typename T>
void Data2D<T>::save_info(const std::string& filename) const
{
    smart_assert(!filename.empty(),"No file for written");

 //   string info_file = filename + "_info.txt";
	string info_file = filename;
	info_file.append("_info.txt");
    FILE* pFile = fopen(info_file.c_str(),"wt");
    smart_assert(pFile,"File error");

    ofstream fout( info_file.c_str() );
    fout << this->getNumRow() << " ";
    fout << this->getNumCol() << "\n";

    fout.close();
}

//load and save
template<typename T>
void Data2D<T>::save(const std::string& filename) const
{
    smart_assert(!filename.empty(),"No file for written");
    smart_assert(!this->isEmpty(),"Image is empty, can't save");

 //   save_info(filename);
    unsigned int size=this->getNumData();

    FILE* pFile=fopen ( filename.c_str() , "wb" );
    smart_assert(pFile,"File error");

    fwrite(this->m_pData,sizeof(T),size,pFile);
    fclose(pFile);
}

template<typename T>
void Data2D<T>::load_info(const string& filename)
{
    string info_file = filename;
	info_file.append("_info.txt");
    // open file
    ifstream fin( info_file.c_str() );
    smart_assert(fin.is_open(),"The info file is not found");

    // size of the data
    int row, col;
    fin >> row;
    fin >> col;
    fin.close();

    this->resize(row,col);
}

template<typename T>
void Data2D<T>::load(const std::string& filename)
{
    //load dimension information and allocate the memory first
 //   load_info(filename);

    FILE* pFile=fopen(filename.c_str(),"rb");
    smart_assert(pFile,filename.c_str());

    int readsize=fread(this->m_pData,sizeof(T),this->m_numData,pFile);
    smart_assert(readsize==this->getNumData(),"File size dose not match with data size");
}

//////////////////////////////////////////////////////////////////////////////////
//get operations
//////////////////////////////////////////////////////////////////////////////////
template<typename T>
T& Data2D<T>::at(const int row,const int col)
{
    smart_assert(inRange(row,col), "Input index is out of range");
	T& t = m_pData[row*m_nCol + col];
    return t;
//	return m_pData[row*m_nCol + col];
}

template<typename T>
T& Data2D<T>::at(const MyBasic::Index2D& p)
{
    return at(p.row,p.col);
}

template<typename T>
T& Data2D<T>::at(const int id)
{
    smart_assert(inRange(id),"Input index is out of range");
	T& t = m_pData[id];
    return t;
//	return m_pData[id];
}

template<typename T>
const T& Data2D<T>::at(const int row,const int col) const
{
    smart_assert(inRange(row,col), "Input index is out of range");
    return m_pData[row*m_nCol+col];
}

template<typename T>
const T& Data2D<T>::at(const MyBasic::Index2D& p) const
{
    return at(p.row,p.col);
}

template<typename T>
const T& Data2D<T>::at(const int id) const
{
    smart_assert(inRange(id),"Input index is out of range");
    return m_pData[id];
}

//////////////////////////////////////////////////////////////////////////////////
//set operation for a patch of data
//////////////////////////////////////////////////////////////////////////////////
template<typename T>
void Data2D<T>::set(const Range& range_row, const Range& range_col, T val)
{
    set(Range2D(range_row,range_col),val);
}


template<typename T>
void Data2D<T>::set(const Range2D& range, T val)
{
    smart_assert(inRange(range),"Bounding box is out of range");

    for(int row=range.start.row; row<=range.end.row; row++)
        for(int col=range.start.col; col<=range.end.col; col++)
            m_pData[row*m_nCol+col] = val;
}

//set data using a mask
template<typename T>
void Data2D<T>::set(const Data2D<bool> &mask, T val)
{
    //	smart_assert(m_numData==mask.getNumData(),"The size of mask is illegal");
    if(m_nRow!=mask.getNumRow() || m_nCol != mask.getNumCol()) resize(mask.getNumRow(),mask.getNumRow());
    for(int i=0; i<m_numData; i++)
        {
            if(mask.at(i)) m_pData[i] = val;
        }
}
//set all data
template<typename T>
void Data2D<T>::setAll(T val)
{
    if(m_pData) set(Range2D(Range(0,m_nRow-1),Range(0,m_nCol-1)),val);
}

//////////////////////////////////////////////////////////////////////////////////
//comparison
//////////////////////////////////////////////////////////////////////////////////
template<typename T>
bool Data2D<T>::isEqualTo(const Data2D<T>& src) const
{
    if(m_nCol!= src.getNumCol() || m_nRow != src.getNumRow()) return false;
    for(int i=0; i<m_numData; i++)
        if(m_pData[i] != src.at(i)) return false;

    return true;
}

template<typename T>
const T* Data2D<T>::getData() const
{
    return m_pData;
}
//////////////////////////////////////////////////////////////////////////////////
//copy data
//////////////////////////////////////////////////////////////////////////////////

////copy whole data to object
template<typename T>
void Data2D<T>::copyFrom(const Data2D<T> &src)
{
    if(m_nCol!=src.getNumCol() || m_nRow != src.getNumRow()) resize(src.getNumRow(),src.getNumCol());
    for(int i=0; i<m_numData; i++)  m_pData[i] = src.at(i);
}


//copy data using rects
template<typename T>
void Data2D<T>::copyFrom(const Data2D<T> &src,const Range2D &rsrc, const Range2D &rdst)
{
    MyBasic::Range2D tmp_rsrc(rsrc);
    MyBasic::Range2D tmp_rdst(rdst);
    if(tmp_rsrc==MyBasic::Range2D::all()) tmp_rsrc = src.getRange();
    if(tmp_rdst==MyBasic::Range2D::all()) tmp_rdst = getRange();

    smart_assert(inRange(tmp_rdst), "The box is outof range");
    smart_assert(src.inRange(tmp_rsrc), "The source box is outof range");
    smart_assert(tmp_rsrc.getSize()==tmp_rdst.getSize(),"The copy ranges should be equal");

    Size2D size(tmp_rsrc.getSize());
    for(int row=0; row<size.nRow; row++)
        for(int col=0; col<size.nCol; col++)
            {
                at(tmp_rdst.start.row+row,tmp_rdst.start.col+col) = src.at(tmp_rsrc.start.row+row,tmp_rsrc.start.col+col);
            }
}

//copy data using a mask
template<typename T>
void Data2D<T>::copyFrom(const Data2D<T> &src,const Data2D<bool> &mask)  //does not check memory overlap
{
    smart_assert(src.getSize()==mask.getSize(),"Sizes of mask and src don't match");
    if(m_nCol!=src.getNumCol() || m_nRow != src.getNumRow()) resize(src.getNumRow(),src.getNumCol());

    for(int i=0; i<m_numData; i++)
        {
            if(mask.at(i)) m_pData[i]  =  src.at(i);
        }
}


////copy whole data to object
template<typename T> template< typename T2>
void Data2D<T>::copyFrom(const Data2D<T2> &src)
{
    if(m_nCol!=src.getNumCol() || m_nRow != src.getNumRow()) resize(src.getNumRow(),src.getNumCol());
    for(int i=0; i<m_numData; i++)  m_pData[i] = src.at(i);
}


//copy data using rects
template<typename T> template<typename T2>
void Data2D<T>::copyFrom(const Data2D<T2> &src,const Range2D &rsrc, const Range2D &rdst)
{
    MyBasic::Range2D tmp_rsrc(rsrc);
    MyBasic::Range2D tmp_rdst(rdst);
    if(tmp_rsrc==MyBasic::Range2D::all()) tmp_rsrc = src.getRange();
    if(tmp_rdst==MyBasic::Range2D::all()) tmp_rdst = getRange();

    smart_assert(inRange(tmp_rdst), "The box is outof range");
    smart_assert(src.inRange(tmp_rsrc), "The source box is outof range");
    smart_assert(tmp_rsrc.getSize()==tmp_rdst.getSize(),"The copy ranges should be equal");

    Size2D size(tmp_rsrc.getSize());
    for(int row=0; row<size.nRow; row++)
        for(int col=0; col<size.nCol; col++)
            {
                at(tmp_rdst.start.row+row,tmp_rdst.start.col+col) = src.at(tmp_rsrc.start.row+row,tmp_rsrc.start.col+col);
            }
}

//copy data using a mask
template<typename T> template< typename T2>
void Data2D<T>::copyFrom(const Data2D<T2> &src,const Data2D<bool> &mask)  //does not check memory overlap
{
    smart_assert(src.getSize()==mask.getSize(),"Sizes of mask and src don't match");
    if(m_nCol!=src.getNumCol() || m_nRow != src.getNumRow()) resize(src.getNumRow(),src.getNumCol());

    for(int i=0; i<m_numData; i++)
        {
            if(mask.at(i)) m_pData[i]  = src.at(i);
        }
}

//////////////////////////////////////////////////////////////////////////////////
//min and max
//////////////////////////////////////////////////////////////////////////////////
template <class T>
T Data2D<T>::getMin() const
{
    smart_assert(!isEmpty(),"The Data is empty");

    T min = m_pData[0];
    for(int i=1; i<m_numData; i++)
        {
            if(m_pData[i]<min) min = m_pData[i];
        }
    return min;
}

template <class T>
T Data2D<T>::getMax() const
{
    smart_assert(!isEmpty(),"The Data is empty");

    T max = m_pData[0];
    for(int i=1; i<m_numData; i++)
        {
            if(m_pData[i]>max) max = m_pData[i];
        }
    return max;
}

template <class T>
double Data2D<T>::getSum() const
{
    double sumval = 0;
    for(int i=0; i<m_numData; i++) sumval += m_pData[i];

    return sumval;
}

//////////////////////////////////////////////////////////////////////////////////
//check the input index is legal or not
//////////////////////////////////////////////////////////////////////////////////
template <class T>
bool Data2D<T>::inRange(const Index2D& p) const
{
    return inRange(p.row, p.col);
}

template <class T>
bool Data2D<T>::inRange(const Range2D& r) const
{
    return inRange(Index2D(r.start.row,r.start.col)) && inRange(Index2D(r.end.row,r.end.col));
}

template <class T>
bool Data2D<T>::inRange(int row, int col) const
{
    return (0<=col && col<m_nCol && 0<=row && row<m_nRow);
}

template <class T>
bool Data2D<T>::inRange(int id) const
{
    return (0<=id && id<m_numData);
}

//////////////////////////////////////////////////////////////////////////////////
//tranforms between id and point
//////////////////////////////////////////////////////////////////////////////////
template <class T>
int Data2D<T>::getID(int row,int col) const
{
    smart_assert(inRange(row,col), "Input index is out of range");
    return row*m_nCol+col;
}

template <class T>
int Data2D<T>::getID(MyBasic::Index2D& p) const
{
    return getID(p.row,p.col);
}

//////////////////////////////////////////////////////////////////////////////////
//get data information
//////////////////////////////////////////////////////////////////////////////////
template <class T>
bool Data2D<T>::isEmpty() const
{
    return m_numData==0;
}

//the 3D size and number of data contained in the object
template <class T>
MyBasic::Size2D Data2D<T>::getSize() const
{
    return MyBasic::Size2D(m_nRow,m_nCol);
}

template <class T>
MyBasic::Range2D Data2D<T>::getRange() const
{
    return MyBasic::Range2D(0,m_nRow-1,0,m_nCol-1);
}

template <class T>
unsigned int Data2D<T>::getNumCol() const
{
    return m_nCol;
}

template <class T>
unsigned int Data2D<T>::getNumRow() const
{
    return m_nRow;
}

template <class T>
unsigned int Data2D<T>::getNumData() const
{
    return m_numData;
}

template <class T>
unsigned int Data2D<T>::getCapacity() const
{
    return m_capacity;
}

template <class T>
void Data2D<T>::resize(int row, int col)
{
    smart_assert(row>=0 && col>=0, "Input size should be positive");
    m_nCol = col;
    m_nRow = row;
    int new_m_numData = row*col;
    if(new_m_numData > m_capacity )
        {
            T* new_m_pData = new T[row*col];
            if(m_numData>0) memcpy(new_m_pData,m_pData,m_numData*sizeof(T));
            memset(new_m_pData+m_numData,0,(new_m_numData-m_numData)*sizeof(T));
            m_capacity  = new_m_numData;
            delete[] m_pData;
            m_pData = new_m_pData;
        }
    m_numData = new_m_numData;
}

template <class T>
void Data2D<T>::clear()
{
   delete[] m_pData;
   m_pData = NULL;
   m_nRow =0;
   m_nCol = 0;
   m_numData = 0;
   m_capacity=0;
}

template<typename T>
Data2D<T>& Data2D<T>::operator = (const Data2D<T>& src)
{
    copyFrom(src);
    return *this;
}
/*
template<typename T>
Data2D<T>& Data2D<T>::operator = (Data2D<T> src)
{
    copyFrom(src);
    return *this;
}
*/

template<typename T> template< typename T2>
Data2D<T>& Data2D<T>::operator = (const Data2D<T2>& src)
{
    copyFrom(src);
    return *this;
}

//added on May 31
template <class T>
Data2D<T>& Data2D<T> :: operator+=(const Data2D<T>& arg) // adding another Data2D
{
    smart_assert( m_nCol==arg.getNumCol() && m_nRow==arg.getNumRow(),"Data2D of different size (in operator+=)");
    for (unsigned i=0; i<(m_nCol*m_nRow); i++) m_pData[i] += arg.m_pData[i];
    return (*this);
}

template <class T>
Data2D<T>& Data2D<T> :: operator-=(const Data2D<T>& arg) // subtracting another Data2D
{
    smart_assert( m_nCol==arg.getNumCol() && m_nRow==arg.getNumRow(),"Data2D of different size (in operator-=)");
    for (unsigned i=0; i<(m_nCol*m_nRow); i++) m_pData[i] -= arg.m_pData[i];
    return (*this);
}

template <class T>
Data2D<T>& Data2D<T> :: operator+=(const T& val) // adding a constant to each element
{
    for (unsigned i=0; i<(m_nCol*m_nRow); i++) m_pData[i] += val;
    return (*this);
}

template <class T>
Data2D<T>& Data2D<T> :: operator-=(const T& val) // subtracting a constant from each element
{
    for (unsigned i=0; i<(m_nCol*m_nRow); i++) m_pData[i] -= val;
    return (*this);
}

template <class T>
Data2D<T>& Data2D<T> :: operator*=(const double& scalar) // multiplication by a scalar
{
    for (unsigned i=0; i<(m_nCol*m_nRow); i++) m_pData[i] *= scalar;
    return (*this);
}

template <class T>
Data2D<T>& Data2D<T> :: operator/=(const double& scalar) // multiplication by a scalar
{
    smart_assert(scalar!=0, "Divided by 0");
    for (unsigned i=0; i<(m_nCol*m_nRow); i++) m_pData[i] /= scalar;
    return (*this);
}

template <class T>
Data2D<T> Data2D<T> :: operator()(const MyBasic::Range2D& ROI) const
{
    Data2D<T> result(ROI.getSize());
    result.copyFrom(*this,ROI,MyBasic::Range2D(0,result.getNumRow()-1,0,result.getNumCol()-1));
    return result;
}


//////////////////////////////////////////////////////////////////////////////
//operations on Data2D
/////////////////////////////////////////////////////////////////////////////
template <class T>
Data2D<T> operator*(const Data2D<T>& a, const double& scalar)
{
    Data2D<T> result( a );
    return  result*= scalar;
}

template <class T>
Data2D<T> operator*(const double& scalar, const Data2D<T>& a)
{
    Data2D<T> result( a );
    return  result*= scalar;
}

template <class T>
Data2D<T> operator+(const Data2D<T>& a, const double& val)
{
    Data2D<T> result( a );
    return  result+= (T) val;
}

template <class T>
Data2D<T> operator+(const double& val, const Data2D<T>& a)
{
    Data2D<T> result( a );
    return  result+= (T) val;
}

template <class T>
Data2D<T> operator-(const Data2D<T>& a, const double& val)
{
    Data2D<T> result( a );
    return  result-= (T) val;
}

template <class T>
Data2D<T> operator-(const double& val, const Data2D<T>& a)
{
    Data2D<T> result(a.getNumCol(), a.getNumRow(), (T) val);
    return  result-= a;
}

template <class T>
Data2D<T> operator+(const Data2D<T>& a, const Data2D<T>& b)
{
    smart_assert( a.getNumCol()==b.getNumCol() && a.getNumRow()==b.getNumRow(),"Adding 2 arrays of different size (in operator+)");
    Data2D<T> result( a );
    return  result+=b;
}

template <class T>
Data2D<T> operator-(const Data2D<T>& a, const Data2D<T>& b)
{
    smart_assert( a.getNumCol()==b.getNumCol() && a.getNumRow()==b.getNumRow(),"Adding 2 arrays of different size (in operator-)");
    Data2D<T> result( a );
    return  result-=b;
}
#endif


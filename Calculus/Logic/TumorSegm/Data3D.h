/**
Author:
Liqun Liu (lliu329@uwo.ca)                                   
Danfeng Chen (dchen285@uwo.ca)  

The code is for research use only.
Details about the code please refer to README.txt
**/

#pragma once

/* This file is for 3D matrix, and it supports random access,
   copy data patches/blocks with bounding box or mask, min and max
   functions
*/


#include "MyBasic.h"
#include "Data2D.h"
#include "smart_assert.h"

using namespace MyBasic;

template<typename T>
class Data3D
{


public:
    //////////////////////////////////////////////////////////////////////////////////
    //constructor and destructor
    //////////////////////////////////////////////////////////////////////////////////

    //rewrite default constructor
    Data3D()
    {
        m_nSli = 0;
        m_nRow = 0;
        m_nCol = 0;
    }

    Data3D(int slice, int row, int col, T val = T())
    {
        m_nSli = 0;
        m_nRow = 0;
        m_nCol = 0;
        resize(slice,row,col);
        setAll(val);
    }

    //constructor with single input parameter, use explicit
    explicit Data3D(MyBasic::Size3D size, T val = T())
    {
        m_nSli = 0;
        m_nRow = 0;
        m_nCol = 0;
        resize(size.nSli,size.nRow,size.nCol);
        setAll(val);
    }

    //added on May 31
    Data3D(const Data3D<T>& src) : m_mat(src.m_mat)
    {
        m_nSli = src.getNumSli();
        m_nRow = src.getNumRow();
        m_nCol = src.getNumCol();
    }

    template<typename T2>
    Data3D(const Data3D<T2>& src) : m_mat(src.getMat())
    {
        m_nSli = src.getNumSli();
        m_nRow = src.getNumRow();
        m_nCol = src.getNumCol();
    }

    virtual ~Data3D(void)
    {

    }

    //////////////////////////////////////////////////////////////////////////////////
    //print the matrix
    //////////////////////////////////////////////////////////////////////////////////
    void print(void) const
    {
        cout<<"The matrix is----------------->"<<endl;
        for(int z=0; z<m_nSli; z++)
            {
                cout<<"The "<<z<<"th slice---->"<<endl;
                for(int y=0; y<m_nRow; y++)
                    {
                        for(int x=0; x<m_nCol; x++)
                            cout<<at(z,y,x)<<' ';
                        cout<<endl;
                    }
            }
    }


    //load and save
    void save_info(const std::string filename)
    {
         std::string info_filename = filename;
		 info_filename.append("_info.txt");
         FILE* pFile = fopen(info_filename.c_str(),"wt");
         smart_assert(pFile,info_filename.c_str());

         fprintf(pFile,"%d %d %d",m_nSli,m_nRow,m_nCol);
         fclose(pFile);
    }
    void save(const std::string filename)
    {
        m_mat.save(filename);
        /*
        FILE* pFile=fopen(filename.c_str(),"wb");
        smart_assert(pFile,filename.c_str());
        //write data
        fwrite(m_mat.getData(),sizeof(T),dim[0]*dim[1]*dim[2],pFile);
        fclose(pFile);
        */
    }

    void load_info(const std::string filename)
    {
       string info_file = filename;
	   info_file.append("_info.txt");

      // open file
      ifstream fin( info_file.c_str() );
      smart_assert(fin.is_open(),"The info file is not found");

      // size of the data
      int sli, row, col;
      fin >> sli;
      fin >> row;
      fin >> col;
      fin.close();

      this->resize(sli,row,col);
    }

    void load(const std::string filename)
    {
    //load dimension information and allocate the memory first
   // load_info(filename);
    m_mat.load(filename);
    }

    //////////////////////////////////////////////////////////////////////////////////
    //get operations
    //////////////////////////////////////////////////////////////////////////////////
    inline T& at(const int slice, const int row,const int col)
    {
        smart_assert(inRange(slice,row,col), "Input index is out of range");
        return m_mat.at(slice,row*m_nCol+col);
    }

    inline T& at(const MyBasic::Index3D& p)
    {
        return at(p.sli,p.row,p.col);
    }

    inline T& at(const int id)
    {
        smart_assert(inRange(id),"Input index is out of range");
        return m_mat.at(id);
    }

    inline const T& at(const int slice, const int row,const int col)const
    {
        smart_assert(inRange(slice,row,col), "Input index is out of range");
        return m_mat.at(slice,row*m_nCol+col);
    }

    inline const T& at(const MyBasic::Index3D& p) const
    {
        return at(p.sli,p.row,p.col);
    }

    inline const T& at(const int id) const
    {
        smart_assert(inRange(id),"Input index is out of range");
        return m_mat.at(id);
    }

    //////////////////////////////////////////////////////////////////////////////////
    //set operation for a patch of data
    //////////////////////////////////////////////////////////////////////////////////

    void set(const Range& range_sli,const Range& range_row, const Range& range_col, T val)
    {
        set(Range3D(range_sli,range_row,range_col),val);
    }

    void set(const Range3D& range, T val)
    {
        smart_assert(inRange(range),"Bounding box is out of range");

        for(int sli=range.start.sli; sli<=range.end.sli; sli++)
            for(int row=range.start.row; row<=range.end.row; row++)
                for(int col=range.start.col; col<=range.end.col; col++)
                    at(sli,row,col) = val;
    }

    //set data using a mask
    void set(const Data3D<bool> &mask, T val)
    {
        if(m_nSli!=mask.getNumSli() || m_nRow!=mask.getNumRow() || m_nCol != mask.getNumCol()) resize(mask.getNumSli(),mask.getNumRow(),mask.getNumRow());
        m_mat.set(mask.getMat(),val);
    }

    //set all data
    void setAll(T val)
    {
        if(!isEmpty()) set(Range3D(Range(0,m_nSli-1),Range(0,m_nRow-1),Range(0,m_nCol-1)),val);
    }

    //set a slice
    void setSlice(int s, const Data2D<T>& src)
    {
         smart_assert(src.getSize()==MyBasic::Size2D(m_nRow,m_nCol),
                    "The size of input 2D mat does not match");
        unsigned int id = s*m_nRow*m_nCol;
        for(unsigned int i=0;i<src.getNumData();i++)
        {
           m_mat.at(id+i) = src.at(i);
        }
    }

    void setSlice(int s, T value)
    {
        unsigned int id = s*m_nRow*m_nCol;
        unsigned int num = m_nRow*m_nCol;
        for(unsigned int i=0;i<num;i++)
        {
           m_mat.at(id+i) = value;
        }
    }

    //////////////////////////////////////////////////////////////////////////////////
    //set mat
    //////////////////////////////////////////////////////////////////////////////////
    void setMat(Data2D<T>& mat)
    {
        m_mat=mat;
    }

    //////////////////////////////////////////////////////////////////////////////////
    //get mat
    //////////////////////////////////////////////////////////////////////////////////
    const Data2D<T>& getMat() const
    {
        return m_mat;
    }

    //edit on May 29, 2014
    Data2D<T> getSlice(int slice) const
    {
        return Data2D<T>(m_nRow, m_nCol,m_mat.getData()+ slice*m_nRow*m_nCol);

    }

    const T* getData() const
    {
       return m_mat.getData();
    }
    //////////////////////////////////////////////////////////////////////////////////
    //comparison
    //////////////////////////////////////////////////////////////////////////////////
    bool isEqualTo(const Data3D<T>& src) const
    {
        if(m_nSli!= src.getNumSli() || m_nCol!= src.getNumCol() || m_nRow != src.getNumRow()) return false;
        return m_mat.isEqualTo(src.getMat());
    }


    //////////////////////////////////////////////////////////////////////////////////
    //copy data
    //////////////////////////////////////////////////////////////////////////////////

        ////copy whole data to object
    void copyFrom(const Data3D<T> &src)
    {
        if(m_nSli!=src.getNumSli() || m_nCol!=src.getNumCol() || m_nRow != src.getNumRow()) resize(src.getNumSli(),src.getNumRow(),src.getNumCol());
        m_mat.copyFrom(src.getMat());
    }


    //copy data using rects
    void copyFrom(const Data3D<T> &src,const Range3D &rsrc,const Range3D &rdst)
    {
        smart_assert(inRange(rdst), "The box is outof range");
        smart_assert(src.inRange(rsrc), "The destination box is outof range");
        smart_assert(Size3D::isEqual(rsrc.getSize(),rdst.getSize()),"The copy ranges should be equal");

        Size3D size(rsrc.getSize());
        for(int sli=0; sli<size.nSli; sli++)
            for(int row=0; row<size.nRow; row++)
                for(int col=0; col<size.nCol; col++)
                    {
                        at(rdst.start.sli+sli, rdst.start.row+row, rdst.start.col+col) = (T)src.at(rsrc.start.sli+sli, rsrc.start.row+row, rsrc.start.col+col);
                    }

    }

    //copy data using a mask
    void copyFrom(const Data3D<T> &src,const Data3D<bool> &mask) //does not check memory overlap
    {
        smart_assert(Size3D::isEqual(src.getSize(),mask.getSize()),"Sizes of mask and src don't match");
        if(m_nCol!=src.getNumCol() || m_nRow != src.getNumRow()) resize(src.getNumSli(), src.getNumRow(),src.getNumCol());

        m_mat.copyFrom(src.getMat(),mask.getMat());
    }

    ////copy whole data to object
    template<typename T2>
    void copyFrom(const Data3D<T2> &src)
    {
        if(m_nSli!=src.getNumSli() || m_nCol!=src.getNumCol() || m_nRow != src.getNumRow()) resize(src.getNumSli(),src.getNumRow(),src.getNumCol());
        m_mat.copyFrom(src.getMat());
    }


    //copy data using rects
    template<typename T2>
    void copyFrom(const Data3D<T2> &src,const Range3D &rsrc,const Range3D &rdst)
    {
        smart_assert(inRange(rdst), "The box is outof range");
        smart_assert(src.inRange(rsrc), "The destination box is outof range");
        smart_assert(Size3D::isEqual(rsrc.getSize(),rdst.getSize()),"The copy ranges should be equal");

        Size3D size(rsrc.getSize());
        for(int sli=0; sli<size.nSli; sli++)
            for(int row=0; row<size.nRow; row++)
                for(int col=0; col<size.nCol; col++)
                    {
                        at(rdst.start.sli+sli, rdst.start.row+row, rdst.start.col+col) = (T)src.at(rsrc.start.sli+sli, rsrc.start.row+row, rsrc.start.col+col);
                    }

    }

    //copy data using a mask
    template<typename T2>
    void copyFrom(const Data3D<T2> &src,const Data3D<bool> &mask) //does not check memory overlap
    {
        smart_assert(Size3D::isEqual(src.getSize(),mask.getSize()),"Sizes of mask and src don't match");
        if(m_nCol!=src.getNumCol() || m_nRow != src.getNumRow()) resize(src.getNumSli(), src.getNumRow(),src.getNumCol());

        m_mat.copyFrom(src.getMat(),mask.getMat());
    }

    //////////////////////////////////////////////////////////////////////////////////
    //min and max
    //////////////////////////////////////////////////////////////////////////////////
    T getMin() const
    {
        return m_mat.getMin();
    }

    T getMax() const
    {
        return m_mat.getMax();
    }

    double getSum() const
    {
        return m_mat.getSum();
    }

    MyBasic::Range3D getRange() const
    {
       return MyBasic::Range3D(0,m_nSli-1,0,m_nRow-1,0,m_nCol-1);
    }
    //////////////////////////////////////////////////////////////////////////////////
    //check the input index is legal or not
    //////////////////////////////////////////////////////////////////////////////////
    inline bool inRange(const Index3D& p) const
    {
        return inRange(p.sli,p.row, p.col);
    }
    inline bool inRange(const Range3D& r) const
    {
        return inRange(Index3D(r.start.sli,r.start.row,r.start.col)) && inRange(Index3D(r.end.sli,r.end.row,r.end.col));
    }
    inline bool inRange(int sli, int row, int col) const
    {
        return (0<=sli && sli<m_nSli && 0<=col && col<m_nCol && 0<=row && row<m_nRow);
    }
    inline bool inRange(int id) const
    {
        return m_mat.inRange(id);
    }

    //////////////////////////////////////////////////////////////////////////////////
    //tranforms between id and point
    //////////////////////////////////////////////////////////////////////////////////
    inline int getID(int sli, int row,int col) const
    {
        smart_assert(inRange(sli,row,col), "Input index is out of range");
        return sli*m_nRow*m_nCol+row*m_nCol+col;
    }
    inline int getID(MyBasic::Index3D& p) const
    {
        return getID(p.sli,p.row,p.col);
    }

    //////////////////////////////////////////////////////////////////////////////////
    //get data information
    //////////////////////////////////////////////////////////////////////////////////
    inline bool isEmpty() const
    {
        return m_mat.isEmpty();
    }

    //the 3D size and number of data contained in the object
    inline MyBasic::Size3D getSize() const
    {
        return Size3D(m_nSli,m_nRow,m_nCol);
    }
    inline int getNumCol() const
    {
        return m_nCol;
    }
    inline int getNumRow() const
    {
        return m_nRow;
    }
    inline int getNumSli() const
    {
        return m_nSli;
    }
    inline int getNumData() const
    {
        return m_mat.getNumData();
    }
    inline int getCapacity() const
    {
        return m_mat.getCapacity();
    }

    void resize(int slice, int row, int col)
    {
        smart_assert(slice>=0 && row>=0 && col>=0, "Input size should be positive");
        m_nSli = slice;
        m_nCol = col;
        m_nRow = row;

        m_mat.resize(m_nSli,m_nRow*m_nCol);
    }

    void resize(MyBasic::Size3D _size);

    void clear();
    //////////////////////////////////////////////////////////////////////////////////
    //operator overloading
    //////////////////////////////////////////////////////////////////////////////////

    // basic arithmetic operators
    Data3D<T>& operator+=(const Data3D<T>& arg); // adding another Data3D
    Data3D<T>& operator-=(const Data3D<T>& arg); // subtracting another Data3D
    Data3D<T>& operator+=(const T& val); // adding a constant to each element of the table
    Data3D<T>& operator-=(const T& val); // subtracting a constant from each element of the table
    Data3D<T>& operator*=(const double& s); // multiplication by a constant scalar
    Data3D<T>& operator/=(const double& s);
    Data3D<T> operator()(const MyBasic::Range3D& ROI) const;


protected:
    int m_nCol;
    int m_nRow;
    int m_nSli;
    Data2D<T> m_mat;
};

template <class T>
void Data3D<T> :: resize(MyBasic::Size3D _size)
{
    resize(_size.nSli,_size.nRow,_size.nCol);
}

template <class T>
void Data3D<T> :: clear()
{
    m_nCol=0;
    m_nRow=0;
    m_nSli=0;
    m_mat.clear();
}

//added on May 31
template <class T>
Data3D<T>& Data3D<T> :: operator+=(const Data3D<T>& arg) // adding another Data3D
{
    smart_assert( m_nCol==arg.getNumCol() && m_nRow==arg.getNumRow() && m_nSli == arg.getNumSli(),"Data3D of different size (in operator+=)");
    m_mat += arg.m_mat;
    return (*this);
}

template <class T>
Data3D<T>& Data3D<T> :: operator-=(const Data3D<T>& arg) // subtracting another Data3D
{
    smart_assert( m_nCol==arg.getNumCol() && m_nRow==arg.getNumRow() && m_nSli == arg.getNumSli(),"Data3D of different size (in operator-=)");
    m_mat -= arg.m_mat;
    return (*this);
}

template <class T>
Data3D<T>& Data3D<T> :: operator+=(const T& val) // adding a constant to each element
{
    m_mat += val;
    return (*this);
}

template <class T>
Data3D<T>& Data3D<T> :: operator-=(const T& val) // subtracting a constant from each element
{
    m_mat -= val;
    return (*this);
}

template <class T>
Data3D<T>& Data3D<T> :: operator*=(const double& scalar) // multiplication by a scalar
{
    m_mat *= scalar;
    return (*this);
}

template <class T>
Data3D<T>& Data3D<T> :: operator/=(const double& scalar) // multiplication by a scalar
{
    m_mat /= scalar;
    return (*this);
}

template <class T>
Data3D<T> Data3D<T> :: operator()(const MyBasic::Range3D& ROI) const
{
    Data3D<T> result(ROI.getSize());
    result.copyFrom(*this, ROI,result.getRange());
    return result;
}

///////////////////////////////////////////////////////////////////////////////
// operators for Data3D
//////////////////////////////////////////////////////////////////////////////
template <class T>
Data3D<T> operator*(const Data3D<T>& a, const double& scalar)
{
    Data3D<T> result( a );
    return  result*= scalar;
}

template <class T>
Data3D<T> operator*(const double& scalar, const Data3D<T>& a)
{
    Data3D<T> result( a );
    return  result*= scalar;
}

template <class T>
Data3D<T> operator+(const Data3D<T>& a, const double& val)
{
    Data3D<T> result( a );
    return  result+= (T) val;
}

template <class T>
Data3D<T> operator+(const double& val, const Data3D<T>& a)
{
    Data3D<T> result( a );
    return  result+= (T) val;
}

template <class T>
Data3D<T> operator-(const Data3D<T>& a, const double& val)
{
    Data3D<T> result( a );
    return  result-= (T) val;
}

template <class T>
Data3D<T> operator-(const double& val, const Data3D<T>& a)
{
    Data3D<T> result( a.getSize(), (T) val);
    return  result-= a;
}

template <class T>
Data3D<T> operator+(const Data3D<T>& a, const Data3D<T>& b)
{
    smart_assert( a.getSize() == b.getSize(),"Adding 2 arrays of different size (in operator+)");
    Data3D<T> result( a );
    return  result+=b;
}

template <class T>
Data3D<T> operator-(const Data3D<T>& a, const Data3D<T>& b)
{
    smart_assert( a.getSize() == b.getSize(),"Adding 2 arrays of different size (in operator-)");
    Data3D<T> result( a );
    return  result-=b;
}




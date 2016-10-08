/**
Author:
Liqun Liu (lliu329@uwo.ca)                                   
Danfeng Chen (dchen285@uwo.ca)  

The code is for research use only.
Details about the code please refer to README.txt
**/

#pragma once

#include "common.h"
#include <climits>
#include <math.h>
#include <stdio.h>
namespace MyBasic
{
/*
class Size2D;
class Size3D;
class Range;
class Range2D;
class Range3D;
class Index2D;
class Index3D;
*/

class Size2D
{
public:
    Size2D()
    {
        set(0,0);
    }
    Size2D(int r, int c)
    {
        set(r,c);
    }
    void set(int r, int c)
    {
        nRow = r;
        nCol = c;
    }
    void set(const Size2D& s)
    {
        set(s.nRow,s.nCol);
    }
    bool isZero(void) const
    {
        return nRow==0 || nCol==0;
    }
    bool operator==(const Size2D& op) const
    {
        return Size2D::isEqual(*this,op);
    }
    bool operator !=(const Size2D& op) const
    {
        return nRow!=op.nRow || nCol!=op.nCol;
    }
    Size2D& operator = (const Size2D& op)
    {
        nRow = op.nRow;
        nCol = op.nCol;
        return *this;
    }
    Size2D& operator -= (const Size2D& op)
    {
        nRow -= op.nRow;
        nCol -= op.nCol;
        return *this;
    }
    Size2D& operator += (const Size2D& op)
    {
        nRow += op.nRow;
        nCol += op.nCol;
        return *this;
    }
    Size2D& operator /= (double op)
    {
        nRow /= op;
        nCol /= op;
        return *this;
    }
    Size2D& operator *= (double op)
    {
        nRow *= op;
        nCol *= op;
        return *this;
    }
    static inline bool isEqual(const Size2D& s1, const Size2D& s2)
    {
        return s1.nRow==s2.nRow && s1.nCol==s2.nCol;
    }
    void print() const
    {
       printf("%d %d\n",nRow,nCol);
    }
    int nRow, nCol;
};

class Size3D
{
public:
    Size3D():nRow(0),nCol(0),nSli(0) {}
    Size3D(int s,int r, int c)
    {
        set(s,r,c);
    }
    void set(int s,int r, int c)
    {
        nRow = r;
        nCol = c;
        nSli = s;
    }
    void set(const Size3D& s)
    {
        set(s.nSli, s.nRow, s.nCol);
    }
    bool isZero(void) const
    {
        return nRow==0 || nCol==0 || nSli==0;
    }
    bool operator==(const Size3D& src) const
    {
        return Size3D::isEqual(*this,src);
    }
    bool operator !=(const Size3D& op) const
    {
        return nSli!=op.nSli || nRow!=op.nRow || nCol!=op.nCol;
    }
    Size3D& operator = (const Size3D& op)
    {
        nRow = op.nRow;
        nCol = op.nCol;
        return *this;
    }
    Size3D& operator -= (const Size3D& op)
    {
        nSli -= op.nSli;
        nRow -= op.nRow;
        nCol -= op.nCol;
        return *this;
    }
    Size3D& operator += (const Size3D& op)
    {
        nSli += op.nSli;
        nRow += op.nRow;
        nCol += op.nCol;
        return *this;
    }
    Size3D& operator /= (double op)
    {
        nSli /= op;
        nRow /= op;
        nCol /= op;
        return *this;
    }
    Size3D& operator *= (double op)
    {
        nSli *= op;
        nRow *= op;
        nCol *= op;
        return *this;
    }
    static inline bool isEqual(const Size3D& s1, const Size3D& s2)
    {
        return s1.nRow==s2.nRow && s1.nCol==s2.nCol && s1.nSli==s2.nSli;
    }
    void print() const
    {
       printf("%d %d %d\n",nSli,nRow,nCol);
    }
    int nRow,nCol,nSli;
};

class Index2D
{
public:
    Index2D()
    {
        set(0,0);
    }
    Index2D(const Index2D& src)
    {
        row = src.row;
        col = src.col;
    }
    Index2D(const Size2D& src)
    {
        row = src.nRow;
        col = src.nCol;
    }
    Index2D(int r, int c)
    {
        set(r,c);
    }
    void set(int r, int c)
    {
        row = r;
        col = c;
    }
    void set(const Index2D& ind)
    {
        set(ind.row, ind.col);
    }
    Index2D& operator =(const Index2D& src)
    {
        set(src);
        return *this;
    }

    Index2D& operator =(const Size2D& src)
    {
        set(src.nRow,src.nCol);
        return *this;
    }
    bool operator == (const Index2D& op) const
    {
        return row==op.row && col==op.col;
    }
    bool operator !=(const Index2D& op) const
    {
        return row!=op.row || col!=op.col;
    }
    Index2D& operator +=(const Index2D& op)
    {
        row += op.row;
        col += op.col;
        return *this;
    }

    Index2D& operator +=(const Size2D& op)
    {
        row += op.nRow;
        col += op.nCol;
        return *this;
    }

    Index2D& operator -=(const Index2D& op)
    {
        row -=op.row;
        col-=op.col;
        return *this;
    }
    Index2D& operator -=(const Size2D& op)
    {
        row -= op.nRow;
        col -= op.nCol;
        return *this;
    }
    Index2D& operator /=(int op)
    {
        row = row/op;
        col = col/op;
        return *this;
    }
    bool isValid() const
    {
        return row>=0 && col>=0;
    }
    double norm() const
    {
       return sqrt(double(row*row+col*col));
    }
    void print() const
    {
       printf("%d %d\n",row,col);
    }
    int row, col;
};

class Index3D
{
public:
    Index3D()
    {
        set(0,0,0);
    }
    Index3D(const Index3D& src)
    {
        set(src.sli,src.row,src.col);
    }
    Index3D(const Size3D& src)
    {
        sli = src.nSli;
        row = src.nRow;
        col = src.nCol;
    }
    Index3D(int s,int r, int c)
    {
        set(s,r,c);
    }
    void set(int s,int r, int c)
    {
        sli = s;
        row = r;
        col = c;
    }
    void set(const Index3D& ind)
    {
        set(ind.sli, ind.row, ind.col);
    }

    Index3D& operator =(const Index3D& src)
    {
        set(src);
        return *this;
    }
    Index3D& operator =(const Size3D& src)
    {
        set(src.nSli,src.nRow,src.nCol);
        return *this;
    }
    bool operator ==(const Index3D& op)
    {
        return sli==op.sli && row==op.row && col==op.col;
    }
    bool operator !=(const Index3D& op) const
    {
        return sli!=op.sli || row!=op.row || col!=op.col;
    }
    Index3D& operator +=(const Index3D& op)
    {
        sli += op.sli;
        row += op.row;
        col += op.col;
        return *this;
    }
    Index3D& operator +=(const Size3D& op)
    {
        sli+=op.nSli;
        row += op.nRow;
        col += op.nCol;
        return *this;
    }
    Index3D& operator -=(const Index3D& op)
    {
        sli-=op.sli;
        row -=op.row;
        col-=op.col;
        return *this;
    }
    Index3D& operator -=(const Size3D& op)
    {
        sli-=op.nSli;
        row -= op.nRow;
        col -= op.nCol;
        return *this;
    }
    Index3D& operator /=(int op)
    {
        sli = sli/op;
        row = row/op;
        col = col/op;
        return *this;
    }
    bool isValid() const
    {
        return row>=0 && col>=0 && sli>=0;
    }
    double norm() const
    {
       return sqrt(double(row*row+col*col));
    }
    void print() const
    {
       printf("%d %d %d\n",sli,row,col);
    }
    int sli,row, col;
};

class Range
{
public:
    Range()
    {
        start=0;
        end = 0;
    }
    Range(const Range& src)
    {
        start = src.start;
        end = src.end;
    }
    Range(int _start, int _end)
    {
        start=_start;
        end=_end;
    }
    int getSize() const
    {
        return end-start+1;
    }
    bool isEmpty() const
    {
        return (0==getSize());
    }
    static inline Range all()
    {
        return Range(INT_MIN, INT_MAX);
    }
    bool operator == (const Range& r) const
    {
       return start==r.start && end==r.end;
    }
    Range& operator =(const Range& src)
    {
        start = src.start;
        end = src.end;
        return *this;
    }
    Range& operator = (Range& r)
    {
       start = r.start;
       end = r.end;
       return *this;
    }
    Range& operator += (int p)
    {
        start += p;
        end += p;
        return *this;
    }

    Range& operator -= (int p)
    {
        start -= p;
        end -= p;
        return *this;
    }
    void print() const
    {
       printf("%d %d\n",start,end);
    }

public:
    int start, end;
};

class Range2D
{
public:
    Range2D() { }
    Range2D(const Range2D& src):start(src.start),end(src.end) {}
    Range2D(const Index2D& top_left,const Index2D& bottom_right):start(top_left),end(bottom_right) {}
    Range2D(int str,int edr,int stc, int edc)
    {
        start.row = str;
        start.col = stc;
        end.row = edr;
        end.col = edc;
    }
    Range2D(const Range& r,const Range& c)
    {
        start.row = r.start;
        start.col = c.start;
        end.row = r.end;
        end.col = c.end;
    }
    Range getRange(DIRECTION d) const
    {
       if(d==ROW) return Range(start.row,end.row);
       else if(d==COLUMN) return Range(start.col,end.col);
       else return Range::all();
    }
    Range getRowRange() const
    {
       return getRange(ROW);
    }
    Range getColRange() const
    {
       return getRange(COLUMN);
    }
    Size2D getSize() const
    {
        return Size2D(getRowRange().getSize(),getColRange().getSize());
    }
    int getWidth() const
    {
        return end.col-start.col+1;
    }
    int getHeight() const
    {
        return end.row-start.row+1;
    }
    int area() const
    {
       Size2D s = getSize();
       return s.nRow*s.nCol;
    }
    bool isEmpty() const
    {
        return getRowRange().isEmpty() || getColRange().isEmpty();
    }
    static inline Range2D all()
    {
        return Range2D(Range::all(),Range::all());
    }
    bool inRange(const Index2D& p)
    {
        return p.row>=start.row&&p.row<=end.row&&p.col>=start.col&&p.col<=end.col;
    }
    bool operator == (const Range2D& src) const
    {
        return start==src.start && end==src.end;
    }
    Range2D& operator = (const Range2D& src)
    {
        start = src.start;
        end = src.end;
        return *this;
    }
    Range2D& operator += (Index2D p)
    {
        start += p;
        end += p;
        return *this;
    }
    Range2D& operator -= (Index2D p)
    {
        start -= p;
        end -= p;
        return *this;
    }
    void print() const
    {
       printf("%d %d %d %d\n",start.row,end.row,start.col,end.col);
    }

public:
    Index2D start;
    Index2D end;
};

class Range3D
{
public:
    Range3D() {}
    Range3D(int sts,int eds,int str,int edr,int stc, int edc)
    {
        start.sli = sts;
        start.row = str;
        start.col = stc;
        end.sli = eds;
        end.row = edr;
        end.col = edc;
    }
    Range3D(const Range& s, const Range& r, const Range& c)
    {
        start.sli = s.start;
        end.sli = s.end;
        start.row = r.start;
        end.row = r.end;
        start.col = c.start;
        end.col = c.end;
    }
    Range3D(const Index3D& corner1,const Index3D corner2):start(corner1),end(corner2) {}

    Range getRowRange() const
    {
        return Range(start.row,end.row);
    }
    Range getColRange() const
    {
        return Range(start.col,end.col);
    }
    Range getSliRange() const
    {
        return Range(start.sli,end.sli);
    }
    int volume() const
    {
        Size3D s = getSize();
        return s.nSli*s.nRow*s.nCol;
    }
    int getWidth() const
    {
        return end.col-start.col+1;
    }
    int getHeight() const
    {
        return end.row-start.row+1;
    }
    int getLength() const
    {
        return end.sli-start.sli+1;
    }
    Size3D getSize()const
    {
        return Size3D(getSliRange().getSize(),getRowRange().getSize(),getColRange().getSize());
    }

    bool isEmpty() const
    {
        return getSliRange().isEmpty() || getRowRange().isEmpty() || getColRange().isEmpty();
    }
    static inline Range3D all()
    {
        return Range3D(Range::all(),Range::all(), Range::all());
    }
    bool inRange(const Index3D& p)
    {
        return (p.sli>=start.sli&&p.sli<=end.sli)
               &&(p.row>=start.row&&p.row<=end.row)
               &&(p.col>=start.col&&p.col<=end.col);
    }
    bool operator == (const Range3D& src)
    {
        return start==src.start && end==src.end;
    }
    Range3D& operator = (const Range3D& src)
    {
        start = src.start;
        end = src.end;
        return *this;
    }
    Range3D& operator += (Index3D p)
    {
        start += p;
        end += p;
        return *this;
    }
    Range3D& operator -= (Index3D p)
    {
        start -= p;
        end += p;
        return *this;
    }
    void print() const
    {
       printf("%d %d %d %d %d %d\n",start.sli,end.sli,start.row,end.row,start.col,end.col);
    }
public:
    Index3D start;
    Index3D end;
};
}

//operations for basic data structures
inline MyBasic::Size2D operator +(const MyBasic::Size2D& op1, const MyBasic::Size2D& op2)
{
    MyBasic::Size2D result(op1);
    return result+=op2;

}

inline MyBasic::Size2D operator-(const MyBasic::Size2D& op1, const MyBasic::Size2D& op2)
{
    MyBasic::Size2D result;
    result = op1;
    return result-=op2;
}
inline MyBasic::Size2D operator/(const MyBasic::Size2D& op1, double op2)
{
    MyBasic::Size2D result(op1);
    return result/=op2;
}
inline MyBasic::Size3D operator+(const MyBasic::Size3D& op1, const MyBasic::Size3D& op2)
{
    MyBasic::Size3D result(op1);
    return result+=op2;
}

inline MyBasic::Size3D operator-(const MyBasic::Size3D& op1, const MyBasic::Size3D& op2)
{
    MyBasic::Size3D result(op1);
    return result-=op2;
}
inline MyBasic::Size3D operator/(const MyBasic::Size3D& op1, double op2)
{
    MyBasic::Size3D result(op1);
    return result/=op2;
}

inline MyBasic::Index2D operator +(const MyBasic::Index2D& op1, const MyBasic::Index2D& op2)
{
    MyBasic::Index2D result(op1);
    return result+=op2;

}

inline MyBasic::Index2D operator-(const MyBasic::Index2D& op1, const MyBasic::Index2D& op2)
{
    MyBasic::Index2D result;
    result = op1;
    return result-=op2;
}
inline MyBasic::Index2D operator/(const MyBasic::Index2D& op1, int op2)
{
    MyBasic::Index2D result(op1);
    return result/=op2;
}
inline MyBasic::Index3D operator+(const MyBasic::Index3D& op1, const MyBasic::Index3D& op2)
{
    MyBasic::Index3D result(op1);
    return result+=op2;
}

inline MyBasic::Index3D operator-(const MyBasic::Index3D& op1, const MyBasic::Index3D& op2)
{
    MyBasic::Index3D result(op1);
    return result-=op2;
}
inline MyBasic::Index3D operator/(const MyBasic::Index3D& op1, int op2)
{
    MyBasic::Index3D result(op1);
    return result/=op2;
}
inline MyBasic::Range2D operator +(const MyBasic::Range2D& op1, const MyBasic::Index2D& op2)
{
    MyBasic::Range2D result(op1);
    return result+=op2;

}

inline MyBasic::Range2D operator-(const MyBasic::Range2D& op1, const MyBasic::Index2D& op2)
{
    MyBasic::Range2D result;
    result = op1;
    return result-=op2;
}

inline MyBasic::Range3D operator+(const MyBasic::Range3D& op1, const MyBasic::Index3D& op2)
{
    MyBasic::Range3D result(op1);
    return result+=op2;
}

inline MyBasic::Range3D operator-(const MyBasic::Range3D& op1, const MyBasic::Index3D& op2)
{
    MyBasic::Range3D result(op1);
    return result-=op2;
}

/*
inline std::string operator <<(const MyBasic::Index2D& op)
{
  std::ostringstream os;
  os<<op.row;
  os<<" ";
  os<<op.col;
  return os.str();
}

inline std::string operator <<(const MyBasic::Index3D& op)
{
  std::ostringstream os;
   os<<op.sli;
  os<<" ";
  os<<op.row;
  os<<" ";
  os<<op.col;
  os<<" ";
  return os.str();
}

inline std::string operator <<(const MyBasic::Size2D& op)
{
  MyBasic::Index2D tmp(op);
  return <<tmp;
}

inline std::string operator <<(const MyBasic::Size3D& op)
{
  MyBasic::Index2D tmp(op);
  return <<tmp;
}

inline std::string operator <<(const MyBasic::Range2D& op)
{
  std::string start,end;
  start = <<op.start;
  end = <<op.end;
  return start+end;
}

inline std::string operator <<(const MyBasic::Size3D& op)
{
  std::string start,end;
  start = <<op.start;
  end = <<op.end;
  return start+end;
}

*/

#include "MyMath.h"

MyBasic::Size2D abs(const MyBasic::Size2D& s)
{
    return MyBasic::Size2D(abs(s.nRow),abs(s.nCol));
}

MyBasic::Index2D abs(const MyBasic::Index2D& i)
{
    return MyBasic::Size2D(abs(i.row),abs(i.col));
}

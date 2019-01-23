#ifndef _X_LINE_H
#define _X_LINE_H

#include "XPoint.hpp"

class XLine{
public:
	XLine(){}
	XLine(const XPoint& pt1, const XPoint& pt2):_pt1(pt1), _pt2(pt2){}
	XLine(int x1, int y1, int x2, int y2) :_pt1(x1, y1), _pt2(x2, y2){}

	bool isNull()const{ return _pt1 == _pt2; }

	const XPoint& pt1()const{ return _pt1; }
	const XPoint& pt2()const{ return _pt2; }

	int x1()const{ return _pt1.x(); }
	int y1()const{ return _pt1.y(); }
	int x2()const{ return _pt2.x(); }
	int y2()const{ return _pt2.y(); }

	void setP1(const XPoint& pt1){ _pt1 = pt1; }
	void setP2(const XPoint& pt2){ _pt2 = pt2; }
	void setPoints(const XPoint& pt1, const XPoint& pt2){ _pt1 = pt1; _pt2 = pt2; }
	void setLine(int x1, int y1, int x2, int y2){ _pt1.set(x1, y1); _pt2.set(x2, y2); }

	bool operator==(const XLine& l)const{ return _pt1 == l._pt1&&_pt2 == l._pt2; }
	bool operator!=(const XLine& l)const{ return !(*this == l); }

private:
	XPoint	_pt1;
	XPoint	_pt2;
};

#endif //_X_LINE_H


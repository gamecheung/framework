#ifndef _X_POINT_H
#define _X_POINT_H

template <typename _Type>
class _XPoint{
public:
	_XPoint(_Type x=0, _Type y=0):_x(x),_y(y){}

	void setX(_Type x){_x=x;}
	void setY(_Type y){_y=y;}
	void set(_Type x, _Type y){_x=x;_y=y;}

	_Type x()const{return _x;}
	_Type y()const{return _y;}

	void transpose()
	{
		_Type _t = _x;
		_x = _y;
		_y = _t;
	}

	_XPoint<_Type> transposed() const
	{
		return _XPoint<_Type>(_y,_x);
	}

	_XPoint<_Type> operator+(const _XPoint<_Type>& p) const
	{ return _XPoint<_Type>(_x+p._x,_y+p._y); }

	_XPoint<_Type> operator-(const _XPoint<_Type>& p) const
	{ return _XPoint<_Type>(_x-p._x,_y-p._y); }

	_XPoint<_Type> operator*(double factor) const
	{ return _XPoint<_Type>(_x*factor,_y*factor); }

	_XPoint<_Type> operator/(double factor) const
	{ return _XPoint<_Type>(_x/factor,_y/factor); }

	friend _XPoint<_Type> operator*(double f,const _XPoint<_Type>& p)
	{ return p*f; }

	//friend _Point<_Type> operator/(double f,const _Point<_Type>& s)
	//{ return s/f; }


	_XPoint<_Type>& operator+=(const _XPoint<_Type>& p)
	{ _x+=p._x; _y+=p._y; return *this; }

	_XPoint<_Type>& operator-=(const _XPoint<_Type>& p)
	{ _x-=p._x; _y-=p._y; return *this; }

	_XPoint<_Type>& operator*=(double factor)
	{ _x*=factor; _y*=factor; return (*this); }

	_XPoint<_Type>& operator/=(double factor)
	{ _x/=factor; _y/=factor; return (*this); }


	friend  bool operator>(const _XPoint<_Type> &p1, const _XPoint<_Type> &p2)
	{ return p1._x > p2._x&&p1._y > p2._y; }
	friend  bool operator<(const _XPoint<_Type> &p1, const _XPoint<_Type> &p2)
	{ return p1._x < p2._x&&p1._y < p2._y; }
	friend  bool operator==(const _XPoint<_Type> &p1, const _XPoint<_Type> &p2)
	{ return p1._x==p2._x && p1._y==p2._y; }
	friend  bool operator!=(const _XPoint<_Type> &p1, const _XPoint<_Type> &p2)
	{ return !(p1==p2);}
	friend  bool operator>=(const _XPoint<_Type> &p1, const _XPoint<_Type> &p2)
	{ return p1 > p2 || p1 == p2;  }
	friend  bool operator<=(const _XPoint<_Type> &p1, const _XPoint<_Type> &p2)
	{ return p1 < p2 || p1 == p2; }

	bool isNull()const{return _x==0&&_y==0;}

private:
	_Type	_x;
	_Type	_y;
};

typedef _XPoint<int>		XPoint;
typedef _XPoint<double>		XPointF;

#endif //_X_POINT_H

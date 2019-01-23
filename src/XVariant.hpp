#ifndef _X_VARIANT_HPP
#define _X_VARIANT_HPP

#include "XString.hpp"

class XVariant{
public:
	enum Type{Invalid,Bool,Char,Color,Date,Time,DateTime,Double,Int,Line,Point,String,StringList,
		Rect};
	XVariant(int val) :_type(Int), _dInt(val){}
	XVariant(double val) :_type(Double), _dDouble(val){}
	XVariant(bool b) :_type(Bool), _dBool(b){}

	int toInt()const{ return _dInt; }
	double toDouble()const{ return _dDouble; }
	bool toBool()const{ return _dBool; }

private:
	Type	_type;
	int		_dInt;
	double	_dDouble;
	bool	_dBool;
};

#endif //_X_VARIANT_HPP

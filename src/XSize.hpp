#ifndef _X_SIZE_H
#define _X_SIZE_H

#include "XNamespace.h"
#include "XString.hpp"

template <class _Type>
class _XSize{
public:
	_XSize(_Type w=0, _Type h=0):_w(w),_h(h){}

	void setWidth(_Type w){_w=w;}
	void setHeight(_Type h){_h=h;}
	void set(_Type w, _Type h){_w=w;_h=h;}

	_Type width()const{return _w;}
	_Type height()const{return _h;}

	void transpose()
	{
		_Type _t = _w;
		_w = _h;
		_h = _t;
	}

	_XSize<_Type> transposed() const
	{
		return _XSize<_Type>(_h,_w);
	}

	_XSize<_Type> scaled(const _XSize<_Type>& sz, Xd::AspectRatioMode mode = Xd::IgnoreAspectRatio){
		if (mode == Xd::IgnoreAspectRatio){
			return sz;
		}
		bool useHeight;
		_Type rw = sz._h*_w / _h;
		if (mode == Xd::KeepAspectRatio){
			useHeight = (rw <= sz._w);
		}
		else
		{
			useHeight = (rw >= sz._w);
		}

		if (useHeight){
			return _XSize(rw, sz._h);
		}
		else{
			return _XSize(sz._w, sz._w*_h / _w);
		}
	}
	void scale(const _XSize<_Type>& sz, Xd::AspectRatioMode mode = Xd::IgnoreAspectRatio){
		*this = scaled(sz, mode);
	}

	XString toString()const{ return XString("%1,%2").arg(_w).arg(_h); }

	_XSize<_Type> operator+(const _XSize<_Type>& s) const
	{ return _XSize<_Type>(_w+s._w,_h+s._h); }

	_XSize<_Type> operator-(const _XSize<_Type>& s) const
	{ return _XSize<_Type>(_w-s._w,_h-s._h); }

	_XSize<_Type> operator*(double factor) const
	{ return _XSize<_Type>(_w*factor,_h*factor); }

	_XSize<_Type> operator/(double factor) const
	{ return _XSize<_Type>(_w/factor,_h/factor); }

	friend _XSize<_Type> operator*(double f,const _XSize<_Type>& s)
	{ return s*f; }

	//friend _XSize<_Type> operator/(double f,const _XSize<_Type>& s)
	//{ return s/f; }


	_XSize<_Type>& operator+=(const _XSize<_Type>& s)
	{ _w+=s._w; _h+=s._h; return *this; }

	_XSize<_Type>& operator-=(const _XSize<_Type>& s)
	{ _w-=s._w; _h-=s._h; return *this; }

	_XSize<_Type>& operator*=(double factor)
	{ _w*=factor; _h*=factor; return (*this); }

	_XSize<_Type>& operator/=(double factor)
	{ _w/=factor; _h/=factor; return (*this); }

	friend  bool operator==(const _XSize<_Type> &s1, const _XSize<_Type> &s2)
	{ return s1._w==s2._w && s1._h==s2._h; }
	friend  bool operator!=(const _XSize<_Type> &s1, const _XSize<_Type> &s2)
	{ return !(s1==s2);}

	
	static _XSize<_Type> fromString(const XString& str){ return fromString<_Type>(str); }

private:
	template <typename _Ty>
	static _XSize<_Ty> fromString(const XString& str){ return _XSize<_Ty>(); }

	template <>
	static _XSize<int> fromString<int>(const XString& str){
		_XSize<int> retval;
		XStringList strs = str.split(",");
		if (strs.count() == 2){
			retval.set(strs.at(0).toInt(), strs.at(1).toInt());
		}
		return retval;
	}

protected:
	_Type	_w;
	_Type	_h;
};

typedef _XSize<int>		XSize;
typedef _XSize<double>	XSizeF;


//template <>
//class _XSize<int>{
//public:
//	static _XSize<int> fromString(const XString& str){
//		_XSize<int> retval;
//	    XStringList strs = str.split(",");
//	    if (strs.count() == 2){
//			
//	        retval.set(strs.at(0).toInt(), strs.at(1).toInt());
//	    }
//	    return retval;
//	}
//};




#endif //_X_SIZE_H

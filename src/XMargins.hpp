#ifndef _X_MARGINS_HPP
#define _X_MARGINS_HPP

template <class _Type>
class _XMargins{
public:
	_XMargins(_Type left=0,_Type top=0,_Type right=0,_Type bottom=0)
		:_left(left), _top(top), _right(right), _bottom(bottom){}
	_Type left()const{ return _left; }
	_Type top()const{ return _top; }
	_Type right()const{ return _right; }
	_Type bottom()const{ return _bottom; }
	void setLeft(_Type left){ _left = left; }
	void setTop(_Type top){ _top = top; }
	void setRight(_Type right){ _right = right; }
	void setBottom(_Type bottom){ _bottom = bottom; }
	void set(_Type left, _Type top, _Type right, _Type bottom){
		_left = left; _top = top; _right = right; _bottom = bottom;
	}

private:
	_Type _left;
	_Type _top;
	_Type _right;
	_Type _bottom;
};

typedef _XMargins<int> XMargins;
typedef _XMargins<double> XMarginsF;


#endif //_X_MARGINS_HPP

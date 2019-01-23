#ifndef _X_RECT_HPP
#define _X_RECT_HPP

#include "XPoint.hpp"
#include "XSize.hpp"
#include "XMargins.hpp"
#include "XString.hpp"

template <class _Ty>
class _XRect{
public:
	_XRect(const _XPoint<_Ty>& topLeft, const _XPoint<_Ty>& bottomRight)
		:_x1(topLeft.x()), _y1(topLeft.y()), _x2(bottomRight.x()), _y2(bottomRight.y()){}
	_XRect(const _XPoint<_Ty>& topLeft, const _XSize<_Ty>&  size)
		: _x1(topLeft.x()), _y1(topLeft.y()), _x2(topLeft.x() + size.width() - 1), _y2(topLeft.y()+size.height()-1){}
	_XRect(_Ty left = 0, _Ty top = 0, _Ty width = -1, _Ty height = -1)
		:_x1(left), _y1(top), _x2(left + width - 1), _y2(top + height - 1){}
	//virtual ~_XRect(){}

	bool isNull()const{ return _x2 == _x1 - 1 && _y2 == _y1 - 1; }
	bool isEmpty()const{ return _x1 > _x2 || _y1 > _y2; }
	bool isValid()const{ return _x1 <= _x2&&_y1 <= _y2; }

	_Ty left()const{ return _x1; }
	_Ty top()const{ return _y1; }
	_Ty right()const{ return _x2; }
	_Ty bottom()const{ return _y2; }
	void setLeft(_Ty left){ _x1 = left; }
	void setTop(_Ty top){ _y1 = top; }
	void setRight(_Ty right){ _x2 = right; }
	void setBottom(_Ty bottom){ _y2 = bottom; }
	_XRect<_Ty>  normalized()const;

	_Ty x()const{ return _x1; }
	_Ty y()const{ return _y1; }
	_Ty width()const{ return _x2 - _x1 + 1; }
	_Ty height()const{ return _y2 - _y1 + 1; }
	_XSize<_Ty>  size()const{ return _XSize<_Ty>(width(), height()); }
	void setX(_Ty x){ _x1 = x; }
	void setY(_Ty y){ _y1 = y; }
	void setWidth(_Ty width){ _x2 = (_x1 + width - 1); }
	void setHeight(_Ty height){ _y2 = (_y1 + height - 1); }
	void setSize(const _XSize<_Ty> & size){
		setWidth(size.width());
		setHeight(size.height());
	}

	_XPoint<_Ty>  topLeft()const{ return _XPoint<_Ty>(_x1, _y1); }
	_XPoint<_Ty>  topRight()const{ return _XPoint<_Ty>(_x2, _y1); }
	_XPoint<_Ty>  bottomLeft()const{ return _XPoint<_Ty>(_x1, _y2); }
	_XPoint<_Ty>  bottomRight()const{ return _XPoint<_Ty>(_x2, _y2); }
	_XPoint<_Ty>  center()const{ return _XPoint<_Ty>((_x1 + _x2) / 2, (_y1 + _y2) / 2); }
	void setTopLeft(const _XPoint<_Ty> & p){ _x1 = p.x(); _y1 = p.y(); }
	void setTopRight(const _XPoint<_Ty> & p){ _x2 = p.x(); _y1 = p.y(); }
	void setBottomLeft(const _XPoint<_Ty> & p){ _x1 = p.x(); _y2 = p.y(); }
	void setBottomRight(const _XPoint<_Ty> & p){ _x2 = p.x(); _y2 = p.y(); }

	void moveLeft(_Ty p){ _x2 += (p - _x1); _x1 = p; }
	void moveTop(_Ty p){ _y2 += (p - _y1); _y1 = p; }
	void moveRight(_Ty p){ _x1 += (p - _x2); _x2 = p; }
	void moveBottom(_Ty p){ _y1 += (p - _y2); _y2 = p; }
	void moveTopLeft(const _XPoint<_Ty> & p){ moveTop(p.y()); moveLeft(p.x()); }
	void moveTopRight(const _XPoint<_Ty> & p){ moveTop(p.y()); moveRight(p.x()); }
	void moveBottomLeft(const _XPoint<_Ty> & p){ moveBottom(p.y()); moveLeft(p.x()); }
	void moveBottomRight(const _XPoint<_Ty> & p){ moveBottom(p.y()); moveRight(p.x()); }
	void moveCenter(const _XPoint<_Ty>& p){
		_Ty w = _x2 - _x1;
		_Ty h = _y2 - _y1;
		_x1 = p.x() - w / 2;
		_y1 = p.y() - h / 2;
		_x2 = _x1 + w;
		_y2 = _y1 + h;
	}
	void moveTo(const _XPoint<_Ty> & p);
	const _XRect<_Ty>& move(const _XPoint<_Ty>& p){
		_x1 += p.x();
		_x2 += p.x();
		_y1 += p.y();
		_y2 += p.y();
		return *this;
	}

	bool contains(const _XRect<_Ty> & r, bool proper = false)const{
		if (isNull() || r.isNull())
			return false;

		_Ty l1 = _x1;
		_Ty r1 = _x1;
		if (_x2 - _x1 + 1 < 0)
			l1 = _x2;
		else
			r1 = _x2;

		_Ty l2 = r._x1;
		_Ty r2 = r._x1;
		if (r._x2 - r._x1 + 1 < 0)
			l2 = r._x2;
		else
			r2 = r._x2;

		if (proper) {
			if (l2 <= l1 || r2 >= r1)
				return false;
		}
		else {
			if (l2 < l1 || r2 > r1)
				return false;
		}

		_Ty t1 = _y1;
		_Ty b1 = _y1;
		if (_y2 - _y1 + 1 < 0)
			t1 = _y2;
		else
			b1 = _y2;

		_Ty t2 = r._y1;
		_Ty b2 = r._y1;
		if (r._y2 - r._y1 + 1 < 0)
			t2 = r._y2;
		else
			b2 = r._y2;

		if (proper) {
			if (t2 <= t1 || b2 >= b1)
				return false;
		}
		else {
			if (t2 < t1 || b2 > b1)
				return false;
		}

		return true;
	}
	bool contains(const _XPoint<_Ty> & p, bool proper = false)const{
		_Ty l, r;
		if (_x2 < _x1 - 1){
			l = _x2;
			r = _x1;
		}
		else{
			l = _x1;
			r = _x2;
		}

		if (proper){
			if (p.x() <= l || p.x() >= r)
				return false;
		}
		else{
			if (p.x() < l || p.x() > r)
				return false;
		}

		_Ty t, b;
		if (_y2 < _y1 - 1){
			t = _y2;
			b = _y1;
		}
		else{
			t = _y1;
			b = _y2;
		}
		if (proper){
			if (p.y() <= t || p.y() >= b)
				return false;
		}
		else
		{
			if (p.y() < t || p.y() > b)
				return false;
		}

		return true;
	}
	bool intersects(const _XRect<_Ty> & r)const{
		if (isNull() || r.isNull())
			return false;

		_Ty l1 = _x1;
		_Ty r1 = _x1;
		if (_x2 - _x1 + 1 < 0)
			l1 = _x2;
		else
			r1 = _x2;

		_Ty l2 = r._x1;
		_Ty r2 = r._x1;
		if (r._x2 - r._x1 + 1 < 0)
			l2 = r._x2;
		else
			r2 = r._x2;

		if (l1 > r2 || l2 > r1)
			return false;

		_Ty t1 = _y1;
		_Ty b1 = _y1;
		if (_y2 - _y1 + 1 < 0)
			t1 = _y2;
		else
			b1 = _y2;

		_Ty t2 = r._y1;
		_Ty b2 = r._y1;
		if (r._y2 - r._y1 + 1 < 0)
			t2 = r._y2;
		else
			b2 = r._y2;

		if (t1 > b2 || t2 > b1)
			return false;

		return true;
	}

	_XRect<_Ty> marginsAdded(const _XMargins<_Ty>& margins)const{
		return _XRect<_Ty>(topLeft() - _XPoint<_Ty>(margins.left(), margins.top()),
			bottomRight() + _XPoint<_Ty>(margins.right(), margins.bottom()));
	}
	_XRect<_Ty> marginsRemoved(const _XMargins<_Ty>& margins)const{
		return _XRect<_Ty>(topLeft() + _XPoint<_Ty>(margins.left(), margins.top()),
			bottomRight() - _XPoint<_Ty>(margins.right(), margins.bottom()));
	}

	XString toString()const{
		XStringList lst;
		lst.append(XString::number(_x1));
		lst.append(XString::number(_x2));
		lst.append(XString::number(_y1));
		lst.append(XString::number(_y2));
		return lst.join(",");
	}
	_XRect<_Ty> fromString(const XString& str){
		XStringList lst = str.split(",");
		if (lst.count() == 4){
			_x1 = lst.at(0).toInt();
			_x2 = lst.at(1).toInt();
			_y1 = lst.at(2).toInt();
			_y2 = lst.at(3).toInt();
		}
		return *this;
	}

	//friend bool operator==(const XRect & rect1, const XRect & rect2);
	//friend bool operator!=(const XRect & rect1, const XRect & rect2);

protected:
	_Ty _x1;
	_Ty _x2;
	_Ty _y1;
	_Ty _y2;
};

typedef _XRect<int> XRect;
typedef _XRect<double> XRectF;

#endif // _X_RECT_HPP

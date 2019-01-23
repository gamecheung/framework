#ifndef _X_COLOR_HPP
#define _X_COLOR_HPP

#include "XString.hpp"
#include "XGlobal.h"

class XColor{
public:
	enum Spec {Rgb, Hsv, Cmyk, Hsl };
	XColor(int r=-1, int g=-1, int b=-1, int a = 255){ setRgb(r, g, b, a); }

	bool isValid()const{
		return ((0 <= _red&&_red <= 255)
			&& (0 <= _green&&_green <= 255)
			&& (0 <= _blue&&_blue <= 255)
			&& (0 <= _alpha&&_alpha <= 255));
	}

	// r:0-255 g:0-255 b:0-255 a:0-255
	void setRgb(int r, int g, int b, int a = 255){
		_red = r; _green = g; _blue = b; _alpha = a;
	}
	void setRgbF(double r, double g, double b, double a=1){
		setRgb((int)(r * 255), (int)(g * 255), (int)(b * 255), (int)(a * 255));
	}
	void getRgb(int* r, int* g, int* b, int* a = nullptr)const{
		assert(r&&g&&b);
		*r = _red; *g = _green; *b = _blue;
		if (a) *a = _alpha;
	}
	int red()const{ return  _red; }
	int green()const{ return _green; }
	int blue()const{ return _blue; }
	int alpha()const{return _alpha;}
	double redF()const{ return red() / 255.0; }
	double greenF()const{ return green() / 255.0; }
	double blueF()const{ return blue() / 255.0; }
	double alphaF()const{ return alpha() / 255.0; }

	// h:0-359 s:0-255 v:0-255 a:0-255
	void setHsv(int hue, int saturation, int value, int alpha = 255){
		int h = hue;
		double s = saturation / 255.0;
		double v = value / 255.0;
		double a = alpha / 255.0;

		int i = h / 60;
		double f = h / 60.0 - i;
		double p = v*(1 - s);
		double q = v*(1 - f*s);
		double t = v*(1 - (1 - f)*s);

		if (i == 0)			setRgbF(v, t, p, a);
		else if (i == 1)	setRgbF(q, v, p, a);
		else if (i == 2)	setRgbF(p, v, t, a);
		else if (i == 3)	setRgbF(p, q, v, a);
		else if (i == 4)	setRgbF(t, p, v, a);
		else if (i == 5)	setRgbF(v, p, q, a);
	}
	void getHsv(int* h, int* s, int* v, int* a=nullptr)const{
		assert(h&&s&&v);

		int max = xMax(xMax(_red, _green), _blue);
		int min = xMin(xMin(_red, _green), _blue);
		if (max == min)	*h = 0;
		else if (max == _red)	*h = (_green - _blue) * 60 / (max - min);
		else if (max == _green)	*h = (_blue - _red) * 60 / (max - min) + 120;
		else *h = (_red - _green) * 60 / (max - min) + 240;

		*s = max ? (int)((max - min)*255.0 / max) : 0;
		*v = max;

		if (a) *a = _alpha;
	}
	int hue()const{ int h, s, v; getHsv(&h, &s, &v); return h; }
	int saturation()const{ int h, s, v; getHsv(&h, &s, &v); return s; }
	int value()const{ int h, s, v; getHsv(&h, &s, &v); return v; }
	double hueF()const{ return hue() / 255.0; }
	double saturationF()const{ return saturation() / 255.0; }
	double valueF()const{ return value() / 360.0; }

	static XColor fromString(const XString& str){
		auto vals = str.split(",");
		if (vals.count() >= 3){
			return XColor(vals.at(0).toInt(),vals.at(1).toInt(),vals.at(2).toInt(),
				(vals.count() == 4) ? vals.at(3).toInt() : 255);
		}
		return XColor();
	}
	XString toString(bool alpha=true)const{
		return XString(alpha ? "%1,%2,%3,%4" : "%1,%2,%3")
			.arg(_red).arg(_green).arg(_blue).arg(_alpha);
	}

	friend  bool operator==(const XColor& c1, const XColor& c2)
	{
		return ((c1._red == c2._red)
			&& (c1._green == c2._green)
			&& (c1._blue == c2._blue)
			&& (c1._alpha == c2._alpha));
	}
	friend  bool operator!=(const XColor& c1, const XColor& c2)
	{ return !(c1 == c2); }

	friend std::wostream& operator <<(std::wostream& os, const XColor color){
		os << XString("XColor(%1,%2,%3,%4)")
			.arg(color._red).arg(color._green).arg(color._blue).arg(color._alpha);
		return os;
	}

private:
	int		_red;
	int		_green;
	int		_blue;
	int		_alpha;
};

#endif //_X_COLOR_HPP

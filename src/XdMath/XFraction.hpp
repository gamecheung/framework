#ifndef _X_FRACTION_HPP
#define _X_FRACTION_HPP

#include "XMath.hpp"

class XFraction
{
public:
	XFraction(int remember, int denominator = 1)
		:m_remember(remember), m_denominator(denominator)
	{ }

	bool isValid()const{ return (m_denominator != 0); }
	int remember()const{ return m_remember; }
	int denominator()const{ return m_denominator; }
	void setRemember(int remember){ m_remember = remember; }
	void setDenominator(int denominator){ m_denominator = denominator; }

	void simplify()
	{
		if (!isValid())
		{
			Q_ASSERT(false);
			return;
		}

		if (m_remember == 0)
		{
			m_denominator = 1;
			return;
		}

		int abs_remember = xAbs(m_remember);
		int abs_denominator = xAbs(m_denominator);
		int gcd = XMath::GCD(abs_remember, abs_denominator);
		m_remember /= gcd;
		m_denominator /= gcd;
		if (m_denominator < 0)
		{
			m_remember = -m_remember;
			m_denominator = -m_denominator;
		}
	}
	XFraction simplefied()const
	{
		auto ret = *this;
		ret.simplify();
		return ret;
	}

	static XFraction fromDouble(const double d)
	{
		return XFraction(d * 10000, 10000).simplefied();
	}

	XFraction& operator+= (const XFraction& fraction){ *this = *this + fraction; return *this; }
	XFraction& operator-= (const XFraction& fraction){ *this = *this - fraction; return *this; }
	XFraction& operator*= (const XFraction& fraction){ *this = *this * fraction; return *this; }
	XFraction& operator/= (const XFraction& fraction){ *this = *this / fraction; return *this; }
	XFraction operator-()const
	{
		if (m_denominator < 0)
			return XFraction(m_remember, -m_denominator);
		else
			return XFraction(-m_remember, m_denominator);
	}

	friend bool operator==(const XFraction& fraction1, const XFraction& fraction2)
	{
		if (!fraction1.isValid() && !fraction2.isValid())
			return true;

		auto f1 = fraction1.simplefied();
		auto f2 = fraction2.simplefied();
		return ((f1.m_remember == f2.m_remember) && (f1.m_denominator == f2.m_denominator));
	}
	friend bool operator!=(const XFraction& fraction1, const XFraction& fraction2)
	{
		return !(fraction1 == fraction2);
	}

	// (a/b)+(c/d) = (ad+bc)/(bd)
	friend XFraction operator+(const XFraction& fraction1, const XFraction& fraction2)
	{
		return XFraction(fraction1.m_remember*fraction2.m_denominator + fraction1.m_denominator*fraction2.m_remember,
			fraction1.m_denominator*fraction2.m_denominator).simplefied();
	}
	friend XFraction operator-(const XFraction& fraction1, const XFraction& fraction2)
	{
		return fraction1 + (-fraction2);
	}
	friend XFraction operator*(const XFraction& fraction1, const XFraction& fraction2)
	{
		return XFraction(fraction1.m_remember*fraction2.m_remember, fraction1.m_denominator*fraction2.m_denominator).simplefied();
	}
	friend XFraction operator/(const XFraction& fraction1, const XFraction& fraction2)
	{
		return XFraction(fraction1.m_remember*fraction2.m_denominator, fraction1.m_denominator*fraction2.m_remember).simplefied();
	}

private:
	int m_remember;
	int m_denominator;
};

#endif // _X_FRACTION_HPP


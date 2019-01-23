#ifndef _X_QUADRATIC_HPP
#define _X_QUADRATIC_HPP

// y = ax^2+bx+c
class XQuadratic
{
public:
	typedef struct{
		double value1;
		double value2;
		bool	ok;
	}Roots;

	XQuadratic(double a, double b, double c)
		:m_a(a), m_b(b), m_c(c){}

	double y(double x)const{ return m_a*x*x + m_b*x + m_c; }
	Roots roots(bool* ok = nullptr)const
	{
		Roots roots;
		double delt = m_b*m_b - 4 * m_a*m_c;
		if (delt >= 0)
		{
			delt = sqrt(delt);
			roots.value1 = (-m_b + delt) / (2 * m_a);
			roots.value2 = (-m_b - delt) / (2 * m_a);
			roots.ok = true;
		}
		else
		{
			roots.ok = false;
		}

		return roots;
	}

private:
	double m_a;
	double m_b;
	double m_c;
};

#endif // _X_QUADRATIC_HPP

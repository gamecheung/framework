#ifndef _X_MATH_HPP
#define _X_MATH_HPP

#include <cassert>
#include "XGlobal.h"

class XMath
{
public:
	// Greastest Common Divisor
	static int GCD(int a, int b)
	{
		assert(a > 0 && b > 0);

		int m = xMax(a, b);
		int n = xMin(a, b);

		if (m%n == 0)
		{
			return n;
		}
		else
		{
			return GCD(n, m%n);
		}
	}

	// Lowest Comm Multiple
	static int LCM(int a,int b)
	{
		assert(a > 0 && b > 0);
		return a*b / GCD(a, b);
	}
	
	
	static int exGCD(int a, int b, int& x, int& y)
	{
		if (b == 0)
		{
			x = 1; 
			y = 0;
			return a;
		}
		else
		{
			int d = exGCD(b, a%b, x, y);
			int t = x;
			x = y;
			y = t-a / b*y;
			return d;
		}
	}

	static int INV(int num, int mod)
	{
		int x, y;
		exGCD(num, mod, x, y);
		while (x < 0)
		{
			x += mod;
			y -= num;
		}
		return x;
	}

	// 排列
	static int arrangement(int n)
	{
		assert(n > 0);
		int rs = n;
		while (--n)
		{
			rs = rs*n;
		}
		return rs;
	}

	// 组合
	static int combination(int m,int n)
	{
		if (m == 0)
			return 0;
		if (n == 0)
			return 1;

		return arrangement(m) / (arrangement(n)*arrangement(m - n));
	}

	template <typename Type>
	static double (max)(const Type& data)
	{
		double ret = firstValue(data);
		for (const auto& d : data)
		{
			if (d > ret)
				ret = d;
		}
		return ret;
	}

	template <typename Type>
	static double (min)(const Type& data)
	{
		double ret = firstValue(data);
		for (const auto& d : data)
		{
			if (d < ret)
				ret = d;
		}
		return ret;
	}

	template<typename Type>
	static double mean(const Type& data)
	{
		int count = 0;
		double total = 0;
		for (const auto& d : data)
		{
			total += d;
			++count;
		}
		assert(count > 0);
		return total / count;
	}

	

	template <typename Type>
	static double range(const Type& data)
	{
		return ((max)(data) - (min)(data));
	}


	// 方差
	template<typename Type>
	static double variance(const Type& data)
	{
		double mean = XMath::mean(data);
		double ret = 0;
		for (auto& d : data)
		{
			ret += pow(d - mean, 2);
		}
		return ret;
	}

	template<typename Type>
	static double stdevp(const Type& data)
	{
		double mean = XMath::mean(data);
		int count = 0;
		double total = 0;
		for (auto& d : data)
		{
			total += pow(d - mean, 2);
			++count;
		}

		if (count > 1)
			return sqrt(total / (count));
		else
			return 0;
	}

	template<typename Type>
	static double stdev(const Type& data)
	{
		double mean = XMath::mean(data);
		int count = 0;
		double total = 0;
		for (auto& d : data)
		{
			total += pow(d - mean, 2);
			++count;
		}

		if (count > 1)
			return sqrt(total / (count -1));
		else
			return 0;
	}

	template<typename Type>
	static double CV(const Type& data)
	{
		double mean = XMath::mean(data);
		if (mean != 0)
			return stdev(data) / mean;
		else
			return 0;
	}


private:
	template <typename Type>
	static double firstValue(const Type& data)
	{
		for (const auto& d : data)
		{
			return d;
		}
		assert(false);
		return 0;
	}
};

#endif //_X_MATH_HPP

#ifndef _XGLOBAL_H
#define _XGLOBAL_H

#include <cstdint>
#include <cstdlib>
#include "c/xglobal.h"
#include "XCompilerDetection.h"
#include "XSystemDetection.h"


// use: #pragma chMSG(Fix this later)
#define _chSTR(s)	#s
#define chSTR(s)	_chSTR(s)
#define chMSG(desc)	message(__FILE__ "(" chSTR(__LINE__) "):" #desc)

// Avoid "unused parameter" warnings
#define X_UNUSED(x) (void)x;

#define X_DELETE(p)		if(p){delete (p);p=nullptr;}

#define X_IF(test,cond,code) if((cond)==(test)){code;}
#define X_RETURN_IF(test,cond)	X_IF(test,cond,return)
#define X_RETURN_TRUE_IF(test,cond)	X_IF(test,cond,return true)
#define X_RETURN_FALSE_IF(test,cond)	X_IF(test,cond,return false)
#define X_BREAK_IF(test,cond)	X_IF(test,cond,break)

#define X_DISABLE_COPY(Class)	\
	Class(const Class &) X_DECL_EQ_DELETE;	\
	Class &operator=(const Class &) X_DECL_EQ_DELETE;

#define X_DEF_SINGLETON(Cls)	static Cls& instance(){static Cls obj;return obj;}

#define X_DECL_SINGLETON(Cls) \
	public: \
	static Cls* getInstance(){if(*pInstance()==nullptr) *pInstance()=new Cls();return *pInstance();} \
	static void release(){X_DELETE(*pInstance());} \
	private: \
	static Cls** pInstance(){static Cls* obj=nullptr;return &obj;} 

template <typename T>
inline T xAbs(const T& t){ return t >= 0 ? t : -t; }

inline int xRound(double d)
{ return d >= 0.0 ? int(d + 0.5) : int(d - double(int(d - 1)) + 0.5) + int(d - 1); }
inline int xRound(float d)
{ return d >= 0.0f ? int(d + 0.5f) : int(d - float(int(d - 1)) + 0.5f) + int(d - 1); }

inline int64_t xRound64(double d)
{ return d >= 0.0 ? int64_t(d + 0.5) : int64_t(d - double(int64_t(d - 1)) + 0.5) + int64_t(d - 1); }
inline int64_t xRound64(float d)
{ return d >= 0.0f ? int64_t(d + 0.5f) : int64_t(d - float(int64_t(d - 1)) + 0.5f) + int64_t(d - 1); }

template <typename T>
inline const T& xMin(const T& a, const T& b){ return (a < b) ? a : b; }
template <typename T>
inline const T& xMax(const T& a, const T& b){ return (a > b) ? a : b; }
template <typename T>
inline const T& xBound(const T& min,const T& val, const T& max)
{ return xMax(min, xMin(max,val)); }


inline int xRand(int min, int max)
{
	return rand() % (max - min+1) + min;
}

template <typename _Ty>
inline bool xInRange(const _Ty& n, const _Ty& lv, const _Ty& rv, bool lb = true, bool rb = true)
{
	assert(lv <= rv);

	if (n < lv)
		return false;
	if (!lb && n == lv)
		return false;

	if (n > rv)
		return false;
	if (!rb&&n == rv)
		return false;

	return true;
}

template <typename _Ty>
inline void xSwap(_Ty& n1, _Ty& n2)
{
	auto t = n1;
	n1 = n2;
	n2 = t;
}

inline bool xMaybeFalse(bool pre, bool cur)
{
	if (pre && cur)
		return true;
	return false;
}
inline bool xMaybeTrue(bool pre,bool cur)
{
	if (pre || cur)
		return true;
	return false;
}


#endif //_XGLOBAL_H


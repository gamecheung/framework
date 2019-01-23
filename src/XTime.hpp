#ifndef _X_TIME_HPP
#define _X_TIME_HPP

#include <cstdint>
#include "XSystemDetection.h"
#include "XString.hpp"

#if defined(X_OS_WIN32)
#include <Windows.h>
#endif

class XTime{
public:
	XTime(int h=0,int m=0,int s=0,int ms=0) { setHMS(h, m, s, ms); }
    ~XTime(){}

	bool isNull()const{ return _mds == NullTime; }
	bool isValid()const{ return (_mds > NullTime)&&(_mds < MSECS_PER_DAY); }
	static bool isValid(int h, int m, int s, int ms = 0)
	{ return (uint32_t)h < 24 && (uint32_t)m < 60 && (uint32_t)s < 60 && (uint32_t)ms < 1000; }

	int hour()const{
		if (!isValid())
			return -1;
		return _mds / MSECS_PER_HOUR;
	}
	int minute()const{
		if (!isValid())
			return -1;
		return (_mds % MSECS_PER_HOUR)/MSECS_PER_MIN;
	}
	int second()const{
		if (!isValid())
			return -1;
		return (_mds/1000) % SECS_PER_MIN;
	}
	int msec()const{
		if (!isValid())
			return -1;
		return _mds % 1000;
	}

    XString toString(const XString& format=XString(L"hh:mm:ss"))const{
        XString retval = format;
        XString nn;

        nn = XString::number(hour());
        if (retval.find(L"hh")){ retval.replace(L"hh", nn.leftJustified(2, L'0')); }
        else{ retval.replace("h", nn); }

        nn = XString::number(minute());
        if (retval.find(L"mm")){ retval.replace(L"mm", nn.leftJustified(2, L'0'));  }
        else{  retval.replace("m", nn); }

        nn = XString::number(second());
        if (retval.find(L"ss")){ retval.replace(L"ss", nn.leftJustified(2, L'0')); }
        else{ retval.replace(L"s", nn); }

		nn = XString::number(msec());
		if (retval.find(L"ms")){ retval.replace(L"ms", nn.leftJustified(3, L'0')); }
		//else{ retval.replace(L"s", nn); }

        return retval;
    }
	// format = hh:mm:ss
	XTime& fromString(const XString& str){
		setHMS(str.mid(0, 2).toInt(), str.mid(3, 2).toInt(), str.mid(6, 2).toInt());
		return *this;
	}

	bool setHMS(int h, int m, int s, int ms = 0){
		if (!isValid(h, m, s, ms)){
			_mds = NullTime;
			return false;
		}
		_mds = (h*SECS_PER_HOUR + m*SECS_PER_MIN + s) * 1000 + ms;
		return true;
	}

	XTime addHours(int h)const;
	int hoursTo(const XTime& t)const;
	XTime addMinutes(int m)const;
	int minutesTo(const XTime& t)const;
	XTime addSeconds(int s)const;
	int secondsTo(const XTime& t)const
	{
		if (!isValid() || !t.isValid())
			return 0;
		return (t._mds - _mds)/1000;
	}
	XTime addMSecs(int ms)const;
	int msecsTo(const XTime& t)const{
		if (!isValid() || !t.isValid())
			return 0;
		return t._mds - _mds;
	}

	bool operator==(const XTime &other) const;
	bool operator!=(const XTime &other) const;
	bool operator< (const XTime &other) const;
	bool operator<=(const XTime &other) const;
	bool operator> (const XTime &other) const;
	bool operator>=(const XTime &other) const;

	static XTime currentTime(){
		XTime t;
	#if defined(X_OS_WIN32)
		SYSTEMTIME st;
		memset(&st, 0, sizeof(SYSTEMTIME));
		GetLocalTime(&st);
		t.setHMS(st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	#else
		assert(false);
	#endif
		return t;
	}

	void start(){ *this = XTime::currentTime(); }
	int restart(){
		XTime t = XTime::currentTime();
		int n = msecsTo(t);
		if (n < 0)
			n += 86400 * 1000;
		*this = t;
		return n;
	}
	int elapsed()const{
		int n = msecsTo(XTime::currentTime());
		if (n < 0)
			n += 86400 * 1000;
		return n;
	}

private:
	enum TimeFlag { NullTime = -1 };
	enum {
		SECS_PER_DAY = 86400,
		MSECS_PER_DAY = 86400000,
		SECS_PER_HOUR = 3600,
		MSECS_PER_HOUR = 3600000,
		SECS_PER_MIN = 60,
		MSECS_PER_MIN = 60000,
		TIME_T_MAX = 2145916799,  // int maximum 2037-12-31T23:59:59 UTC
		JULIAN_DAY_FOR_EPOCH = 2440588 // result of julianDayFromDate(1970, 1, 1)
	};
	int _mds;
};

#endif // XTIME_HPP

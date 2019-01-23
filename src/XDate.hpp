#ifndef _X_DATE_HPP
#define _X_DATE_HPP

#include <cstdint>
#include "XSystemDetection.h"
#include "XString.hpp"

#if defined(X_OS_WIN32)
#include <Windows.h>
#endif

class XDate{
public:
    XDate(int y = 0, int m = 0, int d = 0)
        :_year (y), _month(m), _day(d) {}

	bool isNull()const{ return ((_year == 0) && (_month == 0) && (_day == 0)); }
	bool isValid()const{ return isValid(_year, _month, _day); }

    int year()const{ return _year; }
    int month()const{ return _month; }
    int day()const{ return _day; }
    void setDate(int year, int month, int day)
	{
        _year = year;
        _month = month;
        _day = day;
    }

    XString toString(const XString& format = XString(L"yyyy-MM-dd"))const{
        XString retval = format;
        XString nn;

        nn = XString::number(year());
        if (retval.find(L"yyyy")){ retval.replace(L"yyyy", nn); }
        else if (retval.find(L"yy")){ retval.replace(L"yy", nn.right(2)); }

        nn = XString::number(month());
        if (retval.find(L"MM")){ retval.replace(L"MM", nn.leftJustified(2, L'0')); }
        else{ retval.replace("M", nn); }

        nn = XString::number(day());
        if (retval.find(L"dd")){ retval.replace(L"dd", nn.leftJustified(2, L'0')); }
        else{ retval.replace(L"d", nn); }

        return retval;
    }

	static XDate currentDate(){
		XDate date;
#if defined(X_OS_WIN32)
		SYSTEMTIME st;
		memset(&st, 0, sizeof(SYSTEMTIME));
		GetLocalTime(&st);
		date.setDate(st.wYear, st.wMonth, st.wDay);
#else
		assert(false);
#endif
		return date;
	}

	// format = yyyy-MM-dd
	static XDate fromString(const XString& str){
		if (str.length()==10)
			return XDate(str.mid(0, 4).toInt(), str.mid(5, 2).toInt(), str.mid(8, 2).toInt());
		return XDate();
	}
	static bool isLeapYear(int year){
		return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
	}
	static bool isValid(int year, int month, int day)
	{
		static int monthDays[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

		if (year == 0)
			return false;

		return (day > 0 && month > 0 && month <= 12)
			&& (day <= monthDays[month-1] || (day==29&&month==2&&isLeapYear(year)));
	}


	bool operator==(const XDate &other) const
	{ return ((year() == other.year()) && (month() == other.month()) && (day() == other.day())); }
	bool operator!=(const XDate &other) const
	{ return !(*this == other); }


protected:
private:
    //static int64_t nullJd(){ return (std::numeric_limits<int64_t>::min)(); }
    //static int64_t minJd(){ return -784350574879; }
    //static int64_t maxJd(){ return 784354017364; }
private:
    //int64_t _jd;
    int _year;
    int _month;
    int _day;
};

#endif //_X_DATE_HPP

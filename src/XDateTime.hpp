#ifndef _X_DATE_TIME_HPP
#define _X_DATE_TIME_HPP

#include "XDate.hpp"
#include "XTime.hpp"

class XDateTime{
public:
	const XDate& date()const{ return _date; }
	const XTime& time()const{ return _time; }
    void setDate(const XDate& date){ _date = date; }
    void setTime(const XTime& time){ _time = time; }
    static XDateTime currentDateTime(){
        XDateTime dt;
        dt.setDate(XDate::currentDate());
        dt.setTime(XTime::currentTime());
        return dt;
    }

    XString toString(const XString& format = XString("yyyy-MM-dd hh:mm:ss"))const{
        XString retval = format;
        retval = _date.toString(retval);
        retval = _time.toString(retval);
        return retval;
    }
	// format = yyyy-MM-dd hh:mm:ss
	XDateTime& fromString(const XString& str){
		setDate(XDate().fromString(str.mid(0,10)));
		setTime(XTime().fromString(str.mid(11)));
		return *this;
	}
	

private:
    XDate   _date;
    XTime   _time;
};

#endif //_X_DATE_TIME_HPP

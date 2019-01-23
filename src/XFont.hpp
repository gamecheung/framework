#ifndef _X_FONT_H
#define _X_FONT_H

#include "XString.hpp"

class XFont{
public:
    XFont(const XString& family = XString(), int pointSize = -1, bool bold = false,
        bool italic = false, bool underline = false, bool strikeOut = false)
        :_family(family), _pointSize(pointSize), _bold(bold),
        _italic(italic), _underline(underline), _strikeOut(strikeOut){}

	const XString& family()const{ return _family; }
	void setFamily(const XString& family){ _family = family; }

	int pointSize()const{ return _pointSize; }
	void setPointSize(int pointSize){ _pointSize = pointSize; }

	bool bold()const{ return _bold; }
	void setBold(bool b){ _bold = b; }

	bool italic()const{ return _italic; }
	void setItalic(bool b){ _italic = b; }

	bool underline()const{ return _underline; }
	void setUnderline(bool b){ _underline = b; }

	bool strikeOut()const{ return _strikeOut; }
	void setStrikeOut(bool b){ _strikeOut = b; }

    XString toString()const{
        XStringList strs;
        strs.append(_family);
        strs.append(XString::number(_pointSize));
        strs.append(_bold ? "true" : "false");
        strs.append(_italic ? "true" : "false");
        strs.append(_underline ? "true" : "false");
        strs.append(_strikeOut ? "true" : "false");
        return strs.join(",");
    }
    static XFont fromString(const XString& str){
        XStringList strs = str.split(",");
        if (strs.count() == 6){
            return XFont(
                strs.at(0),
                strs.at(1).toInt(),
                (strs.at(2) == XString("true")) ? true : false,
                (strs.at(3) == XString("true")) ? true : false,
                (strs.at(4) == XString("true")) ? true : false,
                (strs.at(5) == XString("true")) ? true : false
                );
        }
        return XFont();
    }

private:
	XString _family;
	int		_pointSize;
	bool	_bold;
	bool	_italic;
	bool	_underline;
	bool	_strikeOut;
};



#endif //_X_FONT_H

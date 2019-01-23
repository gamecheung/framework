#ifndef _X_CHAR_HPP
#define _X_CHAR_HPP

class XChar{
public:
	XChar(wchar_t ch = 0) :_ch(ch){}

    wchar_t data()const{ return _ch; }
	int digitValue()const{ return XChar::digitValue(_ch); }
	static int digitValue(wchar_t ch){
		if (L'0' <= ch && ch <= L'9')
			return ch - L'0';
		return -1;
	}


	bool isDigit()const { return XChar::isDigit(_ch); }
	static bool isDigit(wchar_t ch){
		return (0x30 <= ch&&ch <= 0x39);
	}
	
	bool isLetter()const{ return XChar::isLetter(_ch); }
	static bool isLetter(wchar_t ch){
		return ((L'A' <= ch&&ch <= L'Z') || (L'a' <= ch&&ch <= L'z'));
	}

	bool isLower()const{ return XChar::isLower(_ch); }
	static bool isLower(wchar_t ch){
		return (L'a' <= ch&&ch <= L'z');
	}

	bool isUpper()const{ return XChar::isUpper(_ch); }
	static bool isUpper(wchar_t ch){
		return (L'A' <= ch&&ch <= L'Z');
	}

	bool isSpace()const { return XChar::isSpace(_ch); }
	static bool isSpace(wchar_t ch){
		return (ch == 0x20 || (0x09 <= ch && ch <= 0x0D));
	}

	wchar_t toLower(){ _ch = XChar::toLower(_ch); return _ch; }
	static wchar_t toLower(wchar_t ch){
		if (XChar::isUpper(ch))
			return ch+(L'a'-L'A');
		return ch;
	}

	wchar_t toUpper(){ _ch = XChar::toUpper(_ch); return _ch; }
	static wchar_t toUpper(wchar_t ch){
		if (XChar::isLower(ch))
			return ch - (L'a' - L'A');
		return ch;
	}

	bool isNewLine()const{ return XChar::isNewLine(_ch); }
	static bool isNewLine(wchar_t ch){
		return ch == L'\n';
	}

	static wchar_t newLine(){ return L'\n'; }

private:
	wchar_t	_ch;
};

#endif //_X_CHAR_HPP

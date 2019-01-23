#ifndef _X_CRYPTOGRAPHIC_HPP
#define _X_CRYPTOGRAPHIC_HPP

#include <vector>
#include "XByteArray.hpp"
#include "XdMath/XMath.hpp"
#include "win32/XDbgHelper.hpp"

class XCipher
{
public:
	XCipher(const XByteArray* table = nullptr)
	{
		setTable(table);
	}
	virtual XByteArray encode(const XByteArray& data)const=0;
	virtual XByteArray decode(const XByteArray& data)const=0;

	void setTable(const XByteArray* table)
	{
		if (table)
		{
			_table = *table;
		}
		else
		{
			_table.resize(256);
			for (int i = 0; i < 256; ++i)
				_table[i] = i;
		}
	}

	const XByteArray& table()const{ return _table; }

protected:
	static int indexOfTable(const XByteArray& table, char ch)
	{
		for (int i = 0; i < table.size(); ++i)
		{
			if (table.at(i) == ch)
				return i;
		}
		assert(false);
		return -1;
	}

protected:
	XByteArray	_table;
};

class XSingleCodeCipher:public XCipher
{
public:
	XSingleCodeCipher(const XByteArray* table = nullptr) :XCipher(table){}
	virtual XByteArray encode(const XByteArray& data)const{ return map(_table, _cipherTable, data); }
	virtual XByteArray decode(const XByteArray& data)const{ return map(_cipherTable, _table, data); }

protected:
	static char map(const XByteArray& t1, const XByteArray& t2, char ch)
	{
		assert(t1.size() == t2.size());
		for (int i = 0; i < t1.size(); ++i)
		{
			if (t1.at(i) == ch)
				return t2.at(i);
		}
		return ch;
	}
	static XByteArray map(const XByteArray& t1, const XByteArray& t2, const XByteArray& data)
	{
		assert(!t1.isEmpty() && !t2.isEmpty() && t1.size() == t2.size());
		XByteArray rs(data.size());
		for (int i = 0; i < rs.size(); ++i)
		{
			rs[i] = map(t1, t2, data.at(i));
		}
		return rs;
	}
	void setTable(const XByteArray* table)
	{
		XCipher::setTable(table);
		_cipherTable.resize(_table.size());
	}

protected:
	XByteArray	_cipherTable;
};

class XShiftCipher :public XSingleCodeCipher
{
public:
	XShiftCipher(const XByteArray* table = nullptr) :XSingleCodeCipher(table){}
	XShiftCipher(int offset, const XByteArray* table = nullptr)
	{
		setTable(table);

		for (int i = 0; i < _table.size(); ++i)
		{
			_cipherTable[i] = _table.at((i + offset) % _table.size());
		}
	}
};


class XAffineCipher :public XSingleCodeCipher
{
public:
	XAffineCipher(int a, int b, const XByteArray* table = nullptr) :_a(a), _b(b)
	{
		setTable(table);

		assert(XMath::GCD(a, _table.size()) == 1);
		assert(0 <= b&&b < _table.size());

		_inv = XMath::INV(a, _table.size());

	}

	virtual XByteArray encode(const XByteArray& data) const
	{
		XByteArray rs(data.size());
		for (int i = 0; i < data.size(); ++i)
			rs[i]= encode(data.at(i));
		return rs;
	}
	virtual XByteArray decode(const XByteArray& data)const
	{ 
		XByteArray rs(data.size());
		for (int i = 0; i < data.size(); ++i)
			rs[i] = decode(data.at(i));
		return rs;
	}

private:
	// c = (ax+b)(mod 26)
	char encode(const char ch)const
	{
		int x = indexOfTable(_table, ch);
		int y = (_a*x + _b) % _table.size();
		return _table.at(y);
	}
	// p = a^-1(c-b)(mod 26)
	char decode(const char ch)const
	{
		int y = indexOfTable(_table,ch);
		int x = _inv*(y - _b) % _table.size();
		while (x < 0)
			x += _table.size();

		return _table.at(x);
	}

private:
	int _a;
	int _b;
	int _inv;
};

class XKeywordCipher : public XSingleCodeCipher
{
public:
	XKeywordCipher(const XByteArray& keyword, int offset = 0,const XByteArray* table=nullptr)
	{
		assert(keyword.size()>0);

		setTable(table);

		XByteArray trKeyword(keyword.size(), keyword.at(0));
		int pos = 1;
		for (auto& ch : keyword)
		{
			if (!trKeyword.contains(ch))
			{
				trKeyword[pos++] = ch;
			}
		}
		trKeyword.resize(pos);

		int idx = 0;
		for (auto& ch : trKeyword)
		{
			_cipherTable[(idx + offset) % _table.size()] = ch;
			++idx;
		}

		pos = 0;
		for (; idx < _cipherTable.size(); ++idx)
		{
			while (trKeyword.contains(_table.at(pos)))
				++pos;

			_cipherTable[(idx + offset) % _table.size()] = _table.at(pos++);
		}
	}
};

class XMultiCodeCipher :public XCipher
{
public:
	XMultiCodeCipher(const XByteArray& key, const XByteArray* table = nullptr) :_key(key),XCipher(table){}

	void setKey(const XByteArray& key){ _key = key; }
	const XByteArray& key()const{ return _key; }

protected:
	XByteArray _key;
};



class XVigenereCipher :public XMultiCodeCipher
{
public:
	XVigenereCipher(const XByteArray& key, const XByteArray* table = nullptr) :XMultiCodeCipher(key, table){}
	virtual XByteArray encode(const XByteArray& data)const
	{
		assert(!_table.isEmpty());
		assert(!_key.isEmpty());

		XByteArray rs(data.size());
		for (int i = 0; i < rs.size(); ++i)
		{
			char chKey = _key.at(i%_key.size());
			int index = indexOfTable(_table, chKey) + indexOfTable(_table, data.at(i));
			rs[i] = _table.at(index%_table.size());
		}

//#ifdef _DEBUG_X_CIPHER
//		xPrint(XString("txt: %1\n").arg(data.toString()));
//		xPrint("key£º");
//		for (int i = 0; i <= data.size() / _key.size(); ++i)
//			xPrint(_key.toString());
//		xPrint("\n");
//		xPrint(XString("rst: %1\n").arg(rs.toString()));
//#endif

		return rs;
	}
	virtual XByteArray decode(const XByteArray& data)const 
	{ 
		assert(!_table.isEmpty());
		assert(!_key.isEmpty());

		XByteArray rs(data.size());
		for (int i = 0; i < rs.size(); ++i)
		{
			char chKey = _key.at(i%_key.size());
			int index = indexOfTable(_table, data.at(i)) - indexOfTable(_table, chKey) + _table.size();
			rs[i] = _table.at(index%_table.size());
		}

		return rs;
	}


};



class XAutoKeyCipher :public XMultiCodeCipher
{
public:
	XAutoKeyCipher(const XByteArray& key, const XByteArray* table = nullptr) :XMultiCodeCipher(key, table){}
	virtual XByteArray encode(const XByteArray& data)const
	{
		assert(!_table.isEmpty());
		assert(!_key.isEmpty());

		XByteArray rs(data.size());
		for (int i = 0; i < rs.size(); ++i)
		{
			char chKey = (i < _key.size()) ?
				_key.at(i) : data.at(i - _key.size());

			int index = indexOfTable(_table, chKey) + indexOfTable(_table, data.at(i));
			rs[i] = _table.at(index%_table.size());
		}

		return rs;
	}
	virtual XByteArray decode(const XByteArray& data)const
	{
		assert(!_table.isEmpty());
		assert(!_key.isEmpty());

		XByteArray rs(data.size());
		for (int i = 0; i < rs.size(); ++i)
		{
			char chKey = (i < _key.size()) ?
				_key.at(i) : rs.at(i - _key.size());

			int index = indexOfTable(_table, data.at(i)) - indexOfTable(_table, chKey) + _table.size();
			rs[i] = _table.at(index%_table.size());
		}

		return rs;
	}

};


class XCryphtographic
{
public:
	enum Algorithm
	{
		Shift,
		Keyword
	};
	enum Direction{Encode,Decode};
	XCryphtographic(Algorithm method)
		:_method(method)
	{
		
	}

	void addData(const char* data, int length)
	{
		_data.append(data, length);
	}

	void reset()
	{

	}

	const XByteArray& result()
	{
		return _result;
	}

	//static XString enc(const XString& str, Algorithm method)
	//{
	//	return calculate(XByteArray(str), method).toHexString();
	//}
	//static XString dec(const XString& str, Algorithm method)
	//{
	//}

	static XByteArray calculate(const XByteArray& data, Algorithm method)
	{
		switch (method)
		{
		case XCryphtographic::Shift:
			return shift(data);
			break;
		default:
			break;
		}
		return XByteArray();
	}

	static XByteArray shift(const XByteArray& data, int sft = 127)
	{
		XByteArray rs(data.size());
		for (int i = 0; i < data.size(); ++i)
			rs[i] = (data.at(i) + sft);
		return rs;
	}
	

private:
	Algorithm	_method;
	XByteArray	_data;
	XByteArray	_result;
};

#endif //_X_CRYPTOGRAPHIC_HPP

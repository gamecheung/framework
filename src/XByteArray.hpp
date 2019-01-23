#ifndef _X_BYTE_ARRAY_HPP
#define _X_BYTE_ARRAY_HPP

#include "XString.hpp"

class XByteArray
{
public:
	XByteArray():_data(nullptr),_size(0){}
	XByteArray(const char* data, int length) :_data(nullptr), _size(0){ append(data, length); }
	XByteArray(const char* data) :_data(nullptr), _size(0){ append(data, strlen(data)); }
	XByteArray(const XString& str) :_data(nullptr), _size(0){ append(str); }
	XByteArray(int size)
	{
		_data = new char[size];
		assert(_data);
		_size = size;
	}
	XByteArray(int size, char ch)
	{
		_data = new char[size];
		assert(_data);
		_size = size;
		memset(_data, ch, _size);
	}
	~XByteArray() { clear(); }

	XByteArray(const XByteArray& other)
	{
		_data = new char[other._size];
		memcpy(_data, other._data, other._size);
		_size = other._size;
	}
	XByteArray& operator=(const XByteArray& other)
	{
		clear();
		append(other);
		return *this;
	}
	

	void append(const char* data, int length)
	{
		char* preData = _data;
		auto preSize = _size;

		_data = new char[_size + length];
		assert(_data);

		if (preData)
		{
			memcpy(_data, preData, preSize);
			delete[] preData;
		}

		memcpy(&_data[preSize], data, length);
		_size = preSize + length;

	}
	void append(const XByteArray& arr){ append(arr.data(), arr.size()); }
	void append(const XString& str){ append(str.toStdString().c_str(), str.length()); }

	void clear()
	{
		if (_data)
		{
			delete[] _data;
			_data = nullptr;
			_size = 0;
		}
	}

	const char* data()const{ return _data; }
	int size()const{ return _size; }

	const char at(int idx)const
	{
		assert(0 <= idx&&idx < _size);
		return _data[idx];
	}

	char& operator[](int idx)
	{
		assert(0 <= idx&&idx < _size);
		return _data[idx];
	}

	void resize(int size)
	{
		char* preData = _data;
		auto preSize = _size;

		_data = new char[size];
		assert(_data);

		if (preData)
		{
			memcpy(_data, preData, xMin(preSize,size));
			delete[] preData;
		}
		_size = size;
	}

	char* begin()const{ return _data; }
	char* end()const{ return _data + _size; }


	XString toHexString()const
	{
		XString hexStr;
		for (int i = 0; i < _size; ++i)
		{
			hexStr.append(XString::number(_data[i], 16).leftJustified(2, '0'));
		}
		return hexStr;
	}

	static XByteArray fromHexString(const XString& str, bool* ok = nullptr)
	{
		XByteArray arr(str.length() / 2);

		if (str.length() % 2 == 1)
		{
			if (ok)
				*ok = false;
			return arr;
		}

		bool bOk = false;
		for (int i = 0; i < arr.size(); ++i)
		{
			arr[i] = (char)str.mid(i * 2, 2).toInt(16,&bOk);
			if (!bOk)
			{
				if (ok)
					*ok = false;
				return arr;
			}
		}

		if (ok)
			*ok = true;
		return arr;
	}

	XString toString()const
	{
		return XString((const char*)_data, _size);
	}


	bool contains(const char ch)const
	{
		for (int i = 0; i < _size; ++i)
		{
			if (_data[i] == ch)
				return true;
		}
		return false;
	}

	bool isEmpty()const{ return _size == 0; }

private:
	char*	_data;
	int		_size;
};

#endif //_X_BYTE_ARRAY_HPP

#ifndef _X_LIST_HPP
#define _X_LIST_HPP

#include <list>
#include <cstdint>
#include <cassert>
#include "XGlobal.h"

template <typename _Type>
class XList :public std::list<_Type>{
public:
	void append(const _Type& val){ push_back(val); }
	void prepand(const _Type& val){ push_front(val); }
	size_t count()const{ return size(); }
	size_t count(const _Type& val)const{
		size_t cnt = 0;
		const_iterator itr = begin();
		const_iterator itrEnd = end();
		for (; itr != itrEnd; itr++)
		{
			if (*itr == val){
				++cnt;
			}
		}
		return cnt;
	}


	bool contains(const _Type& val)const{
		const_iterator itr = begin();
		const_iterator itrEnd = end();
		for (; itr != itrEnd; itr++)
		{
			if (*itr == val){
				return true;
			}
		}
		return false;
	}

	const _Type& at(size_t i)const{
		assert(0 <= i&&i <= count());
		const_iterator itr = begin();
		while (i--){
			itr++;
		}
		return (*itr);
	}
	_Type& operator[](size_t i){
		assert(0 <= i&&i <= count());
		iterator itr = begin();
		while (i--){
			itr++;
		}
		return (*itr);
	}

	void removeAt(size_t i){
		assert(i <= count());
		iterator itr = begin();
		while (i--){
			itr++;
		}
		erase(itr);
	}
	int removeAll(const _Type& value){
		size_t cnt = 0;
		const_iterator itr = begin();
		for (; itr != end();)
		{
			if (*itr == value){
				itr = erase(itr);
				++cnt;
			}
			else{
				++itr;
			}
		}
		return cnt;
	}

	bool isEmpty()const{ return empty(); }
};


#endif //_X_LIST_HPP


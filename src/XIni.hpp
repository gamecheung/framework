#ifndef _X_INI_HPP
#define _X_INI_HPP

#include <unordered_map>
#include <fstream>
#include "XChar.hpp"
#include "XString.hpp"
#include "XList.hpp"

class XIniItem{
public:
	XString groupName;
	std::unordered_map<std::wstring, std::wstring> items;
};

class XIni{
public:
	XIni() :_currentItem(nullptr),_state(ST_None){}

	bool load(const XString& fileName){
		std::wifstream fin(fileName);
		if (!fin.is_open())
			return false;

		_items.clear();

		_parsingGroupName.clear();
		_parsingKey.clear();
		_parsingValue.clear();
		_parsingComment.clear();

		std::wstring lineStr;
		while (std::getline(fin, lineStr)){
			for (auto itr = lineStr.begin(); itr != lineStr.end(); ++itr)
			{
				if (!parse(*itr))
					return false;
			}
			if (!parse(L'\n'))
				return false;
		}
		return true;
	}
	bool save(const XString& fileName){
		std::wofstream fout(fileName);
		if (!fout)
			return false;

		for (auto itr = _items.begin(); itr !=_items.end(); ++itr)
		{
			fout << L'[' << itr->groupName << L']' << XChar::newLine();
			for (auto itr2 = itr->items.begin(); itr2 != itr->items.end(); ++itr2){
				fout << itr2->first << L'=' << itr2->second << XChar::newLine();
			}
		}

		return true;
	}

	void setGroup(const XString& groupName){
		_currentItem = nullptr;
		for (auto itr = _items.begin(); itr !=_items.end(); ++itr)
		{
			if (itr->groupName == groupName)
				_currentItem = static_cast<XIniItem*>(&(*itr));
		}
		if (_currentItem == nullptr){
			XIniItem item;
			item.groupName = groupName;
			_items.append(item);
			_currentItem = &_items[_items.count() - 1];
		}
	}

	XString value(const XString& key, const XString& defVal = XString())const{
		assert(_currentItem);
		auto itr = _currentItem->items.find(key);
		if (itr == _currentItem->items.end()){
			return defVal;
		}
		return _currentItem->items[key];
	}
	

	void setValue(const XString& key, const XString& value){
		assert(_currentItem);
		_currentItem->items[key] = value;
	}
	

private:
	bool parse(wchar_t ch){
		switch (_state)
		{
		case XIni::ST_None:
			if (L'[' == ch){
				_state = XIni::ST_Group;
			}
			else if (L';' == ch || L'#' == ch){
				_state = XIni::ST_Comment;
			}
			else if (!XChar::isSpace(ch)){
				_parsingKey.push_back(ch);
				_state = XIni::ST_Key;
			}
			break;
		case XIni::ST_Group:
			if (L']' == ch){
				setGroup(_parsingGroupName);
				_parsingGroupName.clear();
				_state = XIni::ST_None;
			}
			else if (L'\n' == ch){
				return false;
			}
			else{
				_parsingGroupName.push_back(ch);
			}
			break;
		case XIni::ST_Key:
			if (L'='==ch){
				_state = XIni::ST_Value;
			}
			else if (XChar::isNewLine(ch)){
				return false;
			}
			else{
				_parsingKey.push_back(ch);
			}
			break;
		case XIni::ST_Value:
			if (XChar::isNewLine(ch)){
				setValue(_parsingKey.trimmed(), _parsingValue.trimmed());
				_parsingKey.clear();
				_parsingValue.clear();
				_state = XIni::ST_None;
			}
			else{
				_parsingValue.push_back(ch);
			}
			break;
		case XIni::ST_Comment:
			if (XChar::isNewLine(ch)){
				_parsingComment.clear();
				_state = XIni::ST_None;
			}
			else{
				_parsingComment.push_back(ch);
			}
			break;
		default:
			break;
		}
		return true;
	}

private:
	enum State{ST_None,ST_Group,ST_Key,ST_Value,ST_Comment};
	XList<XIniItem>	_items;
	XIniItem*	_currentItem;

	XIni::State	_state;
	XString		_parsingGroupName;
	XString		_parsingKey;
	XString		_parsingValue;
	XString		_parsingComment;
};

#endif //_X_INI_HPP


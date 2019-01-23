#ifndef _X_CONFIG_HPP
#define _X_CONFIG_HPP

#include "XGlobal.h"
#include "XString.hpp"
#include "XXml.hpp"


#define DEF_XCFG(key) XString key##(){return value(L ## #key);} void set##key(const XString& val){setValue(L ## #key,val);}
#define DEF_XCFG_EX(key,defVal) XString key##(){return value(L ## #key,defVal);} void set##key(const XString& val){setValue(L ## #key,val);}

class XConfig{
public:
	XConfig(const XString& pathfile=XString())
		:_pathfile(pathfile), _needSave(false)
		, elemRoot(L"config"), elemItem(L"item"), attrKey(L"key"), attrVal(L"val")
	{
		load(_pathfile);
	}
	~XConfig(){
		if (_needSave)
			save(_pathfile);
	}

	bool load(const XString& pathfile){
		_pathfile = pathfile;
		_map.clear();

        XXml xml;
		if (!xml.load(_pathfile))
			return false;

        if (xml.key() != elemRoot)
            return false;

        auto items = xml.children();
        for (auto& var : items){
            if (var.data().key != elemItem)
                return false;
            _map[var.data().attr(attrKey)] = var.data().val;
        }
		_needSave = false;
		return true;
	}
	bool save(const XString& pathfile=XString()){
		if (!pathfile.isEmpty())
			_pathfile = pathfile;

		XXml xml;
        xml.setKey(elemRoot);

		for (auto itr = _map.begin(); itr != _map.end(); ++itr)
		{
            XXmlNode node;
            node.setKey(elemItem);
            node.setAttr(attrKey, itr->first);
            node.setValue(itr->second);
            xml.addChild(node);
		}
		_needSave = false;
		return xml.save(_pathfile);
	}

	XString value(const XString& key,const XString& defVal=XString()){
		auto itr = _map.find(key);
		if (itr == _map.end()){
			_map[key] = defVal;
			_needSave = true;
		}
		return _map[key];
	}
	void setValue(const XString& key, const XString& value){
		if (_map[key] != value){
			_map[key] = value;
			_needSave = true;
		}
	}
	void setValue(const XString& key, int n) { setValue(key, XString::number(n)); }
	void setValue(const XString& key, double d) { setValue(key, XString::number(d)); }
	void setValue(const XString& key, bool b) { setValue(key, XString::fromBool(b)); }

private:
//	XConfig & operator=(const XConfig &) {}	// avoid warning C4512
//
	XString		_pathfile;
	std::unordered_map<XString, XString>	_map;
	bool		_needSave;
	
	XString elemRoot;
	XString elemItem;
	XString attrKey;
	XString attrVal;
};

#endif //_X_CONFIG_H

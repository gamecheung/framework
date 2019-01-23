#ifndef _X_XML_HPP
#define _X_XML_HPP

#include <unordered_map>
#include <fstream>
#include "XTree.hpp"
#include "XString.hpp"

class XXmlData{
public:
	XString key;
	XString val;
	std::unordered_map<std::wstring, std::wstring> attrs;
    void clean(){
        key.clear();
        val.clear();
        attrs.clear();
    }
    XString attr(const XString& attrKey) const{
        auto itr = attrs.find(attrKey.toStdWString());
        if (itr != attrs.end()){
            return itr->second;
        }
        return std::wstring();
    }
};


class XXmlNode:public XTreeNode<XXmlData>{
public:
	XXmlNode(const XString& key = XString()){ setKey(key); }
	XString key()const{ return escape(_data.key,true); }
	void setKey(const XString& key){ _data.key =escape( key); }
	XString value()const{ return escape( _data.val,true); }
	void setValue(const XString& value){ _data.val =escape( value); }

	XTreeNode<XXmlData> child(const XString& elem)const{
		for (auto& var : children()){
			if (var.data().key == elem)
				return var;
		}
		return XTreeNode<XXmlData>();
	}

	const XXmlNode& childAt(int idx)const
	{
		return static_cast<const XXmlNode&>(XTreeNode<XXmlData>::childAt(idx));
	}
	XXmlNode& childAt(int idx)
	{
		return static_cast<XXmlNode&>(XTreeNode<XXmlData>::childAt(idx));
	}

	XString attr(const XString& key){
		auto itr = _data.attrs.find(escape( key));
		if (itr != _data.attrs.end()){
			return escape( itr->second,true);
		}
		return XString();
	}
	void setAttr(const XString& key, const XString& val){
		_data.attrs[escape(key)] =escape( val);
	}
	void setAttr(const XString& key, int n){
		setAttr(key, XString::number(n));
	}
	void setAttr(const XString& key, double n){
		setAttr(key, XString::number(n));
	}
	void setAttr(const XString& key, bool b){
		setAttr(key, XString::fromBool(b));
	}

	bool fromString(const XString& str) { return parse(str.c_str()); }

	XString toString()const{
		XString tagBegin;
		XString tagEnd;
		XString tagMid;

        assert(!_data.key.empty());

		// tag begin
		tagBegin.append(L"<");
		tagBegin.append(_data.key);
		for (auto itr = _data.attrs.begin(); itr!=_data.attrs.end(); ++itr)
		{
			tagBegin.append(L" ");
			tagBegin.append(itr->first);
			tagBegin.append(L"=\"");
			tagBegin.append(itr->second);
			tagBegin.append(L"\"");
		}
		tagBegin.append(L">");

		// tag mid
		if (_children.size() == 0){
			tagMid.append(_data.val);
		}
		else{
			int count = childCount();
			tagMid.append(L"\n");
			for (int i = 0; i < count; i++)
			{
				tagMid.append(L"\t");
				XTreeNode<XXmlData> pNode = childAt(i);
				XXmlNode* node = static_cast<XXmlNode*>(&pNode);
				tagMid.append(node->toString());
				tagMid.append(L"\n");
			}
		}

		tagEnd.append(L"</");
		tagEnd.append(_data.key);
		tagEnd.append(L">");

		XString retval;
		retval.append(tagBegin);
		retval.append(tagMid);
		retval.append(tagEnd);
		return retval;
	}

private:
	XString escape(const XString& str,bool back=false) const{
		struct{
			XString src; XString dst;
		}static dict[] = {
			L"&", L"&amp;",
			L"<", L"&lt;",
			L">", L"&gt;",
			L"\"", L"&quot;",
			L"\'", L"&apos;"
		};
		static const int dictCount = sizeof(dict) / sizeof(dict[0]);

		XString retval = str;
		if (!back){
			for (size_t i = 0; i < dictCount; i++)
			{
				retval.replace(dict[i].src, dict[i].dst);
			}
		}
		else{
			for (size_t i = 0; i < dictCount; i++)
			{
				retval.replace(dict[i].dst, dict[i].src);
			}
		}
		return retval;
	}

    bool parse(const wchar_t* pstr)
    {
        _pCur = pstr;

        if (!parseStartTag())   return false;

        if (!_parsedEndTag){
            if (haveChildNode()){
                XXmlNode node;
                while (node.parse(_pCur)){
                    this->addChild(node);
                    _pCur = node._pCur;
                    node._data.clean();
                    node._children.clear();
                }
            }
            else{
                if (!parseValue())  return false;
            }
            if (!parseEndTag()) return false;
        }

        return true;
    }

    bool haveChildNode()
    {
        const wchar_t* pos = _pCur;
        while (XChar::isSpace(*pos))
            ++pos;
        return (*pos==L'<');
    }

    bool parseValue()
    {
        const wchar_t* pos = _pCur;
        while (*pos != L'<'){
            if (*pos == L'\0')
                return false;
            ++pos;
        }
        this->setValue(std::wstring(_pCur, pos - _pCur));
        _pCur = pos;
        return true;
    }

    bool parseStartTag()
    {
        skipSpace();

        if (*_pCur != L'<')     return false;
        ++_pCur;
        skipSpace();

        if (!parseTagName())    return false;

        while (parseAttr());

        skipSpace();

        if (*_pCur == L'/'){
            _parsedEndTag = true;
            ++_pCur;
            skipSpace();
        }
        else{
            _parsedEndTag = false;
        }
        if (*_pCur != L'>') return false;
        ++_pCur;
        return true;
    }

    bool parseEndTag()
    {
        skipSpace();
        if (*_pCur != L'<')     return false;
        ++_pCur;

        skipSpace();
        if (*_pCur != L'/')     return false;
        ++_pCur;

        skipSpace();
        const wchar_t* pos = _pCur;
        while (!XChar::isSpace(*pos) && (*pos != L'>')){
            ++pos;
        }
        XString tagName(std::wstring(_pCur, pos - _pCur));
        if (this->key() != tagName)   return false;
        _pCur = pos;
        skipSpace();
        if (*_pCur != L'>') return false;
        ++_pCur;
        return true;
    }

    void skipSpace()
    { 
        while (XChar::isSpace(*_pCur))
            ++_pCur;
    }
    bool parseTagName()
    {
        const wchar_t* pos = _pCur;
        while (!XChar::isSpace(*pos) && (*pos != L'/') && (*pos != L'>'))
            ++pos;
        this->setKey(std::wstring(_pCur, pos - _pCur));
        _pCur = pos;
        return (_data.key.length() != 0);
    }

    bool parseAttr()
    {
        std::wstring attrName;
        std::wstring attrValue;

        skipSpace();

        // parse attr name
        const wchar_t* pos = _pCur;
        while (!XChar::isSpace(*pos)
            && (*pos != L'/')
            && (*pos != L'>')
            && (*pos != L'=')
            ){
            ++pos;
        }
        attrName = std::wstring(_pCur, pos - _pCur);
        _pCur = pos;

        // parse =
        skipSpace();
        if (*_pCur != L'=') return false;
        ++_pCur;
        skipSpace();

        // parse " vlaue "
        if ((*_pCur != L'\'') && (*_pCur != L'\"'))   return false;
        ++_pCur;
        pos = _pCur;
        while ((*pos != L'\'') && (*pos != L'\"')){
            ++pos;
        }
        attrValue = std::wstring(_pCur, pos - _pCur);
        _pCur=pos+1;

        this->setAttr(attrName, attrValue);
        return true;
    }

private:
    const wchar_t* _pCur;
    bool    _parsedEndTag;
};

class XXml:public XXmlNode{
public:
	XXml(const XString& pathfile = XString()){ load(pathfile); }

    bool load(const XString& fileName){

		removeAllChild();

        std::wifstream fin(fileName);
        fin.imbue(std::locale(""));
        if (!fin)
            return false;

        std::wstring text((std::istreambuf_iterator<wchar_t>(fin)),
            std::istreambuf_iterator<wchar_t>());

        return fromString(text);
    }

	bool save(const XString& fileName){
		std::wofstream fout(fileName);
        fout.imbue(std::locale(""));
		if (!fout)
			return false;

		fout << toString();
		return true;
	}
private:
};

#endif //_X_XML_HPP

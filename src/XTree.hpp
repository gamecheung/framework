#ifndef _X_TREE_HPP
#define _X_TREE_HPP

#include "XList.hpp"

template <typename T>
class XTreeNode{
public:
	XTreeNode(){}
	void setData(const T& data){ _data = data; } 
	const T& data()const{ return _data; }
	T& data(){ return _data; };

	const XList<XTreeNode>& children()const{ return _children; }
	int childCount()const{ return _children.size(); }
	const XTreeNode& childAt(int idx)const{ 
		assert(0 <= idx&& idx <= (int)_children.size());
		return _children.at(idx); 
	}
	XTreeNode& childAt(int idx)
	{
		assert(0 <= idx&& idx <= (int)_children.size());
		return _children[idx];
	}

	void addChild(const XTreeNode& child){ 
        _children.append(child); 
    }
	void removeChild(const XTreeNode& child){ _children.remove(child); }
	void removeChildAt(int i){ _children.removeAt(i); }
	void removeAllChild(){ _children.clear(); }

	bool operator==(const XTreeNode<T>& other)const{
		return ((_data == other._data) && (_children == other._children));
	}

protected:
    void setParent(XTreeNode* parent){ _parent = parent; }

protected:
	T	_data;
	XList<XTreeNode> _children;
};

template <typename T>
class XTree :public XTreeNode<T>{

};

#endif //_X_TREE_HPP


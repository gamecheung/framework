#ifndef _X_OBJECT_HPP
#define _X_OBJECT_HPP

#include <list>

class XObject{
public:
	XObject(XObject* parent = nullptr) 
		:_parent(nullptr)
		, _isSelfDestruct(false){
		setParent(parent);
	}
	virtual ~XObject(){
		_isSelfDestruct = true;
		removeFromParent();
		for (auto& var : _children){
			delete var;
		}
	}
	void setParent(XObject* parent){
		removeFromParent();
		_parent = parent;
		if (_parent){
			_parent->appendChild(this);
		}
	}

private:
	void removeFromParent(){
		if (_parent){
			_parent->removeChild(this);
		}
	}
	void appendChild(XObject* child){
		for (auto& var : _children){
			if (var == child){
				return;
			}
		}
		_children.push_back(child);
	}
	void removeChild(XObject* child){
		if (_isSelfDestruct)
			return;

		auto itr = _children.begin();
		while (itr != _children.end()){
			if (*itr == child){
				_children.erase(itr);
				return;
			}
			++itr;
		}
	}

protected:
	XObject*	_parent;
private:
	std::list<XObject*>	_children;
	bool		_isSelfDestruct;
};

#endif //_X_OBJECT_HPP

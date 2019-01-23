#ifndef _X_STATE_MACHINE_HPP
#define _X_STATE_MACHINE_HPP

#include <cassert>

template<class _Ty>
class XState{
public:
	virtual ~XState(){}
	virtual void enter(_Ty*){}
	virtual void exit(_Ty*){}
	virtual void update(_Ty*, int)=0;
};

template<class _Ty>
class XStateMachine{
public:
	XStateMachine(_Ty* owner, XState<_Ty>* state) :_owner(owner), _state(state){
		assert(_owner);
		assert(_state);
	}
	//void update()const{
	//	assert(_state);
	//	_state->update(_owner);
	//}
	void update(int why=0){
		assert(_state);
		_state->update(_owner, why);
	}
	void changeState(XState<_Ty>* state){
		assert(state && _state);
		_state->exit(_owner);
		_state = state;
		_state->enter(_owner);
	}
	XState<_Ty>* state()const{ return _state; }
private:
	_Ty*			_owner;
	XState<_Ty>*	_state;
};

#endif //_X_STATE_MACHINE_HPP

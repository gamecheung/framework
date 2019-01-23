/*******************************************************************************
** 
** Copyright (C) 2016 The Xd Company Ltd.
**
** 线程类
**
** Date: 2016-06-20
** Author: XD
** Contact: QQ 978028760
**
*******************************************************************************/

#ifndef _X_THREAD_HPP
#define _X_THREAD_HPP

#include <thread>
#include "XGlobal.h"
#include "XObject.hpp"

class XThread :public XObject{
public:
    XThread(XObject* parent = nullptr)
		:XObject(parent)
		,_isRunning(false){}
    virtual ~XThread(){}

	void start(){
        waitStop();
        _wantStop = false;
        _thread = std::thread(XThread::threadFunc, this);
    }
    void stop(){ _wantStop = true; }

	std::thread::id id()const{ return _thread.get_id(); }
	void join(){
		if (_thread.joinable())
			_thread.join();
	}

	bool isRunning()const{ return _isRunning; }
	bool isFinished()const{ return !_isRunning; }
    void waitStop()
	{ 
		stop(); 
		join();
	}


	static std::thread::id currentId(){ return std::this_thread::get_id(); }
	static void sleep(unsigned int secs){std::this_thread::sleep_for(std::chrono::seconds(secs));}
	static void msleep(unsigned int msecs){std::this_thread::sleep_for(std::chrono::milliseconds(msecs));}

protected:
	virtual void run() = 0;

private:
	static int threadFunc(XThread * obj){
		obj->_isRunning = true;
		obj->run();
		obj->_isRunning = false;

		return 0;
	}

protected:
	std::thread		_thread;
	bool			_isRunning;
    bool            _wantStop;
};


#endif // _X_THREAD_H



/*******************************************************************************
** 
** Copyright (C) 2016 The Xd Company Ltd.
**
**
** Date: 2016-06-14
** Author: XD
** Contact: QQ 978028760
**
*******************************************************************************/

#ifndef _X_MUTEX_LOCKER_HPP
#define _X_MUTEX_LOCKER_HPP

#include <mutex>

class XMutexLocker {
public:
    XMutexLocker(std::mutex* mutex):_mutex(mutex){_mutex->lock();}
    ~XMutexLocker(){_mutex->unlock();}

protected:
private:
    std::mutex*	_mutex;
};


#endif // _X_MUTEX_LOCKER_HPP



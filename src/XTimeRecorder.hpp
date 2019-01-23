#ifndef _X_TIME_RECORDER_HPP
#define _X_TIME_RECORDER_HPP

#include <XTime.hpp>
#include <win32\XDbgHelper.hpp>

class XTimeRecorder
{
public:
	XTimeRecorder(const XString& id = XString()) :m_id(id), m_timeUsed(0)
	{
		m_time = XTime::currentTime();
		xPrint(XString("XTimeRecorder(%1) start.").arg(m_id));
	}
	~XTimeRecorder()
	{
		print(XString("XTimeRecorder(%1) end.").arg(m_id));
	}

	void print(const XString& msg)
	{
		int t = m_time.restart();
		m_timeUsed += t;
		xPrint(XString("%1: Time Used: %2 ms. Time Elapsed: %3 ms.\n").arg(msg).arg(t).arg(m_timeUsed));
	}

private:
	XTime	m_time;
	XString	m_id;
	int		m_timeUsed;
};

#endif //_X_TIME_RECORDER_HPP

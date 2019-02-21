#ifndef _Q_THREAD_EX_H
#define _Q_THREAD_EX_H

#include <QThread>

class QThreadEx : public QThread
{
	Q_OBJECT

signals:
	void stateChanged(int state);

public:
	enum State{Stop,Run,Pause};
	QThreadEx(QObject* parent = nullptr) :QThread(parent), m_state(Stop), m_interval(10) {}
	~QThreadEx(){stop(true);}
	
public:
	State state()const{return m_state;}
	int interval()const{ return m_interval; }
	bool wantStop()const{ return m_wantStop; }
	
public slots :
	void start(bool wait = false)
	{
		m_wantPause = false;
		m_wantStop = false;
		if (!QThread::isRunning())
			return QThread::start();

		if (wait) waitState(Run);
	}
	void stop(bool wait=false)
	{
		m_wantStop = true;
		if (wait)	waitState(Stop);
	}
	void pause(bool wait = false)
	{
		Q_ASSERT(m_state != Stop);
		m_wantPause = true;
		if (wait)	waitState(Pause);
	}
	void restart(bool wait = false)
	{
		stop(true);
		start(wait);
	}
	void setInterval(int interval)
	{
		Q_ASSERT(interval > 0);
		m_interval = interval;
	}

protected:
	virtual void preRunEvent(){}
	virtual void runEvent()=0;
	virtual void endRunEvent(){}

	void run()
	{
		setState(Run);

		preRunEvent();
		while(!m_wantStop)
		{
			if(!m_wantPause)
			{
				setState(Run);
				runEvent();
			}
			else
			{
				setState(Pause);
			}
			QThread::msleep(m_interval);
		}

		endRunEvent();

		setState(Stop);
	}
	
private:
	void waitState(State state)
	{
		while (m_state != state)
			QThread::msleep(m_interval);
	}
	void setState(State state)
	{
		if (m_state != state)
		{
			m_state = state;
			emit stateChanged(m_state);
		}
	}

private:
	bool	m_wantStop;
	bool	m_wantPause;
	State	m_state;
	int		m_interval;
};

#endif //_Q_THREAD_EX_H

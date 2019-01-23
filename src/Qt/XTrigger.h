#ifndef _X_TRIGGER_H
#define _X_TRIGGER_H

#include <QObject>
#include <QTime>
#include <QTimer>

class XTrigger : public QObject
{
	Q_OBJECT
signals:
	void triggered();

public:
	XTrigger(QObject* parent = nullptr) : QObject(parent)
	{
		m_time.restart();
		setInterval(100);
		connect(&m_timer, SIGNAL(timeout()), this, SLOT(trueTrigger()));
	}
	XTrigger(int interval, QObject* parent = nullptr) : QObject(parent)
	{
		m_time.restart();
		setInterval(interval);
		connect(&m_timer, SIGNAL(timeout()), this, SLOT(trueTrigger()));
	}

	void setInterval(int interval){ m_interval = interval; m_timer.setInterval(m_interval); }
	int interval()const{ return m_interval; }

public slots :
	void trigger()
	{
		int elapsed = m_time.elapsed();
		if (elapsed > m_interval)
		{
			trueTrigger();
		}
		else if (!m_timer.isActive())
		{
			m_timer.start();
		}
	}
	void trueTrigger()
	{
		m_timer.stop();
		m_time.restart();
		emit triggered();
	}
	
private:
	int		m_interval;
	QTimer	m_timer;
	QTime	m_time;
};

#endif //_X_TRIGGER_H

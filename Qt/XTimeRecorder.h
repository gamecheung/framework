#ifndef _X_TIME_RECORDER_HPP
#define _X_TIME_RECORDER_HPP

#include <QDebug>
#include <QTime>

class XTimeRecorder
{
public:
	XTimeRecorder(const QString& id = QString()) :m_id(id), m_timeUsed(0)
	{
		m_time = QTime::currentTime();
		qDebug() << QString("XTimeRecorder(%1) start.").arg(m_id);
	}
	~XTimeRecorder()
	{
		print(QString("XTimeRecorder(%1) end.").arg(m_id));
	}

	void print(const QString& msg)
	{
		int t = m_time.restart();
		m_timeUsed += t;
		qDebug()<<(QString("%1: Time Used: %2 ms. Time Elapsed: %3 ms.\n").arg(msg).arg(t).arg(m_timeUsed));
	}

private:
	QTime	m_time;
	QString	m_id;
	int		m_timeUsed;
};

#endif //_X_TIME_RECORDER_HPP

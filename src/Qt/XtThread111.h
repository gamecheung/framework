#ifndef _XT_THREAD_H
#define _XT_THREAD_H

#include <QThread>
#include <QWidget>
#include <QToolButton>
#include <QHBoxLayout>
#include <QStyle>
#include "../XGlobal.h"

class XtThread : public QThread
{
	Q_OBJECT

signals:
	void errored(const QString& err);
	void stateChanged(const QString& state);
	void statusChanged();
	void paused();

public:
	enum Status{Stop,Run,Pause};
	XtThread(QObject* parent = nullptr) :QThread(parent), m_status(Stop)
	{
		connect(this, SIGNAL(started()), this, SLOT(onThreadStarted()));
		connect(this, SIGNAL(finished()), this, SLOT(onThreadStopped()));
		connect(this, SIGNAL(paused()), this, SLOT(onThreadPaused()));
	}
	virtual ~XtThread(){ stop(true); }

	bool haveError()const{ return !m_lastErr.isEmpty(); }
	Status status()const{ return m_status; }

public slots:
	void start()
	{
		m_wantStop = false;
		m_wantPause = false;
		if (!isRunning())
		{
			QThread::start();
		}
		else
		{
			setStatus(Run);
		}
	}
	void stop(bool wait = false)
	{
		m_wantStop = true;
		if (wait)
		{
			while (isRunning())
			{
				QThread::msleep(1);
			}
		}
	}
	void pause()
	{
		m_wantPause = true;
		emit paused();
	}
	void restart()
	{
		stop(true);
		start();
	}

	bool wantStop()const{ return m_wantStop; }
	bool wantPause()const{ return  m_wantPause; }

protected:
	void run()
	{
		if (preRunEvent())
		{
			bool rs = true;
			while (!m_wantStop)
			{
				if (!m_wantPause)
				{
					rs = runEvent();
					if (!rs)
						break;
				}
				QThread::msleep(1);
			}
			if (rs) 
				subRunEvent();
		}

		finalRunEvent();
	}
	virtual bool preRunEvent(){ return true; }
	virtual bool runEvent() = 0;
	virtual bool subRunEvent(){ return true; }
	virtual bool finalRunEvent(){ return true; }

protected:
	void setLastErr(const QString& err)
	{
		m_lastErr = err;
		emit errored(tr("Thread(%1) error: %2").arg(objectName()).arg(m_lastErr));
	}
	void setState(const QString& state)
	{
		emit stateChanged(tr("Thread(%1) state changed: %2").arg(objectName()).arg(state));
	}
	void setStatus(Status status)
	{
		if (m_status != status)
		{
			m_status = status;
			emit statusChanged();
		}
	}

private slots:
	void onThreadStarted(){ setState(tr("Started")); setStatus(Run); }
	void onThreadStopped(){ setState(tr("Stopped")); setStatus(Stop); }
	void onThreadPaused(){ setState(tr("Paused")); setStatus(Pause); }

private:
	QString m_lastErr;
	bool	m_wantStop;
	bool	m_wantPause;
	Status	m_status;
};

class XtThreadControlWidget : public QWidget
{
	Q_OBJECT
public:
	XtThreadControlWidget(XtThread* thread, QWidget* parent = nullptr)
		:QWidget(parent), m_thread(thread)
	{
		Q_ASSERT(thread);

		QHBoxLayout* layout = new QHBoxLayout(this);
		layout->setMargin(0);
		m_startButton = addButton(style()->standardIcon(QStyle::SP_MediaPlay));
		connect(m_startButton, SIGNAL(clicked()), m_thread, SLOT(start()));
		m_pauseButton = addButton(style()->standardIcon(QStyle::SP_MediaPause));
		connect(m_pauseButton, SIGNAL(clicked()), m_thread, SLOT(pause()));
		m_stopButton = addButton(style()->standardIcon(QStyle::SP_MediaStop));
		connect(m_stopButton, SIGNAL(clicked()), m_thread, SLOT(stop()));

		connect(m_thread, SIGNAL(statusChanged()), this, SLOT(updateStatus()));
		updateStatus();
	}

private:
	QToolButton* addButton(const QIcon& icon)
	{
		QToolButton* button = new QToolButton(this);
		button->setIcon(icon);
		layout()->addWidget(button);
		return button;
	}

private slots:
	void updateStatus()
	{
		auto status = m_thread->status();
		m_startButton->setEnabled((status == XtThread::Stop) || (status == XtThread::Pause));
		m_pauseButton->setEnabled((status == XtThread::Run));
		m_stopButton->setEnabled((status == XtThread::Run) || (status == XtThread::Pause));
	}

private:
	XtThread*		m_thread;
	QToolButton* m_startButton;
	QToolButton* m_pauseButton;
	QToolButton* m_stopButton;
};

#endif //_XT_THREAD_H

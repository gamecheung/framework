#ifndef _X_PROGRESS_H
#define _X_PROGRESS_H

#include <QThread>
#include <QDialog>

class XProgress : public QThread
{
	Q_OBJECT

signals:
	void titleChanged(const QString& title);
	void textChanged(const QString& text);
	void normalFinished();
	void finishedWithException();

public:
	XProgress(QObject* parent=nullptr):QThread(parent){}

	const QString& title()const{ return m_title; }
	const QString& text()const{ return m_text; }
	const QStringList& warns()const{ return m_warns; }
	const QStringList& errors()const{ return m_errors; }

protected:
	virtual void runEvent() = 0;

	void run()
	{
		runEvent();
		if (m_warns.isEmpty() && m_errors.isEmpty())
			emit normalFinished();
		else
			emit finishedWithException();
	}

	void setTitle(const QString& title)
	{
		if (m_title != title)
		{
			m_title = title;
			emit titleChanged(m_title);
		}
	}
	void setText(const QString& text)
	{
		if (m_text != text)
		{
			m_text = text;
			emit textChanged(m_text);
		}
	}

private:
	QString m_title;
	QString m_text;
	QStringList m_warns;
	QStringList m_errors;
};

#endif // _X_PROGRESS_H

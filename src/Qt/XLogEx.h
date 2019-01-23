#ifndef _X_LOG_EX_H
#define _X_LOG_EX_H

#include <QDialog>
#include <QListWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDateTime>
#include "../XLog.hpp"

class XLogEx : public QObject, public XLog
{
	Q_OBJECT
signals:
	void infoWrited(const QDateTime& datetime,const QString& info);
	void warnWrited(const QDateTime& datetime,const QString& warn);
	void errorWrited(const QDateTime& datetime,const QString& error);

public:
	XLogEx(QObject *parent = nullptr):QObject(parent){};
	~XLogEx(){};

public slots:
	void writeInfo(const QString& text){ XLog::writeInfo(text.toStdWString()); }
	void writeWarn(const QString& text){ XLog::writeWarn(text.toStdWString()); }
	void writeError(const QString& text){ XLog::writeError(text.toStdWString()); }
protected:
	void write(const XDateTime& dateTime, Level level, const XString& text){
		XLog::write(dateTime,level,text);
		XDate date = dateTime.date();
		XTime time = dateTime.time();
		QDateTime datetime(
			QDate(date.year(), date.month(), date.day()),
			QTime(time.hour(), time.minute(), time.second()));
		QString q_text = QString::fromStdWString(text);
		switch (level)
		{
		case XLog::Info: emit infoWrited(datetime, q_text); break;
		case XLog::Warn: emit warnWrited(datetime, q_text); break;
		case XLog::Error: emit errorWrited(datetime, q_text); break;
		}
	}
};


#endif // _X_LOG_EX_H


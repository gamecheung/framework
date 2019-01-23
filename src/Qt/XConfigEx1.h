#ifndef _Q_XCONFIGEX_H
#define _Q_XCONFIGEX_H

#include <QObject>
#include "../XConfig.hpp"

class XConfigEx:public QObject,public XConfig
{
	Q_OBJECT
signals:
	void valueChanged(const QString& key, const QString& value);

public:
	XConfigEx(const QString& file = QString(), QObject* parent = nullptr)
		:XConfig(file.toStdWString()), QObject(parent){}

	QString value(const QString& key)
	{
		return QString::fromStdWString(XConfig::value(key.toStdWString()));
	}

public slots:
	void setValue(const QString& key, const QString& value)
	{
		if (this->value(key) != value)
		{
			XConfig::setValue(key.toStdWString(), value.toStdWString());
			emit valueChanged(key, value);
		}
	}
	void setValue(const QString& key, int n)
	{
		setValue(key, QString::number(n));
	}
	void setValue(const QString& key, double d)
	{
		setValue(key, QString::number(d));
	}
	void setValue(const QString& key, bool b)
	{
		setValue(key, QString::fromStdWString(XString::fromBool(b)));
	}
	
};

#endif //_Q_XCONFIGEX_H

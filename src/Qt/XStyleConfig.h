#ifndef _X_STYLE_CONFIG_H
#define _X_STYLE_CONFIG_H

#include <QObject>
#include <QDir>
#include <QFile>
#include <QApplication>

class XStyleConfig
{
public:
	XStyleConfig(const QString& dir=QString("./style"))
		: m_dir(dir)
	{}

	QStringList styles()const { return QDir(m_dir).entryList(QDir::Dirs | QDir::NoDotAndDotDot); }
	void setStyle(const QString& style) const
	{
		QString qss;

		loadAllQssFile(m_dir, qss);
		loadAllQssFile(QString("%1/%2").arg(m_dir).arg(style), qss);

		qApp->setStyleSheet(qss);
	}

private:
	void loadAllQssFile(const QString& dir, QString& qss) const
	{
		QStringList files = QDir(dir).entryList(QStringList() << "*.qss", QDir::Files);
		for (auto& var : files)
		{
			QFile file(QString("%1/%2").arg(dir).arg(var));
			if (file.open(QIODevice::ReadOnly))
			{
				qss.append(file.readAll());
			}
		}
	}

private:
	QString m_dir;
};


#endif //_X_STYLE_CONFIG_H

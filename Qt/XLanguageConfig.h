#ifndef _X_LANGUAGE_CONFIG_H
#define _X_LANGUAGE_CONFIG_H

#include <QObject>
#include <QDir>
#include <QFile>
#include <QTranslator>
#include <QApplication>

class XLanguageConfig
{
public:
	XLanguageConfig(const QString& dir=QString("./language"))
		: m_dir(dir)
	{}

	QStringList languages()const { return QDir(m_dir).entryList(QDir::Dirs | QDir::NoDotAndDotDot); }

	void setLanguage(const QString& language) const
	{
		loadAllQmFile(m_dir);
		loadAllQmFile(QString("%1/%2").arg(m_dir).arg(language));
	}

private:
	void loadAllQmFile(const QString& dir) const
	{
		QStringList files = QDir(dir).entryList(QStringList() << "*.qm", QDir::Files);
		for (auto& var : files)
		{
			QString file(QString("%1/%2").arg(dir).arg(var));
			QTranslator* translator = new QTranslator(qApp);
			translator->load(file);
			qApp->installTranslator(translator);
		}
	}


private:
	QString m_dir;
};


#endif //_X_LANGUAGE_CONFIG_H

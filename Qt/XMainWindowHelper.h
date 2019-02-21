#ifndef _XMAIN_WINDOW_HELPER_H
#define _XMAIN_WINDOW_HELPER_H

#include <QMainWindow>
#include <QHash>
#include <QEvent>
#include <QSettings>
#include <QCoreApplication>
#include <QCloseEvent>

class XMainWindowHelper : public QObject{
	Q_OBJECT
public:
	~XMainWindowHelper()
	{
		instances()->remove(m_obj);
	}
	static XMainWindowHelper* getInstance(QMainWindow* obj){
		if (!instances()->contains(obj)){
			(*instances())[obj] = new XMainWindowHelper(obj);
		}
		return (*instances())[obj];
	}

private slots:
	void newFile()
	{
		if (maybeSave())
		{
			setCurrentFile(QString());
		}
	}
	//void open();
	//bool save();
	//bool saveAs();
	//void about();
	void documentWasModified(){ m_needSave = true; }

private:
	bool maybeSave()
	{
		return true;
	}
	bool saveFile(const QString& fileName);
	void setCurrentFile(const QString& fileName){}
	void readSettings()
	{
		QSettings settings(QCoreApplication::applicationName()+".ini", QSettings::IniFormat);
		m_obj->restoreGeometry(settings.value("geometry").toByteArray());
		m_obj->restoreState(settings.value("state").toByteArray());
	}
	void writeSettings()
	{
		QSettings settings(QCoreApplication::applicationName() + ".ini", QSettings::IniFormat);
		settings.setValue("geometry", m_obj->saveGeometry());
		settings.setValue("state", m_obj->saveState());
	}

protected:
	bool eventFilter(QObject *obj, QEvent *event)
	{
		if (obj == m_obj)
		{
			if (event->type() == QEvent::Close)
			{
				QCloseEvent* e = dynamic_cast<QCloseEvent*>(event);
				if (maybeSave())
				{
					writeSettings();
					event->accept();
				}
				else
				{
					event->ignore();
				}
			}
			return true;
		}
		return QObject::eventFilter(obj, event);
	}

private:
    XMainWindowHelper(QMainWindow* obj) :QObject(obj), m_obj(obj){ 
        Q_ASSERT(m_obj);
		m_obj->installEventFilter(this);
		readSettings();

		m_obj->setUnifiedTitleAndToolBarOnMac(true);
		setCurrentFile(QString());
    }
	static QHash<QMainWindow*, XMainWindowHelper*>* instances(){
		static QHash<QMainWindow*, XMainWindowHelper*> s_instances;
		return &s_instances;
	}
private:
	QMainWindow*	m_obj;
	QString			m_curFile;
	bool			m_needSave;
};

#define XMAIN_WINDOW_HELPER(obj)  XMainWindowHelper::getInstance(obj)

#endif // _XMAIN_WINDOW_HELPER

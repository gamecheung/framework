#ifndef _X_DOCK_WIDGET_HELPER_H
#define _X_DOCK_WIDGET_HELPER_H

#include <QDockWidget>
#include <QHash>
#include <QTimer>

class XDockWidgetHelper : public QObject{
	Q_OBJECT
public:
	~XDockWidgetHelper(){instances()->remove(m_obj);}
	static XDockWidgetHelper* getInstance(QDockWidget* obj){
		if (!instances()->contains(obj)){
			(*instances())[obj] = new XDockWidgetHelper(obj);
		}
		return (*instances())[obj];
	}

	void setSize(const QSize& size)
	{
		saveDockWidgetSize();

		if (size.width() > m_obj->size().width())
			m_obj->setMinimumWidth(size.width());
		else
			m_obj->setMaximumWidth(size.width());

		if (size.height() > m_obj->size().height())
			m_obj->setMinimumHeight(size.height());
		else
			m_obj->setMaximumHeight(size.height());

		QTimer::singleShot(1, this, SLOT(restoreDockWidgetSize()));
	}

private slots:
	void saveDockWidgetSize()
	{
		m_preMaxSize = m_obj->maximumSize();
		m_preMinSize = m_obj->minimumSize();
	}
	void restoreDockWidgetSize()
	{
		m_obj->setMinimumSize(m_preMinSize);
		m_obj->setMaximumSize(m_preMaxSize);
	}

private:
    XDockWidgetHelper(QDockWidget* obj) :QObject(obj), m_obj(obj){ 
        Q_ASSERT(m_obj);
        // code here
    }
	static QHash<QDockWidget*, XDockWidgetHelper*>* instances(){
		static QHash<QDockWidget*, XDockWidgetHelper*> s_instances;
		return &s_instances;
	}
private:
	QDockWidget*	m_obj;
	QSize	m_preMinSize;
	QSize	m_preMaxSize;
};

#define X_DOCK_WIDGET_HELPER(obj)  XDockWidgetHelper::getInstance(obj)

#endif // _X_DOCK_WIDGET_HELPER_H

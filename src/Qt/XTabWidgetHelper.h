#ifndef _XTAB_WIDGET_HELPER_H
#define _XTAB_WIDGET_HELPER_H

#include <QTabWidget>
#include <QHash>


class XTabWidgetHelper : public QObject{
	Q_OBJECT
public:
	~XTabWidgetHelper(){instances()->remove(m_obj);}
	static XTabWidgetHelper* getInstance(QTabWidget* obj){
		if (!instances()->contains(obj)){
			(*instances())[obj] = new XTabWidgetHelper(obj);
		}
		return (*instances())[obj];
	}



private:
    XTabWidgetHelper(QTabWidget* obj) :QObject(obj), m_obj(obj){ 
        Q_ASSERT(m_obj);
        // code here
    }
	static QHash<QTabWidget*, XTabWidgetHelper*>* instances(){
		static QHash<QTabWidget*, XTabWidgetHelper*> s_instances;
		return &s_instances;
	}
private:
	QTabWidget*	m_obj;
};

#define XTAB_WIDGET_HELPER(obj)  XTabWidgetHelper::getInstance(obj)

#endif // _XTAB_WIDGET_HELPER

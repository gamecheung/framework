#ifndef _XTOOL_BUTTON_HELPER_H
#define _XTOOL_BUTTON_HELPER_H

#include <QToolButton>
#include <QHash>
#include <QMenu>
#include <QWidgetAction>


class XToolButtonHelper : QObject{
	Q_OBJECT
public:
	~XToolButtonHelper(){instances()->remove(_obj);}
	static XToolButtonHelper* getInstance(QToolButton* obj){
		if (!instances()->contains(obj)){
			(*instances())[obj] = new XToolButtonHelper(obj);
		}
		return (*instances())[obj];
	}
	QAction* addWidgetMenu(QWidget* widget)
	{
		Q_ASSERT(widget);

		QMenu* menu = _obj->menu();
		if (menu == nullptr)
		{
			menu = new QMenu(_obj);
			_obj->setMenu(menu);
		}

		QWidgetAction* action = new QWidgetAction(menu);
		action->setDefaultWidget(widget);
		menu->addAction(action);
		return action;
	}
private:
    XToolButtonHelper(QToolButton* obj) :QObject(obj), _obj(obj){ 
        Q_ASSERT(_obj);
        // code here
    }
	static QHash<QToolButton*, XToolButtonHelper*>* instances(){
		static QHash<QToolButton*, XToolButtonHelper*> s_instances;
		return &s_instances;
	}
private:
	QToolButton*	_obj;
};

#define XTOOL_BUTTON_HELPER(obj)  XToolButtonHelper::getInstance(obj)

#endif // _XTOOL_BUTTON_HELPER

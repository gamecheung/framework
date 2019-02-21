#ifndef _XBUBBLE_WINDOW_HELPER_H
#define _XBUBBLE_WINDOW_HELPER_H

#include <QWidget>
#include <QHash>
#include <QPropertyAnimation>
#include <QApplication>
#include <QDesktopWidget>

class XBubbleWindowHelper : public QObject{
	Q_OBJECT
public:
	~XBubbleWindowHelper(){instances()->remove(m_obj);}
	static XBubbleWindowHelper* getInstance(QWidget* obj){
		if (!instances()->contains(obj)){
			(*instances())[obj] = new XBubbleWindowHelper(obj);
		}
		return (*instances())[obj];
	}

public:
	void show(int showTime = 500, int stayTime = 3000, int closeTime = 500)
	{
		m_aniShow->stop();
		m_aniStay->stop();
		m_aniClose->stop();

		m_aniShow->setDuration(showTime);
		m_aniStay->setDuration(stayTime);
		m_aniClose->setDuration(closeTime);

		QRect rect = QApplication::desktop()->availableGeometry();
		QPoint pos;
		pos.setX(rect.width() - m_obj->width());
		pos.setY(rect.height() - m_obj->height());

		m_aniShow->setPropertyName("pos");
		m_aniShow->setStartValue(QPoint(pos.x(), rect.height()));
		m_aniShow->setEndValue(pos);

		m_aniStay->setPropertyName("pos");
		m_aniStay->setStartValue(pos);
		m_aniStay->setEndValue(pos);

		m_aniClose->setPropertyName("pos");
		m_aniClose->setStartValue(pos);
		m_aniClose->setEndValue(QPoint(pos.x(), rect.height()));

		m_obj->show();
		m_aniShow->start();
	}

private:
	XBubbleWindowHelper(QWidget* obj) :QObject(obj), m_obj(obj){
        Q_ASSERT(m_obj);
		Q_ASSERT(m_obj->isWindow());

		m_aniShow = new QPropertyAnimation(this);
		m_aniShow->setTargetObject(m_obj);
		m_aniStay = new QPropertyAnimation(this);
		m_aniStay->setTargetObject(m_obj);
		m_aniClose = new QPropertyAnimation(this);
		m_aniClose->setTargetObject(m_obj);
		connect(m_aniShow, SIGNAL(finished()), m_aniStay, SLOT(start()));
		connect(m_aniStay, SIGNAL(finished()), m_aniClose, SLOT(start()));
		connect(m_aniClose, SIGNAL(finished()), m_obj, SLOT(close()));
    }
	static QHash<QWidget*, XBubbleWindowHelper*>* instances(){
		static QHash<QWidget*, XBubbleWindowHelper*> s_instances;
		return &s_instances;
	}
private:
	QWidget*	m_obj;
	QPropertyAnimation* m_aniShow;
	QPropertyAnimation*	m_aniStay;
	QPropertyAnimation* m_aniClose;
};

#define XBUBBLE_WINDOW_HELPER(obj)  XBubbleWindowHelper::getInstance(obj)

#endif // _XBUBBLE_WINDOW_HELPER

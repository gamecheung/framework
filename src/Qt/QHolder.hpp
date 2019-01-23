#ifndef _Q_HOLDER_HPP
#define _Q_HOLDER_HPP

#include <QWidget>
#include <QApplication>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPainter>

class QWidgetDisabler{
public:
	QWidgetDisabler(QWidget* obj)
		:_obj(obj){
		Q_ASSERT(_obj);
		_obj->setDisabled(true);
	}
	~QWidgetDisabler(){
		_obj->setDisabled(false);
	}
private:
	QWidget* _obj;
};

class XWidgetHidder{
public:
	XWidgetHidder(QWidget* obj)
		:_obj(obj){
		Q_ASSERT(_obj);
		_obj->setVisible(false);
	}
	~XWidgetHidder(){
		_obj->setVisible(true);
	}
private:
	QWidget* _obj;
};

class XWidgetQssDisabler
{
public:
	XWidgetQssDisabler(QWidget* widget)
		:m_widget(widget)
	{
		Q_ASSERT(m_widget);
		m_qss = m_widget->styleSheet();
		m_widget->setStyleSheet("");
	}
	~XWidgetQssDisabler(){ m_widget->setStyleSheet(m_qss); }
private:
	QWidget*	m_widget;
	QString		m_qss;
};

class XQssDisabler
{
public:
	XQssDisabler(){ m_qss = qApp->styleSheet(); qApp->setStyleSheet(""); }
	~XQssDisabler(){ qApp->setStyleSheet(m_qss); }
private:
	QString m_qss;
};

class XGraphicsItemRefresher
{
public:
	XGraphicsItemRefresher(QGraphicsItem* obj) :m_obj(obj)
	{
		Q_ASSERT(m_obj);
		m_sRect = m_obj->boundingRect();
	}
	~XGraphicsItemRefresher()
	{
		if (m_obj->scene())
		{
			m_eRect = m_obj->boundingRect();
			QRectF rect;
			rect.setLeft(qMin(m_sRect.left(), m_eRect.left()));
			rect.setTop(qMin(m_sRect.top(), m_eRect.top()));
			rect.setRight(qMax(m_sRect.right(), m_eRect.right()));
			rect.setBottom(qMax(m_sRect.bottom(), m_eRect.bottom()));
			rect.moveTopLeft(QPointF(rect.x() + m_obj->pos().x(), rect.y() + m_obj->pos().y()));

			m_obj->scene()->update(rect);
		}
	}
private:
	QGraphicsItem* m_obj;
	QRectF	m_sRect;
	QRectF	m_eRect;
};

class QPainterHolder
{
public:
	QPainterHolder(QPainter* painter) :m_painter(painter){ m_painter->save(); }
	~QPainterHolder(){ m_painter->restore(); }

private:
	QPainter* m_painter;
};



#endif //_Q_HOLDER_HPP

#ifndef _X_PIXMAP_FACTORY_H
#define _X_PIXMAP_FACTORY_H

#include <QSize>
#include <QPainter>
#include <QPixmap>
#include <QDebug>

class XPixmapFactory{
public:
	virtual ~XPixmapFactory(){}
	QPixmap pixmap(){
		QPixmap pixmap(size());
		pixmap.fill(Qt::transparent);
		QPainter painter(&pixmap);
		draw(&painter);
		painter.end(); 
		return pixmap;
	}
protected:
	virtual QSize size() = 0;
	virtual void draw(QPainter* painter) = 0;
};


class XRectPixmapFactory :public XPixmapFactory{
public:
	QPixmap pixmap(const QSize& size,const QColor& color=Qt::black,int width = 1,bool fill=false){
		m_size = size;
		m_color = color;
		m_width = width;
		m_fill = fill;
		return XPixmapFactory::pixmap();
	}
protected:
	virtual QSize size(){ return m_size; }
	virtual void draw(QPainter* painter)
	{
		QPen pen = painter->pen();
		pen.setColor(m_color);
		pen.setWidth(m_width);
		painter->setPen(pen);
		painter->drawRect(0, 0, m_size.width()-1, m_size.height()-1);
		if (m_fill)
			painter->fillRect(0, 0, m_size.width(), m_size.height(),m_color);
	}
private:
	QSize	m_size;
	QColor	m_color;
	int		m_width;
	bool	m_fill;
};

class XAlignPixmapFactory :public XPixmapFactory{
public:
	QPixmap pixmap(Qt::Alignment align){
		_align = align;
		return XPixmapFactory::pixmap();
	}
protected:
	virtual QSize size(){
		return QSize(9, 9);
	}
	virtual void draw(QPainter* painter){
		switch (_align)
		{
		case Qt::AlignLeft:
			painter->drawLine(0, 1, 8, 1);
			painter->drawLine(0, 3, 6, 3);
			painter->drawLine(0, 5, 8, 5);
			painter->drawLine(0, 7, 6, 7);
			break;
		case Qt::AlignHCenter:
			painter->drawLine(0, 1, 8, 1);
			painter->drawLine(1, 3, 7, 3);
			painter->drawLine(0, 5, 8, 5);
			painter->drawLine(1, 7, 7, 7);
			break;
		case Qt::AlignRight:
			painter->drawLine(0, 1, 8, 1);
			painter->drawLine(2, 3, 8, 3);
			painter->drawLine(0, 5, 8, 5);
			painter->drawLine(2, 7, 8, 7);
			break;
		case Qt::AlignTop:
			painter->drawLine(0, 0, 8, 0);
			painter->drawLine(1, 2, 7, 2);
			painter->drawLine(0, 4, 8, 4);
			break;
		case Qt::AlignVCenter:
			painter->drawLine(0, 2, 8, 2);
			painter->drawLine(1, 4, 7, 4);
			painter->drawLine(0, 6, 8, 6);
			break;
		case Qt::AlignBottom:
			painter->drawLine(0, 4, 8, 4);
			painter->drawLine(1, 6, 7, 6);
			painter->drawLine(0, 8, 8, 8);
			break;
		}
	}
private:
	Qt::Alignment _align;
};

class XAlignPixmapFactory2 :public XPixmapFactory{
public:
	QPixmap pixmap(Qt::Alignment align){
		_align = align;
		return XPixmapFactory::pixmap();
	}
protected:
	virtual QSize size(){
		return QSize(9, 9);
	}
	virtual void draw(QPainter* painter){
		switch (_align)
		{
		case Qt::AlignLeft:
			painter->drawLine(0, 0, 0, 8);
			painter->fillRect(QRect(2, 2, 7, 2), Qt::black);
			painter->fillRect(QRect(2, 5, 5, 2), Qt::black);
			break;
		case Qt::AlignHCenter:
			painter->drawLine(4, 0, 4, 8);
			painter->fillRect(QRect(0, 2, 9, 2), Qt::black);
			painter->fillRect(QRect(1, 5, 7, 2), Qt::black);
			break;
		case Qt::AlignRight:
			painter->drawLine(8, 0, 8, 8);
			painter->fillRect(QRect(0, 2, 7, 2), Qt::black);
			painter->fillRect(QRect(2, 5, 5, 2), Qt::black);
			break;
		case Qt::AlignTop:
			painter->drawLine(0, 0, 8, 0);
			painter->fillRect(QRect(2, 2, 2, 7), Qt::black);
			painter->fillRect(QRect(5, 2, 2, 5), Qt::black);
			break;
		case Qt::AlignVCenter:
			painter->drawLine(0, 4, 8, 4);
			painter->fillRect(QRect(2, 0, 2, 9), Qt::black);
			painter->fillRect(QRect(5, 1, 2, 7), Qt::black);
			break;
		case Qt::AlignBottom:
			painter->drawLine(0, 8, 8, 8);
			painter->fillRect(QRect(2, 0, 2, 7), Qt::black);
			painter->fillRect(QRect(5, 2, 2, 5), Qt::black);
			break;
		}
	}
private:
	Qt::Alignment _align;
};

#endif //_X_PIXMAP_FACTORY_H

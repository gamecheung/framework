#ifndef _X_BOUNDING_RECT_H
#define _X_BOUNDING_RECT_H

#include <QRectF>

class XBoundingRect
{
public:
	XBoundingRect(const QRectF& rect)
		:m_rect(rect){ }
	XBoundingRect(const QPointF& point)
	{
		m_rect.adjust(point.x(), point.y(), point.x(), point.y());
	}

	const QRectF& value()const{ return m_rect; }
	XBoundingRect& append(const QRectF& rect)
	{
		if (rect.left() < m_rect.left())		m_rect.setLeft(rect.left());
		if (rect.top() < m_rect.top())			m_rect.setTop(rect.top());
		if (rect.right() > m_rect.right())		m_rect.setRight(rect.right());
		if (rect.bottom() > m_rect.bottom())	m_rect.setBottom(rect.bottom());
		return *this;
	}
	XBoundingRect& append(const QPointF& pos)
	{
		if (pos.x() < m_rect.left())		m_rect.setLeft(pos.x());
		if (pos.y() < m_rect.top())			m_rect.setTop(pos.y());
		if (pos.x() > m_rect.right())		m_rect.setRight(pos.x());
		if (pos.y() > m_rect.bottom())		m_rect.setBottom(pos.y());
		return *this;
	}

private:
	QRectF m_rect;
};

#endif //_X_BOUNDING_RECT_H

#ifndef _X_FUNCTION_H
#define _X_FUNCTION_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>

class XFunctionWidget : public QWidget
{
	Q_OBJECT

public:
	XFunctionWidget(QWidget *parent = 0)
	{
		setMouseTracking(true);
		setCursor(Qt::CrossCursor);
	}
	~XFunctionWidget(){}

protected:
	void paintEvent(QPaintEvent *event)
	{
		QPainter painter(this);
		painter.drawLine(0, 0, width(), height());
		painter.drawRect(0, 0, width()-1, height()-1);

		painter.drawText(m_cursorPos, QString("(%1,%2)").arg(m_cursorPos.x()).arg(m_cursorPos.y()));
	}

	void mouseMoveEvent(QMouseEvent *event)
	{
		m_cursorPos = event->pos();
		update();
	}

private:
	QPoint m_cursorPos;
};


#endif //_X_FUNCTION_H

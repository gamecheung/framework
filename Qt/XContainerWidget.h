#ifndef _X_CONTAINER_WIDGET_H
#define _X_CONTAINER_WIDGET_H

#include <QLabel>
#include <QPixmap>
#include <QVBoxLayout>
#include <QPainter>

class XContainerWidget : public QLabel
{
	Q_OBJECT
public:
	XContainerWidget(const QPixmap& pixmap, const QRect& mask,const QString& text=QString(), QWidget* parent = nullptr)
		:QLabel(parent), m_pixmap(pixmap), m_mask(mask), m_minVal(0), m_maxVal(100), m_curVal(0)
		, m_alignment(Qt::AlignBottom), m_brush(Qt::blue)
	{
		m_textLabel = new QLabel(text, this);
		m_textLabel->setAlignment(Qt::AlignCenter);

		QVBoxLayout* layout = new QVBoxLayout();
		layout->addWidget(m_textLabel);
		setLayout(layout);
		setScaledContents(true);

		updatePixmap();
	}

public slots:
	void setRange(int minVal, int maxVal)
	{ 
		if (minVal >= maxVal)
		{
			minVal = maxVal - 1;
			Q_ASSERT(false);
		}

		m_minVal = minVal; 
		m_maxVal = maxVal; 
		updatePixmap();
	}
	void setMinValue(int value)
	{
		m_minVal = value;
		updatePixmap();
	}
	void setMaxValue(int value)
	{
		m_maxVal = value;
		updatePixmap();
	}
	void setValue(int value)
	{ 
		Q_ASSERT(m_minVal <= value && value <= m_maxVal);

		m_curVal = value; 
		updatePixmap();
	}
	void setValue(int value, const QBrush& brush)
	{
		m_brush = brush;
		setValue(value);
	}
	void setText(const QString& text){ m_textLabel->setText(text); }
	void setAlignment(Qt::Alignment align){ m_alignment = align; updatePixmap(); }
	void setBrush(const QBrush& brush){ m_brush = brush; updatePixmap(); }

public:
	int minValue()const{ return m_minVal; }
	int maxValue()const{ return m_maxVal; }
	int currentValue()const{ return m_curVal; }
	double percent()const{ return m_percent; }

	QLabel* textLabel()const{ return m_textLabel; }
	const QBrush& brush()const{ return m_brush; }

private:
	void updatePixmap()
	{
		int total = m_maxVal - m_minVal;
		Q_ASSERT(total > 0);

		int val = m_curVal - m_minVal;
		m_percent = val*100.0 / total;

		QRect rect = m_mask;

		if (m_alignment&Qt::AlignTop || m_alignment& Qt::AlignBottom ||m_alignment&Qt::AlignVCenter)
		{
			rect.setHeight(rect.height()*m_percent/100);
		}
		if (m_alignment&Qt::AlignLeft || m_alignment& Qt::AlignRight)
		{
			rect.setWidth(rect.width()*m_percent/100);
		}

		if (m_alignment&Qt::AlignTop) rect.moveTop(m_mask.top());
		if (m_alignment&Qt::AlignBottom) rect.moveBottom(m_mask.bottom());
		if (m_alignment&Qt::AlignVCenter) rect.moveCenter(m_mask.center());
		if (m_alignment&Qt::AlignLeft) rect.moveLeft(m_mask.left());
		if (m_alignment&Qt::AlignRight) rect.moveRight(m_mask.right());
		if (m_alignment&Qt::AlignHCenter) rect.moveCenter(m_mask.center());

		QPixmap pixmap = m_pixmap.copy();
		QPainter painter(&pixmap);
		painter.fillRect(rect, m_brush);
		painter.end();
		setPixmap(pixmap);
	}


private:
	QLabel* m_textLabel;
	QPixmap m_pixmap;
	QRect	m_mask;
	int		m_minVal;
	int		m_maxVal;
	int		m_curVal;
	double  m_percent;
	Qt::Alignment m_alignment;
	QBrush	m_brush;
};

#endif //_X_CONTAINER_WIDGET_H

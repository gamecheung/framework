#ifndef _X_FUNC_WIDGET_H
#define _X_FUNC_WIDGET_H

#include <QWidget>
#include <QVector>
#include <QPoint>
#include <QPainter>
#include <cstdint>
#include <cassert>

class XFuncWidget : public QWidget{
	Q_OBJECT
public:
	XFuncWidget(const double minVal,const double maxVal,const double recVal,const uint16_t pointCount,QWidget* parent=nullptr)
		:QWidget(parent), _minVal(minVal), _maxVal(maxVal),_recVal(recVal), _pointCount(pointCount), _currentPointIdx(0)
	{
		assert(_minVal <= _recVal&&_recVal <= _maxVal);
		assert(_pointCount >= 2);
		_range = _maxVal - _minVal;
		_vals.resize(_pointCount);
		_pointsX.resize(_pointCount);
		_pointsY.resize(_pointCount);

		setAutoFillBackground(true);
		QPalette palette;
		palette.setColor(QPalette::Background, QColor(0,0,0));
		setPalette(palette);

        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	}
	~XFuncWidget(){}

public slots:
	void appendVal(const double val)
	{
		_vals[_currentPointIdx] = val;
		updatePointY(_currentPointIdx);
		_currentPointIdx = (_currentPointIdx + 1) % _pointCount;
		_str.setNum(val);
		update();
	}

private:
	void updateAllPointsXY()
	{
		double gap = width()*1.0 / (_pointCount-1);
		for (size_t i = 0; i < _pointCount; i++)
		{
			_pointsX[i]=i*gap;
			updatePointY(i);
		}

		int recY = (_maxVal - _recVal)*height() / _range;
		_recPoint1.setX(0);
		_recPoint1.setY(recY);
		_recPoint2.setX(width());
		_recPoint2.setY(recY);

		update();
	}
	void updatePointY(int idx)
	{
		assert(idx < _pointCount);
		_pointsY[idx]=(_maxVal - _vals[idx])*height() / _range;
	}

protected:
	void paintEvent(QPaintEvent * event)
	{
		event;
		QPainter painter(this);
		painter.setPen(QColor(255,255,255,100));
		painter.drawLine(_recPoint1, _recPoint2);
		painter.setPen(Qt::green);
		for (int i = 0; i < _pointCount-1; i++)
		{
			int x1 = (_currentPointIdx + i) % _pointCount;
			int x2 = (_currentPointIdx + i+1) % _pointCount;
			painter.drawLine(_pointsX[i], _pointsY[x1], _pointsX[i + 1], _pointsY[x2]);
		}
		painter.drawText(5, 10, _str);
		painter.setPen(Qt::white);
	}
	void resizeEvent(QResizeEvent * event)
	{
		event;
		updateAllPointsXY();
	}

private:
	double			_minVal;
	double			_maxVal;
	double			_recVal;
	double			_range;
	uint16_t		_pointCount;
	QVector<double>	_vals;
	QVector<int>	_pointsX;
	QVector<int>	_pointsY;
	QPoint			_recPoint1;
	QPoint			_recPoint2;
	int				_currentPointIdx;
	QString			_str;
};

#endif //_X_FUNC_WIDGET_H


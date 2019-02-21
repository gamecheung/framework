#ifndef _X_QT_H
#define _X_QT_H

#include <QObject>
#include <QMutex>
#include <QWidget>
#include <QString>
#include <QSize>
#include <QPainter>
#include <QBitmap>
#include <QPalette>
#include <QApplication>
#include <QDir>
#include <QProcess>

//#define SET_WIDGET_BG_COLOR(w,color)	w->setStyleSheet(QString("background-color:rgba(%1,%2,%3,%4);")	\
//	.arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha()))

inline void SET_WIDGET_BG_COLOR(QWidget* w, const QColor& color)
{
	w->setAutoFillBackground(true);
	QPalette pat = w->palette();
	pat.setColor(QPalette::Background, color);
	w->setPalette(pat);
}

inline void rebootApp()
{
	QString program = QApplication::applicationFilePath();
	QStringList arguments = QApplication::arguments();
	QString workingDirectory = QDir::currentPath();
	QProcess::startDetached(program, arguments, workingDirectory);
	QApplication::exit();
}

class QtHelper{
public:
	static QSize covertStringToSize(const QString& str){
		auto vars = str.split(',');
		if (vars.count() == 2){
			return QSize(vars.at(0).toInt(), vars.at(1).toInt());
		}
		return QSize();
	}
	static QString covertSizeToString(const QSize& size){
		QStringList strs;
		strs.append(QString::number(size.width()));
		strs.append(QString::number(size.height()));
		return strs.join(',');
	}
	static QRect covertStringToRect(const QString& str){
		auto vars = str.split(',');
		if (vars.count() == 4){
			return QRect(
				vars.at(0).toInt(),
				vars.at(1).toInt(),
				vars.at(2).toInt(),
				vars.at(3).toInt());
		}
		return QRect();
	}
	static QString covertRectToString(const QRect& rect){
		return QString("%1,%2,%3,%4")
			.arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height());
	}
	static QPoint covertStringToPoint(const QString& str){
		auto vars = str.split(',');
		if (vars.count() == 2){
			return QPoint(vars.at(0).toInt(), vars.at(1).toInt());
		}
		return QPoint();
	}
	static QString covertPointToString(const QPoint& point){
		QStringList strs;
		strs.append(QString::number(point.x()));
		strs.append(QString::number(point.y()));
		return strs.join(',');
	}
	static QColor covertStringToColor(const QString& str){
		auto vars = str.split(',');
		if (vars.count() == 3){
			return QColor(vars.at(0).toInt(), vars.at(1).toInt(), vars.at(2).toInt());
		}
		if (vars.count() == 4){
			return QColor(vars.at(0).toInt(), vars.at(1).toInt(), vars.at(2).toInt(), vars.at(3).toInt());
		}
		return QColor();
	}
	static QString covertColorToString(const QColor& color){
		QStringList strs;
		strs.append(QString::number(color.red()));
		strs.append(QString::number(color.green()));
		strs.append(QString::number(color.blue()));
		strs.append(QString::number(color.alpha()));
		return strs.join(',');
	}

	static QString toString(const QList<int>& l)
	{
		QStringList ret;
		for (auto& n : l)
			ret.append(QString::number(n));
		return ret.join(",");
	}
	static bool fromString(QList<int>& l, const QString& str)
	{
		l.clear();
		auto ss = str.split(",");
		for (auto& s : ss)
			l.append(s.toInt());

		return true;
	}

	static QList<double> stringToDoubleList(const QString& str)
	{
		QList<double> ret;
		auto strs = str.split(",");
		for (const auto& s : strs)
			ret.append(s.toDouble());

		return ret;
	}
	static QString doubleListToString(const QList<double>& lst)
	{
		QStringList strs;
		for (auto d : lst)
			strs.append(QString::number(d));
		return strs.join(",");
	}

	static QStringList getDirFiles(const QString& dirpath, const QStringList& filters = QStringList())
	{
		QStringList files;
		QDir dir(dirpath);
		dir.setFilter(QDir::Files);
		dir.setNameFilters(filters);
		auto list = dir.entryInfoList();
		for (const auto& file : list)
		{
			files.append(file.filePath());
		}
		return files;
	}
};

#endif //_X_QT_H

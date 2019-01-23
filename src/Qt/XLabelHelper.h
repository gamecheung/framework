#ifndef _XLABEL_HELPER_H
#define _XLABEL_HELPER_H

#include <QLabel>
#include <QHash>
#include <QTimer>

class XLabelHelper : QObject{
	Q_OBJECT
public:
	~XLabelHelper(){instances()->remove(_obj);}
	static XLabelHelper* getInstance(QLabel* obj){
		if (!instances()->contains(obj)){
			(*instances())[obj] = new XLabelHelper(obj);
		}
		return (*instances())[obj];
	}
public slots:
	void flash(const QString& text = QString(), int timeout = 1000){
		if (!text.isEmpty())
			_obj->setText(text);
		_flashCount = timeout / _flashTimer.interval();
		_flashTimer.start();
	}
	void setText(const QString& text){
		_obj->setText(text);
		_srcText = text;
	}
private:
    XLabelHelper(QLabel* obj) :QObject(obj), _obj(obj){ 
        Q_ASSERT(_obj);
		_srcText = obj->text();
		_flashTimer.setInterval(100);
		connect(&_flashTimer, SIGNAL(timeout()), this, SLOT(innerFlash()));
    }
	static QHash<QLabel*, XLabelHelper*>* instances(){
		static QHash<QLabel*, XLabelHelper*> s_instances;
		return &s_instances;
	}
private slots:
	void innerFlash(){
		static bool b = false;
		--_flashCount; 
		if (_flashCount < 0){
			_flashTimer.stop();
			_obj->setStyleSheet("");
			_obj->setText(_srcText);
		}

		b = !b;
		_obj->setStyleSheet(b ? "color:red;" : "color:blue;");
		
	}
private:
	QLabel*	_obj;
	QString _srcText;
	QTimer	_flashTimer;
	int		_flashCount;
};

#define XLABEL_HELPER(obj)  XLabelHelper::getInstance(obj)

#endif // _XLABEL_HELPER

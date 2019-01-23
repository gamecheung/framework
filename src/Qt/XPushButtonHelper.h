#ifndef _XPUSH_BUTTON_HELPER_H
#define _XPUSH_BUTTON_HELPER_H

#include <QPushButton>
#include <QHash>
#include <QTimer>
#include <QColorDialog>

class XPushButtonHelper : QObject{
	Q_OBJECT
public:
	enum Mode{ NoMode,ColorMode,FontMode };
	~XPushButtonHelper()
	{
		instances()->remove(_obj);
	}
	static XPushButtonHelper* getInstance(QPushButton* obj){
		if (!instances()->contains(obj)){
			(*instances())[obj] = new XPushButtonHelper(obj);
		}
		return (*instances())[obj];
	}

	void setMode(Mode mode){ _mode = mode; }
	Mode mode()const{ return _mode; }


	void setBackgroundColor(const QColor& color)
	{
		auto palette = _obj->palette();
		palette.setColor(QPalette::Button, color);
		_obj->setText(color.name());
		_obj->setPalette(palette);
		_obj->setFlat(true);
		_obj->setAutoFillBackground(true);
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
    XPushButtonHelper(QPushButton* obj) :QObject(obj), _obj(obj),_mode(NoMode){ 
        Q_ASSERT(_obj);
		_srcText = obj->text();
		_flashTimer.setInterval(100);
		connect(_obj, SIGNAL(clicked()), this, SLOT(onPushButtonClicked()));
		connect(&_flashTimer, SIGNAL(timeout()), this, SLOT(innerFlash()));
    }
	static QHash<QPushButton*, XPushButtonHelper*>* instances(){
		static QHash<QPushButton*, XPushButtonHelper*> s_instances;
		return &s_instances;
	}

	void setColor()
	{
		QColor color = QColorDialog::getColor(QColor(_obj->text()));
		if (color.isValid())
		{
			setBackgroundColor(color);
		}
	}



private slots:
	void innerFlash(){
		static bool b = false;
		--_flashCount; 
		if (_flashCount < 0){
			_flashTimer.stop();
			_obj->setStyleSheet("");
			_obj->setText(_srcText);
			return;
		}

		b = !b;
		_obj->setStyleSheet(b ? "color:red;" : "color:blue;");
	}

	void onPushButtonClicked()
	{
		switch (_mode)
		{
		case XPushButtonHelper::NoMode:
			break;
		case XPushButtonHelper::ColorMode:
			setColor();
			break;
		case XPushButtonHelper::FontMode:
			break;
		default:
			break;
		}
	}

private:
	QPushButton*	_obj;
	QString _srcText;
	QTimer	_flashTimer;
	int		_flashCount;
	Mode	_mode;
};

#define XPUSH_BUTTON_HELPER(obj)  XPushButtonHelper::getInstance(obj)

#endif // _XPUSH_BUTTON_HELPER

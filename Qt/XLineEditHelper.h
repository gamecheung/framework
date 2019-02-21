#ifndef _XLINE_EDIT_HELPER_H
#define _XLINE_EDIT_HELPER_H

#include <QLineEdit>
#include <QHash>
#include <QToolButton>
#include <QEvent>
#include <QApplication>
#include <QStyle>

class XLineEditHelper : QObject{
	Q_OBJECT
public:
	~XLineEditHelper(){instances()->remove(_obj);}
	static XLineEditHelper* getInstance(QLineEdit* obj){
		if (!instances()->contains(obj)){
			(*instances())[obj] = new XLineEditHelper(obj);
		}
		return (*instances())[obj];
	}
	void enableCleanButton(bool enable){
		_cleanButton.setEnabled(enable);
		updateCleanButtonVisible();
	}
protected:
	bool eventFilter(QObject *obj, QEvent *ev){
		if (obj == _obj){
			if (ev->type() == QEvent::Resize && _cleanButton.isEnabled()){
				_cleanButton.resize(_obj->height(), _obj->height());
				_cleanButton.move(_obj->width() - _obj->height(), 0);
				return true;
			}
		}
		return QObject::eventFilter(obj, ev);
	}
private:
    XLineEditHelper(QLineEdit* obj) :QObject(obj), _obj(obj){ 
        Q_ASSERT(_obj);
		_cleanButton.setParent(_obj);
		_cleanButton.setAutoRaise(true);
		_cleanButton.setCursor(Qt::ArrowCursor);
		_cleanButton.setIcon(qApp->style()->standardIcon(QStyle::SP_LineEditClearButton));
		_obj->installEventFilter(this);
		connect(&_cleanButton, SIGNAL(clicked()), _obj, SLOT(clear()));
		connect(_obj, SIGNAL(textChanged(const QString &)), this, SLOT(updateCleanButtonVisible()));
		enableCleanButton(false);
    }
	static QHash<QLineEdit*, XLineEditHelper*>* instances(){
		static QHash<QLineEdit*, XLineEditHelper*> s_instances;
		return &s_instances;
	}
private slots:
	void updateCleanButtonVisible(){
		if (_cleanButton.isEnabled() && !_obj->text().isEmpty()){
			_cleanButton.setVisible(true);
		}
		else{
			_cleanButton.setVisible(false);
		}
	}
private:
	QLineEdit*	_obj;
	QToolButton	_cleanButton;
};

#define XLINE_EDIT_HELPER(obj)  XLineEditHelper::getInstance(obj)

#endif // _XLINE_EDIT_HELPER

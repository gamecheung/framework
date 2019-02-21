#ifndef _X_SPLASH_SCREEN_HELPER_H
#define _X_SPLASH_SCREEN_HELPER_H

#include <QSplashScreen>
#include <QHash>
#include <QLabel>
#include <QApplication>

class XSplashScreenHelper : public QObject{
	Q_OBJECT
public:
	~XSplashScreenHelper(){instances()->remove(m_obj);}
	static XSplashScreenHelper* getInstance(QSplashScreen* obj){
		if (!instances()->contains(obj)){
			(*instances())[obj] = new XSplashScreenHelper(obj);
		}
		return (*instances())[obj];
	}
	void setAlignment(Qt::Alignment align){ m_align = align; }
	Qt::Alignment alignment()const{ return m_align; }
	void setColor(const QColor& color){ m_color = color; }
	const QColor& color()const{ return m_color; }


public slots:
	void showMessage(const QString& text)
	{ 
		m_obj->showMessage(text, m_align, m_color);
	}

private:
    XSplashScreenHelper(QSplashScreen* obj) :QObject(obj), m_obj(obj){ 
        Q_ASSERT(m_obj);
		m_align = Qt::AlignBottom | Qt::AlignLeft;
		m_color = Qt::black;
    }
	static QHash<QSplashScreen*, XSplashScreenHelper*>* instances(){
		static QHash<QSplashScreen*, XSplashScreenHelper*> s_instances;
		return &s_instances;
	}
private:
	QSplashScreen*	m_obj;
	Qt::Alignment	m_align;
	QColor			m_color;
	//QLabel*			m_label;
};

#define X_SPLASH_SCREEN_HELPER(obj)  XSplashScreenHelper::getInstance(obj)

#endif // _X_SPLASH_SCREEN_HELPER_H

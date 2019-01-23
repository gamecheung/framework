#ifndef _X_MENU_HELPER_H
#define _X_MENU_HELPER_H

#include <QMenu>
#include <QHash>
#include <QLabel>
#include <QWidgetAction>
#include <QAction>
#include <QHBoxLayout>
#include <QStyleOption>
#include <QPainter>

class XMenuItemWidget :public QWidget
{
	Q_OBJECT
public:
	XMenuItemWidget(QWidgetAction* action, QWidget* parent = nullptr)
		:QWidget(parent), m_action(action){
		Q_ASSERT(action);

		m_icon = new QLabel(this);
		m_text = new QLabel(this);
		m_shortcut = new QLabel(this);
		setProperty("menu_item", true);

		m_action->setDefaultWidget(this);
		setLayout(new QHBoxLayout());
		m_icon->setScaledContents(true);
		m_shortcut->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		layout()->addWidget(m_icon);
		layout()->addWidget(m_text);
		layout()->addWidget(m_shortcut);
		layout()->setContentsMargins(12, 3, 12, 3);
		layout()->setSpacing(12);

		connect(action, SIGNAL(changed()), this, SLOT(updateUi()));
		updateUi();
	}
	void setIconSize(const QSize& size){ m_icon->setFixedSize(size); }
	void setIconVisible(bool visible){ m_icon->setVisible(visible); }
	void setShortcutVisible(bool visible){ m_shortcut->setVisible(visible); }

private slots:
	void updateUi()
	{
		m_icon->setPixmap(m_action->icon().pixmap(m_icon->size()));
		m_text->setText(m_action->text());
		m_shortcut->setText(m_action->shortcut().toString());
	}

protected:
	void paintEvent(QPaintEvent* e){
		QStyleOption opt;
		opt.init(this);
		QPainter p(this);
		style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
	}
private:
	QWidgetAction*	m_action;
	QLabel*		m_icon;
	QLabel*		m_text;
	QLabel*		m_shortcut;
};


class XMenuHelper : public QObject{
	Q_OBJECT
public:
	~XMenuHelper(){instances()->remove(m_obj);}
	static XMenuHelper* getInstance(QMenu* obj){
		if (!instances()->contains(obj)){
			(*instances())[obj] = new XMenuHelper(obj);
		}
		return (*instances())[obj];
	}

	QAction* addAction(const QIcon& icon, const QString& text)
	{
		QWidgetAction* wAction = new QWidgetAction(m_obj);
		wAction->setIcon(icon);
		wAction->setText(text);
		createMenuWidget(wAction);
		m_obj->addAction(wAction);
		return wAction;
	}
	QAction* addAction(const QString& text)
	{ return addAction(QIcon(), text); }
	QAction* addAction(const QIcon& icon, const QString& text, const QKeySequence &shortcut)
	{
		auto act = addAction(icon, text);
		act->setShortcut(shortcut);
		return act;
	}
	QAction* addAction(const QString& text, const QKeySequence &shortcut)
	{ return addAction(QIcon(), text, shortcut); }
	QAction* addAction(const QIcon& icon, const QString& text, const QObject *receiver, const char *method)
	{
		auto act = addAction(icon, text);
		connect(act, SIGNAL(triggered()), receiver, method);
		return act;
	}
	QAction* addAction(const QString& text, const QObject *receiver, const char *method)
	{ return addAction(QIcon(),text,receiver,method); }
	QAction* addAction(const QIcon& icon, const QString& text, const QKeySequence &shortcut, const QObject *receiver, const char *method)
	{
		auto act = addAction(icon, text, receiver, method);
		act->setShortcut(shortcut);
		return act;
	}
	QAction* addAction(const QString& text, const QKeySequence &shortcut, const QObject *receiver, const char *method)
	{ return addAction(QIcon(), text, shortcut, receiver, method); }

	void addSeparator(){ m_obj->addSeparator(); }
	void setIconSize(const QSize& size)
	{
		if (size != m_iconSize)
		{
			for (auto& w : m_menuWidgets)
				w->setIconSize(size);

			m_iconSize = size;
		}
	}
	void setIconVisible(bool visible)
	{
		if (visible != m_iconVisible)
		{
			for (auto& w : m_menuWidgets)
				w->setIconVisible(visible);

			m_iconVisible = visible;
		}
	}
	void setShortcutVisible(bool visible)
	{
		if (visible != m_shortcutVisible)
		{
			for (auto& w : m_menuWidgets)
				w->setShortcutVisible(visible);

			m_shortcutVisible = visible;
		}
	}

	void setDefaultQss()
	{
		m_obj->setStyleSheet("XMenuItemWidget::hover{border: 1px solid #AECFF7;border-radius:2px;}");
	}

private:
    XMenuHelper(QMenu* obj) :QObject(obj), m_obj(obj)
		,m_iconSize(32,32),m_iconVisible(true),m_shortcutVisible(true){ 
        Q_ASSERT(m_obj);
        // code here
    }
	static QHash<QMenu*, XMenuHelper*>* instances(){
		static QHash<QMenu*, XMenuHelper*> s_instances;
		return &s_instances;
	}

	void createMenuWidget(QWidgetAction* action)
	{
		auto w = new XMenuItemWidget(action, m_obj);
		w->setIconSize(m_iconSize);
		w->setIconVisible(m_iconVisible);
		w->setShortcutVisible(m_shortcutVisible);
		m_menuWidgets.append(w);
	}

private:
	QMenu*	m_obj;
	QList<XMenuItemWidget*> m_menuWidgets;
	QSize	m_iconSize;
	bool	m_iconVisible;
	bool	m_shortcutVisible;
};

#define X_MENU_HELPER(obj)  XMenuHelper::getInstance(obj)

#endif // _X_MENU_HELPER_H

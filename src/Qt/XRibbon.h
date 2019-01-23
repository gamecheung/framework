#ifndef _X_RIBBON_H
#define _X_RIBBON_H

#include <QTabWidget>
#include <QPushButton>
#include <QToolButton>
#include <QToolBar>
#include <QLabel>
#include <QList>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenu>
#include <QWidgetAction>
#include <QStyleOption>
#include <QPainter>
#include <QMainWindow>
#include <QToolBar>
#include "XMenuHelper.h"


class XRibbonTabWidget :public QToolBar{
	Q_OBJECT
public:
	XRibbonTabWidget(QWidget *parent) :QToolBar(parent)
	{
		QWidget* w = new QWidget(this);
		w->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
		m_tail = QToolBar::addWidget(w);
	}
	void addWidget(const QString& title, QWidget* w){
		QWidget* item = new QWidget(this);
		QVBoxLayout* layoutItem = new QVBoxLayout();
		layoutItem->setMargin(3);
		layoutItem->addWidget(w);
		QLabel* label = new QLabel(title, this);
		label->setEnabled(false);
		label->setAlignment(Qt::AlignCenter);
		layoutItem->addWidget(label);
		auto s1 = w->sizePolicy();
		auto s2 = label->sizePolicy();
		w->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
		label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
		item->setLayout(layoutItem);

		if (m_items.count() > 0)
		{
			insertSeparator(m_tail);
		}
		QToolBar::insertWidget(m_tail,item);
		m_items.append(item);
	}
	void addWidget(QWidget* widget){ addWidget(widget->windowTitle(), widget); }
	void addLayout(const QString& title, QLayout* layout){
		QWidget* w = new QWidget(this);
		w->setLayout(layout);
		addWidget(title, w);
	}
private:
	QList<QWidget* >	 m_items;
	QAction*			m_tail;
};




class XRibbonWidget : public QTabWidget{
	Q_OBJECT
public:
	XRibbonWidget(QMainWindow *parent)
		:QTabWidget(parent)
		, _menu(this)
	{ 
		QToolBar* ribbonToolBar = parent->addToolBar(tr("ribbon"));
		ribbonToolBar->setMovable(false);
		ribbonToolBar->addWidget(this);
		parent->setContextMenuPolicy(Qt::NoContextMenu); // 隐藏工具栏上的右键菜单

		_buttonMenu.setObjectName("menuButton");
        _buttonMenu.setText(tr("File"));
        _buttonMenu.setParent(this);
        setTabSize(QSize(60, 22));

        _buttonMenu.setMenu(&_menu);
    }
    ~XRibbonWidget(){}

	XMenuHelper* menu(){ return X_MENU_HELPER(&_menu); }


	XRibbonTabWidget* tabWidget(const QString& text)
	{
		int count = this->count();
		for (int i = 0; i < count; ++count)
		{
			if (tabText(i) == text)
				return qobject_cast<XRibbonTabWidget*>(widget(i));
		}
		return nullptr;
	}

	XRibbonTabWidget* addTab(const QString& text)
	{
		auto w = new XRibbonTabWidget(this);
		QTabWidget::addTab(w, text);
		return w;
	}
    
private:
    void setTabSize(const QSize& size){
        QString qssTab =
            "QTabBar::tab{"
            "min-width:$(w)px;"
            "max-width:$(w)px;"
            "min-height:$(h)px;"
            "max-height:$(h)px;"
            "}"
            "QTabBar::tab:!selected:!hover{"
            "background:transparent;"
            "}"
            "::tab-bar{left:$(w)px;}"
            ;
      
        qssTab.replace("$(w)", QString::number(size.width()));
        qssTab.replace("$(h)", QString::number(size.height()));

        this->setStyleSheet(qssTab);
		_buttonMenu.setFixedSize(size.width() - 2, size.height() - 2);
    }

private:
    QPushButton _buttonMenu;
    QMenu       _menu;
};

#endif //_X_RIBBON_H

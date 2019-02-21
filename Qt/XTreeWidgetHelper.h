#ifndef _XTREE_WIDGET_HELPER_H
#define _XTREE_WIDGET_HELPER_H

#include <QTreeWidget>
#include <QHash>
#include <QMenu>
#include <QEvent>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QContextMenuEvent>
#include <QDebug>
#include "XItemEditor.h"

class XTreeWidgetHelper : QObject{
	Q_OBJECT
public:
	~XTreeWidgetHelper(){instances()->remove(_obj);}
	static XTreeWidgetHelper* getInstance(QTreeWidget* obj){
		if (!instances()->contains(obj)){
			(*instances())[obj] = new XTreeWidgetHelper(obj);
		}
		return (*instances())[obj];
	}

	void setMenu(QMenu* menu){ m_menu = menu; }

protected:
	bool eventFilter(QObject* obj, QEvent* event)
	{
		if (obj == _obj)
		{
			if (m_menu&&event->type() == QEvent::ContextMenu)
			{
				QContextMenuEvent* ev = static_cast<QContextMenuEvent*>(event);
				m_menu->popup(ev->globalPos());
			}
		}
		return QObject::eventFilter(obj, event);
	}

private:
    XTreeWidgetHelper(QTreeWidget* obj) :QObject(obj), _obj(obj),m_menu(nullptr){ 
        Q_ASSERT(_obj);
		_obj->installEventFilter(this);
    }
	static QHash<QTreeWidget*, XTreeWidgetHelper*>* instances(){
		static QHash<QTreeWidget*, XTreeWidgetHelper*> s_instances;
		return &s_instances;
	}
private:
	QTreeWidget*	_obj;
	QMenu*			m_menu;
};

#define XTREE_WIDGET_HELPER(obj)  XTreeWidgetHelper::getInstance(obj)


class XTreeEditorItem :public QObject, public QTreeWidgetItem
{
	Q_OBJECT
public:
	XTreeEditorItem(const QString& text, QTreeWidgetItem *parent)
		:QTreeWidgetItem(parent,QStringList()<<text)
	{
		Q_ASSERT(parent);

		auto treeWidget = parent->treeWidget();
		Q_ASSERT(treeWidget);
		Q_ASSERT(treeWidget->columnCount() == 2);
	}
	virtual ~XTreeEditorItem(){}


};

class XTreeLineEditItem :public XTreeEditorItem
{
	Q_OBJECT
public:
	XTreeLineEditItem(const QString& text, QTreeWidgetItem *parent = nullptr)
		:XTreeEditorItem(text,parent)
	{
		m_editor = new XTextItemEditor(QString(),parent->treeWidget());
		parent->treeWidget()->setItemWidget(this, 1, m_editor);
	}


private:
	XTextItemEditor* m_editor;
};

class XTreeSpinBoxItem :public XTreeEditorItem
{
	Q_OBJECT
signals:
	void valueChanged(int n);

public:
	XTreeSpinBoxItem(const QString& text, QTreeWidgetItem *parent = nullptr)
		:XTreeEditorItem(text, parent)
	{
		m_editor = new XSpinBoxItemEditor(parent->treeWidget());
		parent->treeWidget()->setItemWidget(this, 1, m_editor);
		connect(m_editor, SIGNAL(valueChanged(int)), this, SIGNAL(valueChanged(int)));
	}

	void setValue(int n){ m_editor->setValue(n); }
	int value()const{ return m_editor->value(); }

private:
	XSpinBoxItemEditor* m_editor;
};

class XTree2IntValueItem :public XTreeEditorItem
{
	Q_OBJECT
signals:
	void valueChanged(int n1, int n2);

public:
	XTree2IntValueItem(const QString& text,
		const QString& text1,const QString& text2,const QString& fmt,
		QTreeWidgetItem *parent = nullptr)
		:XTreeEditorItem(text, parent), m_fmt(fmt)
	{
		auto treeWidget = parent->treeWidget();
		m_item1 = new XTreeSpinBoxItem(text1, this);
		m_item2 = new XTreeSpinBoxItem(text2, this);
		connect(m_item1, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged()));
		connect(m_item2, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged()));
	}

private slots:
	void onValueChanged()
	{
		int n1 = m_item1->value();
		int n2 = m_item2->value();
		this->setText(1, m_fmt.arg(n1).arg(n2));
		emit valueChanged(n1,n2);
	}

	void setValue(int n1, int n2)
	{
		m_item1->setValue(n1);
		m_item2->setValue(n2);
	}

private:
	XTreeSpinBoxItem* m_item1;
	XTreeSpinBoxItem* m_item2;
	QString				m_fmt;
};

class XTreeSizeItem :public XTree2IntValueItem
{
	Q_OBJECT
public:
	XTreeSizeItem(QTreeWidgetItem *parent = nullptr)
		:XTree2IntValueItem(tr("Size"),tr("Width"),tr("Height"),"%1 x %2", parent)
	{
	}

private:

};


#endif // _XTREE_WIDGET_HELPER

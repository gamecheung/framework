#ifndef _X_DELEGATE_H
#define _X_DELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QLabel>
#include <QLineEdit>
#include <QDateEdit>
#include <QTimeEdit>
#include <QDateTimeEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include <QListView>
#include <QDoubleSpinBox>
#include <QColorDialog>
#include <QApplication>
#include <QMap>
#include <QDebug>

class XLabelDelegate :public QStyledItemDelegate
{
	Q_OBJECT
public:
	XLabelDelegate(QObject* parent = nullptr) 
		:QStyledItemDelegate(parent), m_alignment(Qt::AlignVCenter | Qt::AlignLeft){}
	XLabelDelegate(Qt::Alignment align, QObject* parent = nullptr) 
		:QStyledItemDelegate(parent), m_alignment(align){}

	void setAlignment(Qt::Alignment align){ m_alignment = align; }

	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
	{
		if (option.state & QStyle::State_Selected)
			painter->fillRect(option.rect, option.palette.highlight());

		painter->setPen(option.state & QStyle::State_Selected
			? option.palette.highlightedText().color() : option.palette.windowText().color());

		painter->drawText(option.rect, index.model()->data(index).toString(),
			QTextOption(m_alignment));
	}

	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
	{
		return nullptr;
	}
private:
	Qt::Alignment m_alignment;
};

class XMapDelegate :public QStyledItemDelegate
{
	Q_OBJECT
public:

	XMapDelegate(QObject* parent = nullptr)
		:QStyledItemDelegate(parent), m_alignment(Qt::AlignVCenter | Qt::AlignLeft),m_editable(false){}
	XMapDelegate(Qt::Alignment align, QObject* parent = nullptr)
		:QStyledItemDelegate(parent), m_alignment(align),m_editable(false){}

	void setAlignment(Qt::Alignment align){ m_alignment = align; }
	void setMap(const QVariant& key, const QString& value){ m_map[key] = value; }
	void setInvalidValue(const QString& value){ m_invalidValue = value; }
	void setEditable(bool yes){ m_editable = yes; }

	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
	{
		if (m_editable)
		{
			QComboBox* w = new QComboBox(parent);
			w->setView(new QListView(w));
			w->setEditable(false);
			QMapIterator<QVariant, QString> i(m_map);
			while (i.hasNext()){
				i.next();
				w->addItem(i.value(), i.key());
			}
			auto itr = m_map.find(index.model()->data(index));
			if (itr != m_map.end())
				w->setCurrentText(itr.value());
			return w;
		}
		else
			return nullptr;
	}
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
	{
		QComboBox* w = qobject_cast<QComboBox*>(editor);
		Q_ASSERT(w);
		model->setData(index, w->currentData());
	}

	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
	{
		if (option.state & QStyle::State_Selected)
			painter->fillRect(option.rect, option.palette.highlight());

		painter->setPen(option.state & QStyle::State_Selected
			? option.palette.highlightedText().color() : option.palette.windowText().color());

		auto itr = m_map.find(index.model()->data(index));
		painter->drawText(option.rect,(itr!=m_map.end())?itr.value():m_invalidValue, QTextOption(m_alignment));
	}

private:
	Qt::Alignment m_alignment;
	QMap<QVariant, QString>	m_map;
	QString	 m_invalidValue;
	bool	m_editable;
};

class XDelegate :public QStyledItemDelegate
{
	Q_OBJECT
public:
	XDelegate(QObject* parent = nullptr) :QStyledItemDelegate(parent),m_alwaysShow(true){}
	virtual ~XDelegate(){}

	void paint(QPainter *painter, const QStyleOptionViewItem &option,const QModelIndex &index) const
	{
		if (m_alwaysShow)
		{
			if (option.state & QStyle::State_Selected)
				painter->fillRect(option.rect, option.palette.highlight());
			paint(painter, option, index.model()->data(index));
		}
		else
		{
			QStyledItemDelegate::paint(painter, option, index);
		}
	}


	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
	{
		return createEditor(parent, index.model()->data(index));
	}

	void setModelData(QWidget *editor, QAbstractItemModel *model,const QModelIndex &index) const
	{
		auto value = editorData(editor);
		if (value.isValid())
			model->setData(index, value);
		else
			QStyledItemDelegate::setModelData(editor, model, index);
	}

	void setAlwaysShow(bool yes){ m_alwaysShow = yes; }

protected:
	virtual QWidget* createEditor(QWidget* parent, const QVariant& value)const { return nullptr; }
	virtual QVariant editorData(QWidget* editor)const { return QVariant(); }
	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QVariant &value)const = 0;

private:
	bool m_alwaysShow;
};






class XLineEditDelegate :public XDelegate
{
	Q_OBJECT
public:
	XLineEditDelegate(QObject* parent = nullptr) :XDelegate(parent), m_alignment(Qt::AlignVCenter | Qt::AlignLeft){}
	void setAlignment(Qt::Alignment alignment){ m_alignment = alignment; }

protected:
	virtual QWidget* createEditor(QWidget* parent, const QVariant& value)const
	{
		QLineEdit* lineEdit = new QLineEdit(parent);
		lineEdit->setAlignment(m_alignment);
		lineEdit->setText(value.toString());
		return lineEdit;
	}
	virtual QVariant editorData(QWidget* w)const
	{
		return static_cast<QLineEdit*>(w)->text();
	}
	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QVariant &value)const
	{
		QApplication::style()->drawItemText(painter, option.rect, m_alignment, QApplication::palette(), true, value.toString());
	}

private:
	Qt::Alignment m_alignment;
};

class XCheckBoxDelegate :public XDelegate
{
	Q_OBJECT
public:
	XCheckBoxDelegate(QObject* parent = nullptr) :m_editable(true), XDelegate(parent){ setAlwaysShow(true); }
	XCheckBoxDelegate(bool editable, QObject* parent = nullptr) :m_editable(editable), XDelegate(parent){ setAlwaysShow(true); }

	void setEditable(bool editable){ m_editable = editable; }
	bool isEditable()const{ return m_editable; }

protected:
	bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
	{
		if (m_editable && event->type() == QEvent::MouseButtonPress)
		{
			model->setData(index, !index.model()->data(index).toBool());
		}
		return true;
	}
	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QVariant &value)const
	{
		QStyleOptionButton buttonOption;
		buttonOption.rect = option.rect;
		buttonOption.state |= (QStyle::State_Enabled | (value.toBool() ? QStyle::State_On : QStyle::State_Off));

		QCheckBox checkBox;
		buttonOption.rect.setSize(QApplication::style()->subElementRect(QStyle::SE_CheckBoxIndicator, &buttonOption, &checkBox).size());
		buttonOption.rect.moveCenter(option.rect.center());
		QApplication::style()->drawControl(QStyle::CE_CheckBox, &buttonOption, painter, &checkBox);
	}

private:
	bool m_editable;
};

class XSpinBoxDelegate :public XDelegate
{
	Q_OBJECT
public:
	XSpinBoxDelegate(QObject* parent = nullptr) :XDelegate(parent){ setAlwaysShow(false); setRange(INT_MIN, INT_MAX); }
	XSpinBoxDelegate(int mininum, int maxinum, QObject* parent = nullptr)
		:m_mininum(mininum), m_maxinum(maxinum), XDelegate(parent)
	{
		setAlwaysShow(false);
	}
	void setRange(int mininum, int maxinum){ m_mininum = mininum; m_maxinum = maxinum; }

protected:
	virtual QWidget* createEditor(QWidget* parent, const QVariant& value)const
	{
		QSpinBox* spinBox = new QSpinBox(parent);
		spinBox->setRange(m_mininum, m_maxinum);
		spinBox->setValue(value.toInt());
		return spinBox;
	}
	virtual QVariant editorData(QWidget* w)const
	{
		return static_cast<QSpinBox*>(w)->value();
	}
	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QVariant &value)const
	{
		Q_ASSERT(false);
	}
private:
	int m_mininum;
	int m_maxinum;
};

class XDoubleSpinBoxDelegate :public XDelegate
{
	Q_OBJECT
public:
	XDoubleSpinBoxDelegate(QObject* parent = nullptr) :XDelegate(parent){ setAlwaysShow(false); }

protected:
	virtual QWidget* createEditor(QWidget* parent, const QVariant& value)const
	{
		QDoubleSpinBox* spinBox = new QDoubleSpinBox(parent);
		spinBox->setValue(value.toDouble());
		return spinBox;
	}
	virtual QVariant editorData(QWidget* w)const
	{
		return static_cast<QDoubleSpinBox*>(w)->value();
	}
	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QVariant &value)const
	{
		Q_ASSERT(false);
	}
private:
};

class XComboBoxDelegate :public XDelegate
{
	Q_OBJECT
public:
	XComboBoxDelegate(QObject* parent = nullptr) :XDelegate(parent), m_editable(true){ setAlwaysShow(false); }
	void addItems(const QStringList& items){ m_items.append(items); }
	void setItems(const QStringList& items){ m_items = items; }

protected:
	virtual QWidget* createEditor(QWidget* parent, const QVariant& value)const
	{
		QComboBox* comboBox = new QComboBox(parent);
		comboBox->setView(new QListView(comboBox));
		comboBox->setEditable(m_editable);
		comboBox->addItems(m_items);
		comboBox->setCurrentText(value.toString());
		return comboBox;
	}
	virtual QVariant editorData(QWidget* w)const
	{
		return static_cast<QComboBox*>(w)->currentText();
	}
	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QVariant &value)const
	{
			QStyleOptionComboBox comboBoxOption;
			comboBoxOption.rect = option.rect;
			comboBoxOption.state = option.state;
			comboBoxOption.state |= QStyle::State_Enabled;
			comboBoxOption.editable = false;
			comboBoxOption.currentText = value.toString();

			QComboBox comboBox;
			QApplication::style()->drawComplexControl(QStyle::CC_ComboBox, &comboBoxOption, painter, &comboBox);
			QApplication::style()->drawControl(QStyle::CE_ComboBoxLabel, &comboBoxOption, painter, &comboBox);
		
	}

private:
	QStringList	m_items;
	bool		m_editable;
};


class XDateTimeDelegate :public XDelegate
{
	Q_OBJECT
public:
	XDateTimeDelegate(QObject* parent = nullptr) :XDelegate(parent)
	{
		m_canlendarPopup = true;
		m_displayFormat = "yyyy/MM/dd HH:mm:ss";
	}

	void setCanlendarPopup(bool enable){ m_canlendarPopup = enable; }
	void setDisplayFormat(const QString& format){ m_displayFormat = format; }

protected:
	virtual QWidget* createEditor(QWidget* parent, const QVariant& value)const
	{
		QDateTimeEdit* editor = new QDateTimeEdit(parent);
		editor->setCalendarPopup(m_canlendarPopup);
		editor->setDisplayFormat(m_displayFormat);
		editor->setDateTime(value.toDateTime());
		return editor;
	}
	virtual QVariant editorData(QWidget* w)const
	{
		return static_cast<QDateTimeEdit*>(w)->dateTime();
	}
	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QVariant &value)const
	{
		QApplication::style()->drawItemText(painter, option.rect, Qt::AlignVCenter | Qt::AlignLeft,
			QApplication::palette(), true, value.toDateTime().toString(m_displayFormat));
	}

private:
	bool	m_canlendarPopup;
	QString m_displayFormat;
};



class XDateDelegate :public XDelegate
{
	Q_OBJECT
public:
	XDateDelegate(QObject* parent = nullptr) :XDelegate(parent)
	{
		m_canlendarPopup = true;
		m_displayFormat = "yyyy/MM/dd";
	}

	void setCanlendarPopup(bool enable){ m_canlendarPopup = enable; }
	void setDisplayFormat(const QString& format){ m_displayFormat = format; }

protected:
	virtual QWidget* createEditor(QWidget* parent, const QVariant& value)const
	{
		QDateEdit* editor = new QDateEdit(parent);
		editor->setCalendarPopup(m_canlendarPopup);
		editor->setDisplayFormat(m_displayFormat);
		editor->setDate(value.toDate());
		return editor;
	}
	virtual QVariant editorData(QWidget* w)const
	{
		return static_cast<QDateEdit*>(w)->date();
	}
	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QVariant &value)const
	{
		QApplication::style()->drawItemText(painter, option.rect, Qt::AlignVCenter | Qt::AlignLeft,
			QApplication::palette(), true, value.toDate().toString(m_displayFormat));
	}

private:
	bool	m_canlendarPopup;
	QString m_displayFormat;
};


class XTimeDelegate :public XDelegate
{
	Q_OBJECT
public:
	XTimeDelegate(QObject* parent = nullptr) :XDelegate(parent)
	{
		m_displayFormat = "HH:mm:ss";
	}

	void setDisplayFormat(const QString& format){ m_displayFormat = format; }

protected:
	virtual QWidget* createEditor(QWidget* parent, const QVariant& value)const
	{
		QTimeEdit* editor = new QTimeEdit(parent);
		editor->setDisplayFormat(m_displayFormat);
		editor->setTime(value.toTime());
		return editor;
	}
	virtual QVariant editorData(QWidget* w)const
	{
		return static_cast<QTimeEdit*>(w)->time();
	}
	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QVariant &value)const
	{
		QApplication::style()->drawItemText(painter, option.rect, Qt::AlignVCenter | Qt::AlignLeft,
			QApplication::palette(), true, value.toTime().toString(m_displayFormat));
	}

private:
	QString m_displayFormat;
};



class XColorDelegate :public XDelegate
{
	Q_OBJECT
public:
	XColorDelegate(QObject* parent = nullptr) :XDelegate(parent){}

protected:
	bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
	{
		if (event->type() == QEvent::MouseButtonPress)
		{
			QColor color = QColorDialog::getColor(index.data().toString());
			if (color.isValid())
			{
				model->setData(index, color.name().toUpper());
			}
			return true;
		}
		return false;
	}
	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QVariant &value)const
	{
		QColor color(value.toString());
		if (option.state&QStyle::State_Selected)
			color = color.lighter();
		painter->fillRect(option.rect, color);
	}

};

#endif //_X_DELEGATE_H

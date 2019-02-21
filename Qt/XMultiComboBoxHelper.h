#ifndef _XMULTI_COMBO_BOX_HELPER_H
#define _XMULTI_COMBO_BOX_HELPER_H

#include <QComboBox>
#include <QHash>
#include <QListWidget>
#include <QCheckBox>
#include <QLineEdit>
#include <QDebug>
#include <QEvent>
#include <QButtonGroup>

class XMultiComboBoxHelper : public QObject{
	Q_OBJECT

signals:
	void selectedItemsChanged(const QStringList& items);

public:
	~XMultiComboBoxHelper() { instances()->remove(m_obj); }
	static XMultiComboBoxHelper* getInstance(QComboBox* obj){
		if (!instances()->contains(obj)){
			(*instances())[obj] = new XMultiComboBoxHelper(obj);
		}
		return (*instances())[obj];
	}


	const QStringList& items()const{ return m_items; }
	QStringList selectedItems()const
	{
		QStringList items;
		int count = m_listWidget->count();
		for (int i = 1; i < count; i++)
		{
			QCheckBox* cbx = static_cast<QCheckBox*>(m_listWidget->itemWidget(m_listWidget->item(i)));
			if (cbx->isChecked())
				items.append(cbx->text());
		}
		return items;
	}
	bool isSelectedAll()const
	{
		auto cbx = checkBoxOfSelectAll();
		if (cbx)
			return cbx->checkState() == Qt::Checked;
		return false;
	}
	bool isSelectedNone()const
	{
		auto cbx = checkBoxOfSelectAll();
		if (cbx)
			return cbx->checkState() == Qt::Unchecked;
		return false;
	}
	bool isSelectedPartially()const
	{
		auto cbx = checkBoxOfSelectAll();
		if (cbx)
			return cbx->checkState() == Qt::PartiallyChecked;
		return false;
	}

public slots:
	void setSelectedItems(const QStringList& items)
	{
		QStringList sitems = items;
		{
			QSignalBlocker blocker(m_obj);
			int count = m_listWidget->count();
			for (int i = 1; i < count; i++)
			{
				QCheckBox* cbx = static_cast<QCheckBox*>(m_listWidget->itemWidget(m_listWidget->item(i)));
				QSignalBlocker blocker(cbx);
				cbx->setChecked(sitems.contains(cbx->text()));
				sitems.removeOne(cbx->text());
			}
		}
		onSelectedItemsChanged();
	}
	void setSelectAll(bool yes = true)
	{
		{
			QSignalBlocker blocker(m_obj);
			int count = m_listWidget->count();
			for (int i = 1; i < count; i++)
			{
				QCheckBox* cbx = static_cast<QCheckBox*>(m_listWidget->itemWidget(m_listWidget->item(i)));
				QSignalBlocker blocker(cbx);
				cbx->setChecked(yes);
			}
		}
		onSelectedItemsChanged();
	}
	void setItems(const QStringList& items)
	{
		m_items = items;
		updateList();
	}
	void addItems(const QStringList& items)
	{
		m_items.append(items);
		updateList();
	}
	void addItem(const QString& item)
	{
		m_items.append(item);
		updateList();
	}
	void clearItems()
	{
		m_items.clear();
		updateList();
	}

	void setExclusive(bool exclusive)
	{
		m_buttonGroup->setExclusive(exclusive);
		m_listWidget->item(0)->setHidden(exclusive);
	}
	bool exclusive()const{ return m_buttonGroup->exclusive(); }

private slots:
	void onSelectAllChanged()
	{
		setSelectAll(checkBoxOfSelectAll()->isChecked());
	}
	void onSelectedItemsChanged()
	{
		QStringList items = selectedItems();
		{
			QSignalBlocker blocker(m_obj);
			auto cbx = checkBoxOfSelectAll();
			Q_ASSERT(cbx);

			if (items.isEmpty())
				cbx->setCheckState(Qt::Unchecked);
			else if (items.count() == m_items.count())
				cbx->setCheckState(Qt::Checked);
			else
				cbx->setCheckState(Qt::PartiallyChecked);
		}
		m_obj->setCurrentText(items.join(";"));
		emit selectedItemsChanged(items);
	}
	void onLineEditTextChanged(const QString& text)
	{
		// 修复点击空白处文字被清空
		if (text.isEmpty())
			onSelectedItemsChanged();
	}

protected:
	bool eventFilter(QObject* obj, QEvent* event)
	{
		if (obj == m_obj->lineEdit() && event->type() == QEvent::MouseButtonPress)
		{
			m_obj->showPopup();
		}
		return QObject::eventFilter(obj, event);
	}

private:
    XMultiComboBoxHelper(QComboBox* obj) :QObject(obj), m_obj(obj){ 
        Q_ASSERT(m_obj);
		m_listWidget = new QListWidget(m_obj);
		m_obj->setModel(m_listWidget->model());
		m_obj->setView(m_listWidget);
		m_obj->setEditable(true);
		m_obj->lineEdit()->setReadOnly(true);
		m_obj->lineEdit()->installEventFilter(this);
		connect(m_obj, SIGNAL(currentTextChanged(const QString&)), this, SLOT(onLineEditTextChanged(const QString&)));
		
		m_buttonGroup = new QButtonGroup(this);
		m_buttonGroup->setExclusive(false);
	}
	void updateList()
	{
		m_listWidget->clear();
		if (m_items.isEmpty())
			return;

		m_checkBoxAll = appendCheckBox(tr("<All>"));
		connect(m_checkBoxAll, SIGNAL(clicked()), this, SLOT(onSelectAllChanged()));

		for (const auto& item : m_items)
		{
			auto cbx = appendCheckBox(item);
			m_buttonGroup->addButton(cbx);
			connect(cbx, SIGNAL(toggled(bool)), this, SLOT(onSelectedItemsChanged()));
		}
	}

	QCheckBox* appendCheckBox(const QString& text)
	{
		QListWidgetItem* item = new QListWidgetItem(m_listWidget);
		QCheckBox* cbx = new QCheckBox(text, m_listWidget);
		m_listWidget->addItem(item);
		m_listWidget->setItemWidget(item, cbx);
		return cbx;
	}

	QCheckBox* checkBoxOfSelectAll()const
	{
		if (m_items.isEmpty())
			return nullptr;
		return static_cast<QCheckBox*>(m_listWidget->itemWidget(m_listWidget->item(0)));
	}
	static QHash<QComboBox*, XMultiComboBoxHelper*>* instances(){
		static QHash<QComboBox*, XMultiComboBoxHelper*> s_instances;
		return &s_instances;
	}
private:
	QComboBox*	m_obj;
	QStringList	m_items;
	QListWidget* m_listWidget;

	QButtonGroup*	m_buttonGroup;
	QCheckBox*		m_checkBoxAll;
};

#define XMULTI_COMBO_BOX_HELPER(obj)  XMultiComboBoxHelper::getInstance(obj)

#endif // _XMULTI_COMBO_BOX_HELPER

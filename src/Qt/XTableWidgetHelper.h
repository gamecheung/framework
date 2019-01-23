#ifndef _XTABLE_WIDGET_HELPER_H
#define _XTABLE_WIDGET_HELPER_H

#include <QTableWidget>
#include <QHash>
#include <QContextMenuEvent>
#include <QMenu>

class XTableWidgetHelper :public QObject{
    Q_OBJECT
signals:
    void rowInserted(int row);
    void rowRemoved(int row);
	void rowSwaped(int row1, int row2);
	void tableChanged();
	void rowCountChanged();

public:
    ~XTableWidgetHelper(){instances()->remove(_obj);}
    static XTableWidgetHelper* getInstance(QTableWidget* obj){
        if (!instances()->contains(obj)){
            (*instances())[obj] = new XTableWidgetHelper(obj);
        }
        return (*instances())[obj];
    }
	QTableWidgetItem *item(int row, int col){
		Q_ASSERT(0 <= row && row < _obj->rowCount());
		Q_ASSERT(0 <= col && col < _obj->columnCount());
		if (nullptr == _obj->item(row, col)){
			_obj->setItem(row, col, new QTableWidgetItem());
		}
		return _obj->item(row, col);
	}
	QString text(int row, int col){ return item(row, col)->text(); }
	void setText(int row, int col, const QString& text){ item(row, col)->setText(text); }
	void setText(int row, int col, int num){ item(row, col)->setText(QString::number(num)); }
	void setText(int row, int col, double n){ item(row, col)->setText(QString::number(n)); }
	void setText(int row, int col, bool b){ item(row, col)->setData(Qt::DisplayRole, b); }
	QTableWidgetItem* currentItem(){
		if (_obj->currentItem() == nullptr){
			int row = _obj->currentRow();
			int col = _obj->currentColumn();
			_obj->setItem(row, col, new QTableWidgetItem());
		}
		return _obj->currentItem();
	}
	QList<QTableWidgetItem*> selectedItems(){
		QList<QTableWidgetItem*> items;
		auto ranges = _obj->selectedRanges();
		for (auto& range : ranges){
			for (int row = range.topRow(); row <= range.bottomRow(); ++row){
				for (int col = range.leftColumn(); col <= range.rightColumn(); ++col){
					items.append(item(row, col));
				}
			}
		}
		return items;
	}

	void setMenu(QMenu* menu){ m_menu = menu; }

public slots:
    void insertRow(){
        auto items = _obj->selectedItems();
        int row = 0;
        if(items.count()>0)
            row = items.at(0)->row();
        else
            row = _obj->rowCount();
        _obj->insertRow(row);
        emit rowInserted(row);
		emit rowCountChanged();
    }
    void removeRow(){
        int row = _obj->currentRow();
        _obj->removeRow(row);
		emit rowRemoved(row);
		emit rowCountChanged();
    }
    void upRow(){
        int row = _obj->currentRow();
        if(row==0)
            return;
        swapRow(row,row-1);
        _obj->selectRow(row-1);
    }
    void downRow(){
        int row = _obj->currentRow();
        if(row==_obj->rowCount()-1)
            return;
        swapRow(row,row+1);
        _obj->selectRow(row+1);
    }
	void appendRow()
	{
		_obj->setRowCount(_obj->rowCount() + 1);
		emit rowCountChanged();
	}
	void removeLastRow()
	{
		_obj->setRowCount(_obj->rowCount() - 1);
		emit rowCountChanged();
	}
	void setColumnEditable(int column, bool editable)
	{
		Q_ASSERT(0 <= column && column < _obj->columnCount());
		int rowCount = _obj->rowCount();

		if (editable)
		{
			for (int row = 0; row < rowCount; ++row)
			{
				auto item = this->item(row, column);
				item->setFlags(item->flags() | Qt::ItemIsEditable);
			}
		}
		else
		{
			for (int row = 0; row < rowCount; ++row)
			{
				auto item = this->item(row, column);
				item->setFlags(item->flags() & ~Qt::ItemIsEditable);
			}
		}
	}
	void setEditable(bool editable)
	{
		int columnCount = _obj->columnCount();
		for (int i = 0; i < columnCount; ++i)
		{
			setColumnEditable(i, editable);
		}
	}

	void removeSelectedRows()
	{
		auto selection = _obj->selectionModel()->selectedRows();
		QList<int> selectedRows;
		for (auto& sel : selection)
		{
			selectedRows.append(sel.row());
		}
		int rowCount = _obj->rowCount();
		for (int row = rowCount - 1; row >= 0; --row)
		{
			if (selectedRows.contains(row))
			{
				_obj->removeRow(row);
				selectedRows.removeOne(row);
			}
		}
	}

	void setRowBackground(int row,const QBrush& brush)
	{
		int colCount = _obj->columnCount();
		for (int col = 0; col < colCount; ++col)
		{
			item(row, col)->setBackground(brush);
		}
	}

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
    XTableWidgetHelper(QTableWidget* obj) :QObject(obj), _obj(obj),m_menu(nullptr){
        Q_ASSERT(_obj);
		connect(this, SIGNAL(rowInserted(int)), this, SIGNAL(tableChanged()));
		connect(this, SIGNAL(rowRemoved(int)), this, SIGNAL(tableChanged()));
		connect(this, SIGNAL(rowSwaped(int,int)), this, SIGNAL(tableChanged()));

		_obj->installEventFilter(this);
    }
    static QHash<QTableWidget*, XTableWidgetHelper*>* instances(){
        static QHash<QTableWidget*, XTableWidgetHelper*> s_instances;
        return &s_instances;
    }
    void swapRow(int row1,int row2){
        int colCount = _obj->columnCount();
        for(int col=0;col<colCount;++col){
            auto item1 = _obj->item(row1,col);
            auto item2 = _obj->item(row2,col);

			QTableWidgetItem* newItem1 = nullptr;
			QTableWidgetItem* newItem2 = nullptr;

			if (item1)
				newItem2 = new QTableWidgetItem(*item1);
			else
				newItem2 = new QTableWidgetItem();

			if (item2)
				newItem1 = new QTableWidgetItem(*item2);
			else
				newItem1 = new QTableWidgetItem();

			_obj->setItem(row1, col, newItem1);
			_obj->setItem(row2, col, newItem2);
        }
		emit rowSwaped(row1, row2);
    }

private:
    QTableWidget*	_obj;
	QMenu*			m_menu;
};

#define XTABLE_WIDGET_HELPER(obj)  XTableWidgetHelper::getInstance(obj)

#endif // _XTABLE_WIDGET_HELPER

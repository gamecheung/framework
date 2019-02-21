#ifndef _XLIST_WIDGET_HELPER_H
#define _XLIST_WIDGET_HELPER_H

#include <QListWidget>
#include <QHash>


class XListWidgetHelper :public QObject{
	Q_OBJECT
public:
	~XListWidgetHelper(){instances()->remove(_obj);}
	static XListWidgetHelper* getInstance(QListWidget* obj){
		if (!instances()->contains(obj)){
			(*instances())[obj] = new XListWidgetHelper(obj);
		}
		return (*instances())[obj];
	}

	void swapRow(int row1, int row2)
	{
		auto item1 =_obj->item(row1);
		auto w1 = _obj->itemWidget(item1);
		auto item2 = _obj->item(row2);
		auto w2 = _obj->itemWidget(item2);
		if (row1 < row2)
		{
			_obj->takeItem(row2);
			_obj->takeItem(row1);
			_obj->insertItem(row1, item2);
			_obj->insertItem(row2, item1);
		}
		else
		{
			_obj->takeItem(row1);
			_obj->takeItem(row2);
			_obj->insertItem(row2, item1);
			_obj->insertItem(row1, item2);
		}
		_obj->setItemWidget(item1, w1);
		_obj->setItemWidget(item2, w2);
	}

public slots:
	//void insertItem()
	//{
	//	_obj->insertItem(_obj->currentRow(), new QListWidgetItem(_obj));
	//}
	void removeItem()
	{
		auto item = _obj->currentItem();
		if (item)
		{
			delete item;
		}
	}
	void upItem()
	{
		int row = _obj->currentRow();
		if (row == 0)
			return;
		swapRow(row, row - 1);
		_obj->setCurrentRow(row - 1);
	}
	void downItem()
	{
		int row = _obj->currentRow();
		if (row == _obj->count())
			return;
		swapRow(row, row + 1);
		_obj->setCurrentRow(row + 1);
	}

private:
    XListWidgetHelper(QListWidget* obj) :QObject(obj), _obj(obj){ 
        Q_ASSERT(_obj);
        // code here
    }
	static QHash<QListWidget*, XListWidgetHelper*>* instances(){
		static QHash<QListWidget*, XListWidgetHelper*> s_instances;
		return &s_instances;
	}
private:
	QListWidget*	_obj;
};

#define XLIST_WIDGET_HELPER(obj)  XListWidgetHelper::getInstance(obj)

#endif // _XLIST_WIDGET_HELPER

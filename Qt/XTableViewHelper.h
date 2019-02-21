#ifndef _X_TABLE_VIEW_HELPER_H
#define _X_TABLE_VIEW_HELPER_H

#include <QTableView>
#include <QHeaderView>
#include <QHash>
#include <QEvent>
#include <QMenu>
#include <QContextMenuEvent>
#include <QDebug>
#include <QApplication>
#include <QClipboard>

class XTableViewHelper : public QObject{
	Q_OBJECT
public:
	~XTableViewHelper(){instances()->remove(_w);}
    QString saveState()const{
        QStringList states;
        QStringList indexs;
        QStringList visibles;
        QStringList widths;
        QHeaderView* headerView = _w->horizontalHeader();
        int count = headerView->count();
        for (int i = 0; i < count; ++i){
            indexs.append(QString::number(headerView->logicalIndex(i)));
            visibles.append(_w->isColumnHidden(i) ? "false" : "true");
            widths.append(QString::number(_w->columnWidth(i)));
        }
        //qDebug() << logicalIndex;
        states.append(indexs.join(","));
        states.append(visibles.join(","));
        states.append(widths.join(","));
        return states.join("|");
    }
    void restoreState(const QString& state){
        QStringList states = state.split("|");
        if (states.count() != 3)
            return;
        QStringList indexs = states.at(0).split(",");
        QStringList visibles = states.at(1).split(",");
        QStringList widths = states.at(2).split(",");

        QHeaderView* headerView = _w->horizontalHeader();
        int count = headerView->count();
        if (indexs.count() != count)  return;
        if (visibles.count() != count)return;
        if (widths.count() != count)return;

        for (int i = 0; i < count; ++i){
            headerView->swapSections(i, headerView->visualIndex(indexs.at(i).toInt()));
			bool hidden = visibles.at(i) == "false";
			_w->setColumnWidth(i, widths.at(i).toInt());
			_w->setColumnHidden(i, hidden);
        }

        // in case of all column hidden
        if (!visibles.contains("true")&&count>0){
            _w->setColumnHidden(0, false);
        }

        auto actions = _menu.actions();
        Q_ASSERT(actions.count() == count);
        for (auto& action : actions){
			action->blockSignals(true);
            action->setChecked(!_w->isColumnHidden(action->objectName().toInt()));
			action->blockSignals(false);
        }
    }
    void setColumnAlwaysShow(int column){
        Q_ASSERT(0 <= column && column < _menu.actions().count());
        _menu.actions()[column]->setChecked(false);
        _menu.actions()[column]->setChecked(true);
        //_menu.actions()[column]->setEnabled(false);
        _menu.actions()[column]->setVisible(false);
    }
    void setColumnAlwaysHidden(int column){
        Q_ASSERT(0 <= column && column < _menu.actions().count());
        _menu.actions()[column]->setChecked(true);
        _menu.actions()[column]->setChecked(false);
        //_menu.actions()[column]->setEnabled(false);
        _menu.actions()[column]->setVisible(false);
    }

	QList<int> selectedRows()const
	{
		QList<int> ret;
		QMap<int, int> rowMap;
		auto selected = _w->selectionModel()->selectedIndexes();
		for (auto& index : selected)
		{
			if (!ret.contains(index.row()))
			{
				ret.append(index.row());
			}
		}
		return ret;
	}

	static bool exists(QTableView* w){ return instances()->contains(w); }
	static XTableViewHelper* getInstance(QTableView* w){
		if (!instances()->contains(w)){
			(*instances())[w] = new XTableViewHelper(w);
		}
		return (*instances())[w];
	}

public slots:
	void setMenuVisible(bool visible){ _isMenuVisible = visible; }
	void copy()
	{
		auto selection =  _w->selectionModel()->selection();
		if (selection.isEmpty())
			return;

		QStringList strs;
		auto range = selection.at(0);
		for (int row = range.top(); row <= range.bottom(); ++row)
		{
			QStringList rowdata;
			for (int col = range.left(); col <= range.right(); ++col)
			{
				rowdata.append(_w->model()->index(row, col).data().toString());
			}
			strs.append(rowdata.join("\t"));
		}
		QApplication::clipboard()->setText(strs.join("\n"));
	}
protected:
    bool eventFilter(QObject *obj, QEvent *ev)
    {
        if (obj == _w->horizontalHeader()) {
            if (_isMenuVisible && ev->type() == QEvent::ContextMenu) {
                _menu.popup(static_cast<QContextMenuEvent*>(ev)->globalPos());
                return true;
            }
        }
		if (ev->type() == QEvent::ContextMenu)
		{
			m_menu.popup(static_cast<QContextMenuEvent*>(ev)->globalPos());
			return true;
		}
		//if (ev->type() == QEvent::KeyPress)
		//{
		//	for (auto& action : m_actions)
		//	{
		//		if (static_cast<QKeyEvent*>(ev)->matches(action->shortcut()))
		//		{
		//			action->trigger();
		//			return true;
		//		}
		//	}
		//}
        return QObject::eventFilter(obj, ev);
    }
private:
    XTableViewHelper(QTableView* w) :QObject(w), _w(w),_isMenuVisible(true){ 
        Q_ASSERT(_w); 
        //_w->horizontalHeader()->setSectionsMovable(true);
        auto model = _w->model();
        for (int i = 0; i < model->columnCount(); ++i){
            QAction* action = new QAction(model->headerData(i, Qt::Horizontal).toString(), this);
            action->setObjectName(QString::number(i));
            action->setCheckable(true);
            action->setChecked(true);
            _menu.addAction(action);
            connect(action, SIGNAL(toggled(bool)), this, SLOT(setColumnVisible(bool)));
        }
        _w->horizontalHeader()->installEventFilter(this);
		_w->installEventFilter(this);

		appendAction(tr("Copy"), this, SLOT(copy()), QKeySequence::Copy);
    }

	QAction* appendAction(const QString& text, const QObject *receiver, const char *method, const QKeySequence& shortcut = QKeySequence())
	{
		QAction* action = new QAction(text, this);
		connect(action, SIGNAL(triggered()), receiver, method);
		if (!shortcut.isEmpty())
			action->setShortcut(shortcut);
		m_menu.addAction(action);
		m_actions.append(action);
		return action;
	}

	static QHash<QTableView*, XTableViewHelper*>* instances(){
		static QHash<QTableView*, XTableViewHelper*> s_instances;
		return &s_instances;
	}
private slots:
    void setColumnVisible(bool visible){
        int n = sender()->objectName().toInt();
        _w->setColumnHidden(n, !visible);
        if (visible && _w->columnWidth(n) == 0)
            _w->setColumnWidth(n,50);
    }
private:
	QTableView*	_w;
    QMenu           _menu;
    QList<QAction*> _actions;
	bool			_isMenuVisible;

	QMenu			m_menu;
	QList<QAction*>	m_actions;
};

#define XTABLE_VIEW_HELPER(w)  XTableViewHelper::getInstance(w)

class XTableViewStateRestorer
{
public:
	XTableViewStateRestorer(QTableView* table) :_table(table)
	{
		Q_ASSERT(table);
		if (table->model())
			_state = XTABLE_VIEW_HELPER(table)->saveState();
	}
	~XTableViewStateRestorer()
	{
		if (!_state.isEmpty())
		{
			//// table->model()->insertColumn()之后isColumnHidden总是返回false，
			//// 手动设置所有列显示以修复此bug
			//for (int i = 0; i < _table->model()->columnCount(); ++i)
			//	_table->setColumnHidden(i, false);

			XTABLE_VIEW_HELPER(_table)->restoreState(_state);
		}
	}

private:
	QTableView* _table;
	QString		_state;
};

#endif //_X_TABLE_VIEW_HELPER_H

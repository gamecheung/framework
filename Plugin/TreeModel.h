#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include "ExtensionSystem.h"

class TreeItem
{
public:
	TreeItem(const QString& data = QString(), TreeItem *parentItem = 0) :m_spec(nullptr)
	{
		m_parentItem = parentItem;
		m_itemData.append(data);
	}
	TreeItem(const QList<QVariant> &data, TreeItem *parentItem = 0) :m_spec(nullptr)
	{
		m_parentItem = parentItem;
		m_itemData = data;
	}
	~TreeItem(){ qDeleteAll(m_childItems); }

	void setItemData(const QList<QVariant> &data){ m_itemData = data; }
	TreeItem* appendChild(TreeItem *child){ m_childItems.append(child); child->setParent(this); return child; }

	TreeItem *child(int row){ return m_childItems.value(row); }
	int childCount() const{ return m_childItems.count(); }
	int columnCount() const{ return m_itemData.count(); }
	QVariant data(int column) const{ return m_itemData.value(column); }
	int row() const
	{
		if (m_parentItem)
			return m_parentItem->m_childItems.indexOf(const_cast<TreeItem*>(this));

		return 0;
	}
	void setParent(TreeItem* parent){ m_parentItem = parent; }
	TreeItem *parentItem(){ return m_parentItem; }

	void setPluginSpec(PluginSpec* spec){ m_spec = spec; }
	PluginSpec* pluginSpec(){ return m_spec; }

private:
	QList<TreeItem*> m_childItems;
	QList<QVariant> m_itemData;
	TreeItem *	m_parentItem;
	PluginSpec* m_spec;
};

class TreeModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	explicit TreeModel(QObject *parent = 0);
	~TreeModel();

	QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
	Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
	QVariant headerData(int section, Qt::Orientation orientation,
		int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
	QModelIndex index(int row, int column,
		const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
	QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;
	int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
	int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
	TreeItem* rootItem()const{ return m_rootItem; }

	TreeItem* itemForIndex(const QModelIndex &idx)const;

private:
	TreeItem * m_rootItem;
};

#endif // TREEMODEL_H

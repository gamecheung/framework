#include "ExtensionSystemEx.h"
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QDebug>
#include "TreeModel.h"
#include "PluginDetailsDialog.h"

class PluginViewPrivate
{
	TreeModel* model;
	PluginManager* pluginManager;
	friend class PluginView;
};

PluginView::PluginView( QWidget* parent)
	:QTreeView(parent),d(new PluginViewPrivate())
{
	d->model = new TreeModel(this);
	d->model->rootItem()->setItemData(QList<QVariant>() << tr("名称") << tr("错误") << tr("版本") << tr("供应商"));
	setModel(d->model);

	d->pluginManager = PluginManager::instance();

	setAlternatingRowColors(true);
	setUniformRowHeights(true);
	setSortingEnabled(true);
	setSelectionMode(QAbstractItemView::SingleSelection);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setEditTriggers(QAbstractItemView::NoEditTriggers);

	header()->setSortIndicator(0, Qt::AscendingOrder);

	updateView();
	expandAll();
}
PluginView::~PluginView()
{
	delete d;
}


PluginSpec* PluginView::currentPluginSpec()
{
	//const QModelIndex &sourceIndex = m_sortModel->mapToSource(currentIndex());

	//auto item = dynamic_cast<PluginItem *>(d->model->itemForIndex(sourceIndex));
	//return item ? item->m_spec : 0;

	return d->model->itemForIndex(currentIndex())->pluginSpec();
}

void PluginView::updateView()
{
	for (auto& var : d->pluginManager->collections())
	{
		auto cateNode = new TreeItem(QList<QVariant>()
			<< var->name() << "" << "" << "" << ""
			);
		d->model->rootItem()->appendChild(cateNode);

		for (auto& spec : var->plugins())
		{
			auto specNode = new TreeItem(QList<QVariant>()
				<< spec->name() << spec->hasError() << spec->version() << spec->vendor()
				);
			specNode->setPluginSpec(spec);
			cateNode->appendChild(specNode);
		}
	}
}


class PluginDialogPrivate
{
	QLineEdit* filterEdit;
	PluginView* view;
	QPushButton* detailButton;
	QPushButton* closeButton;

	friend class PluginDialog;
};

PluginDialog::PluginDialog(QWidget* parent)
	:QDialog(parent), d(new PluginDialogPrivate())
{
	setWindowTitle(tr("关于插件"));

	d->filterEdit = new QLineEdit(this);
	d->filterEdit->setClearButtonEnabled(true);

	d->view = new PluginView(this);
	connect(d->view, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(showCurrentPluginSpecDetails()));

	d->detailButton = new QPushButton(tr("详情..."), this);
	connect(d->detailButton, SIGNAL(clicked()), this, SLOT(showCurrentPluginSpecDetails()));

	d->closeButton = new QPushButton(tr("关闭"), this);
	connect(d->closeButton, SIGNAL(clicked()), this, SLOT(accept()));

	QHBoxLayout* bottomLayout = new QHBoxLayout();
	bottomLayout->addWidget(d->detailButton);
	bottomLayout->addStretch();
	bottomLayout->addWidget(d->closeButton);


	QVBoxLayout* layout = new QVBoxLayout(this);
	setLayout(layout);
	layout->addWidget(d->filterEdit);
	layout->addWidget(d->view);
	layout->addLayout(bottomLayout);
}

PluginDialog::~PluginDialog()
{
	delete d;
}


void PluginDialog::showCurrentPluginSpecDetails()
{
	PluginSpec* spec = d->view->currentPluginSpec();
	if (spec)
	{
		PluginDetailsDialog dlg(spec);
		dlg.exec();
	}
}


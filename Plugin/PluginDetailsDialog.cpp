#include "PluginDetailsDialog.h"

PluginDetailsDialog::PluginDetailsDialog(PluginSpec* spec, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	if (spec)
	{
		setWindowTitle(tr("插件(%1)详情").arg(spec->name()));
		ui.label_IID->setText(spec->iid());
		ui.label_Name->setText(spec->name());
		ui.label_Version->setText(spec->version());
		ui.label_CompatVersion->setText(spec->compatVersion());
		ui.label_Vendor->setText(spec->vendor());
		ui.label_Group->setText(spec->category());
		ui.label_Location->setText(spec->filePath());
		ui.label_Copyright->setText(spec->copyright());
		ui.textEdit_Description->setPlainText(spec->description());
		for (auto& dep : spec->dependencies())
			ui.listWidget_Dependencies->addItem(QString("%1 (%2)").arg(dep.name).arg(dep.version));

		ui.textEdit_Error->setPlainText(spec->errorString());
	}
}

PluginDetailsDialog::~PluginDetailsDialog()
{

}

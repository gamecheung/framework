#ifndef PLUGINDETAILSDIALOG_H
#define PLUGINDETAILSDIALOG_H

#include <QDialog>
#include "ui_PluginDetailsDialog.h"
#include "ExtensionSystem.h"

class PluginDetailsDialog : public QDialog
{
	Q_OBJECT

public:
	PluginDetailsDialog(PluginSpec* spec, QWidget *parent = 0);
	~PluginDetailsDialog();

private:
	Ui::PluginDetailsDialog ui;
};

#endif // PLUGINDETAILSDIALOG_H

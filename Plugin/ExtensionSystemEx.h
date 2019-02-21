#ifndef EXTENSIONSYSTEM_EX_H
#define EXTENSIONSYSTEM_EX_H

#include "ExtensionSystem.h"
#include <QDialog>
#include <QTreeView>
#include <QTreeWidget>
#include <QSettings>

class PluginViewPrivate;
class EXTENSIONSYSTEM_EXPORT PluginView :public QTreeView
{
	Q_OBJECT
public:
	PluginView(QWidget* parent = nullptr);
	~PluginView();
	PluginSpec* currentPluginSpec();

private:
	void updateView();

private:
	PluginViewPrivate* d;
};



class PluginDialogPrivate;
class EXTENSIONSYSTEM_EXPORT PluginDialog :public QDialog
{
	Q_OBJECT
public:
	PluginDialog(QWidget* parent = nullptr);
	~PluginDialog();

	public slots:
	void showCurrentPluginSpecDetails();

private:
	PluginDialogPrivate* d;
};

#endif // EXTENSIONSYSTEM_EX_H

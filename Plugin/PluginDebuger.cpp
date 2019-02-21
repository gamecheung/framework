#include "ExtensionSystem.h"
#include <QDebug>

PluginDebuger::PluginDebuger(QObject* parent)
	:QObject(parent)
{
	connect(PluginManager::instance(), SIGNAL(loadProgress(const QString&))
		, this, SLOT(print(const QString&)));
}
PluginDebuger::~PluginDebuger()
{

}
void PluginDebuger::print(const QString& msg)
{
	qDebug() << msg;
}
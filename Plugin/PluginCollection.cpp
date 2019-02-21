#include "ExtensionSystem.h"

class PluginCollectionPrivate
{
	QString name;
	QList<PluginSpec*> plugins;

	friend class PluginCollection;
};

PluginCollection::PluginCollection(const QString& name)
	:d(new PluginCollectionPrivate())
{
	d->name = name;
}
PluginCollection::~PluginCollection()
{
	delete d;
}

const QString& PluginCollection::name()const{ return d->name; }
QList<PluginSpec*> PluginCollection::plugins()const{ return d->plugins; }

void PluginCollection::addPlugin(PluginSpec* spec){ d->plugins.append(spec); }
void PluginCollection::removePlugin(PluginSpec* spec){ d->plugins.removeOne(spec); }

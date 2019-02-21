#include "ExtensionSystem.h"

class IPluginPrivate
{
	PluginSpec* pluginSpec;

	friend class IPlugin;
};

IPlugin::IPlugin()
	:d(new IPluginPrivate())
{

}
IPlugin::~IPlugin()
{
	delete d;
}


PluginSpec* IPlugin::pluginSpec()const{ return d->pluginSpec; }

void IPlugin::setPluginSpec(PluginSpec* spec){ d->pluginSpec = spec; }

QWidget* IPlugin::widget()
{
	Q_ASSERT_X(false,"IPlugin::widget()","plugin->widget is null!");
	return nullptr;
}

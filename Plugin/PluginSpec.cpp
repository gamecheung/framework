#include "ExtensionSystem.h"
#include <QPluginLoader>
#include <QJsonArray>

class PluginSpecPrivate
{
	QString iid;
	QString name;
	QString version;
	QString compatVersion;
	bool	required;
	QString vendor;
	QString copyright;
	QString license;
	QString description;
	QString category;
	QString url;
	QString filePath;
	bool	hasError;
	QString errorString;
	QList<PluginDependency> dependencies;

	PluginManager*	pluginManager;
	QPluginLoader	loader;
	IPlugin*		iPlugin;

	friend class PluginSpec;
	PluginSpecPrivate() :hasError(false),iPlugin(nullptr){}
};

PluginSpec::PluginSpec(PluginManager* pluginManager)
	:d(new PluginSpecPrivate())
{
	d->pluginManager = pluginManager;
	d->hasError = false;
}

PluginSpec::~PluginSpec()
{
	//d->loader.unload();
	delete d;
}


bool PluginSpec::read(const QString& filePath)
{
	d->loader.setFileName(filePath);
	d->filePath = filePath;
	if (d->loader.fileName().isEmpty())
		return false;

	if (!readMetaData(d->loader.metaData()))
		return false;

	return true;
}

bool PluginSpec::loadLibrary()
{
	if (hasError())
	{
		Q_ASSERT(false);
		return false;
	}

	if (!d->loader.load())
	{
		setErrorString(QObject::tr("没有找到运行库(%1)！").arg(d->loader.errorString()));
		return false;
	}

	d->iPlugin = qobject_cast<IPlugin*>(d->loader.instance());
	if (d->iPlugin == nullptr)
	{
		setErrorString(QObject::tr("不能够转换为IPlugin对象！"));
		return false;
	}

	d->iPlugin->setPluginSpec(this);
	return true;
}
bool PluginSpec::initialize(QString *errorString)
{
	if (d->iPlugin)
		return d->iPlugin->initialize(errorString);

	*errorString = QObject::tr("d->iPlugin是空指针！");
	return false;
}
bool PluginSpec::run(QString *errorString)
{
	if (d->iPlugin)
		return d->iPlugin->run(errorString);

	*errorString = QObject::tr("d->iPlugin是空指针！");
	return false;
}

bool PluginSpec::shutdown(QString *errorString)
{
	bool rs = false;
	if (d->iPlugin)
	{
		rs = d->iPlugin->shutdown(errorString);
		d->loader.unload();
	}
	else
	{
		*errorString = QObject::tr("d->iPlugin是空指针！");
		rs = false;
	}
	return rs;
}

bool PluginSpec::readMetaData(const QJsonObject &metaData)
{
	if (metaData.isEmpty())
		return false;

	QJsonValue value;

	d->iid = metaData.value("IID").toString();
	if (d->iid.isEmpty())
		return false;

	value = metaData.value("MetaData");
	if (value.isObject())
	{
		QJsonObject pluginInfo = value.toObject();

		d->name = pluginInfo.value("Name").toString();
		d->version = pluginInfo.value("Version").toString();
		d->compatVersion = pluginInfo.value("CompatVersion").toString();
		d->vendor = pluginInfo.value("Vendor").toString();
		d->copyright = pluginInfo.value("Copyright").toString();
		d->required = pluginInfo.value("Required").toBool();
		d->category = pluginInfo.value("Category").toString();
		d->description = pluginInfo.value("Description").toString();
		d->url = pluginInfo.value("Url").toString();

		QJsonArray array = pluginInfo.value("Dependencies").toArray();
		for (const auto& v : array)
		{
			QJsonObject depObject = v.toObject();
			PluginDependency dep;
			dep.name = depObject.value("Name").toString();
			dep.version = depObject.value("Version").toString();
			d->dependencies.append(dep);
		}
	}

	return true;
}

PluginManager* PluginSpec::pluginManager()const{ return d->pluginManager; }
const QString& PluginSpec::iid()const{ return d->iid; }
const QString& PluginSpec::name()const{ return d->name; }
const QString& PluginSpec::version()const{ return d->version; }
const QString& PluginSpec::compatVersion()const{ return d->compatVersion; }
const QString& PluginSpec::vendor()const{ return d->vendor; }
const QString& PluginSpec::copyright()const{ return d->copyright; }
const QString& PluginSpec::license()const{ return d->license; }
const QString& PluginSpec::description()const{ return d->description; }
const QString& PluginSpec::category()const{ return d->category; }
const QString& PluginSpec::url()const{ return d->url; }
const QString& PluginSpec::filePath()const{ return d->filePath; }
const bool PluginSpec::hasError()const{ return d->hasError; }
const QString& PluginSpec::errorString()const{ return d->errorString; }
const QList<PluginDependency>& PluginSpec::dependencies()const{ return d->dependencies; }

void PluginSpec::setErrorString(const QString& errorString)
{
	d->errorString = errorString;
	d->hasError = true;
}
void PluginSpec::clearError()
{
	d->errorString.clear();
	d->hasError = false;
}

QObject* PluginSpec::instance(){ return d->loader.instance(); }

bool PluginSpec::isLoaded()const{ return d->loader.isLoaded(); }
#include "ExtensionSystem.h"
#include <QSettings>
#include <QDir>
#include <QFileInfo>
#include <QLibrary>
#include <QThread>
#include <QMap>
#include <QApplication>
#include <QProgressDialog>
#include <QMessageBox>
#include <QDebug>
#include <win32/XDbgHelper.hpp>
#include "ExtensionSystemEx.h"

class PluginManagerPrivate
{
	QString		pluginIID;
	QSettings*	globalSettings;
	QSettings*	settings;
	QStringList pluginPaths;
	QStringList	forcePluginFiles;
	QList<PluginSpec*> srcPluginSpecs;
	QList<PluginSpec*> loadQueuePluginSpecs;
	QMap<QString, PluginSpec*>	pluginSpecsMap;
	QList<PluginCollection*>	collections;

	friend class PluginManager;
};

PluginManager* PluginManager::self = nullptr;

PluginManager::PluginManager(const QString& pluginIID)
	:QObject(), d(new PluginManagerPrivate())
{

	d->pluginIID = pluginIID;
	initializeManager();
}


void PluginManager::initializeManager()
{
	Q_ASSERT(self == nullptr);
	self = this;
	QObject::connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(shutdown()));
}

PluginManager::~PluginManager()
{
	delete d;
}

PluginManager* PluginManager::instance(){ return self; }

const QString& PluginManager::pluginIID()const{ return d->pluginIID; }
QSettings* PluginManager::globalSettings()const{ return d->globalSettings; }
QSettings* PluginManager::settings()const{ return d->settings; }
const QStringList& PluginManager::pluginPaths()const{ return d->pluginPaths; }
QList<PluginSpec*> PluginManager::pluginSpecs()const{ return d->srcPluginSpecs; }
const QList<PluginCollection*>& PluginManager::collections()const{ return d->collections; }

PluginSpec* PluginManager::pluginSpec(const QString& name) const
{
	auto itr = d->pluginSpecsMap.find(name);
	if (itr != d->pluginSpecsMap.end())
		return *itr;

	return nullptr;
}


void PluginManager::setGlobalSettings(QSettings* globalSettings)
{
	d->globalSettings = globalSettings;
	if (globalSettings)
		globalSettings->setParent(this);
}
void PluginManager::setSettings(QSettings* settings)
{
	d->settings = settings;
	if (settings)
		settings->setParent(this);
}

void PluginManager::setPluginPaths(const QStringList& paths){ d->pluginPaths = paths; }

void PluginManager::addPluginFiles(const QStringList& files){ d->forcePluginFiles.append(files); }
void PluginManager::addPluginFile(const QString& file){ d->forcePluginFiles.append(file); }

void PluginManager::readPlugin(const QString& pluginFile, bool check)
{
	emitProgress(tr("读取文件：%1").arg(pluginFile));

	PluginSpec* spec = new PluginSpec(this);
	if (!spec->read(pluginFile))
	{
		delete spec;
		return;
	}
	if (check)
	{
		if (!spec->iid().startsWith(".") && !spec->iid().startsWith(pluginIID())) 
		{
			delete spec;
			return;
		}
	}

	d->srcPluginSpecs.append(spec);
	collection(spec->category())->addPlugin(spec);

	if (d->pluginSpecsMap.find(spec->name()) != d->pluginSpecsMap.end())
	{
		spec->setErrorString(tr("已存在同名插件： %1").arg(pluginSpec(spec->name())->filePath()));
		return;
	}

	d->pluginSpecsMap[spec->name()] = spec;
}

void PluginManager::readPlugins()
{
	QStringList pluginFiles = getAllPluginFiles();
	for (const auto& pluginFile : pluginFiles)
	{
		readPlugin(pluginFile, true);
	}
	for (const auto& pluginFile : d->forcePluginFiles)
	{
		readPlugin(pluginFile, false);
	}

	d->loadQueuePluginSpecs = loadQueue(d->srcPluginSpecs);
}

void PluginManager::loadPlugins()
{
	for (auto& spec : d->loadQueuePluginSpecs)
	{
		emitProgress(tr("加载：%1").arg(spec->name())); 
		spec->loadLibrary();
	}
	for (auto& spec : d->loadQueuePluginSpecs)
	{
		emitProgress(tr("初始化：%1").arg(spec->name()));
		for (auto& dep : spec->dependencies())
		{
			if (pluginSpec(dep.name)->hasError())
			{
				QString errString = spec->errorString();
				errString.append("\n ");
				errString.append(tr("'%1'依赖的插件'%2'存在错误！").arg(spec->name()).arg(dep.name));
				spec->setErrorString(errString);
			}
		}

		if (!spec->hasError())
		{
			QString errorString;
			if (!spec->initialize(&errorString))
			{
				spec->setErrorString(errorString);
			}
		}
	}
	for (auto& spec : d->loadQueuePluginSpecs)
	{
		emitProgress(tr("启动：%1").arg(spec->name())); 
		for (auto& dep : spec->dependencies())
		{
			if (pluginSpec(dep.name)->hasError())
			{
				QString errString = spec->errorString();
				errString.append("\n ");
				errString.append(tr("'%1'依赖的插件'%2'存在错误！").arg(spec->name()).arg(dep.name));
				spec->setErrorString(errString);
			}
		}

		if (!spec->hasError())
		{
			QString errString;
			if (!spec->run(&errString))
			{
				spec->setErrorString(errString);
			}
		}
	}

	emit loadFinished();
}


void PluginManager::shutdown()
{
	int count = d->loadQueuePluginSpecs.count();
	QProgressDialog progress(tr("关闭插件..."), tr(""), 0, count, nullptr);
 	progress.setMinimumDuration(0);
	progress.setWindowModality(Qt::WindowModal);
	progress.setCancelButton(nullptr);

	QStringList errStrings;
	for (int i = count - 1; i >= 0; --i)
	{
		auto spec = d->loadQueuePluginSpecs.at(i);

		progress.setLabelText(tr("关闭：%1").arg(spec->name()));
		progress.setValue(count-i-1);
		QApplication::processEvents();

		emitProgress(tr("关闭：%1").arg(spec->name()));

		QString errString;
		if (!spec->shutdown(&errString))
			errStrings.append(QString("%1: %2").arg(spec->name()).arg(errString));

	}
	progress.setValue(count);

	if (!errStrings.isEmpty())
	{
		QMessageBox::critical(nullptr, tr("插件关闭错误"), errStrings.join("\n"));
	}

	d->loadQueuePluginSpecs.clear();
	qDeleteAll(d->srcPluginSpecs);
	qDeleteAll(d->collections);
}

QList<PluginSpec *> PluginManager::loadQueue(const QList<PluginSpec *>& pluginSpecs) 
{
	QList<PluginSpec *> queue;
	QList<PluginSpec *> checkedSpecs;
	lastCircularQueue.clear();
	foreach(PluginSpec *spec, pluginSpecs) {
		emitProgress(tr("解析队列：%1").arg(spec->name()));
		if(!checkedSpecs.contain(spec))
		{
			QList<PluginSpec *> circularityCheckQueue;
			loadQueue(spec, queue, circularityCheckQueue);
			checkedSpecs.append(circularityCheckQueue);
		}
		
	}
	return queue;
}

bool PluginManager::loadQueue(PluginSpec *spec, QList<PluginSpec *> &queue,QList<PluginSpec *> &circularityCheckQueue)const
{
	if (spec == nullptr ||spec->hasError())
		return false;

	if (queue.contains(spec))
		return true;

	// check for circular dependencies
	if (circularityCheckQueue.contains(spec)) {
		QString errorString;
		errorString = tr("检测到插件循环依赖:");
		errorString.append("\n");

		int index = circularityCheckQueue.indexOf(spec);
		for (int i = index; i < circularityCheckQueue.size(); ++i) {
			errorString.append(tr("%1(%2)依赖")
				.arg(circularityCheckQueue.at(i)->name()).arg(circularityCheckQueue.at(i)->version()));
			errorString += QLatin1Char('\n');
		}
		errorString.append(PluginManager::tr("%1(%2)").arg(spec->name()).arg(spec->version()));
		spec->setErrorString(errorString);
		Q_ASSERT(false);
		return false;
	}

	circularityCheckQueue.append(spec);

	auto dependencies = spec->dependencies();
	for (auto& dep : dependencies)
	{
		auto depSpec = pluginSpec(dep.name);
		if (depSpec == nullptr)
		{
			spec->setErrorString(tr("插件(%1)加载失败：依赖的插件'%2'没有找到！").arg(spec->name()).arg(dep.name));
			return false;
		}

		if (!loadQueue(depSpec, queue, circularityCheckQueue))
		{
			spec->setErrorString(tr("插件(%1(%2))加载失败：%3")
							.arg(depSpec->name()).arg(depSpec->version()).arg(depSpec->errorString()));
			return false;
		}
	}
	queue.append(spec);
	return true;
}

PluginCollection* PluginManager::collection(const QString& category)
{
	for (auto& var : d->collections)
	{
		if (var->name() == category)
			return var;
	}

	d->collections.append(new PluginCollection(category));
	return collection(category);
}

QStringList PluginManager::getAllPluginFiles()const
{
	QStringList pluginFiles;
	QStringList searchPaths = d->pluginPaths;
	while (!searchPaths.isEmpty())
	{
		const QDir dir(searchPaths.takeFirst());
		QDir::SortFlags sortFlags = QDir::Name;
#ifdef _DEBUG
		sortFlags = QDir::Name|QDir::Reversed;
#endif 
		const QFileInfoList files = dir.entryInfoList(QStringList()<<"*Plugin*",QDir::Files | QDir::NoSymLinks, sortFlags);
		for (const auto& file : files)
		{ 
			const QString filePath = file.absoluteFilePath();
			if (QLibrary::isLibrary(filePath))
			{
				Q_ASSERT(!pluginFiles.contains(filePath));
				pluginFiles.append(filePath);
			}
		}
	}

	return pluginFiles;
}

void PluginManager::emitProgress(const QString& progress)
{
	emit loadProgress(progress);
	QApplication::processEvents();
}

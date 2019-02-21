#ifndef EXTENSIONSYSTEM_H
#define EXTENSIONSYSTEM_H

#include <QObject>
#include <QSettings>

#ifdef EXTENSIONSYSTEM_LIB
# define EXTENSIONSYSTEM_EXPORT Q_DECL_EXPORT
# ifdef _DEBUG
#  include <vld.h>
# endif
#else
# define EXTENSIONSYSTEM_EXPORT Q_DECL_IMPORT
#  ifdef _DEBUG
#   pragma comment(lib, "ExtensionSystemd.lib")
#  else
#   pragma comment(lib, "ExtensionSystem.lib")
#  endif
#endif


struct EXTENSIONSYSTEM_EXPORT PluginDependency
{
	QString name;
	QString version;
};

class PluginManager;
class PluginSpecPrivate;
class EXTENSIONSYSTEM_EXPORT PluginSpec
{
public:
	~PluginSpec();
	PluginManager* pluginManager()const;
	const QString& iid()const;
	const QString& name()const;
	const QString& version()const;
	const QString& compatVersion()const;
	const QString& vendor()const;
	const QString& copyright()const;
	const QString& license()const;
	const QString& description()const;
	const QString& category()const;
	const QString& url()const;
	const QString& filePath()const;
	const bool hasError()const;
	const QString& errorString()const;
	const QList<PluginDependency>& dependencies()const;

	QObject* instance();
	bool isLoaded()const;

private:
	PluginSpec(PluginManager* pluginManager);
	bool read(const QString& filePath);
	bool readMetaData(const QJsonObject &metaData);
	bool loadLibrary();
	bool initialize(QString *errorString);
	bool run(QString *errorString);
	bool shutdown(QString *errorString);
	void setErrorString(const QString& errorString);
	void clearError();

private:
	Q_DISABLE_COPY(PluginSpec)
	friend class PluginManager;
	PluginSpecPrivate* d;
};

class PluginCollectionPrivate;
class EXTENSIONSYSTEM_EXPORT PluginCollection
{
public:
	PluginCollection(const QString& name);
	~PluginCollection();

	const QString& name()const;
	QList<PluginSpec*> plugins()const;

	void addPlugin(PluginSpec* spec);
	void removePlugin(PluginSpec* spec);

private:
	Q_DISABLE_COPY(PluginCollection)
	PluginCollectionPrivate* d;
};

class PluginManagerPrivate;
class EXTENSIONSYSTEM_EXPORT PluginManager : public QObject
{
	Q_OBJECT

signals:
	void loadProgress(const QString& progress);
	void loadFinished();

public:
	PluginManager(const QString& pluginIID=QString());
	~PluginManager();

	const QString& pluginIID()const;
	QSettings* globalSettings()const;
	QSettings* settings()const;
	const QStringList& pluginPaths()const;
	QList<PluginSpec*> pluginSpecs()const;
	PluginSpec* pluginSpec(const QString& name) const;
	const QList<PluginCollection*>& collections()const;

	template<typename _Ty>
	_Ty* pluginInstance(const QString& name)
	{
		PluginSpec* spec = pluginSpec(name);
		if (spec) {
			return qobject_cast<_Ty*>(spec->instance());
		}
		return nullptr;
	}

	void setGlobalSettings(QSettings* globalSettings);
	void setSettings(QSettings* settings);
	void setPluginPaths(const QStringList& paths);
	void addPluginFiles(const QStringList& files);
	void addPluginFile(const QString& file);

	void readPlugins();
	void loadPlugins();

	static PluginManager *instance();

public slots:
	void shutdown();

private:
	Q_DISABLE_COPY(PluginManager)
	void initializeManager();
	void readPlugin(const QString& pathfile, bool check = true);
	QStringList getAllPluginFiles()const;
	QList<PluginSpec *> loadQueue(const QList<PluginSpec *>& pluginSpecs) ;
	bool loadQueue(PluginSpec *spec, QList<PluginSpec *> &queue, QList<PluginSpec *> &circularityCheckQueue)const;
	PluginCollection* collection(const QString& category);
	void emitProgress(const QString& progress);

private:
	PluginManagerPrivate* d;
	static PluginManager* self;
};

class IPluginPrivate;
class EXTENSIONSYSTEM_EXPORT IPlugin: public QObject
{ 
	Q_OBJECT

public:
	IPlugin();
	virtual ~IPlugin();
	PluginSpec* pluginSpec()const;
	virtual QWidget* widget();
private:
	virtual bool initialize(QString *errorString) =0;
	virtual bool run(QString *errorString)=0;
	virtual bool shutdown(QString *errorString) = 0;
	void setPluginSpec(PluginSpec* spec);

private:
	Q_DISABLE_COPY(IPlugin)
	friend class PluginSpec;
	IPluginPrivate* d;
};

class EXTENSIONSYSTEM_EXPORT PluginDebuger :public QObject
{
	Q_OBJECT
public:
	PluginDebuger(QObject* parent = nullptr);
	~PluginDebuger();

private slots:
	void print(const QString& msg);
};

#endif // EXTENSIONSYSTEM_H

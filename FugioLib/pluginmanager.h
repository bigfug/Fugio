#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include "fugio/global.h"
#include <QObject>
#include <QDir>
#include <QJsonDocument>
#include <QRunnable>
#include <QEventLoop>
#include <QTemporaryFile>
#include <QUrl>
#include <QVersionNumber>
#include <QSettings>

FUGIO_NAMESPACE_BEGIN
class GlobalInterface;
FUGIO_NAMESPACE_END

class PluginAction : public QObject, public QRunnable
{
    Q_OBJECT

public:
    PluginAction() {}

    virtual ~PluginAction() {}

    virtual void run( void ) Q_DECL_OVERRIDE
    {
        action();

        emit finished();
    }

    virtual bool action( void ) = 0;

signals:
    void status( const QString &pStatus );
    void verbose( const QString &pStatus );
    void error( const QString &pStatus );
    void finished( void );
};

class PluginActionInstall : public PluginAction
{
    Q_OBJECT

public:
    PluginActionInstall( const QString &pFileName, const QString &pDestName )
        : m_FileName( pFileName ), m_DestName( pDestName )
    {

    }

    virtual ~PluginActionInstall() {}

    virtual bool action( void ) Q_DECL_OVERRIDE;

private:
    static size_t on_extract(void *arg, unsigned long long offset, const void *buf, size_t bufsize);

private:
    const QString m_FileName;
    const QString m_DestName;
};

class PluginActionRemove : public PluginAction
{
    Q_OBJECT

public:
    PluginActionRemove( const QString &pFileName, const QString &pDestName )
        : m_FileName( pFileName ), m_DestName( pDestName )
    {

    }

    virtual ~PluginActionRemove() {}

    virtual bool action( void ) Q_DECL_OVERRIDE;

private:
    static size_t on_extract(void *arg, unsigned long long offset, const void *buf, size_t bufsize);

private:
    const QString m_FileName;
    const QString m_DestName;
};

class PluginActionDownload : public PluginAction
{
    Q_OBJECT

public:
    PluginActionDownload( const QUrl &pUrl, const QString &pDestName )
        : m_Url( pUrl ), m_DestName( pDestName )
    {

    }

    virtual ~PluginActionDownload() {}

    virtual bool action( void ) Q_DECL_OVERRIDE;

private:
    static size_t on_extract(void *arg, unsigned long long offset, const void *buf, size_t bufsize);

signals:
    void downloadProgress( qint64 bytesReceived, qint64 bytesTotal );

private:
    const QUrl m_Url;
    const QString m_DestName;
    QEventLoop m_Loop;
    QTemporaryFile m_TempFile;

};

/*!
 * \brief The PluginRepoManifest class contains a single repository manifest
 */

class PluginRepoManifest
{
public:
    PluginRepoManifest( const QString &pFileName, const QString &pPlatform );

    QStringList pluginList( void ) const
    {
        return( m_PluginVersionMap.keys() );
    }

    QVersionNumber pluginLatestVersion( const QString &pPluginName ) const
    {
        return( m_PluginLatestMap.value( pPluginName ) );
    }

    QVector<QVersionNumber> pluginVersions( const QString &pPluginName ) const
    {
        QVector<QVersionNumber>   VersionList;

        for( const PluginEntry &Entry : m_PluginVersionMap.values( pPluginName ) )
        {
            VersionList << Entry.first;
        }

        return( VersionList );
    }

    QUrl pluginVersionUrl( const QString &pPluginName, const QVersionNumber &pPluginVersion )
    {
        for( const PluginEntry &Entry : m_PluginVersionMap.values( pPluginName ) )
        {
            if( Entry.first == pPluginVersion )
            {
                return( Entry.second );
            }
        }

        return( QUrl() );
    }

private:
    typedef QPair<QVersionNumber,QUrl> PluginEntry;

    QMap<QString, QVersionNumber>   m_PluginLatestMap;
    QMultiMap<QString, PluginEntry>  m_PluginVersionMap;
};

/*!
 * \brief The PluginConfig class contains what plugins are installed for an instance of Fugio
 */

class PluginConfig
{
public:
    PluginConfig( const QString &pFileName );

    QVersionNumber installedPluginVersion( const QString &pPluginName ) const;

    void setInstalledPluginVersion( const QString &pPluginName, const QVersionNumber &pPluginVersion );

	QStringList installedPlugins( void );

private:
    QSettings       m_Config;
};

/*!
 * \brief The PluginArchive class
 */

class PluginArchive
{
public:
	PluginArchive( const QString &pFileName );

	bool moveArchive( const QString &pDestName );

	bool isManifestValid( void ) const;

private:
	QString			m_FileName;
	QJsonDocument	m_Manifest;
};

/*!
 * \brief The PluginCache class controls all the caching of repositories and plugin versions
 */

class PluginCache
{
public:
	PluginCache( void );

	virtual ~PluginCache( void );

	QString cachedPluginFilename( const QString &pPluginName, const QVersionNumber &pPluginVersion ) const;


	void setCachedPluginFilename( const QString &pPluginName, const QVersionNumber &pPluginVersion, const QString &pFileName );

	void addRepo( const QString &pRepoName, const QUrl &pRepoUrl );
	void removeRepo( const QString &pRepoName );

	QUrl repoUrl( const QString &pRepoName ) const;

	QStringList repoNames( void );

	// Directory of plugin repositories manifest files

	QDir repoCacheDirectory( void ) const
	{
		return( mRepoCacheDir );
	}

	// Directory where Fugio stores all the plugin cache data

	QDir pluginCacheDirectory() const;

	// Filename for the plugin config

	QString pluginConfigFilename( void ) const;

private:
	QSettings		 *m_Config;
	QDir		      mPluginConfigDir;
	QDir			  mPluginCacheDir;
	QDir			  mRepoCacheDir;
};

/*!
 * \brief The PluginManager class
 */

class PluginManager : public QObject
{
    Q_OBJECT

public:
    PluginManager();

    void loadPlugins( QDir pDir );

    void unloadPlugins( void );

    void initialisePlugins( fugio::GlobalInterface *pGlobalInterface );

    bool loadPlugin( const QString &pFileName );

    void registerPlugin( QObject *pPluginInstance );

    QStringList loadedPluginNames( void ) const
    {
        return( mLoadedPlugins );
    }

    void setEnabledPlugins( QStringList pEnabledPlugins )
    {
        mEnabledPlugins = pEnabledPlugins;
    }

    void setDisabledPlugins( QStringList pDisabledPlugins )
    {
        mDisabledPlugins = pDisabledPlugins;
    }

    void enablePlugin( QString pPluginName )
    {
        mEnabledPlugins << pPluginName;
    }

    void disablePlugin( QString pPluginName )
    {
        mDisabledPlugins << pPluginName;
    }

    QList<QObject *> plugInstList( void )
    {
        return( mPluginInstances );
    }

	// Directory where installed plugins are located

	QDir pluginDirectory() const;

    void setPluginDirectory( const QDir &pPluginDirectory )
    {
        mPluginDirectory = pPluginDirectory;
    }

private:
    QDir                             mPluginDirectory;

    QStringList						 mEnabledPlugins;
    QStringList						 mDisabledPlugins;
    QStringList						 mLoadedPlugins;

    QList<QObject *>				 mPluginInitList;
    QList<QObject *>                 mPluginInstances;
};

#endif // PLUGINMANAGER_H

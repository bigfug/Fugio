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

    void setPluginDirectory( const QDir &pPluginDirectory )
    {
        mPluginDirectory = pPluginDirectory;
    }

    QDir pluginDirectory() const;

    QDir pluginCacheDirectory() const;
    void setPluginCacheDirectory(const QDir &newPluginCacheDirectory);

    QString pluginConfigFilename( void ) const;

    void downloadPluginToTemp( const QUrl &pUrl );

    PluginActionInstall deployPlugin( const QString &pFileName, const QString &pDestDir ) const;

    QJsonDocument pluginManifest( const QString &pFileName ) const;

private:
    QDir                             mPluginDirectory;
    QDir                             mPluginConfigDir;
    QDir                             mPluginCacheDir;

    QStringList						 mEnabledPlugins;
    QStringList						 mDisabledPlugins;
    QStringList						 mLoadedPlugins;

    QList<QObject *>				 mPluginInitList;
    QList<QObject *>                 mPluginInstances;
};

#endif // PLUGINMANAGER_H

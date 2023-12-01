#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include "fugio/global.h"
#include <QObject>
#include <QDir>

FUGIO_NAMESPACE_BEGIN
class GlobalInterface;
FUGIO_NAMESPACE_END

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

private:
    QStringList						 mEnabledPlugins;
    QStringList						 mDisabledPlugins;
    QStringList						 mLoadedPlugins;

    QList<QObject *>				 mPluginInitList;
    QList<QObject *>                 mPluginInstances;
};

#endif // PLUGINMANAGER_H

#include "pluginmanager.h"

#include <QDateTime>
#include <QTimer>
#include <QPluginLoader>
#include <QNetworkAccessManager>
#include <QApplication>
#include <QStandardPaths>
#include <QMainWindow>
#include <QTranslator>
#include <QFileInfo>

#include <QXmlStreamWriter>
#include <QXmlStreamReader>

#include <QDebug>

#include "nodeprivate.h"
#include "pinprivate.h"
#include "contextprivate.h"

#include <fugio/plugin_interface.h>
#include <fugio/device_factory_interface.h>
#include <fugio/global_signals.h>

#if defined( Q_OS_WIN )
#include <Windows.h>
#endif

#include <QLibraryInfo>

PluginManager::PluginManager()
{

}

void PluginManager::loadPlugins(QDir pDir)
{
#if defined( Q_OS_MACX )
    for( QString dirName : pDir.entryList( QDir::Dirs | QDir::NoDotAndDotDot ) )
    {
        QDir		PlgDir = pDir.absoluteFilePath( dirName );

        if( PlgDir.cd( "Contents/MacOS" ) )
        {
            for( QString fileName : PlgDir.entryList( QDir::Files ) )
            {
                QFileInfo	File( PlgDir.absoluteFilePath( fileName ) );

                if( !File.isReadable() )
                {
                    continue;
                }

                loadPlugin( pDir.absoluteFilePath( File.absoluteFilePath() ) );
            }
        }
        else
        {
            loadPlugins( PlgDir );
        }
    }
#else
    for( QString fileName : pDir.entryList( QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot ) )
    {
        QFileInfo	File( pDir.absoluteFilePath( fileName ) );

        if( File.isDir() )
        {
            QString		NxtDir = pDir.absoluteFilePath( fileName );

#if defined( Q_OS_WIN )
            SetDllDirectory( (LPCTSTR)NxtDir.utf16() );

            loadPlugins( QDir( NxtDir ) );

            SetDllDirectory( Q_NULLPTR );
#else
            QString		CurDir = QDir::currentPath();

            QDir::setCurrent( NxtDir );

            loadPlugins( QDir::current() );

            QDir::setCurrent( CurDir );
#endif

            continue;
        }

        if( !File.isReadable() )
        {
            continue;
        }

#if defined( Q_OS_LINUX )
        if( File.suffix().toLower() != "so" )
        {
            continue;
        }
#elif defined( Q_OS_WIN )
        if( File.suffix().toLower() != "dll" )
        {
            continue;
        }
#endif

        loadPlugin( pDir.absoluteFilePath( fileName ) );
    }
#endif
}

void PluginManager::unloadPlugins()
{
    std::reverse( mPluginInstances.begin(), mPluginInstances.end() );

    for( QList<QObject *>::iterator it = mPluginInstances.begin() ; it != mPluginInstances.end() ; it++ )
    {
        fugio::PluginInterface	*Plugin = qobject_cast<fugio::PluginInterface *>( *it );

        if( Plugin )
        {
            Plugin->deinitialise();
        }
    }

    qDeleteAll( mPluginInstances );

    mPluginInstances.clear();

    qDeleteAll( mPluginInitList );

    mPluginInitList.clear();
}

void PluginManager::initialisePlugins( fugio::GlobalInterface *pGlobalInterface )
{
    bool	LastChance = false;
    int		ResCnt = mPluginInitList.size();

    while( ResCnt > 0 )
    {
        ResCnt = 0;

        for( int i = 0 ; i < mPluginInitList.size() ; )
        {
            QObject							*PlgObj = mPluginInitList.at( i );
            fugio::PluginInterface			*PlgInt = qobject_cast<fugio::PluginInterface *>( PlgObj );

            if( !LastChance )
            {
                qDebug() << "Initialising plugin" << PlgObj->metaObject()->className();
            }
            else
            {
                qDebug() << "Initialising plugin" << PlgObj->metaObject()->className() << "(Last Chance)";
            }

            qApp->processEvents();

            fugio::PluginInterface::InitResult		 PlgRes = PlgInt->initialise( pGlobalInterface, LastChance );

            if( PlgRes == fugio::PluginInterface::INIT_OK )
            {
                mPluginInstances.append( PlgObj );

                mPluginInitList.removeAt( i );

                ResCnt++;

                LastChance = false;

                continue;
            }

            if( PlgRes == fugio::PluginInterface::INIT_FAILED )
            {
                qWarning() << "Can't initialise plugin" << PlgObj->metaObject()->className();

                PlgObj->deleteLater();

                mPluginInitList.removeAt( i );

                ResCnt++;

                continue;
            }

            i++;
        }

        if( !ResCnt && !LastChance )
        {
            ResCnt++;

            LastChance = true;
        }
    }

    qDeleteAll( mPluginInitList );

    mPluginInitList.clear();

    qDebug() << tr( "Plugins loaded: %1" ).arg( mPluginInstances.size() );
}

bool PluginManager::loadPlugin(const QString &pFileName)
{
    QFileInfo		FileInfo( pFileName );
    QString			BaseName = FileInfo.baseName();

    if( !mEnabledPlugins.isEmpty() )
    {
        bool		PluginFound = false;

        for( QString S : mEnabledPlugins )
        {
            if( BaseName.contains( S ) )
            {
                PluginFound = true;

                break;
            }
        }

        if( !PluginFound )
        {
            return( false );
        }
    }

    for( QString S : mDisabledPlugins )
    {
        if( BaseName.contains( S ) )
        {
            qInfo() << "Skipping" << pFileName;

            return( false );
        }
    }

    QPluginLoader	Loader( pFileName );

    if( !Loader.load() )
    {
        qWarning() << pFileName << ":" << Loader.errorString();

        return( false );
    }

    QObject			*PluginInstance = Loader.instance();

    if( !PluginInstance )
    {
        qWarning() << pFileName << ":" << Loader.errorString();

        return( false );
    }

    fugio::PluginInterface	*Plugin = qobject_cast<fugio::PluginInterface *>( PluginInstance );

    if( !Plugin )
    {
        qWarning() << pFileName << "No PluginInterface";

        delete PluginInstance;

        return( false );
    }

    qDebug() << "Loading plugin:" << pFileName;

    registerPlugin( PluginInstance );

    mLoadedPlugins << FileInfo.baseName();

    return( true );

}

void PluginManager::registerPlugin(QObject *pPluginInstance)
{
    mPluginInitList.append( pPluginInstance );
}

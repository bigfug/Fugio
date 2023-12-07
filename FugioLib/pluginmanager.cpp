#include "pluginmanager.h"

#include <QDateTime>
#include <QTimer>
#include <QPluginLoader>
#include <QStandardPaths>
#include <QTranslator>
#include <QFileInfo>
#include <QCoreApplication>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QTemporaryFile>

#include <QXmlStreamWriter>
#include <QXmlStreamReader>

#include <QDebug>

#include <fugio/plugin_interface.h>
#include <fugio/device_factory_interface.h>
#include <fugio/global_signals.h>

#if defined( Q_OS_WIN )
#include <Windows.h>
#endif

#include <QLibraryInfo>
#include <QStandardPaths>

#include "../libs/zip/zip.h"

PluginManager::PluginManager()
{
    mPluginConfigDir = QDir( QStandardPaths::writableLocation( QStandardPaths::GenericDataLocation ) );

    if( !mPluginConfigDir.exists( "fugio-plugins" ) )
    {
        if( !mPluginConfigDir.mkdir( "fugio-plugins" ) )
        {
            exit( 1 );
        }
    }

    if( !mPluginConfigDir.cd( "fugio-plugins" ) )
    {
        exit( 1 );
    }

    if( !mPluginConfigDir.exists( "plugin-cache" ) )
    {
        if( !mPluginConfigDir.mkdir( "plugin-cache" ) )
        {
            exit( 1 );
        }
    }

    mPluginCacheDir = QDir( mPluginConfigDir.absoluteFilePath( "plugin-cache" ) );
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

QDir PluginManager::pluginDirectory() const
{
    return mPluginDirectory;
}

QDir PluginManager::pluginCacheDirectory() const
{
    return mPluginCacheDir;
}

void PluginManager::setPluginCacheDirectory(const QDir &newPluginCacheDirectory)
{
    mPluginCacheDir = newPluginCacheDirectory;
}

QString PluginManager::pluginConfigFilename() const
{
    return( mPluginConfigDir.absoluteFilePath( "fugio-plugin-config.ini" ) );
}

QJsonDocument PluginManager::pluginManifest(const QString &pFileName) const
{
    const bool verbose = true;

    struct zip_t *zfh = zip_open( qPrintable( pFileName ), 0, 'r' );

    if( !zfh )
    {
        fputs( "Couldn't open zip file", stderr );

        return( QJsonDocument() );
    }

    const int PluginEntries = zip_total_entries( zfh );

    if( verbose )
    {
        printf( "Files in plugin zip archive: %d\n", PluginEntries );
    }

    if( zip_entry_open( zfh, "manifest.json" ) )
    {
        fputs( "error finding manifest.json in archive", stderr );

        return( QJsonDocument() );
    }

    const int ManifestSize = zip_entry_size( zfh );

    if( verbose )
    {
        printf( "manifest.json size is %d bytes\n", ManifestSize );
    }

    QByteArray		ManifestData;

    ManifestData.resize( ManifestSize );

    const int ManifestRead = zip_entry_noallocread( zfh, ManifestData.data(), ManifestData.size() );

    if( ManifestRead != ManifestSize )
    {
        fprintf( stderr, "Manifest read error: got %d - wanted %d bytes\n", ManifestRead, ManifestSize );

        return( QJsonDocument() );
    }

    if( verbose )
    {
        fwrite( ManifestData.constData(), ManifestRead, 1, stdout );
    }

    zip_entry_close( zfh );

    zip_close( zfh );

    // we have the raw manifest file in memory - parse it

    QJsonParseError	JsonError;

    QJsonDocument	JSON = QJsonDocument::fromJson( ManifestData, &JsonError );

    if( JSON.isNull() )
    {
        fprintf( stderr, "%s\n", qPrintable( JsonError.errorString() ) );

        return( QJsonDocument() );
    }

    return( JSON );
}

bool PluginActionInstall::action()
{
    QDir    DestBase( m_DestName );

    struct zip_t *zfh = zip_open( qPrintable( m_FileName ), 0, 'r' );

    if( !zfh )
    {
        emit error( tr( "Couldn't open zip file" ) );

        return( false );
    }

    const int PluginEntries = zip_total_entries( zfh );

    //    if( verbose )
    //    {
    //        printf( "Files in plugin zip archive: %d\n", PluginEntries );
    //    }

    //    if( zip_entry_open( zfh, "manifest.json" ) )
    //    {
    //        fputs( "error finding manifest.json in archive", stderr );

    //        exit( 1 );
    //    }

    //    const int ManifestSize = zip_entry_size( zfh );

    //    if( verbose )
    //    {
    //        printf( "manifest.json size is %d bytes\n", ManifestSize );
    //    }

    //    QByteArray		ManifestData;

    //    ManifestData.resize( ManifestSize );

    //    const int ManifestRead = zip_entry_noallocread( zfh, ManifestData.data(), ManifestData.size() );

    //    if( ManifestRead != ManifestSize )
    //    {
    //        fprintf( stderr, "Manifest read error: got %d - wanted %d bytes\n", ManifestRead, ManifestSize );

    //        exit( 1 );
    //    }

    //    if( verbose )
    //    {
    //        fwrite( ManifestData.constData(), ManifestRead, 1, stdout );
    //    }

    //    zip_entry_close( zfh );

    for( int i = 0 ; i < PluginEntries ; i++ )
    {
        if( zip_entry_openbyindex( zfh, i ) != 0 )
        {
            return( false );
        }

        QFileInfo	FI( zip_entry_name( zfh ) );

        QStringList FP = FI.filePath().split( '/', Qt::SkipEmptyParts );
        QString FD = FP.first();

        //            qDebug() << FI.filePath() << FI.isDir() << FI.isFile() << FP << FD;

        if( zip_entry_isdir( zfh ) )
        {
            //printf( "%s (dir)\n", zip_entry_name( zfh ) );
        }
        else
        {
            printf( "%s (file)\n", zip_entry_name( zfh ) );

            static const QStringList  Dirs = { "plugins", "examples", "include" };

            if( Dirs.contains( FD ) )
            {
                QDir    FileBase( DestBase );

                QString FileName = FP.takeLast();

                QString FilePath = FP.join( '/' );

                if( FileBase.mkpath( FilePath ) )
                {
                    QFile   DestFile( FileBase.absoluteFilePath( FI.filePath() ) );

                    if( DestFile.open( QFile::WriteOnly | QFile::NewOnly ) )
                    {
                        if( zip_entry_extract( zfh, &PluginActionInstall::on_extract, &DestFile ) )
                        {
                            return( false );
                        }
                    }
                }

                qDebug() << "plugin" << FP.join( '/' );
            }
            else
            {
                qDebug() << "unknown" << FD << FP.join( '/' );
            }
        }

        zip_entry_close( zfh );
    }

    zip_close( zfh );

    return( true );
}

size_t PluginActionInstall::on_extract(void *arg, unsigned long long offset, const void *buf, size_t bufsize)
{
    return( static_cast<QFile *>( arg )->write( static_cast<const char *>( buf ), bufsize ) );
}

bool PluginActionDownload::action()
{
    QNetworkAccessManager   *Manager = new QNetworkAccessManager( this );

    QNetworkReply         *NetRep = Manager->get( QNetworkRequest( m_Url ) );

    if( !NetRep )
    {
        return( false );
    }

    qDebug() << NetRep->isRunning() << NetRep->errorString();

    if( !NetRep->isRunning() )
    {
        return( false );
    }

    QObject::connect( NetRep, &QIODevice::readyRead, NetRep, [&]( void )
    {
        emit status( "readyRead" );

        if( !m_TempFile.isOpen() )
        {
            if( !m_TempFile.open() )
            {
                NetRep->abort();

                return;
            }

            qDebug() << m_TempFile.fileName();
        }

        m_TempFile.write( NetRep->readAll() );
    } );

    QObject::connect( NetRep, &QNetworkReply::finished, NetRep, [&]( void )
    {
        if( m_TempFile.isOpen() )
        {
            m_TempFile.close();

            QFile::copy( m_TempFile.fileName(), m_DestName );
        }

        m_Loop.quit();
    } );

    QObject::connect( NetRep, &QNetworkReply::downloadProgress, [&]( qint64 bytesReceived, qint64 bytesTotal )
    {
        emit downloadProgress( bytesReceived, bytesTotal );
    } );

    QObject::connect( NetRep, &QNetworkReply::errorOccurred, [&]( QNetworkReply::NetworkError pNetworkError )
    {
        qDebug() << pNetworkError;
    } );

    QObject::connect( NetRep, &QNetworkReply::sslErrors, [&]( const QList<QSslError> &pErrors )
    {
    } );

    m_Loop.exec();

    Manager->deleteLater();

    return( true );
}

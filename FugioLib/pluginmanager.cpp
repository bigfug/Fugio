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
#include <QJsonArray>

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

PluginRepoManifest::PluginRepoManifest( const QString &pFileName, const QString &pPlatform )
{
    QFile   RepoFile( pFileName );

    if( !RepoFile.open( QFile::ReadOnly ) )
    {
        return;
    }

    QJsonParseError	JsonError;

    QJsonDocument	JSON = QJsonDocument::fromJson( RepoFile.readAll(), &JsonError );

    if( JSON.isNull() )
    {
        return;
    }

    QJsonObject      PluginArray = JSON[ "plugins" ].toObject();

    for( const QString &PluginKey : PluginArray.keys() )
    {
        QJsonObject VersionObject = PluginArray.value( PluginKey ).toObject();

        for( const QString &PluginVersionKey : VersionObject.keys() )
        {
			PluginRepoManifest::PluginEntry		CurrPlug;

			CurrPlug.first = QVersionNumber::fromString( PluginVersionKey );

            if( !CurrPlug.first.isNull() )
            {
                QJsonObject PlatformObject = VersionObject.value( PluginVersionKey ).toObject();

                for( const QString &PluginPlatformKey : PlatformObject.keys() )
                {
					CurrPlug.second = QUrl( PlatformObject.value( PluginPlatformKey ).toString() );

                    if( pPlatform == PluginPlatformKey )
                    {
                        qDebug() << PluginPlatformKey << CurrPlug.second;

                        QVersionNumber  PlugLatestVersion = m_PluginLatestMap.value( PluginKey );

                        if( PlugLatestVersion.isNull() || PlugLatestVersion > CurrPlug.first )
                        {
                            m_PluginLatestMap.insert( PluginKey, CurrPlug.first );
						}

						m_PluginVersionMap.insert( PluginKey, CurrPlug );
                    }
                }
            }
        }
    }
}

PluginConfig::PluginConfig(const QString &pFileName)
    : m_Config( pFileName, QSettings::IniFormat )
{

}

QVersionNumber PluginConfig::installedPluginVersion( const QString &pPluginName ) const
{
	QString		ConfigKey = QString( "installed/%1/version" ).arg( pPluginName );

	return( QVersionNumber::fromString( m_Config.value( ConfigKey ).toString() ) );
}

void PluginConfig::setInstalledPluginVersion( const QString &pPluginName, const QVersionNumber &pPluginVersion)
{
    m_Config.beginGroup( "installed" );

    if( pPluginVersion.isNull() )
    {
        m_Config.remove( pPluginName );
    }
    else
    {
		m_Config.beginGroup( pPluginName );

        m_Config.setValue( "version", pPluginVersion.toString() );

		m_Config.endGroup();
    }

    m_Config.endGroup();
}

void PluginCache::setCachedPluginFilename( const QString &pPluginName, const QVersionNumber &pPluginVersion, const QString &pFileName )
{
	QString		ConfigKey = QString( "cached/%1/%2" ).arg( pPluginName, pPluginVersion.toString() );

	if( pFileName.isEmpty() )
	{
		m_Config->remove( ConfigKey );
	}
	else
	{
		m_Config->setValue( ConfigKey, pFileName );
	}

	m_Config->endGroup();
}

void PluginCache::addRepo( const QString &pRepoName, const QUrl &pRepoUrl )
{
	QString     RepoKey = QString( "repositories/%1" ).arg( pRepoName );

	m_Config->setValue( RepoKey, pRepoUrl.toString() );
}

void PluginCache::removeRepo( const QString &pRepoName )
{
	QString     RepoKey = QString( "repositories/%1" ).arg( pRepoName );

	m_Config->remove( RepoKey );
}

QUrl PluginCache::repoUrl( const QString &pRepoName ) const
{
	QString     RepoKey = QString( "repositories/%1" ).arg( pRepoName );

	return( m_Config->value( RepoKey ).toUrl() );
}

QStringList PluginConfig::installedPlugins()
{
	QStringList		PluginNames;

	m_Config.beginGroup( "installed" );

	PluginNames = m_Config.childKeys();

	m_Config.endGroup();

	return( PluginNames );
}

bool PluginActionRemove::action()
{
    QDir    DestBase( m_DestName );

    struct zip_t *zfh = zip_open( qPrintable( m_FileName ), 0, 'r' );

    if( !zfh )
    {
        emit error( tr( "Couldn't open zip file" ) );

        return( false );
    }

    const int PluginEntries = zip_total_entries( zfh );

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

                    if( DestFile.exists() )
                    {
                        if( !DestFile.remove() )
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

PluginArchive::PluginArchive(const QString &pFileName)
	: m_FileName( pFileName )
{
	const bool verbose = true;

	struct zip_t *zfh = zip_open( qPrintable( pFileName ), 0, 'r' );

	if( !zfh )
	{
		fputs( "Couldn't open zip file", stderr );

		return;
	}

	const int PluginEntries = zip_total_entries( zfh );

	if( verbose )
	{
		printf( "Files in plugin zip archive: %d\n", PluginEntries );
	}

	if( zip_entry_open( zfh, "manifest.json" ) )
	{
		fputs( "error finding manifest.json in archive", stderr );

		zip_close( zfh );

		return;
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

		zip_close( zfh );

		return;
	}

	if( verbose )
	{
		fwrite( ManifestData.constData(), ManifestRead, 1, stdout );
	}

	zip_entry_close( zfh );

	zip_close( zfh );

	// we have the raw manifest file in memory - parse it

	QJsonParseError	JsonError;

	m_Manifest = QJsonDocument::fromJson( ManifestData, &JsonError );
}

bool PluginArchive::moveArchive( const QString &pDestName )
{
	if( QFile::rename( m_FileName, pDestName ) )
	{
		m_FileName = pDestName;
	}

	return( false );
}

bool PluginArchive::isManifestValid() const
{
	return( !m_Manifest.isNull() );
}

PluginCache::PluginCache( void )
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

	if( !mPluginConfigDir.exists( "repo-cache" ) )
	{
		if( !mPluginConfigDir.mkdir( "repo-cache" ) )
		{
			exit( 1 );
		}
	}

	mRepoCacheDir = QDir( mPluginConfigDir.absoluteFilePath( "repo-cache" ) );

	m_Config = new QSettings( mPluginConfigDir.absoluteFilePath( "plugin-config.ini" ), QSettings::IniFormat );
}

PluginCache::~PluginCache()
{
	if( m_Config )
	{
		delete m_Config;

		m_Config = Q_NULLPTR;
	}
}

QStringList PluginCache::repoNames()
{
	QStringList     RepoNames;

	m_Config->beginGroup( "repositories" );

	RepoNames = m_Config->childKeys();

	m_Config->endGroup();

	return( RepoNames );
}

QString PluginCache::cachedPluginFilename(const QString &pPluginName, const QVersionNumber &pPluginVersion) const
{
	QString		ConfigKey = QString( "cached/%1/%2" ).arg( pPluginName, pPluginVersion.toString() );

	return( m_Config->value( ConfigKey ).toString() );
}

QDir PluginCache::pluginCacheDirectory() const
{
	return mPluginCacheDir;
}

QString PluginCache::pluginConfigFilename() const
{
	return( m_Config ? m_Config->fileName() : Q_NULLPTR );
}

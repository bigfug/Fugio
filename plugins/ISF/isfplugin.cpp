#include "isfplugin.h"

#include <QtPlugin>

#include <QDebug>

#include <QTranslator>
#include <QApplication>

#include <QJsonDocument>

#include <QCryptographicHash>

#include <fugio/isf/uuid.h>
#include <fugio/nodecontrolbase.h>

#include <fugio/opengl/uuid.h>

#include "isfnode.h"

QList<QUuid>				NodeControlBase::PID_UUID;

ISFPlugin					*ISFPlugin::mInstance = nullptr;

ClassEntry		ISFPlugin::mNodeClasses[] =
{
	ClassEntry( "ISF", "OpenGL", NID_ISF, &ISFNode::staticMetaObject ),
	ClassEntry()
};

ClassEntry		ISFPlugin::mPinClasses[] =
{
	ClassEntry()
};

ISFPlugin::ISFPlugin( void )
	: mApp( 0 )
{
	mInstance = this;

	//-------------------------------------------------------------------------
	// Install translator

	static QTranslator		Translator;

	if( Translator.load( QLocale(), QLatin1String( "fugio_isf" ), QLatin1String( "_" ), ":/translations" ) )
	{
		qApp->installTranslator( &Translator );
	}
}

bool ISFPlugin::hasContext()
{
	InterfaceOpenGL		*OGL = qobject_cast<InterfaceOpenGL *>( mApp->findInterface( IID_OPENGL ) );

	if( !OGL || !OGL->hasContext() )
	{
		return( false );
	}

#if defined( GLEW_SUPPORTED )
	if( glewExperimental == GL_FALSE )
	{
		glewExperimental = GL_TRUE;

		if( glewInit() != GLEW_OK )
		{
			return( false );
		}
	}
#endif

	return( true );
}

bool ISFPlugin::hasContextStatic()
{
	return( instance()->hasContext() );
}

PluginInterface::InitResult ISFPlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
{
	if( !pApp->findInterface( IID_OPENGL ) )
	{
		return( pLastChance ? INIT_FAILED : INIT_DEFER );
	}

	mApp = pApp;

	mApp->registerNodeClasses( mNodeClasses );

	mApp->registerPinClasses( mPinClasses );

	QDir	ISFDir = QDir( qApp->applicationDirPath() );

	if( ISFDir.cd( "share/isf" ) )
	{
		scanDirectory( mPluginClassEntry, ISFDir );
	}

	mApp->registerNodeClasses( mPluginClassEntry );

	return( INIT_OK );
}

void ISFPlugin::deinitialise()
{
	mApp->unregisterNodeClasses( mPluginClassEntry );

	mApp->unregisterPinClasses( mPinClasses );

	mApp->unregisterNodeClasses( mNodeClasses );

	mApp = 0;
}

void ISFPlugin::scanDirectory( ClassEntryList &pEntLst, QDir pDir, QStringList pPath )
{
	for( QFileInfo FI : pDir.entryInfoList( QStringList( "*.fs" ), QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot | QDir::Readable ) )
	{
		if( FI.isDir() )
		{
			QStringList	PL = pPath;

			PL.append( FI.baseName() );

			scanDirectory( pEntLst, QDir( FI.absoluteDir() ), PL );
		}
		else
		{
			QFile		ISFFile( FI.absoluteFilePath() );

			if( !ISFFile.open( QFile::ReadOnly ) )
			{
				continue;
			}

			QByteArray	ISFSrc = ISFFile.readAll();

			int			CommentStart = ISFSrc.indexOf( "/*" );
			int			CommentEnd   = ISFSrc.indexOf( "*/", CommentStart );

			if( CommentStart == -1 || CommentEnd == -1 )
			{
				continue;
			}

			QByteArray		Comment = ISFSrc.mid( CommentStart + 2, CommentEnd - CommentStart - 2 );

			QJsonParseError	JERR;

			QJsonDocument	JSON = QJsonDocument::fromJson( Comment, &JERR );

			if( JSON.isNull() )
			{
				continue;
			}

			ClassEntry		ISFEnt;

			ISFEnt.mName = pPath.join( '/' ).append( FI.baseName() );
			ISFEnt.mGroup = "ISF";
			ISFEnt.mMetaObject = &ISFNode::staticMetaObject;

			QByteArray		Hash = QCryptographicHash::hash( ISFEnt.mName.toLatin1(), QCryptographicHash::Md5 );

			ISFEnt.mUuid = QUuid::fromRfc4122( Hash.left( 16 ) );

			pEntLst << ISFEnt;

			mPluginUuid.insert( ISFEnt.mUuid, FI.absoluteFilePath() );
		}
	}
}

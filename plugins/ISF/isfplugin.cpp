#include "isfplugin.h"

#include <QtPlugin>
#include <QSettings>

#include <QDebug>

#include <QTranslator>
#include <QApplication>

#include <QJsonDocument>

#include <QCryptographicHash>

#include <fugio/isf/uuid.h>
#include <fugio/nodecontrolbase.h>

#include <fugio/opengl/uuid.h>

#include <fugio/editor_interface.h>

#include "isfnode.h"

#include "settingsform.h"

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

	if( Translator.load( QLocale(), QLatin1String( "translations" ), QLatin1String( "_" ), ":/" ) )
	{
		qApp->installTranslator( &Translator );
	}
}

bool ISFPlugin::hasContext()
{
	InterfaceOpenGL		*OGL = qobject_cast<InterfaceOpenGL *>( mApp->findInterface( IID_OPENGL ) );

	return( OGL && OGL->hasContext() );
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
		scanDirectory( mSharedClassEntry, ISFDir, mPluginUuid );
	}

	mApp->registerNodeClasses( mSharedClassEntry );

	QString		P = QSettings().value( "isf/path" ).toString();

	if( !P.isEmpty() )
	{
		scanDirectory( mPluginClassEntry, P, mPluginUuid );
	}

	mApp->registerNodeClasses( mPluginClassEntry );

	fugio::EditorInterface	*EI = qobject_cast<fugio::EditorInterface *>( mApp->findInterface( IID_EDITOR ) );

	if( EI )
	{
		EI->registerSettings( this );
	}

	return( INIT_OK );
}

void ISFPlugin::deinitialise()
{
	mApp->unregisterNodeClasses( mPluginClassEntry );

	mApp->unregisterNodeClasses( mSharedClassEntry );

	mApp->unregisterPinClasses( mPinClasses );

	mApp->unregisterNodeClasses( mNodeClasses );

	mApp = 0;
}

void ISFPlugin::scanDirectory( ClassEntryList &pEntLst, QDir pDir, QMap<QUuid, QString> &pUuidList, QStringList pPath )
{
	for( QFileInfo FI : pDir.entryInfoList( QStringList( "*.fs" ), QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot | QDir::Readable ) )
	{
		if( FI.isDir() )
		{
			QStringList	PL = pPath;

			PL.append( FI.baseName() );

			scanDirectory( pEntLst, QDir( FI.absoluteDir() ), pUuidList, PL );
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

			pUuidList.insert( ISFEnt.mUuid, FI.absoluteFilePath() );
		}
	}
}

QWidget *ISFPlugin::settingsWidget()
{
	SettingsForm	*W = new SettingsForm();

	if( W )
	{
		W->setObjectName( "ISF" );

		W->setPath( QSettings().value( "isf/path" ).toString() );
	}

	return( W );
}

void ISFPlugin::settingsAccept( QWidget *S )
{
	SettingsForm	*W = qobject_cast<SettingsForm *>( S );

	if( W && !W->path().isEmpty() )
	{
		QSettings().setValue( "isf/path", W->path() );

		mApp->unregisterNodeClasses( mPluginClassEntry );

		mPluginClassEntry.clear();

		scanDirectory( mPluginClassEntry, W->path(), mPluginUuid );
	}
}

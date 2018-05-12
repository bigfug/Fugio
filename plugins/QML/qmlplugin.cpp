#include "qmlplugin.h"

#include <fugio/text/syntax_error_interface.h>

#include <fugio/text/syntax_highlighter_interface.h>

#include <fugio/qml/uuid.h>

#include "qmlinterfacenode.h"

#include "syntaxhighlighterqml.h"

#include <QtQml>

#include "qmlnode.h"

QMLPlugin		*QMLPlugin::mInstance = 0;

QList<QUuid>				NodeControlBase::PID_UUID;

ClassEntry		QMLPlugin::mNodeClasses[] =
{
	ClassEntry( "QML Interface", "QML", NID_QML_INTERFACE, &QMLInterfaceNode::staticMetaObject ),
	ClassEntry()
};

ClassEntry		QMLPlugin::mPinClasses[] =
{
	ClassEntry()
};

QMLPlugin::QMLPlugin()
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

PluginInterface::InitResult QMLPlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
{
	fugio::SyntaxHighlighterInterface	*SyntaxHighlighter = qobject_cast<fugio::SyntaxHighlighterInterface *>( pApp->findInterface( IID_SYNTAX_HIGHLIGHTER ) );

	if( !SyntaxHighlighter && !pLastChance )
	{
		return( INIT_DEFER );
	}

	if( SyntaxHighlighter )
	{
		SyntaxHighlighter->registerSyntaxHighlighter( SYNTAX_HIGHLIGHTER_QML, QStringLiteral( "QML" ), this );
	}

	mApp = pApp;

	mApp->registerNodeClasses( mNodeClasses );

	mApp->registerPinClasses( mPinClasses );

//	qmlRegisterType<QMLNode>( "com.bigfug.fugio", 1, 0, "Node" );

	return( INIT_OK );
}

void QMLPlugin::deinitialise()
{
	mApp->unregisterPinClasses( mPinClasses );

	mApp->unregisterNodeClasses( mNodeClasses );

	fugio::SyntaxHighlighterInterface	*SyntaxHighlighter = qobject_cast<fugio::SyntaxHighlighterInterface *>( mApp->findInterface( IID_SYNTAX_HIGHLIGHTER ) );

	if( SyntaxHighlighter )
	{
		SyntaxHighlighter->unregisterSyntaxHighlighter( SYNTAX_HIGHLIGHTER_QML );
	}

	mApp = 0;
}

SyntaxHighlighterInstanceInterface *QMLPlugin::syntaxHighlighterInstance(QUuid pUuid) const
{
	if( pUuid == SYNTAX_HIGHLIGHTER_QML )
	{
		return( new SyntaxHighlighterQML( QMLPlugin::instance() ) );
	}

	return( nullptr );
}

#include "textplugin.h"

#include <QTranslator>
#include <QCoreApplication>

#include <fugio/text/uuid.h>

#include "texteditornode.h"
#include "regexpnode.h"
#include "numbertostringnode.h"
#include "stringjoinnode.h"
#include "linebuffernode.h"
#include "texteditorremotenode.h"
#include "stringsplitnode.h"
#include "stringtofloatnode.h"
#include "stringtointegernode.h"
#include "midnode.h"
#include "leftnode.h"
#include "rightnode.h"
#include "containsnode.h"
#include "chopnode.h"

#include "syntaxerrorpin.h"

#include <fugio/text/syntax_highlighter_factory_interface.h>

QList<QUuid>				NodeControlBase::PID_UUID;

fugio::GlobalInterface	*TextPlugin::mApp = 0;
TextPlugin				*TextPlugin::mInstance = 0;

using namespace fugio;

ClassEntry		mNodeClasses[] =
{
	ClassEntry( "Chop",					"String", NID_CHOP, &ChopNode::staticMetaObject ),
	ClassEntry( "Contains",				"String", NID_CONTAINS, &ContainsNode::staticMetaObject ),
	ClassEntry( "Number To String",		"String", NID_NUMBER_TO_STRING, &NumberToStringNode::staticMetaObject ),
	ClassEntry( "RegExp",				"String", NID_REGEXP, &RegExpNode::staticMetaObject ),
	ClassEntry( "Join",					"String", NID_STRING_JOIN, &StringJoinNode::staticMetaObject ),
	ClassEntry( "Split",				"String", NID_STRING_SPLIT, &StringSplitNode::staticMetaObject ),
	ClassEntry( "Left",					"String", NID_LEFT, &LeftNode::staticMetaObject ),
	ClassEntry( "Right",				"String", NID_RIGHT, &RightNode::staticMetaObject ),
	ClassEntry( "Mid",					"String", NID_MID, &MidNode::staticMetaObject ),
	ClassEntry( "Line Buffer",			"String", NID_LINE_BUFFER, &LineBufferNode::staticMetaObject ),
	ClassEntry( "String To Integer",	"String", NID_STRING_TO_INTEGER, &StringToIntegerNode::staticMetaObject ),
	ClassEntry( "String To Float",		"String", NID_STRING_TO_FLOAT, &StringToFloatNode::staticMetaObject ),
	ClassEntry( "Text Editor",			"GUI", NID_TEXT_EDIT, &TextEditorNode::staticMetaObject ),
	ClassEntry( "Text Editor Remote",	"Network", NID_TEXT_EDIT_REMOTE, &TextEditorRemoteNode::staticMetaObject ),
	ClassEntry()
};

ClassEntry		mPinClasses[] =
{
	ClassEntry( "Syntax Error",		"Text", PID_SYNTAX_ERROR, &SyntaxErrorPin::staticMetaObject ),
	ClassEntry()
};

TextPlugin::TextPlugin()
{
	mInstance = this;

	//-------------------------------------------------------------------------
	// Install translator

	static QTranslator		Translator;

	if( Translator.load( QLocale(), QLatin1String( "translations" ), QLatin1String( "_" ), ":/" ) )
	{
		qApp->installTranslator( &Translator );
	}

	//-------------------------------------------------------------------------

	SyntaxErrorPin::registerMetaType();
}

PluginInterface::InitResult TextPlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
{
	Q_UNUSED( pLastChance )

	mApp = pApp;

	mApp->registerNodeClasses( mNodeClasses );

	mApp->registerPinClasses( mPinClasses );

	mApp->registerInterface( IID_SYNTAX_HIGHLIGHTER, this );

	return( INIT_OK );
}

void TextPlugin::deinitialise()
{
	mApp->unregisterInterface( IID_SYNTAX_HIGHLIGHTER );

	mApp->unregisterPinClasses( mPinClasses );

	mApp->unregisterNodeClasses( mNodeClasses );

	mApp = 0;
}

void TextPlugin::registerSyntaxHighlighter( const QUuid &pUuid, const QString &pName, SyntaxHighlighterFactoryInterface *pFactory )
{
	mSyntaxHighlighterFactories.insert( pUuid, SyntaxHighlighterPair( pName, pFactory ) );
}

void TextPlugin::unregisterSyntaxHighlighter( const QUuid &pUuid )
{
	mSyntaxHighlighterFactories.remove( pUuid );
}

SyntaxHighlighterFactoryInterface *TextPlugin::syntaxHighlighterFactory( const QUuid &pUuid ) const
{
	return( mSyntaxHighlighterFactories.value( pUuid ).second );
}

QList<SyntaxHighlighterInterface::SyntaxHighlighterIdentity> TextPlugin::syntaxHighlighters() const
{
	QList<SyntaxHighlighterInterface::SyntaxHighlighterIdentity>	L;

	for( QMap<QUuid,SyntaxHighlighterPair>::const_iterator it = mSyntaxHighlighterFactories.begin() ; it != mSyntaxHighlighterFactories.end() ; it++ )
	{
		L.append( SyntaxHighlighterInterface::SyntaxHighlighterIdentity( it.key(), it.value().first ) );
	}

	return( L );
}

SyntaxHighlighterInstanceInterface *TextPlugin::syntaxHighlighterInstance( const QUuid &pUuid ) const
{
	SyntaxHighlighterFactoryInterface	*Factory = syntaxHighlighterFactory( pUuid );

	return( Factory ? Factory->syntaxHighlighterInstance( pUuid ) : nullptr );
}

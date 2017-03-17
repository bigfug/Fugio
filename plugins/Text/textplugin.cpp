#include "textplugin.h"

#include <QTranslator>
#include <QCoreApplication>

#include <fugio/text/uuid.h>

#include "texteditornode.h"
#include "regexpnode.h"
#include "numbertostringnode.h"
#include "stringjoinnode.h"
#include "linebuffernode.h"

QList<QUuid>				NodeControlBase::PID_UUID;

fugio::GlobalInterface	*TextPlugin::mApp = 0;

using namespace fugio;

ClassEntry		mNodeClasses[] =
{
	ClassEntry( "Number To String",	"String", NID_NUMBER_TO_STRING, &NumberToStringNode::staticMetaObject ),
	ClassEntry( "RegExp",			"String", NID_REGEXP, &RegExpNode::staticMetaObject ),
	ClassEntry( "Join",				"String", NID_STRING_JOIN, &StringJoinNode::staticMetaObject ),
	ClassEntry( "Line Buffer",		"String", NID_LINE_BUFFER, &LineBufferNode::staticMetaObject ),
	ClassEntry( "Text Editor",		"GUI", NID_TEXT_EDIT, &TextEditorNode::staticMetaObject ),
	ClassEntry()
};

ClassEntry		mPinClasses[] =
{
	ClassEntry()
};

TextPlugin::TextPlugin()
{
	//-------------------------------------------------------------------------
	// Install translator

	static QTranslator		Translator;

	if( Translator.load( QLocale(), QLatin1String( "fugio_text" ), QLatin1String( "_" ), ":/translations" ) )
	{
		qApp->installTranslator( &Translator );
	}
}

PluginInterface::InitResult TextPlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
{
	Q_UNUSED( pLastChance )

	mApp = pApp;

	mApp->registerNodeClasses( mNodeClasses );

	mApp->registerPinClasses( mPinClasses );

	return( INIT_OK );
}

void TextPlugin::deinitialise()
{
	mApp->unregisterPinClasses( mPinClasses );

	mApp->unregisterNodeClasses( mNodeClasses );

	mApp = 0;
}

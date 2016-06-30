#include "textplugin.h"

#include <fugio/text/uuid.h>

#include "texteditornode.h"
#include "regexpnode.h"
#include "numbertostringnode.h"
#include "stringjoinnode.h"

QList<QUuid>				NodeControlBase::PID_UUID;

using namespace fugio;

ClassEntry		mNodeClasses[] =
{
	ClassEntry( "Number To String",	"String", NID_NUMBER_TO_STRING, &NumberToStringNode::staticMetaObject ),
	ClassEntry( "RegExp",			"String", NID_REGEXP, &RegExpNode::staticMetaObject ),
	ClassEntry( "Join",				"String", NID_STRING_JOIN, &StringJoinNode::staticMetaObject ),
	ClassEntry( "Text Editor",		"GUI", NID_TEXT_EDIT, &TextEditorNode::staticMetaObject ),
	ClassEntry()
};

ClassEntry		mPinClasses[] =
{
	ClassEntry()
};

TextPlugin::TextPlugin()
	: mApp( 0 )
{

}

PluginInterface::InitResult TextPlugin::initialise( fugio::GlobalInterface *pApp )
{
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

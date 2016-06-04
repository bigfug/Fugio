#include "coreplugin.h"

#include <QtPlugin>

#include <QDebug>

#include "fugio/core/uuid.h"

#include "numberrangenode.h"
#include "randomnumbernode.h"
#include "anytriggernode.h"
#include "ratecontrolnode.h"
#include "numberrangelimitnode.h"
#include "boolrangenode.h"
#include "togglenode.h"
#include "triggerrangenumbernode.h"
#include "outputrangenode.h"
#include "splitsizenode.h"
#include "joinsizenode.h"
#include "splitlistnode.h"
#include "fadenode.h"
#include "indexnode.h"
#include "booltotriggernode.h"
#include "signalnumbernode.h"
#include "envelopenode.h"
#include "numberspreadnode.h"
#include "splitpointnode.h"
#include "duplicatepinsnode.h"
#include "lastupdatedinputnode.h"
#include "counternode.h"
#include "listsizenode.h"
#include "listindexnode.h"
#include "framedelaynode.h"
#include "typesizenode.h"

#include "floatpin.h"
#include "integerpin.h"
#include "stringpin.h"
#include "triggerpin.h"
#include "boolpin.h"
#include "bytearraypin.h"
#include "sizepin.h"
#include "arraypin.h"
#include "pointpin.h"
#include "size3dpin.h"
#include "listpin.h"
#include "choicepin.h"
#include "variantpin.h"
#include "stringlistpin.h"

QList<QUuid>				NodeControlBase::PID_UUID;

ClassEntry		CorePlugin::mNodeClasses[] =
{
	ClassEntry( QT_TR_NOOP( "Any Trigger" ), NID_ANY_TRIGGER, &AnyTriggerNode::staticMetaObject ),
	ClassEntry( "Bool Range", NID_BOOL_RANGE, &BoolRangeNode::staticMetaObject ),
	ClassEntry( "Bool To Trigger", NID_BOOL_TO_TRIGGER, &BoolToTriggerNode::staticMetaObject ),
	ClassEntry( "Counter", NID_COUNTER, &CounterNode::staticMetaObject ),
	ClassEntry( "Duplicate Pins", "Fugio", NID_DUPLICATE_PINS, &DuplicatePinsNode::staticMetaObject ),
	ClassEntry( "Envelope", "Number", NID_ENVELOPE, &EnvelopeNode::staticMetaObject ),
	ClassEntry( "Frame Delay", "Context", NID_FRAME_DELAY, &FrameDelayNode::staticMetaObject ),
	ClassEntry( "Smooth", "Number", ClassEntry::None, NID_FADE, &FadeNode::staticMetaObject ),
	ClassEntry( "Join Size", "Size", NID_JOIN_SIZE, &JoinSizeNode::staticMetaObject ),
	ClassEntry( "Last Updated Input", NID_LAST_UPDATED_INPUT, &LastUpdatedInputNode::staticMetaObject ),
	ClassEntry( "Index", NID_INDEX, &IndexNode::staticMetaObject ),
	ClassEntry( "List Size", NID_LIST_SIZE, &ListSizeNode::staticMetaObject ),
	ClassEntry( "List Index", NID_LIST_INDEX, &ListIndexNode::staticMetaObject ),
	ClassEntry( "Number Range Limit", NID_NUMBER_RANGE_LIMIT, &NumberRangeLimitNode::staticMetaObject ),
	ClassEntry( "Number Range Map", NID_NUMBER_RANGE, &NumberRangeNode::staticMetaObject ),
	ClassEntry( "Number Spread", NID_NUMBER_SPREAD, &NumberSpreadNode::staticMetaObject ),
	ClassEntry( "Output Range", NID_OUTPUT_RANGE, &OutputRangeNode::staticMetaObject ),
	ClassEntry( "Random", "Number", NID_RANDOM_NUMBER, &RandomNumberNode::staticMetaObject ),
	ClassEntry( "Rate Control", NID_RATE_CONTROL, &RateControlNode::staticMetaObject ),
	ClassEntry( "Signal", "Number", NID_SIGNAL_NUMBER, &SignalNumberNode::staticMetaObject ),
	ClassEntry( "Toggle", NID_TOGGLE, &ToggleNode::staticMetaObject ),
	ClassEntry( "Trigger Range Number", NID_TRIGGER_RANGE_NUMBER, &TriggerRangeNumberNode::staticMetaObject ),
	ClassEntry( "Type Size", NID_TYPE_SIZE, &TypeSizeNode::staticMetaObject ),
	ClassEntry( "Split List", NID_SPLIT_LIST, &SplitListNode::staticMetaObject ),
	ClassEntry( "Split Point", NID_SPLIT_POINT, &SplitPointNode::staticMetaObject ),
	ClassEntry( "Split Size", NID_SPLIT_SIZE, &SplitSizeNode::staticMetaObject ),
	ClassEntry()
};

ClassEntry		CorePlugin::mPinClasses[] =
{
	ClassEntry( "Array", PID_ARRAY, &ArrayPin::staticMetaObject ),
	ClassEntry( "Bool", PID_BOOL, &BoolPin::staticMetaObject ),
	ClassEntry( "Byte Array", PID_BYTEARRAY, &ByteArrayPin::staticMetaObject ),
	ClassEntry( "Choice", PID_CHOICE, &ChoicePin::staticMetaObject ),
	ClassEntry( "Float", PID_FLOAT, &FloatPin::staticMetaObject ),
	ClassEntry( "Integer", PID_INTEGER, &IntegerPin::staticMetaObject ),
	ClassEntry( "List", PID_LIST, &ListPin::staticMetaObject ),
	ClassEntry( "Point", PID_POINT, &PointPin::staticMetaObject ),
	ClassEntry( "Size", PID_SIZE, &SizePin::staticMetaObject ),
	ClassEntry( "Size3d", PID_SIZE_3D, &Size3dPin::staticMetaObject ),
	ClassEntry( "String", PID_STRING, &StringPin::staticMetaObject ),
	ClassEntry( "String List", PID_STRING_LIST, &StringListPin::staticMetaObject ),
	ClassEntry( "Trigger", PID_TRIGGER, &TriggerPin::staticMetaObject ),
	ClassEntry( "Variant", PID_VARIANT, &VariantPin::staticMetaObject ),
	ClassEntry()
};

CorePlugin::CorePlugin( void )
	: mApp( 0 )
{
}

CorePlugin::~CorePlugin( void )
{
}

PluginInterface::InitResult CorePlugin::initialise( fugio::GlobalInterface *pApp )
{
	mApp = pApp;

	//mApp->registerVideoOutputFactory( this );

	mApp->registerNodeClasses( mNodeClasses );

	mApp->registerPinClasses( mPinClasses );

	mApp->registerPinSplitter( PID_SIZE, NID_SPLIT_SIZE );
	//mApp->registerPinSplitter( PID_SIZE_3D, NID_SPLIT_SIZE );
	mApp->registerPinSplitter( PID_LIST, NID_SPLIT_LIST );
	mApp->registerPinSplitter( PID_POINT, NID_SPLIT_POINT );

	mApp->registerPinJoiner( PID_SIZE, NID_JOIN_SIZE );
	//mApp->registerPinJoiner( PID_SIZE_3D, NID_JOIN_SIZE );

	return( INIT_OK );
}

void CorePlugin::deinitialise()
{
	mApp->unregisterPinClasses( mPinClasses );

	mApp->unregisterNodeClasses( mNodeClasses );

	mApp = 0;
}

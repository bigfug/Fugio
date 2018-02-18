#include "coreplugin.h"

#include <QtPlugin>

#include <QDebug>
#include <QTranslator>
#include <QDir>
#include <QApplication>

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
#include "smoothnode.h"
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
#include "switchnode.h"
#include "floatthresholdnode.h"
#include "autorangenode.h"
#include "getsizenode.h"
#include "triggerarraynode.h"
#include "triggerbooleannode.h"
#include "makearraynode.h"
#include "flipflopnode.h"
#include "bitstoboolnode.h"
#include "booltobitsnode.h"
#include "joinpointnode.h"
#include "joinrectnode.h"
#include "splitrectnode.h"
#include "integertofloatnode.h"
#include "blockupdatesnode.h"
#include "processclicknode.h"

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
#include "rectpin.h"
#include "bitarraypin.h"
#include "linepin.h"
#include "transformpin.h"
#include "polygonpin.h"

#include "loggernode.h"

QList<QUuid>				 NodeControlBase::PID_UUID;
CorePlugin					*CorePlugin::mInstance = Q_NULLPTR;

ClassEntry		CorePlugin::mNodeClasses[] =
{
	ClassEntry( QT_TR_NOOP( "Any Trigger" ), NID_ANY_TRIGGER, &AnyTriggerNode::staticMetaObject ),
	ClassEntry( "Auto Number Range", NID_AUTO_NUMBER_RANGE, &AutoRangeNode::staticMetaObject ),
	ClassEntry( "Bits To Bool", NID_BITS_TO_BOOL, &BitsToBoolNode::staticMetaObject ),
	ClassEntry( "Bool To Bits", NID_BOOL_TO_BITS, &BoolToBits::staticMetaObject ),
	ClassEntry( "Bool Range", NID_BOOL_RANGE, &BoolRangeNode::staticMetaObject ),
	ClassEntry( "Bool To Trigger", NID_BOOL_TO_TRIGGER, &BoolToTriggerNode::staticMetaObject ),
	ClassEntry( "Counter", NID_COUNTER, &CounterNode::staticMetaObject ),
	ClassEntry( "Duplicate Pins", "Fugio", NID_DUPLICATE_PINS, &DuplicatePinsNode::staticMetaObject ),
	ClassEntry( "Envelope", "Number", NID_ENVELOPE, &EnvelopeNode::staticMetaObject ),
	ClassEntry( "Float Threshold", NID_FLOAT_THRESHOLD, &FloatThresholdNode::staticMetaObject ),
	ClassEntry( "Flip Flop", NID_FLIP_FLOP, &FlipFlopNode::staticMetaObject ),
	ClassEntry( "Frame Delay", "Context", NID_FRAME_DELAY, &FrameDelayNode::staticMetaObject ),
	ClassEntry( "Get Size", "Size", NID_GET_SIZE, &GetSizeNode::staticMetaObject ),
	ClassEntry( "Smooth", "Number", ClassEntry::None, NID_FADE, &SmoothNode::staticMetaObject ),
	ClassEntry( "Join Point", "Point", NID_JOIN_POINT, &JoinPointNode::staticMetaObject ),
	ClassEntry( "Join Size", "Size", NID_JOIN_SIZE, &JoinSizeNode::staticMetaObject ),
	ClassEntry( "Join Rect", "Rect", NID_JOIN_RECT, &JoinRectNode::staticMetaObject ),
	ClassEntry( "Last Updated Input", NID_LAST_UPDATED_INPUT, &LastUpdatedInputNode::staticMetaObject ),
	ClassEntry( "Index", NID_INDEX, &IndexNode::staticMetaObject ),
	ClassEntry( "Integer To Float", NID_INTEGER_TO_FLOAT, &IntegerToFloatNode::staticMetaObject ),
	ClassEntry( "List Size", NID_LIST_SIZE, &ListSizeNode::staticMetaObject ),
	ClassEntry( "List Index", NID_LIST_INDEX, &ListIndexNode::staticMetaObject ),
	ClassEntry( "Logger", NID_LOGGER, &LoggerNode::staticMetaObject ),
	ClassEntry( "Make Array", NID_MAKE_ARRAY, &MakeArrayNode::staticMetaObject ),
	ClassEntry( "Number Range Limit", NID_NUMBER_RANGE_LIMIT, &NumberRangeLimitNode::staticMetaObject ),
	ClassEntry( "Number Range Map", NID_NUMBER_RANGE, &NumberRangeNode::staticMetaObject ),
	ClassEntry( "Number Spread", NID_NUMBER_SPREAD, &NumberSpreadNode::staticMetaObject ),
	ClassEntry( "Output Range", NID_OUTPUT_RANGE, &OutputRangeNode::staticMetaObject ),
	ClassEntry( "Process Click", NID_PROCESS_CLICK, &ProcessClickNode::staticMetaObject ),
	ClassEntry( "Random", "Number", NID_RANDOM_NUMBER, &RandomNumberNode::staticMetaObject ),
	ClassEntry( "Rate Control", NID_RATE_CONTROL, &RateControlNode::staticMetaObject ),
	ClassEntry( "Signal", "Number", NID_SIGNAL_NUMBER, &SignalNumberNode::staticMetaObject ),
	ClassEntry( "Toggle", NID_TOGGLE, &ToggleNode::staticMetaObject ),
	ClassEntry( "Trigger Array", NID_TRIGGER_ARRAY, &TriggerArrayNode::staticMetaObject ),
	ClassEntry( "Trigger Boolean", NID_TRIGGER_BOOLEAN, &TriggerBooleanNode::staticMetaObject ),
	ClassEntry( "Trigger Range Number", NID_TRIGGER_RANGE_NUMBER, &TriggerRangeNumberNode::staticMetaObject ),
	ClassEntry( "Type Size", NID_TYPE_SIZE, &TypeSizeNode::staticMetaObject ),
	ClassEntry( "Split List", NID_SPLIT_LIST, &SplitListNode::staticMetaObject ),
	ClassEntry( "Split Point", NID_SPLIT_POINT, &SplitPointNode::staticMetaObject ),
	ClassEntry( "Split Rect", NID_SPLIT_RECT, &SplitRectNode::staticMetaObject ),
	ClassEntry( "Split Size", NID_SPLIT_SIZE, &SplitSizeNode::staticMetaObject ),
	ClassEntry( "Switch", NID_SWITCH, &SwitchNode::staticMetaObject ),
	ClassEntry( "Block Updates", NID_BLOCK_UPDATES, &BlockUpdatesNode::staticMetaObject ),
	ClassEntry()
};

ClassEntry		CorePlugin::mPinClasses[] =
{
	ClassEntry( "Array", PID_ARRAY, &ArrayPin::staticMetaObject ),
	ClassEntry( "Bit Array", PID_BITARRAY, &BitArrayPin::staticMetaObject ),
	ClassEntry( "Bool", PID_BOOL, &BoolPin::staticMetaObject ),
	ClassEntry( "Byte Array", PID_BYTEARRAY, &ByteArrayPin::staticMetaObject ),
	ClassEntry( "Choice", PID_CHOICE, &ChoicePin::staticMetaObject ),
	ClassEntry( "Float", PID_FLOAT, &FloatPin::staticMetaObject ),
	ClassEntry( "Integer", PID_INTEGER, &IntegerPin::staticMetaObject ),
	ClassEntry( "Line", PID_LINE, &LinePin::staticMetaObject ),
//	ClassEntry( "List", PID_LIST, &ListPin::staticMetaObject ),
	ClassEntry( "Point", PID_POINT, &PointPin::staticMetaObject ),
	ClassEntry( "Polygon", PID_POLYGON, &PolygonPin::staticMetaObject ),
	ClassEntry( "Rect", PID_RECT, &RectPin::staticMetaObject ),
	ClassEntry( "Size", PID_SIZE, &SizePin::staticMetaObject ),
	ClassEntry( "Size3d", PID_SIZE_3D, &Size3dPin::staticMetaObject ),
	ClassEntry( "String", PID_STRING, &StringPin::staticMetaObject ),
	ClassEntry( "Trigger", PID_TRIGGER, &TriggerPin::staticMetaObject ),
	ClassEntry( "Transform", PID_TRANSFORM, &TransformPin::staticMetaObject ),
	ClassEntry( "Variant", PID_VARIANT, &VariantPin::staticMetaObject ),
	ClassEntry()
};

CorePlugin::CorePlugin( void )
	: mApp( 0 )
{
	//-------------------------------------------------------------------------
	// Install translator

	static QTranslator		Translator;

	if( Translator.load( QLocale(), QLatin1String( "translations" ), QLatin1String( "_" ), ":/" ) )
	{
		qApp->installTranslator( &Translator );
	}
}

CorePlugin::~CorePlugin( void )
{
}

PluginInterface::InitResult CorePlugin::initialise( fugio::GlobalInterface *pApp, bool pLastChance )
{
	Q_UNUSED( pLastChance )

	mInstance = this;

	mApp = pApp;

	mApp->registerNodeClasses( mNodeClasses );

	mApp->registerPinClasses( mPinClasses );

	mApp->registerPinSplitter( PID_SIZE, NID_SPLIT_SIZE );
	//mApp->registerPinSplitter( PID_SIZE_3D, NID_SPLIT_SIZE );
	mApp->registerPinSplitter( PID_LIST, NID_SPLIT_LIST );
	mApp->registerPinSplitter( PID_POINT, NID_SPLIT_POINT );
	mApp->registerPinSplitter( PID_RECT, NID_SPLIT_RECT );

	mApp->registerPinJoiner( PID_SIZE, NID_JOIN_SIZE );
	mApp->registerPinJoiner( PID_POINT, NID_JOIN_POINT );
	mApp->registerPinJoiner( PID_RECT, NID_JOIN_RECT );

	mApp->registerPinForMetaType( PID_BOOL, QMetaType::Bool );
	mApp->registerPinForMetaType( PID_STRING, QMetaType::Char );
	mApp->registerPinForMetaType( PID_FLOAT, QMetaType::Double );
	mApp->registerPinForMetaType( PID_FLOAT, QMetaType::Float );
	mApp->registerPinForMetaType( PID_INTEGER, QMetaType::Int );
	mApp->registerPinForMetaType( PID_INTEGER, QMetaType::Long );
	mApp->registerPinForMetaType( PID_INTEGER, QMetaType::LongLong );
	mApp->registerPinForMetaType( PID_INTEGER, QMetaType::Short );
	mApp->registerPinForMetaType( PID_BITARRAY, QMetaType::QBitArray );
	mApp->registerPinForMetaType( PID_BYTEARRAY, QMetaType::QByteArray );
	mApp->registerPinForMetaType( PID_STRING, QMetaType::QChar );
	mApp->registerPinForMetaType( PID_STRING, QMetaType::QString );
	mApp->registerPinForMetaType( PID_POLYGON, QMetaType::QPolygon );
	mApp->registerPinForMetaType( PID_POLYGON, QMetaType::QPolygonF );

	mApp->registerPinForMetaType( PID_POINT, QMetaType::QPoint );
	mApp->registerPinForMetaType( PID_POINT, QMetaType::QPointF );
	mApp->registerPinForMetaType( PID_FLOAT, QMetaType::QReal );
	mApp->registerPinForMetaType( PID_RECT, QMetaType::QRect );
	mApp->registerPinForMetaType( PID_RECT, QMetaType::QRectF );
	mApp->registerPinForMetaType( PID_SIZE, QMetaType::QSize );
	mApp->registerPinForMetaType( PID_SIZE, QMetaType::QSizeF );
	mApp->registerPinForMetaType( PID_TRANSFORM, QMetaType::QTransform );
	mApp->registerPinForMetaType( PID_VARIANT, QMetaType::QVariant );

	return( INIT_OK );
}

void CorePlugin::deinitialise()
{
	mApp->unregisterPinClasses( mPinClasses );

	mApp->unregisterNodeClasses( mNodeClasses );

	mApp = 0;
}

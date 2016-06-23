
#include "splitnode.h"

#include <fugio/core/uuid.h>
#include <fugio/colour/colour_interface.h>
#include <fugio/osc/uuid.h>
#include <fugio/osc/namespace_interface.h>

SplitNode::SplitNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_OSC,	"93DE0697-E71D-4F17-A90B-3A39C66BF7AC" );

	pinInput( "Input", mPinInput, PID_OSC_SPLIT, PIN_INPUT_OSC );

	mPinInput->registerPinInputType( PID_OSC_SPLIT );
}

QList<fugio::NodeControlInterface::AvailablePinEntry> SplitNode::availableOutputPins() const
{
	fugio::osc::NamespaceInterface	*NS = input<fugio::osc::NamespaceInterface *>( mPinInput );

	if( NS )
	{
		QStringList	CurDir;

		oscPath( CurDir );

		return( NS->oscPins( CurDir ) );
	}

	return( QList<fugio::NodeControlInterface::AvailablePinEntry>() );
}

QList<QUuid> SplitNode::pinAddTypesOutput() const
{
	static QList<QUuid>	PinLst =
	{
		PID_BOOL,
		PID_COLOUR,
		PID_STRING,
		PID_FLOAT,
		PID_INTEGER,
		PID_BYTEARRAY,
		PID_OSC_SPLIT,
		PID_LIST
	};

	return( PinLst );
}

QUuid SplitNode::pinAddControlUuid( fugio::PinInterface *pPin ) const
{
	if( pPin->direction() == PIN_OUTPUT )
	{
		return( PID_OSC_SPLIT );
	}

	return( QUuid() );
}

void SplitNode::oscSplit( QStringList pPath, const QVariant &pValue )
{
	QSharedPointer<fugio::PinInterface>		CurPin;

	QStringList		NewLst;

	while( !CurPin && !pPath.isEmpty() )
	{
		QString		CurNam = pPath.join( '/' );

		CurPin = mNode->findOutputPinByName( CurNam );

		if( !CurPin )
		{
			NewLst.push_front( pPath.takeLast() );
		}
	}

	if( !CurPin )
	{
		return;
	}

	if( true )
	{
		fugio::osc::SplitInterface	*II = qobject_cast<fugio::osc::SplitInterface *>( CurPin->hasControl() ? CurPin->control()->qobject() : nullptr );

		if( II )
		{
			II->oscSplit( NewLst, pValue );
		}

		return;
	}

	fugio::VariantInterface	*VI = qobject_cast<fugio::VariantInterface *>( CurPin->hasControl() ? CurPin->control()->qobject() : nullptr );

	if( VI )
	{
		VI->setVariant( pValue );

		pinUpdated( CurPin );
	}
}

void SplitNode::oscPath( QStringList &pPath ) const
{
	fugio::osc::SplitInterface	*II = input<fugio::osc::SplitInterface *>( mPinInput );

	if( II )
	{
		II->oscPath( pPath );

		return;
	}

	II = qobject_cast<fugio::osc::SplitInterface *>( mPinInput->hasControl() ? mPinInput->control()->qobject() : nullptr );

	if( II )
	{
		II->oscPath( pPath );
	}
}

QStringList SplitNode::oscNamespace()
{
	return( QStringList() );
}

QList<fugio::NodeControlInterface::AvailablePinEntry> SplitNode::oscPins( const QStringList &pCurDir ) const
{
	fugio::osc::NamespaceInterface	*NS = input<fugio::osc::NamespaceInterface *>( mPinInput );

	if( NS )
	{
		return( NS->oscPins( pCurDir ) );
	}

	return( QList<fugio::NodeControlInterface::AvailablePinEntry>() );
}

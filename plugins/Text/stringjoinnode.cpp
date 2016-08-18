#include "stringjoinnode.h"

#include <fugio/context_interface.h>

StringJoinNode::StringJoinNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinInputJoinChar = pinInput( "Seperator" );

	mValOutput = pinOutput<fugio::VariantInterface *>( "String", mPinOutput, PID_STRING );
}

void StringJoinNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	if( !pTimeStamp )
	{
		return;
	}

	QStringList		StrLst;
	QString			StrSep;

	for( QSharedPointer<fugio::PinInterface> P : mNode->enumInputPins() )
	{
		if( P->globalId() == mPinInputJoinChar->globalId() )
		{
			continue;
		}

		if( P->controlUuid() == PID_STRING_LIST )
		{
			StrLst << variant( P ).toStringList();
		}
		else
		{
			StrLst << variant( P ).toString();
		}
	}

	StrSep = variant( mPinInputJoinChar ).toString();

	QString		StrRes = StrLst.join( StrSep );

	//qDebug() << StrRes;

	mValOutput->setVariant( StrRes );

	pinUpdated( mPinOutput );
}

QList<QUuid> StringJoinNode::pinAddTypesInput() const
{
	static QList<QUuid> PinLst =
	{
		PID_BOOL,
		PID_BYTEARRAY,
		PID_FLOAT,
		PID_INTEGER,
		PID_STRING,
		PID_STRING_LIST,
		PID_VARIANT
	};

	return( PinLst );
}

bool StringJoinNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_OUTPUT );
}

bool StringJoinNode::pinShouldAutoRename( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_INPUT );
}

#include "stringjoinnode.h"

#include <fugio/context_interface.h>

StringJoinNode::StringJoinNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mPinInputJoinChar = pinInput( "Seperator" );

	mPinOutputValue = pinOutput<fugio::VariantInterface *>( "String", mPinOutput, PID_STRING );
}

void StringJoinNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	QStringList		StrLst;
	QString			StrSep;

	for( QSharedPointer<fugio::PinInterface> P : mNode->enumInputPins() )
	{
		if( P->globalId() == mPinInputJoinChar->globalId() )
		{
			continue;
		}

		StrLst << variant( P ).toString();
	}

	StrSep = variant( mPinInputJoinChar ).toString();

	QString		StrRes = StrLst.join( StrSep );

	//qDebug() << StrRes;

	mPinOutputValue->setVariant( StrRes );

	pinUpdated( mPinOutput );
}

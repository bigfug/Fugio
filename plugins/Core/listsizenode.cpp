#include "listsizenode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/core/list_interface.h>

ListSizeNode::ListSizeNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( ID_LIST,	"80889F86-A9BA-4E9A-9798-E27D1BFA0B7F" );
	FUGID( ID_SIZE,	"4D5C91B1-D0FB-4DA2-875E-54CE85B0ECF2" );

	mPinInputList = pinInput( "List", ID_LIST );

	mOutputSize = pinOutput<fugio::VariantInterface *>( "Size", mPinOutputSize, PID_INTEGER, ID_SIZE );
}

void ListSizeNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	fugio::ListInterface	*LstInf = input<fugio::ListInterface *>( mPinInputList );

	if( LstInf )
	{
		if( mOutputSize->variant().toInt() != LstInf->listSize() )
		{
			mOutputSize->setVariant( LstInf->listSize() );

			pinUpdated( mPinOutputSize );
		}
	}
}

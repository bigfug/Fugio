#include "listindexnode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/core/list_interface.h>

ListIndexNode::ListIndexNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( ID_LIST,		"80889F86-A9BA-4E9A-9798-E27D1BFA0B7F" );
	FUGID( ID_INDEX,	"4D5C91B1-D0FB-4DA2-875E-54CE85B0ECF2" );
	FUGID( ID_VALUE,	"A3AB9EF1-2E28-4CAB-AD21-7952CE48A4C3" );

	mPinInputList = pinInput( "List", ID_LIST );

	mPinInputIndex = pinInput( "Index", ID_INDEX );

	pinOutput<fugio::VariantInterface *>( "Value", mPinOutputValue, PID_VARIANT, ID_VALUE );
}

void ListIndexNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	fugio::ListInterface	*LstInf = input<fugio::ListInterface *>( mPinInputList );

	if( !LstInf )
	{
		return;
	}

	int						 LstIdx = variant( mPinInputIndex ).toInt();

	if( LstIdx < 0 || LstIdx >= LstInf->listSize() )
	{
		return;
	}

	QVariant				 LstVal = LstInf->listIndex( LstIdx );

	fugio::VariantInterface	*VarInf = qobject_cast<fugio::VariantInterface *>( mPinOutputValue->control()->qobject() );

	if( VarInf && VarInf->variant() != LstVal )
	{
		if( mPinOutputValue->controlUuid() != LstInf->listPinControl() )
		{
			mNode->context()->global()->updatePinControl( mPinOutputValue, LstInf->listPinControl() );

			VarInf = qobject_cast<fugio::VariantInterface *>( mPinOutputValue->control()->qobject() );
		}

		if( VarInf )
		{
			VarInf->setVariant( LstVal );

			pinUpdated( mPinOutputValue );
		}
	}
}

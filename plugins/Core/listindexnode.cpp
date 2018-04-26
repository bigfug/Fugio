#include "listindexnode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>
#include <fugio/core/list_interface.h>
#include <fugio/core/array_list_interface.h>
#include <fugio/pin_variant_iterator.h>

ListIndexNode::ListIndexNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( ID_LIST,		"80889F86-A9BA-4E9A-9798-E27D1BFA0B7F" );
	FUGID( ID_INDEX,	"4D5C91B1-D0FB-4DA2-875E-54CE85B0ECF2" );
	FUGID( ID_VALUE,	"A3AB9EF1-2E28-4CAB-AD21-7952CE48A4C3" );

	mPinInputList = pinInput( "List", ID_LIST );

	mPinInputIndex = pinInput( "Index", ID_INDEX );

	mValOutputValue = pinOutput<fugio::VariantInterface *>( "Value", mPinOutputValue, PID_VARIANT, ID_VALUE );
}

void ListIndexNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	const int					 LstIdx = variant<int>( mPinInputIndex );

	if( LstIdx < 0 || LstIdx >= mValOutputValue->variantCount() )
	{
		return;
	}

	fugio::PinVariantIterator	 PinVar( mPinInputList );

	const QVariant				 VarVal = PinVar.index( LstIdx );
	QMetaType::Type				 VarTyp = QMetaType::Type( VarVal.type() );

	if( mPinOutputValue->alwaysUpdate() || mValOutputValue->variantType() != VarTyp || mValOutputValue->variant() != VarVal )
	{
		mValOutputValue->setVariantType( VarTyp );

		mValOutputValue->setVariant( VarVal );

		pinUpdated( mPinOutputValue );
	}

//	fugio::ArrayListInterface	*ArLInf = input<fugio::ArrayListInterface *>( mPinInputList );

//	if( ArLInf )
//	{
//		if( LstIdx < 0 || LstIdx >= ArLInf->count() )
//		{
//			return;
//		}

//		fugio::ArrayInterface	*SrcArr = ArLInf->arrayIndex( LstIdx );

//		fugio::ArrayInterface	*DstInf = qobject_cast<fugio::ArrayInterface *>( mPinOutputValue->control()->qobject() );

//		if( mPinOutputValue->controlUuid() != PID_ARRAY )
//		{
//			mNode->context()->updatePinControl( mPinOutputValue, PID_ARRAY );

//			DstInf = qobject_cast<fugio::ArrayInterface *>( mPinOutputValue->control()->qobject() );
//		}

//		if( DstInf )
//		{
//			DstInf->setArray( SrcArr->array() );
//			DstInf->setCount( SrcArr->count() );
//			DstInf->setSize( SrcArr->size() );
//			DstInf->setStride( SrcArr->stride() );
//			DstInf->setType( SrcArr->type() );

//			pinUpdated( mPinOutputValue );
//		}

//		return;
//	}

//	fugio::ListInterface	*LstInf = input<fugio::ListInterface *>( mPinInputList );

//	if( !LstInf )
//	{
//		return;
//	}

//	if( LstIdx < 0 || LstIdx >= LstInf->listSize() )
//	{
//		return;
//	}

//	QVariant				 LstVal = LstInf->listIndex( LstIdx );

//	if( VarInf && VarInf->variant() != LstVal )
//	{
//		if( mPinOutputValue->controlUuid() != LstInf->listPinControl() )
//		{
//			mNode->context()->updatePinControl( mPinOutputValue, LstInf->listPinControl() );

//			VarInf = qobject_cast<fugio::VariantInterface *>( mPinOutputValue->control()->qobject() );
//		}

//		if( VarInf )
//		{
//			VarInf->setVariant( LstVal );

//			pinUpdated( mPinOutputValue );
//		}
//	}
}

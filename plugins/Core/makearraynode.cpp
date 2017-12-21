#include "makearraynode.h"

#include <QComboBox>
#include <QMatrix4x4>
#include <QSettings>

#include <fugio/core/uuid.h>
#include <fugio/math/uuid.h>
#include <fugio/colour/uuid.h>

#include <fugio/context_interface.h>

MakeArrayNode::MakeArrayNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_FIRST,		"9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_OUTPUT_ARRAY,	"1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );

	mPinInput = pinInput( "Input", PIN_INPUT_FIRST );

	mPinInput->setAutoRename( true );

	mValOutput = pinOutput<fugio::ArrayInterface *>( "Array", mPinOutput, PID_ARRAY, PIN_OUTPUT_ARRAY );
}

void MakeArrayNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	QMetaType::Type		CurrType = QMetaType::Type( variant( mPinInput ).type() );

	if( CurrType == QMetaType::UnknownType )
	{
		return;
	}

	QMetaType	MetaType( CurrType );

	QList<QSharedPointer<fugio::PinInterface>>	PinLst = mNode->enumInputPins();

	if( mValOutput->type() != CurrType )
	{
		mValOutput->setType( CurrType );
		mValOutput->setSize( 1 );

		mValOutput->setStride( MetaType.sizeOf() );
	}

	mValOutput->setCount( PinLst.size() );

	if( PinLst.isEmpty() )
	{
		return;
	}

	quint8		*DstPtr = (quint8 *)mValOutput->array();

	for( QSharedPointer<fugio::PinInterface> P : PinLst )
	{
		QVariant	V = variant( P );

		if( V.type() == CurrType || ( V.canConvert( CurrType ) && V.convert( CurrType ) ) )
		{
			MetaType.construct( DstPtr, V.value<void *>() );
		}

		DstPtr += MetaType.sizeOf();
	}

	pinUpdated( mPinOutput );
}

//QList<QUuid> MakeArrayNode::pinAddTypesInput() const
//{
//	if( mPinInput->isConnected() )
//	{
//		QList<QUuid>	PinLst;

//		switch( mType )
//		{
//			case QMetaType::Int:
//				PinLst << PID_INTEGER;
//				break;

//			case QMetaType::Float:
//				PinLst << PID_FLOAT;
//				break;

//			case QMetaType::QMatrix4x4:
//				PinLst << PID_MATRIX4;
//				break;

//			case QMetaType::QColor:
//				PinLst << PID_COLOUR;
//				break;

//			default:
//				break;
//		}

//		return( PinLst );
//	}

//	return( mNode->context()->global()->pinIds().keys() );
//}

bool MakeArrayNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	if( pPin->direction() != PIN_OUTPUT )
	{
		return( false );
	}

	fugio::VariantInterface	*V = qobject_cast<fugio::VariantInterface *>( pPin->control()->qobject() );

	return( V );
}

//void MakeArrayNode::setType( int pIndex )
//{
//	QComboBox		*GUI = qobject_cast<QComboBox *>( sender() );

//	if( GUI )
//	{
//		QMetaType::Type		Type = QMetaType::Type( GUI->itemData( pIndex ).type() );

//		if( Type != mType )
//		{
//			mType = Type;

//			mNode->context()->updateNode( mNode );
//		}
//	}
//}

bool MakeArrayNode::pinShouldAutoRename( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_INPUT );
}

//QWidget *MakeArrayNode::gui()
//{
//	QComboBox		*GUI = new QComboBox();

//	GUI->addItem( "int", QVariant::fromValue<int>( 0 ) );
//	GUI->addItem( "float", QVariant::fromValue<float>( 0 ) );
//	GUI->addItem( "Colour", QColor() );
//	GUI->addItem( "Matrix4", QMatrix4x4() );

//	connect( GUI, SIGNAL(activated(int)), this, SLOT(setType(int)) );

//	if( mType == QMetaType::UnknownType )
//	{
//		mType = QMetaType::Type( GUI->currentData().type() );
//	}
//	else
//	{
//		for( int i = 0 ; i < GUI->count() ; i++ )
//		{
//			if( mType != QMetaType::Type( GUI->itemData( i ).type() ) )
//			{
//				continue;
//			}

//			GUI->setCurrentIndex( i );

//			break;
//		}
//	}

//	return( GUI );
//}

//void MakeArrayNode::loadSettings( QSettings &pSettings )
//{
//	mType = QMetaType::Type( pSettings.value( "type", int( mType ) ).toInt() );
//}

//void MakeArrayNode::saveSettings(QSettings &pSettings) const
//{
//	pSettings.setValue( "type", int( mType ) );
//}

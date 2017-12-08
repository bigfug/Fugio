#include "makearraynode.h"

#include <QComboBox>
#include <QMatrix4x4>
#include <QSettings>

#include <fugio/core/uuid.h>
#include <fugio/math/uuid.h>
#include <fugio/colour/uuid.h>

#include <fugio/context_interface.h>

MakeArrayNode::MakeArrayNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mType( QMetaType::UnknownType )
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

	if( mType == QMetaType::UnknownType )
	{
		return;
	}

	QList<QSharedPointer<fugio::PinInterface>>	PinLst = mNode->enumInputPins();

	if( mValOutput->type() != mType )
	{
		mValOutput->setType( mType );
		mValOutput->setSize( 1 );

		switch( mType )
		{
			case QMetaType::Int:
				mValOutput->setStride( sizeof( int ) );
				break;

			case QMetaType::Float:
				mValOutput->setStride( sizeof( float ) );
				break;

			case QMetaType::QMatrix4x4:
				mValOutput->setStride( sizeof( float ) * 16 );
				break;

			case QMetaType::QColor:
				mValOutput->setStride( sizeof( QColor ) );
				break;

			default:
				return;
		}
	}

	mValOutput->setCount( PinLst.size() );

	void		*DstPtr = mValOutput->array();

	for( int i = 0 ; i < PinLst.size() ; i++ )
	{
		switch( mType )
		{
			case QMetaType::Int:
				static_cast<int *>( DstPtr )[ i ] = variant( PinLst.at( i ) ).toInt();
				break;

			case QMetaType::Float:
				static_cast<float *>( DstPtr )[ i ] = variant( PinLst.at( i ) ).toFloat();
				break;

			case QMetaType::QMatrix4x4:
				{
					QMatrix4x4		 M = variant( PinLst.at( i ) ).value<QMatrix4x4>();
					float			*D = &static_cast<float *>( DstPtr )[ 16 * i ];

					memcpy( D, M.constData(), sizeof( float ) * 16 );
				}
				break;

			case QMetaType::QColor:
				{
					QColor		 V = variant( PinLst.at( i ) ).value<QColor>();
					QColor		*D = &static_cast<QColor *>( DstPtr )[ i ];

					*D = V;
				}
				break;

			default:
				return;
		}
	}

	pinUpdated( mPinOutput );
}

QList<QUuid> MakeArrayNode::pinAddTypesInput() const
{
	if( mPinInput->isConnected() )
	{
		QList<QUuid>	PinLst;

		switch( mType )
		{
			case QMetaType::Int:
				PinLst << PID_INTEGER;
				break;

			case QMetaType::Float:
				PinLst << PID_FLOAT;
				break;

			case QMetaType::QMatrix4x4:
				PinLst << PID_MATRIX4;
				break;

			case QMetaType::QColor:
				PinLst << PID_COLOUR;
				break;

			default:
				break;
		}

		return( PinLst );
	}

	return( mNode->context()->global()->pinIds().keys() );
}

bool MakeArrayNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	if( pPin->direction() != PIN_OUTPUT )
	{
		return( false );
	}

	fugio::VariantInterface	*V = qobject_cast<fugio::VariantInterface *>( pPin->control()->qobject() );

	return( V );
}

void MakeArrayNode::setType( int pIndex )
{
	QComboBox		*GUI = qobject_cast<QComboBox *>( sender() );

	if( GUI )
	{
		QMetaType::Type		Type = QMetaType::Type( GUI->itemData( pIndex ).type() );

		if( Type != mType )
		{
			mType = Type;

			mNode->context()->updateNode( mNode );
		}
	}
}

bool MakeArrayNode::pinShouldAutoRename( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_INPUT );
}

QWidget *MakeArrayNode::gui()
{
	QComboBox		*GUI = new QComboBox();

	GUI->addItem( "int", QVariant::fromValue<int>( 0 ) );
	GUI->addItem( "float", QVariant::fromValue<float>( 0 ) );
	GUI->addItem( "Colour", QColor() );
	GUI->addItem( "Matrix4", QMatrix4x4() );

	connect( GUI, SIGNAL(activated(int)), this, SLOT(setType(int)) );

	if( mType == QMetaType::UnknownType )
	{
		mType = QMetaType::Type( GUI->currentData().type() );
	}
	else
	{
		for( int i = 0 ; i < GUI->count() ; i++ )
		{
			if( mType != QMetaType::Type( GUI->itemData( i ).type() ) )
			{
				continue;
			}

			GUI->setCurrentIndex( i );

			break;
		}
	}

	return( GUI );
}

void MakeArrayNode::loadSettings( QSettings &pSettings )
{
	mType = QMetaType::Type( pSettings.value( "type", int( mType ) ).toInt() );
}

void MakeArrayNode::saveSettings(QSettings &pSettings) const
{
	pSettings.setValue( "type", int( mType ) );
}

#include "subtractnode.h"

#include <QVector2D>
#include <QVector3D>

#include <fugio/core/uuid.h>
#include <fugio/math/uuid.h>
#include <fugio/math/math_interface.h>

#include <fugio/context_interface.h>

#include "mathplugin.h"

SubtractNode::SubtractNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	static const QUuid	PII_NUMBER1( "{c13a41c6-544b-46bb-a9f2-19dd156d236c}" );
	static const QUuid	PII_NUMBER2( "{608ac771-490b-4ae6-9c81-12b9af526d09}" );

	mPinInput = pinInput( "Input", PII_NUMBER1 );

	pinInput( "Input", PII_NUMBER2 );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Output", mPinOutput, PID_VARIANT );
}

void SubtractNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	QVariant		InputBase = variant( mPinInput );
	QMetaType::Type	InputType = QMetaType::Type( InputBase.type() );

	QVariant		OutputValue;

	switch( InputType )
	{
		case QMetaType::Float:
		case QMetaType::Double:
		case QMetaType::Int:
		case QMetaType::QString:
			OutputValue = subtractNumber( mNode->enumInputPins() );
			break;

		case QMetaType::QPoint:
			OutputValue = subtractPoint( mNode->enumInputPins() );
			break;

		case QMetaType::QVector3D:
			OutputValue = subtractVector3D( mNode->enumInputPins() );
			break;

		default:
			{
				fugio::MathInterface::MathOperatorFunction	MathFunc = MathPlugin::instance()->findMetaTypeMathOperator( InputType, fugio::MathInterface::OP_SUBTRACT );

				if( !MathFunc )
				{
					return;
				}

				OutputValue = MathFunc( mNode->enumInputPins() );
			}
			break;
	}

	if( OutputValue != mValOutput->variant() )
	{
		mValOutput->setVariant( OutputValue );

		pinUpdated( mPinOutput );
	}
}

QList<QUuid> SubtractNode::pinAddTypesInput() const
{
	static QList<QUuid>		PinLst;

	if( PinLst.isEmpty() )
	{
		PinLst << PID_FLOAT;
		PinLst << PID_INTEGER;
		PinLst << PID_STRING;
		PinLst << PID_POINT;
		PinLst << PID_VECTOR3;
	}

	return( PinLst );
}

bool SubtractNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	if( pPin->direction() != PIN_OUTPUT )
	{
		return( false );
	}

	return( true );
}

QVariant SubtractNode::subtractNumber( const QList< QSharedPointer<fugio::PinInterface> > pInputPins )
{
	double		OutVal = 0;
	bool		OutHas = false;

	for( QSharedPointer<fugio::PinInterface> P : pInputPins )
	{
		bool		b;
		double		v = variantStatic( P ).toDouble( &b );

		if( b )
		{
			if( !OutHas )
			{
				OutVal = v;
				OutHas = true;
			}
			else
			{
				OutVal -= v;
			}
		}
	}

	return( OutVal );
}

QVariant SubtractNode::subtractVector3D( const QList< QSharedPointer<fugio::PinInterface> > pInputPins )
{
	QVector3D		OutVal;
	bool			OutHas = false;

	for( QSharedPointer<fugio::PinInterface> P : pInputPins )
	{
		QVariant		InputBase = variantStatic( P );
		QMetaType::Type	InputType = QMetaType::Type( InputBase.type() );

		switch( InputType )
		{
			case QMetaType::QVector3D:
				{
					QVector3D		v = variantStatic( P ).value<QVector3D>();

					if( !OutHas )
					{
						OutVal = v;
						OutHas = true;
					}
					else
					{
						OutVal -= v;
					}
				}
				break;

			case QMetaType::Float:
			case QMetaType::Double:
			case QMetaType::Int:
			case QMetaType::QString:
				{
					bool		c;
					qreal		v = variantStatic( P ).toReal( &c );

					if( c )
					{
						OutVal -= QVector3D( v, v, v );
					}
				}
				break;

			default:
				continue;
		}
	}

	return( OutVal );
}

QVariant SubtractNode::subtractPoint( const QList<QSharedPointer<PinInterface> > pInputPins )
{
	QPoint			OutVal;
	bool			OutHas = false;

	for( QSharedPointer<fugio::PinInterface> P : pInputPins )
	{
		QVariant		InputBase = variantStatic( P );
		QMetaType::Type	InputType = QMetaType::Type( InputBase.type() );

		switch( InputType )
		{
			case QMetaType::QPoint:
				{
					QPoint		v = variantStatic( P ).toPoint();

					if( !OutHas )
					{
						OutVal = v;
						OutHas = true;
					}
					else
					{
						OutVal -= v;
					}
				}
				break;

			case QMetaType::QVector2D:
				{
					QVector2D	v = variantStatic( P ).value<QVector2D>();

					OutVal -= QPoint( v.x(), v.y() );
				}
				break;

			case QMetaType::Float:
			case QMetaType::Double:
			case QMetaType::Int:
			case QMetaType::QString:
				{
					bool		c;
					qreal		v = variantStatic( P ).toReal( &c );

					if( c )
					{
						OutVal -= QPoint( v, v );
					}
				}
				break;

			default:
				continue;
		}
	}

	return( OutVal );
}


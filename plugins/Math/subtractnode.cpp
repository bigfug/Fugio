#include "subtractnode.h"

#include <QVector2D>
#include <QVector3D>
#include <QSizeF>

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
			OutputValue = Operator::sub2<QPoint>( mNode->enumInputPins() );
			break;

		case QMetaType::QPointF:
			OutputValue = Operator::sub2<QPointF>( mNode->enumInputPins() );
			break;

		case QMetaType::QSize:
			OutputValue = Operator::sub2<QSize>( mNode->enumInputPins() );
			break;

		case QMetaType::QSizeF:
			OutputValue = Operator::sub2<QSizeF>( mNode->enumInputPins() );
			break;

		case QMetaType::QVector3D:
			OutputValue = Operator::sub3<QVector3D>( mNode->enumInputPins() );
			break;

		default:
			{
				fugio::MathInterface::MathOperatorFunction	MathFunc = MathPlugin::instance()->findMetaTypeMathOperator( InputType, fugio::MathInterface::OP_SUBTRACT );

				if( !MathFunc )
				{
					return;
				}

//				OutputValue = MathFunc( mNode->enumInputPins() );
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
	static QList<QUuid>		PinLst =
	{
		PID_FLOAT,
		PID_INTEGER,
		PID_STRING,
		PID_POINT,
		PID_VECTOR3
	};

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

template<typename T>
T SubtractNode::Operator::sub2(const QList<QSharedPointer<PinInterface> > pInputPins)
{
	T				OutVal;
	QMetaType::Type	OutType = QMetaType::Type( qMetaTypeId<T>() );
	bool			OutHas = false;

	for( QSharedPointer<fugio::PinInterface> P : pInputPins )
	{
		QVariant		InputBase = variantStatic( P );
		QMetaType::Type	InputType = QMetaType::Type( InputBase.type() );

		if( InputBase.canConvert( OutType ) )
		{
			T			v = InputBase.value<T>();

			if( !OutHas )
			{
				OutVal = v;
				OutHas = true;
			}
			else
			{
				OutVal -= v;
			}

			continue;
		}

		switch( InputType )
		{
			case QMetaType::Float:
			case QMetaType::Double:
			case QMetaType::Int:
			case QMetaType::QString:
				{
					bool		c;
					qreal		v = InputBase.toReal( &c );

					if( c )
					{
						OutVal -= T( v, v );
					}
				}
				break;

			default:
				continue;
		}
	}

	return( OutVal );
}

template<typename T>
T SubtractNode::Operator::sub3(const QList<QSharedPointer<PinInterface> > pInputPins)
{
	T				OutVal;
	QMetaType::Type	OutType = QMetaType::Type( qMetaTypeId<T>() );
	bool			OutHas = false;

	for( QSharedPointer<fugio::PinInterface> P : pInputPins )
	{
		QVariant		InputBase = variantStatic( P );
		QMetaType::Type	InputType = QMetaType::Type( InputBase.type() );

		if( InputBase.canConvert( OutType ) )
		{
			T			v = InputBase.value<T>();

			if( !OutHas )
			{
				OutVal = v;
				OutHas = true;
			}
			else
			{
				OutVal -= v;
			}

			continue;
		}

		switch( InputType )
		{
			case QMetaType::Float:
			case QMetaType::Double:
			case QMetaType::Int:
			case QMetaType::QString:
				{
					bool		c;
					qreal		v = InputBase.toReal( &c );

					if( c )
					{
						OutVal -= T( v, v, v );
					}
				}
				break;

			default:
				continue;
		}
	}

	return( OutVal );
}


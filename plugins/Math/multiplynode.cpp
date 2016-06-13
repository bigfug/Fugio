#include "multiplynode.h"

#include <QVector2D>
#include <QVector3D>
#include <QMatrix4x4>
#include <QPoint>

#include <fugio/core/uuid.h>
#include <fugio/math/uuid.h>
#include <fugio/math/math_interface.h>

#include <fugio/context_interface.h>

#include "mathplugin.h"

MultiplyNode::MultiplyNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PII_NUMBER1, "c13a41c6-544b-46bb-a9f2-19dd156d236c" );
	FUGID( PII_NUMBER2, "608ac771-490b-4ae6-9c81-12b9af526d09" );
	FUGID( PII_RESULT,	"293bcd15-f3f5-4ed1-8814-7a845ab00f6b" );

	mPinInput = pinInput( "Input", PII_NUMBER1 );

	pinInput( "Input", PII_NUMBER2 );

	mValOutput = pinOutput<fugio::VariantInterface *>( "Result", mPinOutput, PID_VARIANT, PII_RESULT );
}

void MultiplyNode::inputsUpdated( qint64 pTimeStamp )
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
			OutputValue = multiplyNumber( mNode->enumInputPins() );
			break;

		case QMetaType::QPoint:
			OutputValue = multiplyPoint( mNode->enumInputPins() );
			break;

		case QMetaType::QMatrix4x4:
			OutputValue = multiplyMatrix4x4( mNode->enumInputPins() );
			break;

		case QMetaType::QVector3D:
			OutputValue = multiplyVector3D( mNode->enumInputPins() );
			break;

		default:
			{
				fugio::MathInterface::MathOperatorFunction	MathFunc = MathPlugin::instance()->findMetaTypeMathOperator( InputType, fugio::MathInterface::OP_MULTIPLY );

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


QList<QUuid> MultiplyNode::pinAddTypesInput() const
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

bool MultiplyNode::canAcceptPin(fugio::PinInterface *pPin) const
{
	if( pPin->direction() != PIN_OUTPUT )
	{
		return( false );
	}

	return( true );
}

QVariant MultiplyNode::multiplyNumber( const QList<QSharedPointer<PinInterface> > pInputPins )
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
				OutVal *= v;
			}
		}
	}

	return( OutVal );
}

QVariant MultiplyNode::multiplyVector3D( const QList<QSharedPointer<PinInterface> > pInputPins )
{
	QVector3D	OutVal;
	bool		OutHas = false;

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
						OutVal *= v;
					}
				}
				break;

			case QMetaType::QMatrix4x4:
				{
					QMatrix4x4		m = variantStatic( P ).value<QMatrix4x4>();

					QVector4D		v = QVector4D( OutVal, 1.0f );

					v = m * v;

					OutVal = v.toVector3D();
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
						OutVal *= QVector3D( v, v, v );
					}
				}
				break;

			default:
				continue;
		}
	}

	return( OutVal );
}

QVariant MultiplyNode::multiplyVector4D( const QList<QSharedPointer<PinInterface> > pInputPins )
{
	QVector4D	OutVal;
	bool		OutHas = false;

	for( QSharedPointer<fugio::PinInterface> P : pInputPins )
	{
		QVariant		InputBase = variantStatic( P );
		QMetaType::Type	InputType = QMetaType::Type( InputBase.type() );

		switch( InputType )
		{
			case QMetaType::QVector4D:
				{
					QVector4D		v = variantStatic( P ).value<QVector4D>();

					if( !OutHas )
					{
						OutVal = v;
						OutHas = true;
					}
					else
					{
						OutVal *= v;
					}
				}
				break;

			case QMetaType::QMatrix4x4:
				{
					QMatrix4x4		m = variantStatic( P ).value<QMatrix4x4>();

					OutVal = m * OutVal;
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
						OutVal *= v;
					}
				}
				break;

			default:
				continue;
		}
	}

	return( OutVal );
}

QVariant MultiplyNode::multiplyPoint(const QList<QSharedPointer<PinInterface> > pInputPins)
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
						OutVal.rx() *= v.x();
						OutVal.ry() *= v.y();
					}
				}
				break;

			case QMetaType::QVector2D:
				{
					QVector2D	v = variantStatic( P ).value<QVector2D>();

					OutVal.rx() *= v.x();
					OutVal.ry() *= v.y();
				}
				break;

			case QMetaType::Float:
			case QMetaType::Double:
			case QMetaType::Int:
			case QMetaType::QString:
				{
					bool		c;
					int			v = variantStatic( P ).toInt( &c );

					if( c )
					{
						OutVal.rx() *= v;
						OutVal.ry() *= v;
					}
				}
				break;

			default:
				continue;
		}
	}

	return( OutVal );
}

QVariant MultiplyNode::multiplyMatrix4x4( const QList<QSharedPointer<PinInterface> > pInputPins )
{
	QMatrix4x4		OutVal;
	bool			OutHas = false;

	for( QSharedPointer<fugio::PinInterface> P : pInputPins )
	{
		QVariant		InputBase = variantStatic( P );
		QMetaType::Type	InputType = QMetaType::Type( InputBase.type() );

		switch( InputType )
		{
			case QMetaType::QMatrix4x4:
				{
					QMatrix4x4		v = variantStatic( P ).value<QMatrix4x4>();

					if( !OutHas )
					{
						OutVal = v;
						OutHas = true;
					}
					else
					{
						OutVal *= v;
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
						OutVal *= v;
					}
				}
				break;

			default:
				continue;
		}
	}

	return( OutVal );
}


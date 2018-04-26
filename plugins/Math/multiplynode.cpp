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

	mValOutputArray = pinOutput<fugio::VariantInterface *>( "Output", mPinOutputArray, PID_VARIANT, PII_RESULT );
}

void MultiplyNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	QList<fugio::PinVariantIterator>	ItrLst;
	int									ItrMax = 0;
	QMetaType::Type						OutTyp = QMetaType::UnknownType;

	for( QSharedPointer<fugio::PinInterface> P : mNode->enumInputPins() )
	{
		ItrLst << fugio::PinVariantIterator( P );

		ItrMax = std::max( ItrMax, ItrLst.last().count() );

		if( OutTyp == QMetaType::UnknownType )
		{
			OutTyp = ItrLst.last().type();
		}
	}

	if( !ItrMax || OutTyp == QMetaType::UnknownType )
	{
		return;
	}

	mValOutputArray->setVariantCount( ItrMax );

	mValOutputArray->setVariantType( OutTyp );

	switch( OutTyp )
	{
		case QMetaType::Double:
			Operator::mul<double>( ItrLst, mValOutputArray, ItrMax );
			break;

		case QMetaType::Float:
			Operator::mul<float>( ItrLst, mValOutputArray, ItrMax );
			break;

		case QMetaType::QPoint:
			Operator::mul<QPoint,qreal>( ItrLst, mValOutputArray, ItrMax );
			break;

		case QMetaType::QPointF:
			Operator::mul<QPointF,qreal>( ItrLst, mValOutputArray, ItrMax );
			break;

		case QMetaType::QSize:
			Operator::mul<QSize, qreal>( ItrLst, mValOutputArray, ItrMax );
			break;

		case QMetaType::QSizeF:
			Operator::mul<QSizeF, qreal>( ItrLst, mValOutputArray, ItrMax );
			break;

		case QMetaType::Int:
			Operator::mul<int>( ItrLst, mValOutputArray, ItrMax );
			break;

		case QMetaType::QVector2D:
			Operator::muls<QVector2D,float>( ItrLst, mValOutputArray, ItrMax );
			break;

		case QMetaType::QVector3D:
			Operator::muls<QVector3D,float>( ItrLst, mValOutputArray, ItrMax );
			break;

		case QMetaType::QVector4D:
			Operator::muls<QVector4D>( ItrLst, mValOutputArray, ItrMax );
			break;

		case QMetaType::QQuaternion:
			Operator::muls<QQuaternion,float>( ItrLst, mValOutputArray, ItrMax );
			break;

		case QMetaType::QMatrix4x4:
			Operator::muls<QMatrix4x4>( ItrLst, mValOutputArray, ItrMax );
			break;

		default:
			{
				fugio::MathInterface::MathOperatorFunction	MathFunc = MathPlugin::instance()->findMetaTypeMathOperator( OutTyp, fugio::MathInterface::OP_MULTIPLY );

				if( !MathFunc )
				{
					return;
				}

				MathFunc( ItrLst, mValOutputArray, ItrMax );
			}
			break;
	}

	pinUpdated( mPinOutputArray );
}


QList<QUuid> MultiplyNode::pinAddTypesInput() const
{
	static QList<QUuid>		PinLst =
	{
		PID_FLOAT,
		PID_INTEGER,
		PID_STRING,
		PID_POINT,
		PID_VECTOR3,
		PID_SIZE,
		PID_MATRIX4
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

template<typename T, typename S>
void MultiplyNode::Operator::mul( const QList<PinVariantIterator> &ItrLst, fugio::VariantInterface *OutDst, int ItrMax )
{
	for( int i = 0 ; i < ItrMax ; i++ )
	{
		T			OutVal = ItrLst.at( 0 ).index( i ).value<T>();

		for( int j = 1 ; j < ItrLst.size() ; j++ )
		{
			OutVal *= ItrLst.at( j ).index( i ).value<S>();
		}

		OutDst->setVariant( i, OutVal );
	}
}

template<typename T, typename S>
void MultiplyNode::Operator::muls(const QList<PinVariantIterator> &ItrLst, fugio::VariantInterface *OutDst, int ItrMax)
{
	for( int i = 0 ; i < ItrMax ; i++ )
	{
		T			OutVal = ItrLst.at( 0 ).index( i ).value<T>();

		for( int j = 1 ; j < ItrLst.size() ; j++ )
		{
			const QVariant	V = ItrLst.at( j ).index( i );

			if( V.canConvert<T>() )
			{
				OutVal *= V.value<T>();
			}
			else
			{
				OutVal *= V.value<S>();
			}
		}

		OutDst->setVariant( i, OutVal );
	}
}

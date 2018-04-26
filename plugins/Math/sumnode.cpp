#include "sumnode.h"

#include <fugio/pin_variant_iterator.h>

#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QSizeF>
#include <QQuaternion>
#include <QMatrix4x4>

#include <fugio/core/uuid.h>
#include <fugio/math/uuid.h>
#include <fugio/math/math_interface.h>

#include "mathplugin.h"

SumNode::SumNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_NUMBER1, "c13a41c6-544b-46bb-a9f2-19dd156d236c" );
	FUGID( PIN_OUTPUT_NUMBER, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );

	mPinInput = pinInput( "Input", PIN_NUMBER1 );

	mValOutputValue = pinOutput<fugio::VariantInterface *>( "Output", mPinOutput, PID_VARIANT, PIN_OUTPUT_NUMBER );

	mPinInput->setDescription( tr( "The first number to add together" ) );

	mPinOutput->setDescription( tr( "The sum of the input pins added together" ) );
}

void SumNode::inputsUpdated( qint64 pTimeStamp )
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

	mValOutputValue->setVariantType( OutTyp );

	switch( OutTyp )
	{
		case QMetaType::Double:
			Operator::op<double>( ItrLst, mValOutputValue, ItrMax );
			break;

		case QMetaType::Float:
			Operator::op<float>( ItrLst, mValOutputValue, ItrMax );
			break;

		case QMetaType::QPoint:
			Operator::op<QPoint>( ItrLst, mValOutputValue, ItrMax );
			break;

		case QMetaType::QPointF:
			Operator::op<QPointF>( ItrLst, mValOutputValue, ItrMax );
			break;

		case QMetaType::QSize:
			Operator::op<QSize>( ItrLst, mValOutputValue, ItrMax );
			break;

		case QMetaType::QSizeF:
			Operator::op<QSizeF>( ItrLst, mValOutputValue, ItrMax );
			break;

		case QMetaType::Int:
			Operator::op<int>( ItrLst, mValOutputValue, ItrMax );
			break;

		case QMetaType::QString:
			Operator::op<QString>( ItrLst, mValOutputValue, ItrMax );
			break;

		case QMetaType::QVector2D:
			Operator::op<QVector2D>( ItrLst, mValOutputValue, ItrMax );
			break;

		case QMetaType::QVector3D:
			Operator::op<QVector3D>( ItrLst, mValOutputValue, ItrMax );
			break;

		case QMetaType::QVector4D:
			Operator::op<QVector4D>( ItrLst, mValOutputValue, ItrMax );
			break;

		case QMetaType::QQuaternion:
			Operator::op<QQuaternion>( ItrLst, mValOutputValue, ItrMax );
			break;

		case QMetaType::QMatrix4x4:
			Operator::op<QMatrix4x4>( ItrLst, mValOutputValue, ItrMax );
			break;

		default:
			{
				fugio::MathInterface::MathOperatorFunction	MathFunc = MathPlugin::instance()->findMetaTypeMathOperator( OutTyp, fugio::MathInterface::OP_SUM );

				if( !MathFunc )
				{
					return;
				}

				MathFunc( ItrLst, mValOutputValue, ItrMax );
			}
			break;
	}

	pinUpdated( mPinOutput );
}

QList<QUuid> SumNode::pinAddTypesInput() const
{
	static QList<QUuid>		PinLst;

	if( PinLst.isEmpty() )
	{
		PinLst << PID_FLOAT;
		PinLst << PID_INTEGER;
		PinLst << PID_STRING;
		PinLst << PID_POINT;
		PinLst << PID_SIZE;
		PinLst << PID_VECTOR3;
	}

	return( PinLst );
}

bool SumNode::canAcceptPin(fugio::PinInterface *pPin) const
{
	if( pPin->direction() != PIN_OUTPUT )
	{
		return( false );
	}

	return( true );
}

template<typename T>
void SumNode::Operator::op( const QList<fugio::PinVariantIterator> &ItrLst, fugio::VariantInterface *OutDst, int ItrMax )
{
	T			OutVal;

	for( int i = 0 ; i < ItrMax ; i++ )
	{
		for( int j = 0 ; j < ItrLst.size() ; j++ )
		{
			T	NewVal = ItrLst.at( j ).index( i ).value<T>();

			OutVal = ( !( i + j ) ? NewVal : OutVal + NewVal );
		}
	}

	OutDst->setVariant( OutVal );
}

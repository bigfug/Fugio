#include "addnode.h"

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

AddNode::AddNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_NUMBER1, "c13a41c6-544b-46bb-a9f2-19dd156d236c" );
	FUGID( PIN_NUMBER2, "608ac771-490b-4ae6-9c81-12b9af526d09" );
	FUGID( PIN_OUTPUT_NUMBER, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );

	mPinInput = pinInput( "Input", PIN_NUMBER1 );

	QSharedPointer<fugio::PinInterface>	P2 = pinInput( "Input", PIN_NUMBER2 );

	mValOutputArray = pinOutput<fugio::VariantInterface *>( "Output", mPinOutput, PID_VARIANT, PIN_OUTPUT_NUMBER );

	mPinInput->setDescription( tr( "The first number to add together" ) );

	P2->setDescription( tr( "The second number to add together" ) );

	mPinOutput->setDescription( tr( "The sum of the input pins added together" ) );
}

void AddNode::inputsUpdated( qint64 pTimeStamp )
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

	mValOutputArray->setVariantType( OutTyp );
	mValOutputArray->setVariantCount( ItrMax );

	switch( OutTyp )
	{
		case QMetaType::Double:
			Operator::add<double>( ItrLst, mValOutputArray, ItrMax );
			break;

		case QMetaType::Float:
			Operator::add<float>( ItrLst, mValOutputArray, ItrMax );
			break;

		case QMetaType::QPoint:
			Operator::add<QPoint>( ItrLst, mValOutputArray, ItrMax );
			break;

		case QMetaType::QPointF:
			Operator::add<QPointF>( ItrLst, mValOutputArray, ItrMax );
			break;

		case QMetaType::QSize:
			Operator::add<QSize>( ItrLst, mValOutputArray, ItrMax );
			break;

		case QMetaType::QSizeF:
			Operator::add<QSizeF>( ItrLst, mValOutputArray, ItrMax );
			break;

		case QMetaType::Int:
			Operator::add<int>( ItrLst, mValOutputArray, ItrMax );
			break;

		case QMetaType::QString:
			Operator::add<QString>( ItrLst, mValOutputArray, ItrMax );
			break;

		case QMetaType::QVector2D:
			Operator::add<QVector2D>( ItrLst, mValOutputArray, ItrMax );
			break;

		case QMetaType::QVector3D:
			Operator::add<QVector3D>( ItrLst, mValOutputArray, ItrMax );
			break;

		case QMetaType::QVector4D:
			Operator::add<QVector4D>( ItrLst, mValOutputArray, ItrMax );
			break;

		case QMetaType::QQuaternion:
			Operator::add<QQuaternion>( ItrLst, mValOutputArray, ItrMax );
			break;

		case QMetaType::QMatrix4x4:
			Operator::add<QMatrix4x4>( ItrLst, mValOutputArray, ItrMax );
			break;

		default:
			{
				fugio::MathInterface::MathOperatorFunction	MathFunc = MathPlugin::instance()->findMetaTypeMathOperator( OutTyp, fugio::MathInterface::OP_ADD );

				if( !MathFunc )
				{
					return;
				}

				MathFunc( ItrLst, mValOutputArray, ItrMax );
			}
			break;
	}

	pinUpdated( mPinOutput );
}

QList<QUuid> AddNode::pinAddTypesInput() const
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

bool AddNode::canAcceptPin(fugio::PinInterface *pPin) const
{
	if( pPin->direction() != PIN_OUTPUT )
	{
		return( false );
	}

	return( true );
}

template<typename T>
void AddNode::Operator::add( const QList<fugio::PinVariantIterator> &ItrLst, fugio::VariantInterface *OutDst, int ItrMax )
{
	for( int i = 0 ; i < ItrMax ; i++ )
	{
		T			OutVal;

		for( int j = 0 ; j < ItrLst.size() ; j++ )
		{
			T	NewVal = ItrLst.at( j ).index( i ).value<T>();

			OutVal = ( !j ? NewVal : OutVal + NewVal );
		}

		OutDst->setVariant( i, OutVal );
	}
}

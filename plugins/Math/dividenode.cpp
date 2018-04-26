#include "dividenode.h"

#include <QVector2D>
#include <QVector3D>
#include <QMatrix4x4>
#include <QPoint>

#include <fugio/core/uuid.h>
#include <fugio/math/uuid.h>
#include <fugio/math/math_interface.h>

#include <fugio/context_interface.h>

#include "mathplugin.h"
DivideNode::DivideNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PII_NUMBER1, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PII_NUMBER2, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PII_RESULT,	"261cc653-d7fa-4c34-a08b-3603e8ae71d5" );

	mPinNumerator   = pinInput( "Input", PII_NUMBER1 );
	mPinDenominator = pinInput( "Input", PII_NUMBER2 );

	mValOutputArray = pinOutput<fugio::VariantInterface *>( "Output", mPinOutputArray, PID_VARIANT, PII_RESULT );
}

void DivideNode::inputsUpdated( qint64 pTimeStamp )
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
			Operator::div<double>( ItrLst, mValOutputArray, ItrMax );
			break;

		case QMetaType::Float:
			Operator::div<float>( ItrLst, mValOutputArray, ItrMax );
			break;

		case QMetaType::QPoint:
			Operator::div<QPoint,int>( ItrLst, mValOutputArray, ItrMax );
			break;

		case QMetaType::QPointF:
			Operator::div<QPointF,qreal>( ItrLst, mValOutputArray, ItrMax );
			break;

		case QMetaType::QSize:
			Operator::div<QSize, int>( ItrLst, mValOutputArray, ItrMax );
			break;

		case QMetaType::QSizeF:
			Operator::div<QSizeF, qreal>( ItrLst, mValOutputArray, ItrMax );
			break;

		case QMetaType::Int:
			Operator::div<int>( ItrLst, mValOutputArray, ItrMax );
			break;

		case QMetaType::QVector2D:
			Operator::divs<QVector2D,float>( ItrLst, mValOutputArray, ItrMax );
			break;

		case QMetaType::QVector3D:
			Operator::divs<QVector3D,float>( ItrLst, mValOutputArray, ItrMax );
			break;

		case QMetaType::QVector4D:
			Operator::divs<QVector4D>( ItrLst, mValOutputArray, ItrMax );
			break;

		case QMetaType::QQuaternion:
			Operator::div<QQuaternion,float>( ItrLst, mValOutputArray, ItrMax );
			break;

		case QMetaType::QMatrix4x4:
			Operator::div<QMatrix4x4,float>( ItrLst, mValOutputArray, ItrMax );
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

QList<QUuid> DivideNode::pinAddTypesInput() const
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

bool DivideNode::canAcceptPin(fugio::PinInterface *pPin) const
{
	if( pPin->direction() != PIN_OUTPUT )
	{
		return( false );
	}

	return( true );
}

template<typename T, typename S>
void DivideNode::Operator::div( const QList<fugio::PinVariantIterator> &ItrLst, fugio::VariantInterface *OutDst, int ItrMax )
{
	for( int i = 0 ; i < ItrMax ; i++ )
	{
		T			OutVal = ItrLst.at( 0 ).index( i ).value<T>();

		for( int j = 1 ; j < ItrLst.size() ; j++ )
		{
			S		NewVal = ItrLst.at( j ).index( i ).value<S>();

			if( NewVal )
			{
				OutVal /= NewVal;
			}
		}

		OutDst->setVariant( i, OutVal );
	}
}

template<typename T, typename S>
void DivideNode::Operator::divs(const QList<fugio::PinVariantIterator> &ItrLst, fugio::VariantInterface *OutDst, int ItrMax)
{
	for( int i = 0 ; i < ItrMax ; i++ )
	{
		T			OutVal = ItrLst.at( 0 ).index( i ).value<T>();

		for( int j = 1 ; j < ItrLst.size() ; j++ )
		{
			const QVariant	V = ItrLst.at( j ).index( i );

			if( V.canConvert<T>() )
			{
				OutVal /= V.value<T>();
			}
			else
			{
				OutVal /= V.value<S>();
			}
		}

		OutDst->setVariant( i, OutVal );
	}
}

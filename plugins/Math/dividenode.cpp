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

	mPinNumerator   = pinInput( "Input" );
	mPinDenominator = pinInput( "Input" );

	mValOutputArray = pinOutput<fugio::VariantInterface *>( "Output", mPinOutputArray, PID_VARIANT, PII_RESULT );
}

void DivideNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	if( mPinOutputArray->controlUuid() == PID_VARIANT )
	{
		fugio::VariantInterface				*mValOutput = qobject_cast<fugio::VariantInterface *>( mPinOutputArray->control()->qobject() );

		QVariant		InputBase = variant( mPinNumerator );
		QMetaType::Type	InputType = QMetaType::Type( InputBase.type() );

		QVariant		OutputValue;

		switch( InputType )
		{
			case QMetaType::Float:
			case QMetaType::Double:
			case QMetaType::Int:
			case QMetaType::QString:
				OutputValue = divideNumber( mNode->enumInputPins() );
				break;

			case QMetaType::QPoint:
				OutputValue = Operator::op0<QPoint>( mNode->enumInputPins() );
				break;

			case QMetaType::QPointF:
				OutputValue = Operator::op0<QPointF>( mNode->enumInputPins() );
				break;

			case QMetaType::QSize:
				OutputValue = Operator::op0<QSize>( mNode->enumInputPins() );
				break;

			case QMetaType::QSizeF:
				OutputValue = Operator::op0<QSizeF>( mNode->enumInputPins() );
				break;

			case QMetaType::QMatrix4x4:
				OutputValue = Operator::op0<QMatrix4x4>( mNode->enumInputPins() );
				break;

			case QMetaType::QVector3D:
				OutputValue = Operator::op3<QVector3D>( mNode->enumInputPins() );
				break;

			default:
				{
					fugio::MathInterface::MathOperatorFunction	MathFunc = MathPlugin::instance()->findMetaTypeMathOperator( InputType, fugio::MathInterface::OP_DIVIDE );

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

			pinUpdated( mPinOutputArray );
		}
	}
	else
	{
		QList<fugio::PinVariantIterator>	ItrLst;
		int									ItrMax = 0;
		QMetaType::Type						OutTyp = QMetaType::UnknownType;

		for( QSharedPointer<fugio::PinInterface> P : mNode->enumInputPins() )
		{
			ItrLst << fugio::PinVariantIterator( P );

			ItrMax = std::max( ItrMax, ItrLst.last().size() );

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

QVariant DivideNode::divideNumber( const QList<QSharedPointer<PinInterface> > pInputPins )
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
				OutVal /= v;
			}
		}
	}

	return( OutVal );
}

template<typename T>
T DivideNode::Operator::op2( const QList<QSharedPointer<PinInterface> > pInputPins )
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
				OutVal /= v;
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
						OutVal /= T( v, v );
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
T DivideNode::Operator::op3(const QList<QSharedPointer<PinInterface> > pInputPins)
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
				OutVal /= v;
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
						OutVal /= T( v, v, v );
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
T DivideNode::Operator::op1(const QList<QSharedPointer<PinInterface> > pInputPins)
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
				OutVal /= v;
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
						OutVal /= v;
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
T DivideNode::Operator::op0(const QList<QSharedPointer<PinInterface> > pInputPins)
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
//				OutVal /= v;
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
						OutVal /= v;
					}
				}
				break;

			default:
				continue;
		}
	}

	return( OutVal );
}

template<typename T, typename S>
void DivideNode::Operator::div( const QList<fugio::PinVariantIterator> &ItrLst, fugio::VariantInterface *OutDst, int ItrMax )
{
	for( int i = 0 ; i < ItrMax ; i++ )
	{
		T			OutVal = ItrLst.at( 0 ).index( i ).value<T>();

		for( int j = 1 ; j < ItrLst.size() ; j++ )
		{
			OutVal /= ItrLst.at( 0 ).index( i ).value<S>();
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

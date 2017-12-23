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

	mValOutputArray = pinOutput<fugio::ArrayInterface *>( "Output", mPinOutput, PID_ARRAY, PIN_OUTPUT_NUMBER );

	mValOutputArray->setSize( 1 );

	mPinInput->setDescription( tr( "The first number to add together" ) );

	P2->setDescription( tr( "The second number to add together" ) );

	mPinOutput->setDescription( tr( "The sum of the input pins added together" ) );
}

void AddNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	if( mPinOutput->controlUuid() == PID_VARIANT )
	{
		fugio::VariantInterface						*mValOutput = qobject_cast<fugio::VariantInterface *>( mPinOutput->control()->qobject() );

		QVariant		InputBase = variant( mPinInput );
		QMetaType::Type	InputType = QMetaType::Type( InputBase.type() );

		QVariant		OutputValue;

		switch( InputType )
		{
			case QMetaType::Float:
			case QMetaType::Double:
			case QMetaType::Int:
			case QMetaType::QString:
				OutputValue = addNumber( mNode->enumInputPins() );
				break;

			case QMetaType::QPoint:
				OutputValue = Operator::add2<QPoint>( mNode->enumInputPins() );
				break;

			case QMetaType::QPointF:
				OutputValue = Operator::add2<QPointF>( mNode->enumInputPins() );
				break;

			case QMetaType::QSize:
				OutputValue = Operator::add2<QSize>( mNode->enumInputPins() );
				break;

			case QMetaType::QSizeF:
				OutputValue = Operator::add2<QSizeF>( mNode->enumInputPins() );
				break;

			case QMetaType::QVector3D:
				OutputValue = Operator::add3<QVector3D>( mNode->enumInputPins() );
				break;

			default:
				{
					fugio::MathInterface::MathOperatorFunction	MathFunc = MathPlugin::instance()->findMetaTypeMathOperator( InputType, fugio::MathInterface::OP_ADD );

					if( !MathFunc )
					{
						return;
					}

//					OutputValue = MathFunc( mNode->enumInputPins() );
				}
				break;
		}

		if( OutputValue != mValOutput->variant() )
		{
			mValOutput->setVariant( OutputValue );

			pinUpdated( mPinOutput );
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

		mValOutputArray->setType( OutTyp );
		mValOutputArray->setCount( ItrMax );
		mValOutputArray->setStride( QMetaType::sizeOf( OutTyp ) );

		switch( OutTyp )
		{
			case QMetaType::Double:
				Operator::add<double>( ItrLst, mValOutputArray->array(), ItrMax );
				break;

			case QMetaType::Float:
				Operator::add<float>( ItrLst, mValOutputArray->array(), ItrMax );
				break;

			case QMetaType::QPoint:
				Operator::add<QPoint>( ItrLst, mValOutputArray->array(), ItrMax );
				break;

			case QMetaType::QPointF:
				Operator::add<QPointF>( ItrLst, mValOutputArray->array(), ItrMax );
				break;

			case QMetaType::QSize:
				Operator::add<QSize>( ItrLst, mValOutputArray->array(), ItrMax );
				break;

			case QMetaType::QSizeF:
				Operator::add<QSizeF>( ItrLst, mValOutputArray->array(), ItrMax );
				break;

			case QMetaType::Int:
				Operator::add<int>( ItrLst, mValOutputArray->array(), ItrMax );
				break;

			case QMetaType::QString:
				Operator::add<QString>( ItrLst, mValOutputArray->array(), ItrMax );
				break;

			case QMetaType::QVector2D:
				Operator::add<QVector2D>( ItrLst, mValOutputArray->array(), ItrMax );
				break;

			case QMetaType::QVector3D:
				Operator::add<QVector3D>( ItrLst, mValOutputArray->array(), ItrMax );
				break;

			case QMetaType::QVector4D:
				Operator::add<QVector4D>( ItrLst, mValOutputArray->array(), ItrMax );
				break;

			case QMetaType::QQuaternion:
				Operator::add<QQuaternion>( ItrLst, mValOutputArray->array(), ItrMax );
				break;

			case QMetaType::QMatrix4x4:
				Operator::add<QMatrix4x4>( ItrLst, mValOutputArray->array(), ItrMax );
				break;

			default:
				{
					fugio::MathInterface::MathOperatorFunction	MathFunc = MathPlugin::instance()->findMetaTypeMathOperator( OutTyp, fugio::MathInterface::OP_ADD );

					if( !MathFunc )
					{
						return;
					}

					MathFunc( ItrLst, mValOutputArray->array(), ItrMax );
				}
				break;
		}

		pinUpdated( mPinOutput );
	}
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

QVariant AddNode::addNumber(const QList<QSharedPointer<PinInterface> > pInputPins)
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
				OutVal += v;
			}
		}
	}

	return( OutVal );
}

template<typename T>
T AddNode::Operator::add2(const QList<QSharedPointer<PinInterface> > pInputPins)
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
				OutVal += v;
			}

			continue;
		}

		switch( InputType )
		{
			case QMetaType::QSize:
				{
					QSize		v = InputBase.toSize();

					OutVal += T( v.width(), v.height() );
				}
				break;

			case QMetaType::QSizeF:
				{
					QSizeF		v = InputBase.toSizeF();

					OutVal += T( v.width(), v.height() );
				}
				break;

			case QMetaType::Float:
			case QMetaType::Double:
			case QMetaType::Int:
			case QMetaType::QString:
				{
					bool		c;
					qreal		v = InputBase.toReal( &c );

					if( c )
					{
						OutVal += T( v, v );
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
T AddNode::Operator::add3(const QList<QSharedPointer<PinInterface> > pInputPins)
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
				OutVal += v;
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
						OutVal += T( v, v, v );
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
void AddNode::Operator::add( const QList<fugio::PinVariantIterator> &ItrLst, void *OutDst, int ItrMax )
{
	T	*OutPtr = reinterpret_cast<T *>( OutDst );

	for( int i = 0 ; i < ItrMax ; i++ )
	{
		T			OutVal;

		for( int j = 0 ; j < ItrLst.size() ; j++ )
		{
			T	NewVal = ItrLst.at( j ).index( i ).value<T>();

			OutVal = ( !j ? NewVal : OutVal + NewVal );
		}

		*OutPtr++ = OutVal;
	}
}

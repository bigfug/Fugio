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

	mValOutputArray = pinOutput<fugio::ArrayInterface *>( "Output", mPinOutputArray, PID_ARRAY, PII_RESULT );

	mValOutputArray->setSize( 1 );
}

void MultiplyNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	if( mPinOutputArray->controlUuid() == PID_VARIANT )
	{
		fugio::VariantInterface				*mValOutput = qobject_cast<fugio::VariantInterface *>( mPinOutputArray->control()->qobject() );

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
				OutputValue = Operator::op0<QPoint>( mNode->enumInputPins() );
				break;

			case QMetaType::QPointF:
				OutputValue = Operator::op0<QPointF>( mNode->enumInputPins() );
				break;

			case QMetaType::QMatrix4x4:
				OutputValue = Operator::op1<QMatrix4x4>( mNode->enumInputPins() );
				break;

			case QMetaType::QVector3D:
				OutputValue = Operator::op3<QVector3D>( mNode->enumInputPins() );
				break;

			default:
				{
					fugio::MathInterface::MathOperatorFunction	MathFunc = MathPlugin::instance()->findMetaTypeMathOperator( InputType, fugio::MathInterface::OP_MULTIPLY );

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

		if( mValOutputArray->count() != ItrMax || mValOutputArray->type() != OutTyp )
		{
			if( mValOutputArray->type() != QMetaType::UnknownType )
			{
				const QMetaType::Type	CurTyp = mValOutputArray->type();
				const int				TypSiz = QMetaType::sizeOf( CurTyp );

				for( int i = 0 ; i < mValOutputArray->count() ; i++ )
				{
					QMetaType::destroy( mValOutputArray->type(), reinterpret_cast<quint8 *>( mValOutputArray->array() ) + TypSiz * i );
				}
			}

			mValOutputArray->setType( OutTyp );
			mValOutputArray->setCount( ItrMax );
			mValOutputArray->setStride( QMetaType::sizeOf( OutTyp ) );

			if( true )
			{
				const QMetaType::Type	CurTyp = mValOutputArray->type();
				const int				TypSiz = QMetaType::sizeOf( CurTyp );

				for( int i = 0 ; i < mValOutputArray->count() ; i++ )
				{
					QMetaType::construct( mValOutputArray->type(), reinterpret_cast<quint8 *>( mValOutputArray->array() ) + TypSiz * i, Q_NULLPTR );
				}
			}
		}

		switch( OutTyp )
		{
			case QMetaType::Double:
				Operator::mul<double>( ItrLst, mValOutputArray->array(), ItrMax );
				break;

			case QMetaType::Float:
				Operator::mul<float>( ItrLst, mValOutputArray->array(), ItrMax );
				break;

			case QMetaType::QPoint:
				Operator::mul<QPoint,int>( ItrLst, mValOutputArray->array(), ItrMax );
				break;

			case QMetaType::QPointF:
				Operator::mul<QPointF,qreal>( ItrLst, mValOutputArray->array(), ItrMax );
				break;

			case QMetaType::QSize:
				Operator::mul<QSize, int>( ItrLst, mValOutputArray->array(), ItrMax );
				break;

			case QMetaType::QSizeF:
				Operator::mul<QSizeF, qreal>( ItrLst, mValOutputArray->array(), ItrMax );
				break;

			case QMetaType::Int:
				Operator::mul<int>( ItrLst, mValOutputArray->array(), ItrMax );
				break;

			case QMetaType::QVector2D:
				Operator::muls<QVector2D,float>( ItrLst, mValOutputArray->array(), ItrMax );
				break;

			case QMetaType::QVector3D:
				Operator::muls<QVector3D,float>( ItrLst, mValOutputArray->array(), ItrMax );
				break;

			case QMetaType::QVector4D:
				Operator::muls<QVector4D>( ItrLst, mValOutputArray->array(), ItrMax );
				break;

			case QMetaType::QQuaternion:
				Operator::muls<QQuaternion,float>( ItrLst, mValOutputArray->array(), ItrMax );
				break;

			case QMetaType::QMatrix4x4:
				Operator::muls<QMatrix4x4>( ItrLst, mValOutputArray->array(), ItrMax );
				break;

			default:
				{
					fugio::MathInterface::MathOperatorFunction	MathFunc = MathPlugin::instance()->findMetaTypeMathOperator( OutTyp, fugio::MathInterface::OP_MULTIPLY );

					if( !MathFunc )
					{
						return;
					}

					MathFunc( ItrLst, mValOutputArray->array(), ItrMax );
				}
				break;
		}

		pinUpdated( mPinOutputArray );
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

template<typename T>
T MultiplyNode::Operator::op2( const QList<QSharedPointer<PinInterface> > pInputPins )
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
				OutVal *= v;
			}

			continue;
		}

		switch( InputType )
		{
			case QMetaType::QSize:
				{
					QSize		v = InputBase.toSize();

					OutVal *= T( v.width(), v.height() );
				}
				break;

			case QMetaType::QSizeF:
				{
					QSizeF		v = InputBase.toSizeF();

					OutVal *= T( v.width(), v.height() );
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
						OutVal *= T( v, v );
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
T MultiplyNode::Operator::op3(const QList<QSharedPointer<PinInterface> > pInputPins)
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
				OutVal *= v;
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
						OutVal *= T( v, v, v );
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
T MultiplyNode::Operator::op1(const QList<QSharedPointer<PinInterface> > pInputPins)
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
				OutVal *= v;
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

template<typename T>
T MultiplyNode::Operator::op0(const QList<QSharedPointer<PinInterface> > pInputPins)
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

template<typename T, typename S>
void MultiplyNode::Operator::mul( const QList<PinVariantIterator> &ItrLst, void *OutDst, int ItrMax )
{
	T	*OutPtr = reinterpret_cast<T *>( OutDst );

	for( int i = 0 ; i < ItrMax ; i++, OutPtr++ )
	{
		T			OutVal = ItrLst.at( 0 ).index( i ).value<T>();

		for( int j = 1 ; j < ItrLst.size() ; j++ )
		{
			OutVal *= ItrLst.at( 0 ).index( i ).value<S>();
		}

		*OutPtr = OutVal;
	}
}

template<typename T, typename S>
void MultiplyNode::Operator::muls(const QList<PinVariantIterator> &ItrLst, void *OutDst, int ItrMax)
{
	T	*OutPtr = reinterpret_cast<T *>( OutDst );

	for( int i = 0 ; i < ItrMax ; i++, OutPtr++ )
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

		*OutPtr = OutVal;
	}
}

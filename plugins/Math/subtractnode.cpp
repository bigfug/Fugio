#include "subtractnode.h"

#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QQuaternion>
#include <QMatrix4x4>
#include <QSizeF>

#include <fugio/core/uuid.h>
#include <fugio/math/uuid.h>
#include <fugio/math/math_interface.h>

#include <fugio/context_interface.h>

#include "mathplugin.h"

SubtractNode::SubtractNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_NUMBER1, "c13a41c6-544b-46bb-a9f2-19dd156d236c" );
	FUGID( PIN_NUMBER2, "608ac771-490b-4ae6-9c81-12b9af526d09" );
	FUGID( PIN_OUTPUT_NUMBER, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );

	mPinInput = pinInput( "Input", PIN_NUMBER1 );

	pinInput( "Input", PIN_NUMBER2 );

	mValOutputArray = pinOutput<fugio::ArrayInterface *>( "Output", mPinOutput, PID_ARRAY, PIN_OUTPUT_NUMBER );

	mValOutputArray->setSize( 1 );
}

void SubtractNode::inputsUpdated( qint64 pTimeStamp )
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
				Operator::sub<double>( ItrLst, mValOutputArray->array(), ItrMax );
				break;

			case QMetaType::Float:
				Operator::sub<float>( ItrLst, mValOutputArray->array(), ItrMax );
				break;

			case QMetaType::QPoint:
				Operator::sub<QPoint>( ItrLst, mValOutputArray->array(), ItrMax );
				break;

			case QMetaType::QPointF:
				Operator::sub<QPointF>( ItrLst, mValOutputArray->array(), ItrMax );
				break;

			case QMetaType::QSize:
				Operator::sub<QSize>( ItrLst, mValOutputArray->array(), ItrMax );
				break;

			case QMetaType::QSizeF:
				Operator::sub<QSizeF>( ItrLst, mValOutputArray->array(), ItrMax );
				break;

			case QMetaType::Int:
				Operator::sub<int>( ItrLst, mValOutputArray->array(), ItrMax );
				break;

			case QMetaType::QVector2D:
				Operator::sub<QVector2D>( ItrLst, mValOutputArray->array(), ItrMax );
				break;

			case QMetaType::QVector3D:
				Operator::sub<QVector3D>( ItrLst, mValOutputArray->array(), ItrMax );
				break;

			case QMetaType::QVector4D:
				Operator::sub<QVector4D>( ItrLst, mValOutputArray->array(), ItrMax );
				break;

			case QMetaType::QQuaternion:
				Operator::sub<QQuaternion>( ItrLst, mValOutputArray->array(), ItrMax );
				break;

			case QMetaType::QMatrix4x4:
				Operator::sub<QMatrix4x4>( ItrLst, mValOutputArray->array(), ItrMax );
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


template<typename T>
void SubtractNode::Operator::sub( const QList<PinVariantIterator> &ItrLst, void *OutDst, int ItrMax )
{
	T	*OutPtr = reinterpret_cast<T *>( OutDst );

	for( int i = 0 ; i < ItrMax ; i++, OutPtr++ )
	{
		T			OutVal;

		for( int j = 0 ; j < ItrLst.size() ; j++ )
		{
			T	NewVal = ItrLst.at( j ).index( i ).value<T>();

			OutVal = ( !j ? NewVal : OutVal - NewVal );
		}

		*OutPtr = OutVal;
	}
}

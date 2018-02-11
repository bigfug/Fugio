#include "transformnode.h"

#include <QTransform>

#include <fugio/core/uuid.h>

#include <fugio/pin_variant_iterator.h>

TransformNode::TransformNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_OUTPUT_TRANSFORM, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );

	mValOutputTransform = pinOutput<fugio::VariantInterface *>( "Transforms", mPinOutputTransform, PID_TRANSFORM, PIN_OUTPUT_TRANSFORM );
}

void TransformNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	bool		OutputUpdated = mPinOutputTransform->alwaysUpdate();

	typedef QPair<QString,fugio::PinVariantIterator>	NamedPinVariantIterator;

	QList<NamedPinVariantIterator>	ItrLst;

	for( QSharedPointer<fugio::PinInterface> P : mNode->enumInputPins() )
	{
		ItrLst << NamedPinVariantIterator( P->name(), fugio::PinVariantIterator( P ) );
	}

	if( ItrLst.isEmpty() )
	{
		return;
	}

	int	Min = 1;
	int Max = 0;

	for( NamedPinVariantIterator &PI : ItrLst )
	{
		Min = std::min( Min, PI.second.count() );
		Max = std::max( Max, PI.second.count() );
	}

	if( !Min || !Max )
	{
		return;
	}

	variantSetCount( mValOutputTransform, Max, OutputUpdated );

	for( int i = 0 ; i < Max ; i++ )
	{
		QTransform	T;

		for( NamedPinVariantIterator &PI : ItrLst )
		{
			QVariant		V = PI.second.index( i );

			if( PI.first == "Scale" )
			{
				scale( T, V );
			}
			else if( PI.first == "Shear" )
			{
				shear( T, V );
			}
			else if( PI.first == "Rotate" )
			{
				rotate( T, V );
			}
			else if( PI.first == "Translate" )
			{
				translate( T, V );
			}
			else if( PI.first == "Transform" )
			{
				transform( T, V );
			}
		}

		variantSetValue( mValOutputTransform, i, T, OutputUpdated );
	}

	if( OutputUpdated )
	{
		pinUpdated( mPinOutputTransform );
	}
}

QStringList TransformNode::availableInputPins() const
{
	static const QStringList	PinLst =
	{
		"Scale",
		"Shear",
		"Rotate",
		"Translate",
		"Transform"
	};

	return( PinLst );
}

bool TransformNode::mustChooseNamedInputPin() const
{
	return( true );
}

bool TransformNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	Q_UNUSED( pPin )

	return( false );
}

void TransformNode::scale( QTransform &T, const QVariant &V )
{
	switch( QMetaType::Type( V.type() ) )
	{
		case QMetaType::Int:
		case QMetaType::UInt:
		case QMetaType::Double:
		case QMetaType::QString:
		case QMetaType::Long:
		case QMetaType::LongLong:
		case QMetaType::Short:
		case QMetaType::Char:
		case QMetaType::ULongLong:
		case QMetaType::UShort:
		case QMetaType::SChar:
		case QMetaType::UChar:
		case QMetaType::Float:
			{
				qreal	s = V.toReal();

				T.scale( s, s );
			}
			break;

		case QMetaType::QSize:
			{
				QSize	s = V.toSize();

				T.scale( s.width(), s.height() );
			}
			break;

		case QMetaType::QSizeF:
			{
				QSizeF	s = V.toSizeF();

				T.scale( s.width(), s.height() );
			}
			break;

		default:
			break;
	}
}

void TransformNode::translate( QTransform &T, const QVariant &V )
{
	switch( QMetaType::Type( V.type() ) )
	{
		case QMetaType::Int:
		case QMetaType::UInt:
		case QMetaType::Double:
		case QMetaType::QString:
		case QMetaType::Long:
		case QMetaType::LongLong:
		case QMetaType::Short:
		case QMetaType::Char:
		case QMetaType::ULongLong:
		case QMetaType::UShort:
		case QMetaType::SChar:
		case QMetaType::UChar:
		case QMetaType::Float:
			{
				qreal	s = V.toReal();

				T.translate( s, s );
			}
			break;

		case QMetaType::QSize:
			{
				QSize	s = V.toSize();

				T.translate( s.width(), s.height() );
			}
			break;

		case QMetaType::QSizeF:
			{
				QSizeF	s = V.toSizeF();

				T.translate( s.width(), s.height() );
			}
			break;

		case QMetaType::QPoint:
			{
				QPoint	s = V.toPoint();

				T.translate( s.x(), s.y() );
			}
			break;

		case QMetaType::QPointF:
			{
				QPointF	s = V.toPointF();

				T.translate( s.x(), s.y() );
			}
			break;

		default:
			break;
	}
}

void TransformNode::shear( QTransform &T, const QVariant &V )
{
	switch( QMetaType::Type( V.type() ) )
	{
		case QMetaType::Int:
		case QMetaType::UInt:
		case QMetaType::Double:
		case QMetaType::QString:
		case QMetaType::Long:
		case QMetaType::LongLong:
		case QMetaType::Short:
		case QMetaType::Char:
		case QMetaType::ULongLong:
		case QMetaType::UShort:
		case QMetaType::SChar:
		case QMetaType::UChar:
		case QMetaType::Float:
			{
				qreal	s = V.toReal();

				T.shear( s, s );
			}
			break;

		case QMetaType::QSize:
			{
				QSize	s = V.toSize();

				T.shear( s.width(), s.height() );
			}
			break;

		case QMetaType::QSizeF:
			{
				QSizeF	s = V.toSizeF();

				T.shear( s.width(), s.height() );
			}
			break;

		default:
			break;
	}
}

void TransformNode::rotate( QTransform &T, const QVariant &V )
{
	switch( QMetaType::Type( V.type() ) )
	{
		case QMetaType::Int:
		case QMetaType::UInt:
		case QMetaType::Double:
		case QMetaType::QString:
		case QMetaType::Long:
		case QMetaType::LongLong:
		case QMetaType::Short:
		case QMetaType::Char:
		case QMetaType::ULongLong:
		case QMetaType::UShort:
		case QMetaType::SChar:
		case QMetaType::UChar:
		case QMetaType::Float:
			{
				qreal	s = V.toReal();

				T.rotate( s );
			}
			break;

		default:
			break;
	}
}

void TransformNode::transform(QTransform &T, const QVariant &V)
{
	QTransform	N = V.value<QTransform>();

	T *= N;
}

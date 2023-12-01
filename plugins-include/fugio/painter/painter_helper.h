#ifndef PAINTERHELPER_H
#define PAINTERHELPER_H

#include <fugio/global.h>
#include <fugio/pin_variant_iterator.h>

#include <QPen>
#include <QBrush>
#include <QPainter>
#include <QColor>
#include <QVariant>

FUGIO_NAMESPACE_BEGIN

class PainterHelper
{
public:
	virtual ~PainterHelper( void ) {}

	void painterSetPen( QPainter &pPainter, fugio::PinVariantIterator &pPinVarInt, int pIndex )
	{
		if( pPinVarInt.isEmpty() )
		{
			pPainter.setPen( Qt::NoPen );
		}
		else
		{
			QVariant	pv = pPinVarInt.index( pIndex );

			if( QMetaType::Type( pv.type() ) == QMetaType::QPen )
			{
				pPainter.setPen( pv.value<QPen>() );
			}
			else if( QMetaType::Type( pv.type() ) == QMetaType::QColor )
			{
				pPainter.setPen( pv.value<QColor>() );
			}
		}
	}

	void painterSetBrush( QPainter &pPainter, fugio::PinVariantIterator &pPinVarInt, int pIndex )
	{
		if( pPinVarInt.isEmpty() )
		{
			pPainter.setBrush( Qt::NoBrush );
		}
		else
		{
			QVariant	bv = pPinVarInt.index( pIndex );

			if( QMetaType::Type( bv.type() ) == QMetaType::QBrush )
			{
				pPainter.setBrush( bv.value<QBrush>() );
			}
			else if( QMetaType::Type( bv.type() ) == QMetaType::QColor )
			{
				pPainter.setBrush( bv.value<QColor>() );
			}
		}
	}
};

FUGIO_NAMESPACE_END

//Q_DECLARE_INTERFACE( fugio::PainterInterface, "com.bigfug.fugio.painter/1.0" )


#endif // PAINTERHELPER_H

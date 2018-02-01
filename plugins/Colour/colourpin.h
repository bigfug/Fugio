#ifndef COLOURPIN_H
#define COLOURPIN_H

#include <QObject>
#include <QColor>
#include <QVariant>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/colour/colour_interface.h>
#include <fugio/core/variant_helper.h>

#include <fugio/pincontrolbase.h>

#include <fugio/serialise_interface.h>
//#include <fugio/core/list_interface.h>

class ColourPin : public fugio::PinControlBase, public fugio::ColourInterface, public fugio::VariantHelper<QColor>, public fugio::SerialiseInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::ColourInterface fugio::VariantInterface fugio::SerialiseInterface )
//	Q_PROPERTY( QVector<QColor> value READ value WRITE setValue NOTIFY valueChanged )

public:
	Q_INVOKABLE explicit ColourPin( QSharedPointer<fugio::PinInterface> pPin );
	
	virtual ~ColourPin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		return( "" );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Colour" );
	}

	//-------------------------------------------------------------------------
	// InterfaceColour

	Q_INVOKABLE virtual void setColour( const QColor &pColour ) Q_DECL_OVERRIDE
	{
		if( pColour != mValues.first() )
		{
			mValues[ 0 ] = pColour;

			emit colourChanged( pColour );
		}
	}

	Q_INVOKABLE virtual void setColour( int pIndex, const QColor &pValue ) Q_DECL_OVERRIDE
	{
		mValues[ pIndex ] = pValue;
	}

	Q_INVOKABLE virtual QColor colour( int pIndex = 0 ) const Q_DECL_OVERRIDE
	{
		return( mValues[ pIndex ] );
	}

	//-------------------------------------------------------------------------
	// fugio::VariantInterface

	virtual void setFromBaseVariant( int pIndex, int pOffset, const QVariant &pValue ) Q_DECL_OVERRIDE;

	virtual QVariant baseVariant( int pIndex, int pOffset ) const Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------
	// fugio::SerialiseInterface

	virtual void serialise( QDataStream &pDataStream ) const Q_DECL_OVERRIDE
	{
		if( mValues.size() == 1 )
		{
			pDataStream << mValues.first();
		}
		else
		{
			pDataStream << mValues;
		}
	}

	virtual void deserialise( QDataStream &pDataStream ) Q_DECL_OVERRIDE
	{
#if QT_VERSION >= QT_VERSION_CHECK( 5, 7, 0 )
		QColor			C;

		pDataStream.startTransaction();

		pDataStream >> C;

		if( pDataStream.commitTransaction() )
		{
			mValues.resize( 1 );

			mValues[ 0 ] = C;

			return;
		}

		pDataStream.rollbackTransaction();
#endif

		QVector<QColor>	L;

		pDataStream >> L;

		mValues = L;
	}

signals:
	void colourChanged( const QColor &pColour );

private:
//	QVector<QColor>			mColours;
};

#endif // COLOURPIN_H

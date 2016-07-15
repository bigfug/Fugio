#ifndef COLOURPIN_H
#define COLOURPIN_H

#include <QObject>
#include <QColor>
#include <QVariant>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/colour/colour_interface.h>
#include <fugio/core/variant_interface.h>

#include <fugio/pincontrolbase.h>

#include <fugio/serialise_interface.h>

class ColourPin : public fugio::PinControlBase, public fugio::ColourInterface, public fugio::VariantInterface, public fugio::SerialiseInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::ColourInterface fugio::VariantInterface fugio::SerialiseInterface )

public:
	Q_INVOKABLE explicit ColourPin( QSharedPointer<fugio::PinInterface> pPin );
	
	virtual ~ColourPin( void );

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		return( "" );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Integer" );
	}

	//-------------------------------------------------------------------------
	// InterfaceColour

	virtual void setColour( const QColor &pColour ) Q_DECL_OVERRIDE
	{
		mColour = pColour;
	}

	virtual QColor colour( void ) const Q_DECL_OVERRIDE
	{
		return( mColour );
	}

	//-------------------------------------------------------------------------
	// fugio::VariantInterface

	virtual void setVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		mColour = pValue.value<QColor>();
	}

	virtual QVariant variant( void ) const Q_DECL_OVERRIDE
	{
		return( QVariant::fromValue<QColor>( mColour ) );
	}

	virtual void setFromBaseVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		QList<QVariant>		L = pValue.toList();

		if( L.size() > 0 ) mColour.setRedF( L.at( 0 ).toReal() );
		if( L.size() > 1 ) mColour.setGreenF( L.at( 1 ).toReal() );
		if( L.size() > 2 ) mColour.setBlueF( L.at( 2 ).toReal() );
		if( L.size() > 3 ) mColour.setAlphaF( L.at( 3 ).toReal() );
	}

	virtual QVariant baseVariant( void ) const Q_DECL_OVERRIDE
	{
		QList<QVariant>		L;

		L << mColour.redF();
		L << mColour.greenF();
		L << mColour.blueF();
		L << mColour.alphaF();

		return( L );
	}

	//-------------------------------------------------------------------------
	// fugio::SerialiseInterface

	virtual void serialise( QDataStream &pDataStream ) Q_DECL_OVERRIDE
	{
		pDataStream << mColour;
	}

	virtual void deserialise( QDataStream &pDataStream ) Q_DECL_OVERRIDE
	{
		pDataStream >> mColour;
	}

private:
	QColor			mColour;
};

#endif // COLOURPIN_H

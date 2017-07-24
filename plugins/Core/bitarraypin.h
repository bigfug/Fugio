#ifndef BITARRAYPIN_H
#define BITARRAYPIN_H

#include <QObject>

#include <QBitArray>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_interface.h>
#include <fugio/core/size_interface.h>

#include <fugio/pincontrolbase.h>

#include <fugio/serialise_interface.h>

class BitArrayPin : public fugio::PinControlBase, public fugio::VariantInterface, public fugio::SerialiseInterface, public fugio::SizeInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::VariantInterface fugio::SerialiseInterface fugio::SizeInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_PIN_URL( "BitArray" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit BitArrayPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~BitArrayPin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		return( QString( "%1 bits" ).arg( mValue.size() ) );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Bit Array" );
	}

	//-------------------------------------------------------------------------
	// fugio::VariantInterface

	virtual void setVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		mValue = pValue.toBitArray();
	}

	virtual QVariant variant( void ) const Q_DECL_OVERRIDE
	{
		return( mValue );
	}

	virtual void setFromBaseVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setVariant( pValue );
	}

	virtual QVariant baseVariant( void ) const Q_DECL_OVERRIDE
	{
		return( variant() );
	}

	//-------------------------------------------------------------------------
	// fugio::SerialiseInterface

	virtual void serialise( QDataStream &pDataStream ) const Q_DECL_OVERRIDE
	{
		pDataStream << mValue;
	}

	virtual void deserialise( QDataStream &pDataStream ) Q_DECL_OVERRIDE
	{
		pDataStream >> mValue;
	}

	//-------------------------------------------------------------------------
	// fugio::SizeInterface interface

public:
	virtual int sizeDimensions() const Q_DECL_OVERRIDE;
	virtual float size(int pDimension) const Q_DECL_OVERRIDE;
	virtual float sizeWidth() const Q_DECL_OVERRIDE;
	virtual float sizeHeight() const Q_DECL_OVERRIDE;
	virtual float sizeDepth() const Q_DECL_OVERRIDE;
	virtual QSizeF toSizeF() const Q_DECL_OVERRIDE;
	virtual QVector3D toVector3D() const Q_DECL_OVERRIDE;

private:
	QBitArray		mValue;
};

#endif // BITARRAYPIN_H

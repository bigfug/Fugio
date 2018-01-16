#ifndef BYTEARRAYPIN_H
#define BYTEARRAYPIN_H

#include <QObject>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_interface.h>
#include <fugio/core/size_interface.h>

#include <fugio/pincontrolbase.h>

#include <fugio/serialise_interface.h>

class ByteArrayPin : public fugio::PinControlBase, public fugio::VariantInterface, public fugio::SerialiseInterface, public fugio::SizeInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::VariantInterface fugio::SerialiseInterface fugio::SizeInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_PIN_URL( "ByteArray" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit ByteArrayPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~ByteArrayPin( void );

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		return( QString( "%1 bytes" ).arg( mValues.first().size() ) );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Byte Array" );
	}

	//-------------------------------------------------------------------------
	// fugio::VariantInterface

	virtual void setVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setVariant( 0, pValue );
	}

	virtual void setVariant( int pIndex, const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		mValues[ pIndex ] = pValue.toByteArray();
	}

	virtual QVariant variant( int pIndex = 0 ) const Q_DECL_OVERRIDE
	{
		return( QVariant::fromValue<QByteArray>( mValues[ pIndex ] ) );
	}

	virtual void setVariantCount( int pCount ) Q_DECL_OVERRIDE
	{
		mValues.resize( pCount );
	}

	virtual int variantCount( void ) const Q_DECL_OVERRIDE
	{
		return( mValues.size() );
	}

	inline virtual QMetaType::Type variantType( void ) const Q_DECL_OVERRIDE
	{
		return( QMetaType::QByteArray );
	}

	virtual void setFromBaseVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setFromBaseVariant( 0, pValue );
	}

	virtual void setFromBaseVariant( int pIndex, const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setVariant( pIndex, pValue );
	}

	virtual QVariant baseVariant( int pIndex ) const Q_DECL_OVERRIDE
	{
		return( variant( pIndex ) );
	}

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
		QByteArray				V;

		pDataStream.startTransaction();

		pDataStream >> V;

		if( pDataStream.commitTransaction() )
		{
			mValues.resize( 1 );

			setVariant( 0, V );

			return;
		}

		pDataStream.rollbackTransaction();

		QVector<QByteArray>	L;

		pDataStream >> L;

		mValues = L;
	}

	virtual void setVariantType( QMetaType::Type ) Q_DECL_OVERRIDE
	{

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
	QVector<QByteArray>		mValues;
};

#endif // BYTEARRAYPIN_H

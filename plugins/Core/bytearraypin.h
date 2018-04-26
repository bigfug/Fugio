#ifndef BYTEARRAYPIN_H
#define BYTEARRAYPIN_H

#include <QObject>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_helper.h>
#include <fugio/core/size_interface.h>

#include <fugio/pincontrolbase.h>

#include <fugio/serialise_interface.h>

class ByteArrayPin : public fugio::PinControlBase, public fugio::VariantHelper<QByteArray>, public fugio::SerialiseInterface, public fugio::SizeInterface
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
		QStringList		L;

		for( const QByteArray &V : mValues )
		{
			L << QString( "%1 bytes" ).arg( V.size() );
		}

		return( L.join( ',' ) );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Byte Array" );
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
#if QT_VERSION >= QT_VERSION_CHECK( 5, 7, 0 )
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
#endif

		QVector<QByteArray>	L;

		pDataStream >> L;

		mValues = L;
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
};

#endif // BYTEARRAYPIN_H

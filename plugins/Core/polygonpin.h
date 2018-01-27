#ifndef POLYGONPIN_H
#define POLYGONPIN_H

#include <QPolygonF>

#include <fugio/core/variant_helper.h>

#include <fugio/pincontrolbase.h>

#include <fugio/serialise_interface.h>

class PolygonPin : public fugio::PinControlBase, public fugio::VariantHelper<QPolygonF>, public fugio::SerialiseInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::VariantInterface fugio::SerialiseInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_PIN_URL( "Boolean" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit PolygonPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~PolygonPin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		return( QString( "%1" ).arg( mValues.size() ) );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Polygon" );
	}

	//-------------------------------------------------------------------------
	// fugio::VariantInterface

	virtual void setFromBaseVariant( int pIndex, int pOffset, const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setVariant( pIndex, pValue );
	}

	virtual QVariant baseVariant( int pIndex, int pOffset ) const Q_DECL_OVERRIDE
	{
		return( variant( pIndex, pOffset ) );
	}

	//-------------------------------------------------------------------------
	// fugio::SerialiseInterface

	virtual void serialise( QDataStream &pDataStream ) const Q_DECL_OVERRIDE
	{
//		if( mValues.size() == 1 )
//		{
//			pDataStream << mValues.first();
//		}
//		else
//		{
//			pDataStream << mValues;
//		}
	}

	virtual void deserialise( QDataStream &pDataStream ) Q_DECL_OVERRIDE
	{
//		double				V;

//		pDataStream.startTransaction();

//		pDataStream >> V;

//		if( pDataStream.commitTransaction() )
//		{
//			mValues.resize( 1 );

//			setVariant( 0, V );

//			return;
//		}

//		pDataStream.rollbackTransaction();

//		QVector<bool>	L;

//		pDataStream >> L;

//		mValues = L;
	}

signals:
//	void valueChanged( bool pValue );
};


#endif // POLYGONPIN_H

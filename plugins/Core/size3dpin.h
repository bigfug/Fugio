#ifndef SIZE3DPIN_H
#define SIZE3DPIN_H

#include <QObject>
#include <QVector3D>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_helper.h>

#include <fugio/pincontrolbase.h>

class Size3dPin : public fugio::PinControlBase, public fugio::VariantHelper<QVector3D>
{
	Q_OBJECT
	Q_INTERFACES( fugio::VariantInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_PIN_URL( "Size3d" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit Size3dPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~Size3dPin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		return( QString( "%1,%2,%3" ).arg( mValues.first().x() ).arg( mValues.first().y() ).arg( mValues.first().z() ) );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Size 3D" );
	}

	//-------------------------------------------------------------------------
	// fugio::VariantInterface

	virtual void setFromBaseVariant( int pIndex, int pOffset, const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		QList<QVariant>     L = pValue.toList();
		QVector3D			V;

		if( L.size() > 0 ) V.setX( L.at( 0 ).toReal() );
		if( L.size() > 1 ) V.setY( L.at( 1 ).toReal() );
		if( L.size() > 1 ) V.setZ( L.at( 2 ).toReal() );

		mValues[ variantIndex( pIndex, pOffset ) ] = V;
	}

	virtual QVariant baseVariant( int pIndex, int pOffset ) const Q_DECL_OVERRIDE
	{
		QList<QVariant>		L;
		QVector3D			V = mValues.at( variantIndex( pIndex, pOffset ) );

		L << V.x();
		L << V.y();
		L << V.z();

		return( L );
	}
};


#endif // SIZE3DNODE_H

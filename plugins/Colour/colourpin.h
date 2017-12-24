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
//#include <fugio/core/list_interface.h>

class ColourPin : public fugio::PinControlBase, public fugio::ColourInterface, public fugio::VariantInterface, public fugio::SerialiseInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::ColourInterface fugio::VariantInterface fugio::SerialiseInterface )
//	Q_PROPERTY( QVector<QColor> value READ value WRITE setValue NOTIFY valueChanged )

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

	Q_INVOKABLE virtual void setColour( const QColor &pColour ) Q_DECL_OVERRIDE
	{
		if( pColour != mColours.first() )
		{
			mColours[ 0 ] = pColour;

			emit colourChanged( pColour );
		}
	}

	Q_INVOKABLE virtual void setColour( int pIndex, const QColor &pValue ) Q_DECL_OVERRIDE
	{
		mColours[ pIndex ] = pValue;
	}

	Q_INVOKABLE virtual QColor colour( int pIndex = 0 ) const Q_DECL_OVERRIDE
	{
		return( mColours[ pIndex ] );
	}

	//-------------------------------------------------------------------------
	// fugio::VariantInterface

	virtual void setVariantCount( int pCount ) Q_DECL_OVERRIDE
	{
		mColours.resize( pCount );
	}

	virtual int variantCount( void ) const Q_DECL_OVERRIDE
	{
		return( mColours.size() );
	}

	virtual void setVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setVariant( 0, pValue );
	}

	virtual void setVariant( int pIndex, const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		mColours[ pIndex ] = pValue.value<QColor>();
	}

	virtual QVariant variant( int pIndex ) const Q_DECL_OVERRIDE
	{
		return( QVariant::fromValue<QColor>( mColours[ pIndex ] ) );
	}

	inline virtual QMetaType::Type variantType( void ) const Q_DECL_OVERRIDE
	{
		return( QMetaType::QColor );
	}

	virtual void setFromBaseVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setFromBaseVariant( 0, pValue );
	}

	virtual void setFromBaseVariant( int pIndex, const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		QList<QVariant>     L = pValue.toList();
		QColor				C;

		if( L.size() > 0 ) C.setRedF( L.at( 0 ).toReal() );
		if( L.size() > 1 ) C.setGreenF( L.at( 1 ).toReal() );
		if( L.size() > 2 ) C.setBlueF( L.at( 2 ).toReal() );
		if( L.size() > 3 ) C.setAlphaF( L.at( 3 ).toReal() );

		mColours[ pIndex ] = C;
	}

	virtual QVariant baseVariant( int pIndex ) const Q_DECL_OVERRIDE
	{
		QList<QVariant>		L;
		QColor				C = mColours.at( pIndex );

		L << C.redF();
		L << C.greenF();
		L << C.blueF();
		L << C.alphaF();

		return( L );
	}

	virtual void setVariantType( QMetaType::Type ) Q_DECL_OVERRIDE
	{

	}

	//-------------------------------------------------------------------------
	// fugio::SerialiseInterface

	virtual void serialise( QDataStream &pDataStream ) const Q_DECL_OVERRIDE
	{
		if( mColours.size() == 1 )
		{
			pDataStream << mColours.first();
		}
		else
		{
			pDataStream << mColours;
		}
	}

	virtual void deserialise( QDataStream &pDataStream ) Q_DECL_OVERRIDE
	{
		QColor			C;

		pDataStream.startTransaction();

		pDataStream >> C;

		if( pDataStream.commitTransaction() )
		{
			mColours.resize( 1 );

			mColours[ 0 ] = C;

			return;
		}

		pDataStream.rollbackTransaction();

		QVector<QColor>	L;

		pDataStream >> L;

		mColours = L;
	}

signals:
	void colourChanged( const QColor &pColour );

private:
	QVector<QColor>			mColours;
};

#endif // COLOURPIN_H

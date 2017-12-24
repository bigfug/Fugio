#ifndef BOOLPIN_H
#define BOOLPIN_H

#include <QObject>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_interface.h>

#include <fugio/pincontrolbase.h>

#include <fugio/serialise_interface.h>

class BoolPin : public fugio::PinControlBase, public fugio::VariantInterface, public fugio::SerialiseInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::VariantInterface fugio::SerialiseInterface )
//	Q_PROPERTY( bool mValue READ value WRITE setValue NOTIFY valueChanged )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_PIN_URL( "Boolean" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit BoolPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~BoolPin( void ) {}

	//-------------------------------------------------------------------------
	// Q_PROPERTY

//	Q_INVOKABLE bool value( void ) const
//	{
//		return( mValue );
//	}

//	Q_INVOKABLE void setValue( bool pValue )
//	{
//		if( pValue != mValue )
//		{
//			mValue = pValue;

//			emit valueChanged( pValue );
//		}
//	}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		return( QString( "%1" ).arg( mValues.first() ) );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Bool" );
	}

	//-------------------------------------------------------------------------
	// fugio::VariantInterface

	virtual void setVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setVariant( 0, pValue );
	}

	virtual void setVariant( int pIndex, const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		mValues[ pIndex ] = pValue.toBool();
	}

	virtual QVariant variant( int pIndex = 0 ) const Q_DECL_OVERRIDE
	{
		return( QVariant::fromValue<bool>( mValues[ pIndex ] ) );
	}

	virtual void setVariantCount( int pCount )
	{
		mValues.resize( pCount );
	}

	virtual int variantCount( void ) const
	{
		return( mValues.size() );
	}

	inline virtual QMetaType::Type variantType( void ) const
	{
		return( QMetaType::Bool );
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

	virtual void setVariantType( QMetaType::Type ) Q_DECL_OVERRIDE
	{

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
		double				V;

		pDataStream.startTransaction();

		pDataStream >> V;

		if( pDataStream.commitTransaction() )
		{
			mValues.resize( 1 );

			setVariant( 0, V );

			return;
		}

		pDataStream.rollbackTransaction();

		QVector<bool>	L;

		pDataStream >> L;

		mValues = L;
	}

signals:
//	void valueChanged( bool pValue );

private:
	QVector<bool>		mValues;
};

#endif // BOOLPIN_H

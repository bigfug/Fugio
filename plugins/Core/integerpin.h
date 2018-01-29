#ifndef INTEGERPIN_H
#define INTEGERPIN_H

#include <QObject>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_helper.h>

#include <fugio/pincontrolbase.h>

#include <fugio/serialise_interface.h>

class IntegerPin : public fugio::PinControlBase, public fugio::VariantHelper<int>, public fugio::SerialiseInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::VariantInterface fugio::SerialiseInterface )
//	Q_PROPERTY( int mValue READ value WRITE setValue NOTIFY valueChanged )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_PIN_URL( "Integer" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit IntegerPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~IntegerPin( void ) {}

	//-------------------------------------------------------------------------
	// Q_PROPERTY

//	Q_INVOKABLE int value( void ) const
//	{
//		return( mValue );
//	}

//	Q_INVOKABLE void setValue( int pValue )
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
		return( "Integer" );
	}

	//-------------------------------------------------------------------------
	// fugio::VariantInterface

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
		int				NewVal;

		pDataStream.startTransaction();

		pDataStream >> NewVal;

		if( pDataStream.commitTransaction() )
		{
			mValues.resize( 1 );

			setVariant( 0, NewVal );

			return;
		}

		pDataStream.rollbackTransaction();
#endif

		QVector<int>	NewVec;

		pDataStream >> NewVec;

		mValues = NewVec;
	}
};

#endif // INTEGERPIN_H

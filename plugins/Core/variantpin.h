#ifndef VARIANTPIN_H
#define VARIANTPIN_H

#include <QObject>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_helper.h>

#include <fugio/pincontrolbase.h>

#include <fugio/serialise_interface.h>

class VariantPin : public fugio::PinControlBase, public fugio::VariantHelper<QVariant>, public fugio::SerialiseInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::VariantInterface fugio::SerialiseInterface )
//	Q_PROPERTY( QVariant value READ value WRITE setValue NOTIFY valueChanged )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_PIN_URL( "Variant" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit VariantPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~VariantPin( void ) {}

	//-------------------------------------------------------------------------
	// Q_PROPERTY

//	Q_INVOKABLE QVariant value( void ) const
//	{
//		return( mValue );
//	}

//	Q_INVOKABLE void setValue( QVariant pValue )
//	{
//		if( pValue != mValue )
//		{
//			mValue = pValue;

//			emit valueChanged( pValue );
//		}
//	}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE;

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( QString( "Variant (%1)" ).arg( QMetaType::typeName( variantType() ) ) );
	}

	//-------------------------------------------------------------------------
	// fugio::VariantInterface


	//-------------------------------------------------------------------------
	// fugio::SerialiseInterface

	virtual void serialise( QDataStream &pDataStream ) const Q_DECL_OVERRIDE
	{
//		pDataStream << mValue;
	}

	virtual void deserialise( QDataStream &pDataStream ) Q_DECL_OVERRIDE
	{
//		QVariant		NewVal;

//		pDataStream >> NewVal;

//		setValue( NewVal );
	}

signals:
//	void valueChanged( QVariant pValue );
};


#endif // VARIANTPIN_H

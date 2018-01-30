#ifndef DATETIMEPIN_H
#define DATETIMEPIN_H

#include <QDateTime>

#include <fugio/core/variant_helper.h>

#include <fugio/serialise_interface.h>

#include <fugio/pincontrolbase.h>

class DateTimePin : public fugio::PinControlBase, public fugio::VariantHelper<QDateTime>, public fugio::SerialiseInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::VariantInterface fugio::SerialiseInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_PIN_URL( "DateTime" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit DateTimePin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~DateTimePin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE;

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "DateTime" );
	}

	//-------------------------------------------------------------------------
	// fugio::SerialiseInterface

	virtual void serialise( QDataStream &pDataStream ) const Q_DECL_OVERRIDE;

	virtual void deserialise( QDataStream &pDataStream ) Q_DECL_OVERRIDE;
};

#endif // DATETIMEPIN_H

#ifndef FONTPIN_H
#define FONTPIN_H

#include <QFont>

#include <fugio/core/variant_helper.h>

#include <fugio/serialise_interface.h>

#include <fugio/pincontrolbase.h>

class FontPin : public fugio::PinControlBase, public fugio::VariantHelper<QFont>, public fugio::SerialiseInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::VariantInterface fugio::SerialiseInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_PIN_URL( "Font" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit FontPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~FontPin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE;

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Font" );
	}

	//-------------------------------------------------------------------------
	// fugio::SerialiseInterface

	virtual void serialise( QDataStream &pDataStream ) const Q_DECL_OVERRIDE;

	virtual void deserialise( QDataStream &pDataStream ) Q_DECL_OVERRIDE;
};

#endif // FONTPIN_H

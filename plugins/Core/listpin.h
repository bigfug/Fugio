#ifndef LISTPIN_H
#define LISTPIN_H

#include <QObject>

#include <fugio/core/uuid.h>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_helper.h>
#include <fugio/core/list_interface.h>

#include <fugio/pincontrolbase.h>

class ListPin : public fugio::PinControlBase, public fugio::VariantHelper<QVariantList>
{
	Q_OBJECT
	Q_INTERFACES( fugio::VariantInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_PIN_URL( "List" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit ListPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~ListPin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		return( QString( "%1" ).arg( mValues.size() ) );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "List" );
	}

	//-------------------------------------------------------------------------
	// fugio::VariantInterface
};


#endif // LISTPIN_H

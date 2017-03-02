#ifndef PARAMPIN_H
#define PARAMPIN_H

#include <QObject>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/artoolkit/param_interface.h>

#include <fugio/pincontrolbase.h>

#if defined( ARTOOLKIT_SUPPORTED )
#include <AR/ar.h>
#endif

class ParamPin : public fugio::PinControlBase, public fugio::ar::ParamInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::ar::ParamInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_PIN_URL( "arParam" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit ParamPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~ParamPin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		return( "" );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "ARParam" );
	}


	// ParamInterface interface
public:
	virtual const ARParam &param() const Q_DECL_OVERRIDE;
	virtual void setParam( const ARParam &pParam ) Q_DECL_OVERRIDE;

private:
#if defined( ARTOOLKIT_SUPPORTED )
	ARParam			mParam;
#endif
};

#endif // PARAMPIN_H

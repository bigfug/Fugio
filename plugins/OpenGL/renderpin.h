#ifndef RENDERPIN_H
#define RENDERPIN_H

#include "opengl_includes.h"

#include <QObject>

#include <fugio/pin_control_interface.h>
#include "fugio/render_interface.h"

#include <fugio/opengl/uuid.h>

#include <fugio/pincontrolbase.h>

class RenderPin : public fugio::PinControlBase, public fugio::RenderInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::RenderInterface )

public:
	Q_INVOKABLE explicit RenderPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~RenderPin( void );

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		return( "" );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Render" );
	}

	//-------------------------------------------------------------------------
	// fugio::RenderInterface

	virtual void render( qint64 pTimeStamp ) Q_DECL_OVERRIDE;
};


#endif // RENDERPIN_H

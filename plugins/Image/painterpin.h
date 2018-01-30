#ifndef PAINTERPIN_H
#define PAINTERPIN_H

#include <QObject>

#include <fugio/pin_control_interface.h>

#include <fugio/image/image.h>
#include <fugio/image/uuid.h>

#include <fugio/pincontrolbase.h>

#include <fugio/image/painter_interface.h>

class PainterPin : public fugio::PinControlBase, public fugio::PainterInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PainterInterface )

public:
	Q_INVOKABLE explicit PainterPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~PainterPin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		return( "" );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Painter" );
	}

	//-------------------------------------------------------------------------
	// fugio::PainterInterface

	virtual void setSource(fugio::PainterInterface *pSource) Q_DECL_OVERRIDE;
	virtual void paint( QPainter &pPainter, const QRect &pRect ) Q_DECL_OVERRIDE;

private:
	fugio::PainterInterface			*mSource;
};


#endif // PAINTERPIN_H

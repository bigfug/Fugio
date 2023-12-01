#ifndef PAINTER_INTERFACE_H
#define PAINTER_INTERFACE_H

#include <fugio/global.h>

class QPainter;
class QRect;

FUGIO_NAMESPACE_BEGIN

class PainterInterface
{
public:
	virtual ~PainterInterface( void ) {}

	virtual void setSource( PainterInterface *pSource ) = 0;

	virtual void paint( QPainter &pPainter, const QRect &pRect ) = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::PainterInterface, "com.bigfug.fugio.painter/1.0" )

#endif // PAINTER_INTERFACE_H

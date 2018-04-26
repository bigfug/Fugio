#ifndef RECTNODE_H
#define RECTNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>
#include <fugio/core/variant_interface.h>

#include <fugio/image/painter_interface.h>
#include <fugio/painter/painter_helper.h>

class RectNode : public fugio::NodeControlBase, public fugio::PainterInterface, public fugio::PainterHelper
{
	Q_OBJECT
	Q_INTERFACES( fugio::PainterInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Rect Painter" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Rect_(Painter)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE RectNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~RectNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	// PainterInterface interface
public:
	virtual void setSource( fugio::PainterInterface *) Q_DECL_OVERRIDE {}
	virtual void paint( QPainter &pPainter, const QRect &pRect ) Q_DECL_OVERRIDE;

protected:
	QSharedPointer<fugio::PinInterface>		 mPinInputPainter;
	QSharedPointer<fugio::PinInterface>		 mPinInputRect;
	QSharedPointer<fugio::PinInterface>		 mPinInputPen;
	QSharedPointer<fugio::PinInterface>		 mPinInputBrush;
	QSharedPointer<fugio::PinInterface>		 mPinInputTransform;

	QSharedPointer<fugio::PinInterface>		 mPinOutputPainter;
	fugio::PainterInterface					*mValOutputPainter;
};

#endif // RECTNODE_H

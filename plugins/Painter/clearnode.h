#ifndef CLEARNODE_H
#define CLEARNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>
#include <fugio/core/variant_interface.h>

#include <fugio/image/painter_interface.h>

class ClearNode : public fugio::NodeControlBase, public fugio::PainterInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PainterInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Clear" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Clear_(Painter)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE ClearNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~ClearNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	// PainterInterface interface
public:
	virtual void setSource( fugio::PainterInterface *) Q_DECL_OVERRIDE {}
	virtual void paint( QPainter &pPainter, const QRect &pRect ) Q_DECL_OVERRIDE;

protected:
	QSharedPointer<fugio::PinInterface>		 mPinInputColour;

	QSharedPointer<fugio::PinInterface>		 mPinOutputPainter;
	fugio::PainterInterface					*mValOutputPainter;
};

#endif // CLEARNODE_H

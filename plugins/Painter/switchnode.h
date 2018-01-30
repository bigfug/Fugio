#ifndef SWITCHNODE_H
#define SWITCHNODE_H

#include <fugio/nodecontrolbase.h>
#include <fugio/core/variant_interface.h>
#include <fugio/image/painter_interface.h>

class SwitchNode : public fugio::NodeControlBase, public fugio::PainterInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PainterInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Switch Node" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Switch_(Painter)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE SwitchNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~SwitchNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	// PainterInterface interface
public:
	virtual void setSource( fugio::PainterInterface * ) Q_DECL_OVERRIDE {}
	virtual void paint( QPainter &pPainter, const QRect &pRect ) Q_DECL_OVERRIDE;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputSwitch;
	QSharedPointer<fugio::PinInterface>			 mPinInputPainter1;
	QSharedPointer<fugio::PinInterface>			 mPinInputPainter2;

	QSharedPointer<fugio::PinInterface>			 mPinOutputPainter;
	fugio::PainterInterface						*mValOutputPainter;
};


#endif // SWITCHNODE_H

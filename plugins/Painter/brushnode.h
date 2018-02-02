#ifndef BRUSHNODE_H
#define BRUSHNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>
#include <fugio/core/variant_interface.h>

class BrushNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Pen Node" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Brush_(Painter)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE BrushNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~BrushNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputColour;

	QSharedPointer<fugio::PinInterface>			 mPinOutputBrush;
	fugio::VariantInterface						*mValOutputBrush;
};

#endif // BRUSHNODE_H

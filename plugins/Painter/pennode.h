#ifndef PENNODE_H
#define PENNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>
#include <fugio/core/variant_interface.h>

class PenNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Pen Node" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Pen_(Painter)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE PenNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~PenNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputColour;

	QSharedPointer<fugio::PinInterface>			 mPinOutputPen;
	fugio::VariantInterface						*mValOutputPen;
};

#endif // PENNODE_H

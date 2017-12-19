#ifndef JOINRECTNODE_H
#define JOINRECTNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>
#include <fugio/core/variant_interface.h>

class JoinRectNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Makes a Rect from raw values." )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Join_(Rect)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE JoinRectNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~JoinRectNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputX;
	QSharedPointer<fugio::PinInterface>			 mPinInputY;
	QSharedPointer<fugio::PinInterface>			 mPinInputWidth;
	QSharedPointer<fugio::PinInterface>			 mPinInputHeight;

	QSharedPointer<fugio::PinInterface>			 mPinOutputRect;
	fugio::VariantInterface						*mValOutputRect;
};

#endif // JOINRECTNODE_H

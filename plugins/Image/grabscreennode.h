#ifndef GRABSCREENNODE_H
#define GRABSCREENNODE_H

#include <QObject>
#include <QImage>

#include <fugio/nodecontrolbase.h>

FUGIO_NAMESPACE_BEGIN
class ImageInterface;
FUGIO_NAMESPACE_END

class GrabScreenNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Grabs a screenshot" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Grab_Screen_(Image)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE GrabScreenNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~GrabScreenNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinTrigger;

	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::VariantInterface							*mValOutput;

	QImage									 mImage;
};

#endif // GRABSCREENNODE_H

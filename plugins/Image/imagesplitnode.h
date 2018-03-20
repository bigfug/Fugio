#ifndef IMAGESPLITNODE_H
#define IMAGESPLITNODE_H

#include <fugio/nodecontrolbase.h>

FUGIO_NAMESPACE_BEGIN
class ImageInterface;
FUGIO_NAMESPACE_END

class ImageSplitNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Splits an image into planes" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Split_(Image)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE ImageSplitNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~ImageSplitNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputImage;
};

#endif // IMAGESPLITNODE_H

#ifndef IMAGESIZENODE_H
#define IMAGESIZENODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

class ImageSizeNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Outputs the size of the input image" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Size_(Image)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE ImageSizeNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~ImageSizeNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputImage;

	QSharedPointer<fugio::PinInterface>			 mPinOutputSize;
	fugio::VariantInterface						*mValOutputSize;
};

#endif // IMAGESIZENODE_H

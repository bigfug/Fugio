#ifndef IMAGENODE_H
#define IMAGENODE_H

#include <QImage>

#include <fugio/nodecontrolbase.h>

#include <fugio/image/image.h>
#include <fugio/core/variant_interface.h>

class ImageNode : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Image" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Image" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit ImageNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~ImageNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual QWidget *gui( void );

	virtual void inputsUpdated( qint64 pTimeStamp );

private:
	QSharedPointer<fugio::PinInterface>			 mPinSize;
	fugio::VariantInterface						*mValSize;

	QSharedPointer<fugio::PinInterface>			 mPinImage;
	fugio::VariantInterface						*mImage;

	QImage										 mImageData;
};

#endif // IMAGENODE_H

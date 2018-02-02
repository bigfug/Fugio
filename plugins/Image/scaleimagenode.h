#ifndef SCALEIMAGENODE_H
#define SCALEIMAGENODE_H

#include <QObject>
#include <QImage>

#include <fugio/nodecontrolbase.h>

class ScaleImageNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Scales an image" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Scale_(Image)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE ScaleImageNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~ScaleImageNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputImage;
	QSharedPointer<fugio::PinInterface>			 mPinWidth;
	QSharedPointer<fugio::PinInterface>			 mPinHeight;

	QSharedPointer<fugio::PinInterface>			 mPinOutputImage;
	fugio::VariantInterface						*mValOutputImage;
};

#endif // SCALEIMAGENODE_H

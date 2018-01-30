#ifndef LEAPRECTIFYNODE_H
#define LEAPRECTIFYNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

class LeapRectifyNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Rectifies an image supplied from the Leap Motion sensor." )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Image_Rectify_(Leap)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE LeapRectifyNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~LeapRectifyNode( void ) {}

	// InterfaceNodeControl interface

	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputImage;
	QSharedPointer<fugio::PinInterface>			 mPinInputDistortion;
	QSharedPointer<fugio::PinInterface>			 mPinInputWidth;
	QSharedPointer<fugio::PinInterface>			 mPinInputHeight;

	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::VariantInterface							*mValOutput;
};


#endif // LEAPRECTIFYNODE_H

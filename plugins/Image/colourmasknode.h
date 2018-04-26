#ifndef COLOURMASKNODE_H
#define COLOURMASKNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

FUGIO_NAMESPACE_BEGIN
class ImageInterface;
FUGIO_NAMESPACE_END

class ColourMaskNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Creates a mask for a specfic colour" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Colour_Mask_(Image)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE ColourMaskNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~ColourMaskNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputImage;
	QSharedPointer<fugio::PinInterface>			 mPinColour;
	QSharedPointer<fugio::PinInterface>			 mPinHueMatch;

	QSharedPointer<fugio::PinInterface>			 mPinOutputImage;
	fugio::VariantInterface						*mValOutputImage;
};


#endif // COLOURMASKNODE_H

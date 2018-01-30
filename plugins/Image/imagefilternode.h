#ifndef IMAGEFILTERNODE_H
#define IMAGEFILTERNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>
#include <fugio/choice_interface.h>

FUGIO_NAMESPACE_BEGIN
class ImageInterface;
FUGIO_NAMESPACE_END

class ImageFilterNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Combines images with filters" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Filter_(Image)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE ImageFilterNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~ImageFilterNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputImage1;
	QSharedPointer<fugio::PinInterface>			 mPinInputImage2;

	QSharedPointer<fugio::PinInterface>			 mPinFilter;
	fugio::ChoiceInterface						*mValFilter;

	QSharedPointer<fugio::PinInterface>			 mPinOutputImage;
	fugio::VariantInterface						*mValOutputImage;
};

#endif // IMAGEFILTERNODE_H

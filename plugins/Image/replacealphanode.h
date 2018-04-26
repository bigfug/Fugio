#ifndef REPLACEALPHANODE_H
#define REPLACEALPHANODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>
#include <fugio/choice_interface.h>

FUGIO_NAMESPACE_BEGIN
class ImageInterface;
FUGIO_NAMESPACE_END

class ReplaceAlphaNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Replaces the alpha channel of an RGBA image" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Replace_Alpha_(Image)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE ReplaceAlphaNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~ReplaceAlphaNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputImage;

	QSharedPointer<fugio::PinInterface>			 mPinInputAlpha;

	QSharedPointer<fugio::PinInterface>			 mPinOutputImage;
	fugio::VariantInterface						*mValOutputImage;
};

#endif // REPLACEALPHANODE_H

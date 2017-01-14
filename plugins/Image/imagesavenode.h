#ifndef IMAGESAVENODE_H
#define IMAGESAVENODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>
#include <fugio/choice_interface.h>

FUGIO_NAMESPACE_BEGIN
class ImageInterface;
FUGIO_NAMESPACE_END

class ImageSaveNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Saves images" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Save_(Image)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE ImageSaveNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~ImageSaveNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputImage;

	QSharedPointer<fugio::PinInterface>			 mPinInputFormat;
	fugio::ChoiceInterface						*mValInputFormat;

	QSharedPointer<fugio::PinInterface>			 mPinInputDirectory;
	QSharedPointer<fugio::PinInterface>			 mPinInputOverwrite;
	QSharedPointer<fugio::PinInterface>			 mPinInputReset;
	QSharedPointer<fugio::PinInterface>			 mPinInputNumber;

	QSharedPointer<fugio::PinInterface>			 mPinOutputFilename;
	fugio::VariantInterface						*mValOutputFilename;

	int											 mFileNumber;
};

#endif // IMAGESAVENODE_H

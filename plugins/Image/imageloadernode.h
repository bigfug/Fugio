#ifndef IMAGELOADERNODE_H
#define IMAGELOADERNODE_H

#include <QObject>
#include <QImage>
#include <QSettings>
#include <QLabel>
#include <QPixmap>

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/image/image.h>
#include <fugio/file/filename_interface.h>

#include <fugio/nodecontrolbase.h>

class ImageLoaderNode : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Loads an image" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Load_(Image)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit ImageLoaderNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~ImageLoaderNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual QWidget *gui( void );

	virtual void inputsUpdated( qint64 pTimeStamp );

signals:
	void pixmapUpdated( const QPixmap &pPixMap );

private:
	QSharedPointer<fugio::PinInterface>			 mPinFileName;
	fugio::FilenameInterface					*mValInputFilename;

	QSharedPointer<fugio::PinInterface>			 mPinImage;
	fugio::VariantInterface						*mImage;

	QImage										 mImageData;
};

#endif // IMAGELOADERNODE_H

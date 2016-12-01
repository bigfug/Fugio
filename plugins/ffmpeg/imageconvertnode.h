#ifndef IMAGECONVERTNODE_H
#define IMAGECONVERTNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

#include <fugio/image/image_interface.h>

#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif

extern "C"
{
	#include <libswscale/swscale.h>
	#include <libavutil/frame.h>
}

class ImageConvertNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Image_Convert" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

	void clearImage();

public:
	Q_INVOKABLE ImageConvertNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~ImageConvertNode( void ) {}

	// NodeControlInterface interface
public:
	virtual QWidget *gui() Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual void loadSettings(QSettings &pSettings) Q_DECL_OVERRIDE;
	virtual void saveSettings(QSettings &pSettings) const Q_DECL_OVERRIDE;

protected:

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInput;

	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::ImageInterface						*mOutput;

	fugio::ImageInterface::Format				 mCurrImageFormat;
	fugio::ImageInterface::Format				 mLastImageFormat;

	SwsContext									*mScaleContext;

	AVPixelFormat								 mSrcPixFmt;

	uint8_t										*mDstDat[ AV_NUM_DATA_POINTERS ];
	int											 mDstLen[ AV_NUM_DATA_POINTERS ];

	static QMap<fugio::ImageInterface::Format,QString>	 mImageFormatMap;
};

#endif // IMAGECONVERTNODE_H

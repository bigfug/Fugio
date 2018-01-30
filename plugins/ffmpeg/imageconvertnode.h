#ifndef IMAGECONVERTNODE_H
#define IMAGECONVERTNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

#include <fugio/image/image.h>

#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif

#if defined( FFMPEG_SUPPORTED )
extern "C"
{
	#include <libswscale/swscale.h>
	#include <libavutil/frame.h>
}
#endif

class ImageConvertNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Image_Convert_(FFMPEG)" ) )
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

protected slots:
	void formatChanged( const QString &pFormat );

private:
	// From QImage

	static QTransform trueMatrix(const QTransform &matrix, int w, int h)
	{
		const QRectF rect(0, 0, w, h);
		const QRect mapped = matrix.mapRect(rect).toAlignedRect();
		const QPoint delta = mapped.topLeft();
		return matrix * QTransform().translate(-delta.x(), -delta.y());
	}

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputImage;
	QSharedPointer<fugio::PinInterface>			 mPinInputWidth;
	QSharedPointer<fugio::PinInterface>			 mPinInputHeight;

	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::VariantInterface						*mValOutputImage;

	fugio::ImageFormat							 mCurrImageFormat;
	fugio::ImageFormat							 mLastImageFormat;
	QSize										 mLastImageSize;

#if defined( FFMPEG_SUPPORTED )
	SwsContext									*mScaleContext;

	AVPixelFormat								 mSrcPixFmt;

	uint8_t										*mDstDat[ AV_NUM_DATA_POINTERS ];
	int											 mDstLen[ AV_NUM_DATA_POINTERS ];
#endif

	static QMap<fugio::ImageFormat,QString>	 mImageFormatMap;
};

#endif // IMAGECONVERTNODE_H

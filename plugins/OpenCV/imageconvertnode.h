#ifndef IMAGECONVERTNODE_H
#define IMAGECONVERTNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

#include <fugio/opencv/uuid.h>

#include <fugio/pin_interface.h>
#include <fugio/image/image.h>

#if defined( OPENCV_SUPPORTED )
#include <opencv2/core/core.hpp>
#endif

class ImageConvertNode : public fugio::NodeControlBase
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit ImageConvertNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~ImageConvertNode( void ) {}

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual void loadSettings( QSettings &pSettings ) Q_DECL_OVERRIDE;
	virtual void saveSettings( QSettings &pSettings ) const Q_DECL_OVERRIDE;

	virtual QWidget *gui( void ) Q_DECL_OVERRIDE;

signals:
	void targetUpdated( const QString &pName );

protected slots:
	void targetChanged( const QString &pName );

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputImage;
	QSharedPointer<fugio::PinInterface>			 mPinInputThreshold;

	QSharedPointer<fugio::PinInterface>			 mPinOutputImage;
	fugio::VariantInterface						*mValOutputImage;

	fugio::ImageFormat				 mDestinationFormat;

	static QMap<QString,fugio::ImageFormat>	 mFormats;

#if defined( OPENCV_SUPPORTED )
	cv::Mat										 mMatImg;
#endif
};

#endif // IMAGECONVERTNODE_H

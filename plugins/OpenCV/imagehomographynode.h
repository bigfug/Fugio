#ifndef IMAGEHOMOGRAPHYNODE_H
#define IMAGEHOMOGRAPHYNODE_H

#include <QImage>
#include <QRectF>

#include <fugio/nodecontrolbase.h>

#include <fugio/opencv/uuid.h>

#include <fugio/pin_interface.h>
#include <fugio/image/image.h>
#include <fugio/core/variant_interface.h>

#if defined( OPENCV_SUPPORTED )
#include <opencv2/core/core.hpp>
#endif

class ImageHomographyNode : public fugio::NodeControlBase
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit ImageHomographyNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~ImageHomographyNode( void );

	virtual QUuid uuid( void )
	{
		const static QUuid uuid = NID_OPENCV_IMAGE_HOMOGRAPHY;

		return( uuid );
	}

	virtual bool initialise( void );

	virtual void inputsUpdated( qint64 pTimeStamp );

private slots:
	void onContextFrame( void );

private:
	void updateHomography( void );

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputWidth;
	QSharedPointer<fugio::PinInterface>			 mPinInputHeight;
	QSharedPointer<fugio::PinInterface>			 mPinInputImage;
	QSharedPointer<fugio::PinInterface>			 mPinInputMargin;

	QSharedPointer<fugio::PinInterface>			 mPinInputMouseButtonLeft;
	QSharedPointer<fugio::PinInterface>			 mPinInputMousePosition;

	QSharedPointer<fugio::PinInterface>			 mPinInputWarp;

	QSharedPointer<fugio::PinInterface>			 mPinOutputImage;
	fugio::VariantInterface							*mOutputImage;

	QSharedPointer<fugio::PinInterface>			 mPinOutputMatrix;
	fugio::VariantInterface						*mOutputMatrix;

	QSharedPointer<fugio::PinInterface>			 mPinOutputInverse;
	fugio::VariantInterface						*mOutputInverse;

#if defined( OPENCV_SUPPORTED )
	cv::Mat									 mMatHom;
	cv::Mat									 mMatDst;
#endif

	QImage									 mImgSrc;
	QImage									 mImgDst;

#if defined( OPENCV_SUPPORTED )
    QVector<cv::Point2f>					 mPntSrc;
	QVector<cv::Point2f>					 mPntDst;
#endif

	int										 mCurIdx;

	QSize									 mLstSzeDst;
	QSize									 mLstSzeSrc;
	bool									 mLstBut;
	QPointF									 mLstPos;
	int										 mLstMargin;

	QPointF									 mImgScl;
};

#endif // IMAGEHOMOGRAPHYNODE_H

#include "solvepnpnode.h"

#include <vector>

#include <QVector3D>

#include "opencvplugin.h"

#include <Eigen/Dense>

Q_DECLARE_METATYPE( Eigen::MatrixXd )

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>
#include <fugio/pin_variant_iterator.h>

#include <fugio/image/uuid.h>
#include <fugio/math/uuid.h>

#if defined( OPENCV_SUPPORTED )
#include <opencv2/calib3d.hpp>
#include <opencv2/core/eigen.hpp>
#endif

SolvePNPNode::SolvePNPNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_OBJECT_POINTS, "0a55de2d-5e09-4096-9b5b-0d7795130a6b" );
	FUGID( PIN_INPUT_IMAGE_POINTS, "dddd74b1-058d-4b3e-874d-4fa0a47c69b0" );
	FUGID( PIN_INPUT_CAMERA_ARRAY, "9454dfe6-f41f-4fe1-b395-c895a93c68a5" );
	FUGID( PIN_INPUT_DIST_COEFFS, "07c017bc-84b6-47c8-a419-8dc07956b894" );
	FUGID( PIN_OUTPUT_ROTATION, "66117174-1afc-46aa-bf89-7c685c2eea81" );
	FUGID( PIN_OUTPUT_TRANSLATION, "e96add73-a556-4e58-ba95-1cbb963150e4" );

	mPinInputObjectPoints = pinInput( tr( "Object Points" ), PIN_INPUT_OBJECT_POINTS );

	mPinInputImagePoints = pinInput( tr( "Image Points" ), PIN_INPUT_IMAGE_POINTS );

	mPinInputCameraArray = pinInput( tr( "Camera Array" ), PIN_INPUT_CAMERA_ARRAY );

	mPinInputDistCoeffs = pinInput( tr( "Dist Coeffs" ), PIN_INPUT_DIST_COEFFS );

	mValOutputRotation = pinOutput<fugio::VariantInterface *>( tr( "Rotation" ), mPinOutputRotation, PID_MATRIX, PIN_OUTPUT_ROTATION );

	mValOutputTranslation = pinOutput<fugio::VariantInterface *>( tr( "Translation" ), mPinOutputTranslation, PID_MATRIX, PIN_OUTPUT_TRANSLATION );
}

void SolvePNPNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	std::vector<cv::Point3f>	ObjectPoints;
	std::vector<cv::Point2f>	ImagePoints;

	fugio::PinVariantIterator	InputObjectPoints( mPinInputObjectPoints );

	for( int i = 0 ; i < InputObjectPoints.count() ; i++ )
	{
		QVector3D		P = InputObjectPoints.index( i ).value<QVector3D>();

		ObjectPoints.push_back( cv::Point3f( P.x(), P.y(), P.z() ) );
	}

	fugio::PinVariantIterator	InputImagePoints( mPinInputImagePoints );

	for( int i = 0 ; i < InputImagePoints.count() ; i++ )
	{
		QPointF			P = InputImagePoints.index( i ).toPointF();

		ImagePoints.push_back( cv::Point2f( P.x(), P.y() ) );
	}

	Eigen::MatrixXd				InputCameraArray = variant<Eigen::MatrixXd>( mPinInputCameraArray );
	Eigen::MatrixXd				InputDistCoeffs  = variant<Eigen::MatrixXd>( mPinInputDistCoeffs );

	cv::Mat						CameraArray;
	cv::Mat						DistCoeffs;

	cv::eigen2cv( InputCameraArray, CameraArray );
	cv::eigen2cv( InputDistCoeffs,  DistCoeffs );

	cv::Mat						Rotation;
	cv::Mat						Translation;

	try
	{
//		if( cv::solvePnPRansac( ObjectPoints, ImagePoints, CameraArray, DistCoeffs, Rotation, Translation ) )
		if( cv::solvePnP( ObjectPoints, ImagePoints, CameraArray, DistCoeffs, Rotation, Translation ) )
		{
			Eigen::MatrixXd		R, T;

			cv::cv2eigen( Rotation, R );
			cv::cv2eigen( Translation, T );

			QVariant		V;

			V.setValue( R );

			mValOutputRotation->setVariant( V );

			V.setValue( T );

			mValOutputTranslation->setVariant( V );

			pinUpdated( mPinOutputRotation );
			pinUpdated( mPinOutputTranslation );
		}
	}
	catch( cv::Exception e )
	{
		//qDebug() << QString::fromStdString( e.msg );
	}
}

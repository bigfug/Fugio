#include "projectpointsnode.h"

#include <vector>

#include <QVector3D>

#include "opencvplugin.h"

#include "../../libs/eigen3/Eigen/Dense"

Q_DECLARE_METATYPE( Eigen::MatrixXd )

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>
#include <fugio/pin_variant_iterator.h>

#include <fugio/image/uuid.h>
#include <fugio/math/uuid.h>

#if defined( OPENCV_SUPPORTED )
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/core/eigen.hpp>
#endif

ProjectPointsNode::ProjectPointsNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_OBJECT_POINTS, "0a55de2d-5e09-4096-9b5b-0d7795130a6b" );
	FUGID( PIN_INPUT_ROTATION, "dddd74b1-058d-4b3e-874d-4fa0a47c69b0" );
	FUGID( PIN_INPUT_TRANSLATION, "9454dfe6-f41f-4fe1-b395-c895a93c68a5" );
	FUGID( PIN_INPUT_CAMERA_MATRIX, "07c017bc-84b6-47c8-a419-8dc07956b894" );
	FUGID( PIN_INPUT_DIST_COEFFS, "66117174-1afc-46aa-bf89-7c685c2eea81" );
	FUGID( PIN_OUTPUT_IMAGE_POINTS, "e96add73-a556-4e58-ba95-1cbb963150e4" );

	mPinInputObjectPoints = pinInput( tr( "Object Points" ), PIN_INPUT_OBJECT_POINTS );

	mPinInputRotation = pinInput( tr( "Rotation" ), PIN_INPUT_ROTATION );

	mPinInputTranslation = pinInput( tr( "Translation" ), PIN_INPUT_TRANSLATION );

	mPinInputCameraMatrix = pinInput( tr( "Camera Matrix" ), PIN_INPUT_CAMERA_MATRIX );

	mPinInputDistCoeffs = pinInput( tr( "Dist Coeffs" ), PIN_INPUT_DIST_COEFFS );

	mValOutputImagePoints = pinOutput<fugio::VariantInterface *>( tr( "Image Points" ), mPinOutputImagePoints, PID_POINT, PIN_OUTPUT_IMAGE_POINTS );
}

void ProjectPointsNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

#if defined( OPENCV_SUPPORTED )
	std::vector<cv::Point3f>	ObjectPoints;

	fugio::PinVariantIterator	InputObjectPoints( mPinInputObjectPoints );

	for( int i = 0 ; i < InputObjectPoints.count() ; i++ )
	{
		QVector3D		P = InputObjectPoints.index( i ).value<QVector3D>();

		ObjectPoints.push_back( cv::Point3f( P.x(), P.y(), P.z() ) );
	}

	Eigen::MatrixXd				InputRotation    = variant<Eigen::MatrixXd>( mPinInputRotation );
	Eigen::MatrixXd				InputTranslation = variant<Eigen::MatrixXd>( mPinInputTranslation );
	Eigen::MatrixXd				InputCameraMatrix = variant<Eigen::MatrixXd>( mPinInputCameraMatrix );
	Eigen::MatrixXd				InputDistCoeffs = variant<Eigen::MatrixXd>( mPinInputDistCoeffs );

	cv::Mat						Rotation;
	cv::Mat						Translation;
	cv::Mat						CameraMatrix;
	cv::Mat						DistCoeffs;

	cv::eigen2cv( InputRotation, Rotation );
	cv::eigen2cv( InputTranslation, Translation );
	cv::eigen2cv( InputCameraMatrix, CameraMatrix );
	cv::eigen2cv( InputDistCoeffs, DistCoeffs );

	try
	{
		std::vector<cv::Point2f>	ImagePoints;

		cv::projectPoints( ObjectPoints, Rotation, Translation, CameraMatrix, DistCoeffs, ImagePoints );

		mValOutputImagePoints->setVariantCount( ImagePoints.size() );

		for( int i = 0 ; i < ImagePoints.size() ; i++ )
		{
			cv::Point2f	P = ImagePoints.at( i );

			mValOutputImagePoints->setVariant( i, QPointF( P.x, P.y ) );
		}

		pinUpdated( mPinOutputImagePoints );
	}
	catch( cv::Exception e )
	{
		qDebug() << QString::fromStdString( e.msg );
	}
#endif
}

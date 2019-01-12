#include "rodriguesnode.h"

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

RodriguesNode::RodriguesNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	FUGID( PIN_INPUT_MATRIX, "0a55de2d-5e09-4096-9b5b-0d7795130a6b" );
	FUGID( PIN_OUTPUT_MATRIX, "66117174-1afc-46aa-bf89-7c685c2eea81" );
	FUGID( PIN_OUTPUT_JACOBIAN, "e96add73-a556-4e58-ba95-1cbb963150e4" );

	mPinInputMatrix = pinInput( tr( "Matrix" ), PIN_INPUT_MATRIX );

	mValOutputMatrix = pinOutput<fugio::VariantInterface *>( tr( "Matrix" ), mPinOutputMatrix, PID_MATRIX, PIN_OUTPUT_MATRIX );

	mValOutputJacobian = pinOutput<fugio::VariantInterface *>( tr( "Jacobian" ), mPinOutputJacobian, PID_MATRIX, PIN_OUTPUT_JACOBIAN );
}

void RodriguesNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	Eigen::MatrixXd				SourceInputMatrix = variant<Eigen::MatrixXd>( mPinInputMatrix );

	cv::Mat						InputMatrix;
	cv::Mat						OutputMatrix;
	cv::Mat						Jacobian;

	cv::eigen2cv( SourceInputMatrix, InputMatrix );


	try
	{
		cv::Rodrigues( InputMatrix, OutputMatrix, Jacobian );

		Eigen::MatrixXd		O, J;

		cv::cv2eigen( OutputMatrix, O );
		cv::cv2eigen( Jacobian, J );

		QVariant		V;

		V.setValue( O );

		mValOutputMatrix->setVariant( V );

		V.setValue( J );

		mValOutputJacobian->setVariant( V );

		pinUpdated( mPinOutputMatrix );
		pinUpdated( mPinOutputJacobian );
	}
	catch( cv::Exception e )
	{
		qDebug() << QString::fromStdString( e.msg );
	}
}

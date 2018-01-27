#include "kinectconfigdialog.h"
#include "ui_kinectconfigdialog.h"

KinectConfigDialog::KinectConfigDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::KinectConfigDialog)
{
	ui->setupUi(this);
}

KinectConfigDialog::~KinectConfigDialog()
{
	delete ui;
}

KinectCameraType KinectConfigDialog::colourCameraType() const
{
	switch( ui->mColourType->currentIndex() )
	{
		case 0:	return( NONE );
		case 1:	return( COLOUR );
		case 2:	return( INFRARED );
	}

	return( NONE );
}

KinectCameraResolution KinectConfigDialog::colourCameraResolution() const
{
	switch( ui->mColourResolution->currentIndex() )
	{
		case 0: return( RES_640x480 );
	}

	return( RES_NONE );
}

KinectCameraResolution KinectConfigDialog::depthCameraResolution() const
{
	switch( ui->mDepthResolution->currentIndex() )
	{
		case 1: return( RES_640x480 );
	}

	return( RES_NONE );
}

bool KinectConfigDialog::detectUsers() const
{
	return( ui->mDetectUsers->isChecked() );
}

bool KinectConfigDialog::skeletonEnabled() const
{
	return( ui->mSkeletonEnabled->isChecked() );
}

bool KinectConfigDialog::skeletonNearMode() const
{
	return( ui->mSkeletonNearMode->isChecked() );
}

bool KinectConfigDialog::skeletonSeatedMode() const
{
	return( ui->mSkeletonSeatedMode->isChecked() );
}

void KinectConfigDialog::setColourCameraType( KinectCameraType pCameraType )
{
	switch( pCameraType )
	{
		case NONE:
			ui->mColourType->setCurrentIndex( 0 );
			break;

		case COLOUR:
			ui->mColourType->setCurrentIndex( 1 );
			break;

		case INFRARED:
			ui->mColourType->setCurrentIndex( 2 );
			break;
	}

	on_mColourType_activated( ui->mColourType->currentIndex() );
}

void KinectConfigDialog::setColourCameraResolution( KinectCameraResolution pResolution )
{
	switch( pResolution )
	{
		case RES_NONE:
			break;

		case RES_640x480:
			ui->mColourResolution->setCurrentIndex( 0 );
			break;
	}
}

void KinectConfigDialog::setDepthCameraResolution(KinectCameraResolution pResolution)
{
	switch( pResolution )
	{
		case RES_NONE:
			ui->mDepthResolution->setCurrentIndex( 0 );
			break;

		case RES_640x480:
			ui->mDepthResolution->setCurrentIndex( 1 );
			break;
	}

	on_mDepthResolution_activated( ui->mDepthResolution->currentIndex() );
}

void KinectConfigDialog::setDetectUsers(bool pEnabled)
{
	ui->mDetectUsers->setChecked( pEnabled );
}

void KinectConfigDialog::setSkeletonEnabled(bool pEnabled)
{
	ui->mSkeletonEnabled->setChecked( pEnabled );

	on_mSkeletonEnabled_clicked( pEnabled );
}

void KinectConfigDialog::setSkeletonNearMode(bool pEnabled)
{
	ui->mSkeletonNearMode->setChecked( pEnabled );
}

void KinectConfigDialog::setSkeletonSeatedMode(bool pEnabled)
{
	ui->mSkeletonSeatedMode->setChecked( pEnabled );
}

void KinectConfigDialog::on_mColourType_activated(int index)
{
	if( index == 0 )
	{
		ui->mColourResolution->setEnabled( false );
	}
	else
	{
		ui->mColourResolution->setEnabled( true );
	}
}

void KinectConfigDialog::on_mDepthResolution_activated(int index)
{
	if( index == 0 )
	{
		ui->mDetectUsers->setEnabled( false );

		ui->mSkeletonEnabled->setEnabled( false );

		on_mSkeletonEnabled_clicked( false );
	}
	else
	{
		ui->mDetectUsers->setEnabled( true );

		ui->mSkeletonEnabled->setEnabled( true );

		on_mSkeletonEnabled_clicked( ui->mSkeletonEnabled->isChecked() );
	}
}

void KinectConfigDialog::on_mSkeletonEnabled_clicked(bool checked)
{
	if( checked )
	{
		ui->mSkeletonNearMode->setEnabled( true );
		ui->mSkeletonSeatedMode->setEnabled( true );
	}
	else
	{
		ui->mSkeletonNearMode->setEnabled( false );
		ui->mSkeletonSeatedMode->setEnabled( false );
	}
}

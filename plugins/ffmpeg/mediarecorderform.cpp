#include "mediarecorderform.h"
#include "ui_mediarecorderform.h"
#include "mediarecordernode.h"
#include "mediapreset/mediapresetmanager.h"

//#include "app.h"
//#include "mainwindow.h"
//#include "io/ioinput.h"
//#include "io/iooutput.h"
//#include "io/iolink.h"

#include <QProgressDialog>
#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>
#include <QMainWindow>

#include <fugio/context_interface.h>

QString timeToString( qreal pTime )
{
	int		Mils = fmod( pTime, 1.0 ) * 1000.0;
	int		Secs = floor( pTime );
	int		Mins = Secs / 60;

	return( QString( "%1:%2:%3" )
			.arg( Mins )
			.arg( Secs % 60, 2, 10, QChar( '0' ) )
			.arg( Mils, 3, 10, QChar( '0' ) ) );
}

MediaRecorderForm::MediaRecorderForm( MediaRecorderNode &pRecorder, QWidget *parent ) :
	QWidget(parent),
	ui(new Ui::MediaRecorderForm), mMediaPreset( 0 ), mProgressDialog( 0 ), VR( pRecorder )
{
	ui->setupUi( this );

	ui->mMediaPreset->addItems( MediaPresetManager::instance()->presetNames() );

	ui->mMediaPreset->setCurrentText( VR.mediaPresetName() );

	updatePreset();

	ui->mVideoFit->setCurrentIndex( VR.frameScale() );

	ui->mSliderQuality->setValue( VR.quality() * qreal( ui->mSliderQuality->maximum() ) );
	ui->mSliderSpeed->setValue( VR.speed() * qreal( ui->mSliderSpeed->maximum() ) );

	ui->mTimeStart->setText( timeToString( VR.timeStart() ) );
	ui->mTimeDuration->setText( timeToString( VR.timeDuration() ) );
	ui->mTimeEnd->setText( timeToString( VR.timeEnd() ) );

	connect( ui->mMediaPreset, SIGNAL(currentIndexChanged(int)), this, SLOT(mediaPresetChanged(int)) );

	connect( ui->mSliderSpeed, SIGNAL(sliderMoved(int)), this, SLOT(speedUpdated(int)) );
	connect( ui->mSliderQuality, SIGNAL(sliderMoved(int)), this, SLOT(qualityUpdated(int)) );

	connect( ui->mVideoFit, SIGNAL(activated(int)), this, SLOT(videoFitUpdated(int)) );

	connect( ui->mTimeStart, SIGNAL(textEdited(QString)), this, SLOT(timeStartEdited(QString)) );

	connect( ui->mTimeDuration, SIGNAL(textEdited(QString)), this, SLOT(timeDurationEdited(QString)) );

	connect( ui->mTimeEnd, SIGNAL(textEdited(QString)), this, SLOT(timeEndEdited(QString)) );

	connect( &VR, SIGNAL(recordingStarted()), this, SLOT(recordingStarted()) );
	connect( &VR, SIGNAL(recordingStopped()), this, SLOT(recordingStopped()) );
	connect( &VR, SIGNAL(recording(qreal)), this, SLOT(recording(qreal)) );
}

MediaRecorderForm::~MediaRecorderForm()
{
	VR.setMediaPreset( nullptr );

	if( mMediaPreset )
	{
		delete mMediaPreset;

		mMediaPreset = 0;
	}

	delete ui;
}

QString MediaRecorderForm::presetName()
{
	return( ui->mMediaPreset->currentText() );
}

qreal stringToTime( const QString &pString )
{
	QStringList		l = pString.split( ':', QString::SkipEmptyParts );

	if( l.size() != 3 )
	{
		return( -1 );
	}

	qreal			 Mins = l[ 0 ].toLongLong() * 60;
	qreal			 Secs = l[ 1 ].toLongLong() % 60;
	qreal			 Mils = l[ 2 ].toLongLong() % 1000;

	return( Mins + Secs + ( Mils / 1000.0 ) );
}

void MediaRecorderForm::on_mButtonRecord_clicked()
{
	QSettings		Settings;

	if( !mMediaPreset )
	{
		return;
	}

	qreal		TimeStart    = stringToTime( ui->mTimeStart->text() );
	qreal		TimeDuration = stringToTime( ui->mTimeDuration->text() );
	qreal		TimeEnd      = stringToTime( ui->mTimeEnd->text() );

	if( TimeDuration <= 0 )
	{
		TimeDuration = TimeEnd - TimeStart;
	}

	if( TimeDuration <= 0 )
	{
		QMessageBox::warning( this, tr( "Media Recorder" ), tr( "You need to set a valid time duration or time end" ) );

		return;
	}

	QString			FileName = QFileDialog::getSaveFileName( this, tr( "Save Media" ), Settings.value( "media-recorder-directory", QDir::currentPath() ).toString(), mMediaPreset->filter() );

	if( FileName.isEmpty() )
	{
		return;
	}

	Settings.setValue( "media-recorder-directory", QFileInfo( FileName ).path() );

	VR.setMediaPreset( mMediaPreset );

	VR.record( FileName );
}

void MediaRecorderForm::mediaPresetChanged( int index )
{
	Q_UNUSED( index )

	updatePreset();

	if( mMediaPreset )
	{
		if( !mMediaPreset->hasVideo() )
		{
			ui->mVideoFit->setEnabled( false );
			ui->mSliderQuality->setEnabled( false );
			ui->mSliderSpeed->setEnabled( false );
		}
		else
		{
			ui->mVideoFit->setEnabled( true );
			ui->mSliderQuality->setEnabled( true );
			ui->mSliderSpeed->setEnabled( true );
		}
	}
}

void MediaRecorderForm::updatePreset()
{
	VR.setMediaPreset( nullptr );

	if( mMediaPreset )
	{
		delete mMediaPreset;

		mMediaPreset = 0;
	}

	if( ( mMediaPreset = MediaPresetManager::instance()->preset( ui->mMediaPreset->currentText() ) ) == 0 )
	{
		return;
	}

	VR.setMediaPresetName( ui->mMediaPreset->currentText() );

	VR.setMediaPreset( mMediaPreset );
}

void MediaRecorderForm::recordingStarted()
{
	ui->mSelectVideo->setEnabled( false );
	ui->mMediaPreset->setEnabled( false );

	ui->mVideoFit->setEnabled( false );

	ui->mTimeStart->setEnabled( false );
	ui->mTimeEnd->setEnabled( false );
	ui->mTimeDuration->setEnabled( false );

	ui->mButtonRecord->setEnabled( false );

	qreal		TimeStart    = VR.timeStart();
	qreal		TimeDuration = VR.timeDuration();
	qreal		TimeEnd      = VR.timeEnd();

	if( TimeDuration <= 0 )
	{
		TimeDuration = TimeEnd - TimeStart;
	}

	QMainWindow			*MainWindow = VR.node()->context()->global()->mainWindow();

	if( ( mProgressDialog = new QProgressDialog( tr( "Recording" ), tr( "Cancel" ), 0, TimeDuration * 10, MainWindow ) ) != 0 )
	{
		mProgressDialog->setWindowModality( Qt::WindowModal );

		mProgressDialog->setMinimumDuration( 0 );

		connect( mProgressDialog, SIGNAL(canceled()), &VR, SLOT(cancel()) );
	}
}

void MediaRecorderForm::recordingStopped()
{
	if( mProgressDialog )
	{
		mProgressDialog->deleteLater();

		mProgressDialog = 0;
	}

	ui->mSelectVideo->setEnabled( true );
	ui->mMediaPreset->setEnabled( true );

	ui->mVideoFit->setEnabled( true );

	ui->mTimeStart->setEnabled( true );
	ui->mTimeEnd->setEnabled( true );
	ui->mTimeDuration->setEnabled( true );

	ui->mButtonRecord->setEnabled( true );
}

void MediaRecorderForm::recording( qreal pTimeStamp )
{
	if( mProgressDialog )
	{
		mProgressDialog->setValue( pTimeStamp * 10 );
	}
}

void MediaRecorderForm::qualityUpdated( int position )
{
	VR.setQuality( qreal( position ) / qreal( ui->mSliderQuality->maximum() ) );
}

void MediaRecorderForm::speedUpdated(int position)
{
	VR.setSpeed( qreal( position ) / qreal( ui->mSliderSpeed->maximum() ) );
}

void MediaRecorderForm::videoFitUpdated( int index )
{
	VR.setFrameScale( MediaRecorderNode::FrameScale( index ) );
}

void MediaRecorderForm::timeStartEdited( const QString pText )
{
	VR.setTimeStart( stringToTime( pText ) );
}

void MediaRecorderForm::timeDurationEdited( const QString pText )
{
	VR.setTimeDuration( stringToTime( pText ) );
}

void MediaRecorderForm::timeEndEdited( const QString pText )
{
	VR.setTimeEnd( stringToTime( pText ) );
}

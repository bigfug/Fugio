#ifndef KINECTCONFIGDIALOG_H
#define KINECTCONFIGDIALOG_H

#include <QDialog>

namespace Ui {
class KinectConfigDialog;
}

typedef enum KinectCameraType
{
	NONE, COLOUR, INFRARED
} KinectCameraType;

typedef enum KinectCameraResolution
{
	RES_NONE, RES_640x480
} KinectCameraResolution;

class KinectConfigDialog : public QDialog
{
	Q_OBJECT

public:
	explicit KinectConfigDialog(QWidget *parent = 0);
	~KinectConfigDialog();

	KinectCameraType colourCameraType( void ) const;
	KinectCameraResolution colourCameraResolution( void ) const;

	KinectCameraResolution depthCameraResolution( void ) const;

	bool detectUsers( void ) const;

	bool skeletonEnabled( void ) const;
	bool skeletonNearMode( void ) const;
	bool skeletonSeatedMode( void ) const;

public slots:
	void setColourCameraType( KinectCameraType pCameraType );
	void setColourCameraResolution( KinectCameraResolution pResolution );

	void setDepthCameraResolution( KinectCameraResolution pResolution );
	void setDetectUsers( bool pEnabled );

	void setSkeletonEnabled( bool pEnabled );
	void setSkeletonNearMode( bool pEnabled );
	void setSkeletonSeatedMode( bool pEnabled );

private slots:
	void on_mColourType_activated(int index);

	void on_mDepthResolution_activated(int index);

	void on_mSkeletonEnabled_clicked(bool checked);

private:
	Ui::KinectConfigDialog *ui;
};

#endif // KINECTCONFIGDIALOG_H

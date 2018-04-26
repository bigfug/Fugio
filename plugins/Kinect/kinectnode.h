#ifndef KINECT_H
#define KINECT_H

#include <QObject>

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/image/image.h>
#include <fugio/choice_interface.h>

#include <fugio/nodecontrolbase.h>

#ifdef KINECT_SUPPORTED

#define WIN32_LEAN_AND_MEAN
#define INC_OLE2

#include <Windows.h>
#include <QWinEventNotifier>

#include <NuiApi.h>
#endif

#include <fugio/kinect/uuid.h>

#include "interactionclient.h"

#include "kinectconfigdialog.h"

class KinectNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Kinect v1 Node" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Kinect" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit KinectNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~KinectNode( void );

	// NodeControlInterface interface
public:
	virtual void loadSettings( QSettings &pSettings ) Q_DECL_OVERRIDE;
	virtual void saveSettings( QSettings &pSettings ) const Q_DECL_OVERRIDE;

	virtual bool initialise( void ) Q_DECL_OVERRIDE;

	virtual bool deinitialise( void ) Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QWidget *gui() Q_DECL_OVERRIDE;

private slots:
	void frameStart( qint64 pTimeStamp );

	void editConfig( void );

#ifdef KINECT_SUPPORTED
	static QSize nuiSize( NUI_IMAGE_RESOLUTION pNuiRes );

	static fugio::ImageFormat nuiFormat( NUI_IMAGE_TYPE pNuiType );

	void deviceOpen( int pDeviceIndex );

	void deviceClose( void );

	void colourFrame( HANDLE pHandle );

	void depthFrame( HANDLE pHandle );

	void skeletonFrame( HANDLE pHandle );
#endif

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputIndex;

	QSharedPointer<fugio::PinInterface>			 mPinInputElevation;

	QSharedPointer<fugio::PinInterface>			 mPinOutputFloorClipPlane;
	fugio::VariantInterface						*mValOutputFloorClipPlane;

	QSharedPointer<fugio::PinInterface>			 mPinOutputCamera;
	fugio::VariantInterface						*mValOutputCamera;

	QSharedPointer<fugio::PinInterface>			 mPinOutputDepth;
	fugio::VariantInterface						*mValOutputDepth;

	QSharedPointer<fugio::PinInterface>			 mPinOutputUser;
	fugio::VariantInterface						*mValOutputUser;

	QSharedPointer<fugio::PinInterface>			 mPinOutputElevation;
	fugio::VariantInterface						*mValOutputElevation;

	QSharedPointer<fugio::PinInterface>			 mPinOutputSkeleton;
	fugio::VariantInterface						*mValOutputSkeleton;

	qint64										 mLastElevationCommand;
	qint64										 mLastElevationUpdate;

	int											 mSensorIndex;

	KinectCameraType							 mCameraType;
	KinectCameraResolution						 mCameraResolution;

	KinectCameraResolution						 mDepthResolution;

	bool										 mUserTracking;

	bool										 mSkeletonTracking;
	bool										 mSkeletonNearMode;
	bool										 mSkeletonSeatedMode;

#ifdef KINECT_SUPPORTED
	INuiSensor*							         m_pNuiSensor;

	HANDLE									     m_pColorStreamHandle;
	HANDLE									     m_pDepthStreamHandle;

	HANDLE									     mNextColorFrameEvent;
	HANDLE									     mNextDepthFrameEvent;
	HANDLE									     mSkeletonEvent;
	HANDLE									     mInteractionEvent;

	LONG										 mSensorElevation;

	bool										 mFrameReceived;

	QWinEventNotifier							 mColourFrameNotifier;
	QWinEventNotifier							 mDepthFrameNotifier;
	QWinEventNotifier							 mSkeletonFrameNotifier;

#if defined( InteractionClient )
	InteractionClient							 mInteractionClient;
	INuiInteractionStream						*mInteractionStream;
#endif

#endif
};

#endif // KINECT_H

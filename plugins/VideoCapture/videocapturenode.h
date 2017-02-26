#ifndef VIDEOCAPTURENODE_H
#define VIDEOCAPTURENODE_H

#include <fugio/nodecontrolbase.h>
#include <fugio/core/variant_interface.h>
#include <fugio/image/image_interface.h>

#if defined( VIDEOCAPTURE_SUPPORTED )
#include <videocapture/Capture.h>
#endif

class VideoCaptureNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Video Capture" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "VideoCapture" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE VideoCaptureNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~VideoCaptureNode( void ) {}

	// NodeControlInterface interface
public:
	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QWidget *gui() Q_DECL_OVERRIDE;

	virtual void loadSettings(QSettings &pSettings) Q_DECL_OVERRIDE;
	virtual void saveSettings(QSettings &pSettings) const Q_DECL_OVERRIDE;

private:
#if defined( VIDEOCAPTURE_SUPPORTED )
	static void frameCallbackStatic( ca::PixelBuffer &pBuffer );

	void frameCallback( ca::PixelBuffer &pBuffer );
#endif

private slots:
#if defined( VIDEOCAPTURE_SUPPORTED )
	void frameStart( void );
#endif

	void setCurrentDevice( int pDevIdx , int pCfgIdx);

	void chooseDevice( void );

signals:
	void deviceIndexUpdated( int pIndex );

protected:
//	QSharedPointer<fugio::PinInterface>			 mPinInput;

	QSharedPointer<fugio::PinInterface>			 mPinOutputImage;
	fugio::ImageInterface						*mValOutputImage;

	int											 mDeviceIndex;
	int											 mFormatIndex;

#if defined( VIDEOCAPTURE_SUPPORTED )
	ca::Capture									 mCapture;
#endif
};


#endif // VIDEOCAPTURENODE_H

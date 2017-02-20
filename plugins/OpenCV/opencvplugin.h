#ifndef OPENCVPLUGIN_H
#define OPENCVPLUGIN_H

#include <QObject>

#include <fugio/opencv/uuid.h>
#include <fugio/global_interface.h>
#include <fugio/plugin_interface.h>

#include <fugio/image/image_interface.h>

#if defined( OPENCV_SUPPORTED )
#include <opencv2/core/core.hpp>
#endif

using namespace fugio;

class OpenCVPlugin : public QObject, public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PluginInterface )
	Q_PLUGIN_METADATA( IID "com.bigfug.fugio.opencv.plugin" )

public:
	explicit OpenCVPlugin( void );

	virtual ~OpenCVPlugin( void );

	//-------------------------------------------------------------------------
	// fugio::PluginInterface

	virtual InitResult initialise( fugio::GlobalInterface *pApp, bool pLastChance );

	virtual void deinitialise( void );

	//-------------------------------------------------------------------------

#if defined( OPENCV_SUPPORTED )
	static cv::Mat image2mat( fugio::ImageInterface *pSrcImg );

	static void mat2image( cv::Mat &pSrcMat, fugio::ImageInterface *pDstImg, fugio::ImageInterface::Format pDstFmt = fugio::ImageInterface::FORMAT_UNKNOWN );
#endif

private:
	GlobalInterface			*mApp;

	static ClassEntry		 mNodeEntries[];
	static ClassEntry		 mPinEntries[];
};

#endif // OPENCVPLUGIN_H

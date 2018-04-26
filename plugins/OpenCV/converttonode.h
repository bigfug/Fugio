#ifndef CONVERTTONODE_H
#define CONVERTTONODE_H

#include <QObject>

#include <fugio/core/uuid.h>
#include <fugio/opencv/uuid.h>
#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/image/image.h>
#include <fugio/choice_interface.h>

#include <fugio/nodecontrolbase.h>

#if defined( OPENCV_SUPPORTED )
#include <opencv2/core/core.hpp>
#endif

class ConvertToNode : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Converts a matrix of data to different formats" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "ConvertTo_(OpenCV)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit ConvertToNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~ConvertToNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp );

private:
	static void conversion( ConvertToNode *pNode );

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputImage;
	QSharedPointer<fugio::PinInterface>			 mPinInputType;

	fugio::ChoiceInterface						*mValInputType;

	QSharedPointer<fugio::PinInterface>			 mPinOutputImage;
	fugio::VariantInterface						*mValOutputImage;

	static QMap<QString,int>					 mTypeMap;

#if defined( OPENCV_SUPPORTED )
	cv::Mat										 mMatImg;
	cv::Mat										 mMatLab;
#endif
};

#endif // CONVERTTONODE_H

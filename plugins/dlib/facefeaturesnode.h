#ifndef FACE_FEATURES_NODE_H
#define FACE_FEATURES_NODE_H

#if defined( DLIB_SUPPORTED )
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing.h>
#include <dlib/image_io.h>

using namespace std;
using namespace dlib;
#endif

#include <QObject>

#include <fugio/nodecontrolbase.h>
#include <fugio/core/variant_interface.h>

class FaceFeaturesNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Track face features" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Face_Features" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE FaceFeaturesNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~FaceFeaturesNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

private slots:

private:
	void loadDataFile( const QString &pFilename );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputData;
	QSharedPointer<fugio::PinInterface>			 mPinInputImage;

	QSharedPointer<fugio::PinInterface>			 mPinOutputRects;
	fugio::VariantInterface						*mValOutputRects;

	QSharedPointer<fugio::PinInterface>			 mPinOutputShapes;
	fugio::VariantInterface						*mValOutputShapes;

	QSharedPointer<fugio::PinInterface>			 mPinOutputChips;
	fugio::VariantInterface						*mValOutputChips;

	volatile bool								 mLoading;
	volatile bool								 mLoaded;

#if defined( DLIB_SUPPORTED )
	frontal_face_detector						 mDetector;
	shape_predictor 							 mShapePredictor;
#endif
};

#endif // FACE_FEATURES_NODE_H

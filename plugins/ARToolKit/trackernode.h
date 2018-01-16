#ifndef TRACKERNODE_H
#define TRACKERNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

#include <fugio/artoolkit/param_interface.h>

#include <fugio/math/matrix_interface.h>

#if defined( ARTOOLKIT_SUPPORTED )
#include <AR/ar.h>
#include <AR/gsub_lite.h>
#include <AR/video.h>
#endif

class TrackerNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "arTracker" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE TrackerNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~TrackerNode( void ) {}

	// NodeControlInterface interface

	virtual bool deinitialise() Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

private:
	void freeTracker( void );

#if defined( ARTOOLKIT_SUPPORTED )
	// para's type is also equivalent to (double(*)[4]).
	static void arglCameraViewRH( const ARdouble para[3][4], float m_modelview[16], const float scale = 0.0 )
	{
		m_modelview[0 + 0*4] = para[0][0]; // R1C1
		m_modelview[0 + 1*4] = para[0][1]; // R1C2
		m_modelview[0 + 2*4] = para[0][2];
		m_modelview[0 + 3*4] = para[0][3];
		m_modelview[1 + 0*4] = -para[1][0]; // R2
		m_modelview[1 + 1*4] = -para[1][1];
		m_modelview[1 + 2*4] = -para[1][2];
		m_modelview[1 + 3*4] = -para[1][3];
		m_modelview[2 + 0*4] = -para[2][0]; // R3
		m_modelview[2 + 1*4] = -para[2][1];
		m_modelview[2 + 2*4] = -para[2][2];
		m_modelview[2 + 3*4] = -para[2][3];
		m_modelview[3 + 0*4] = 0.0;
		m_modelview[3 + 1*4] = 0.0;
		m_modelview[3 + 2*4] = 0.0;
		m_modelview[3 + 3*4] = 1.0;
		if (scale != 0.0) {
			m_modelview[12] *= scale;
			m_modelview[13] *= scale;
			m_modelview[14] *= scale;
		}
	}
#endif

private slots:
	void updateConfidence(float cf);

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputParam;
	QSharedPointer<fugio::PinInterface>			 mPinInputImage;
	QSharedPointer<fugio::PinInterface>			 mPinInputPattern;
	QSharedPointer<fugio::PinInterface>			 mPinInputWidth;

	QSharedPointer<fugio::PinInterface>			 mPinOutputMatrix;
	fugio::VariantInterface						*mValOutputMatrix;

	QSharedPointer<fugio::PinInterface>			 mPinOutputCenter;
	fugio::VariantInterface						*mValOutputCenter;

	QSharedPointer<fugio::PinInterface>			 mPinOutputPoints;
	fugio::VariantInterface						*mValOutputPoints;

	QSharedPointer<fugio::PinInterface>			 mPinOutputConfidence;
	fugio::VariantInterface						*mValOutputConfidence;

#if defined( ARTOOLKIT_SUPPORTED )
	ARParamLT									*mParamLT;
	ARHandle									*mHandle;
	AR3DHandle									*mHandle3D;
	ARPattHandle								*mHandlePatt;
	AR_PIXEL_FORMAT								 mPixFmt;
	int											 mPatternIndex;
	ARdouble									 mPatTrn[ 3 ][ 4 ];
	bool										 mPatFnd;
#endif
};

#endif // TRACKERNODE_H

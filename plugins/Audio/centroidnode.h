#ifndef CENTROIDNODE_H
#define CENTROIDNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>
#include <fugio/core/variant_interface.h>

class CentroidNode : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Calculates the audio spectrum centroid value from a FFT" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Centroid_(FFT)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE CentroidNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~CentroidNode( void ) {}

	// NodeControlInterface interface
public:
	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputFFT;

	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::VariantInterface						*mValOutput;

	float									 mCentroid;
};

#endif // CENTROIDNODE_H

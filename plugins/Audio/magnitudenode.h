#ifndef MAGNITUDENODE_H
#define MAGNITUDENODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>
#include <fugio/core/variant_interface.h>
#include <fugio/audio/audio_instance_base.h>

class MagnitudeNode : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Calculates the magnitude of an audio signal" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Magnitude_(Audio)" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE MagnitudeNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~MagnitudeNode( void ) {}


	// NodeControlInterface interface
public:
	virtual bool initialise();
	virtual bool deinitialise();

	//virtual void inputsUpdated( qint64 pTimeStamp );

private slots:
	void onContextProcess( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinAudio;
	QSharedPointer<fugio::PinInterface>			 mPinSampleCount;

	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::VariantInterface						*mValOutput;

	float										 mMagnitude;

	qint64										 mSamplePosition;

	fugio::AudioInstanceBase					*mProducerInstance;

	QVector<QVector<float>>						 mAudDat;
};

#endif // MAGNITUDENODE_H

#ifndef AUDIOTOARRAYNODE_H
#define AUDIOTOARRAYNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>
#include <fugio/core/variant_interface.h>
#include <fugio/core/array_interface.h>
#include <fugio/audio/audio_producer_interface.h>
#include <fugio/audio/audio_instance_base.h>

class AudioToArrayNode : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Audio_To_Array" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE AudioToArrayNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~AudioToArrayNode( void ) {}

	// NodeControlInterface interface
public:
	virtual void inputsUpdated( qint64 pTimeStamp );

protected slots:
	void onContextFrame( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputAudio;
	QSharedPointer<fugio::PinInterface>			 mPinInputSamples;

	QSharedPointer<fugio::PinInterface>			 mPinOutputBuffer;
	fugio::ArrayInterface						*mValOutputBuffer;

	int											 mSampleCount;
	qint64										 mSamplePosition;

	fugio::AudioInstanceBase					*mProducerInstance;

	QVector<float>								 mWindow;
};

#endif // AUDIOTOARRAYNODE_H

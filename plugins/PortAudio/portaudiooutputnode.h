#ifndef PORTAUDIOOUTPUTNODE_H
#define PORTAUDIOOUTPUTNODE_H

#include <QObject>
#include <QVector>

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/nodecontrolbase.h>

#include <fugio/portaudio/uuid.h>
#include <fugio/playhead_interface.h>

#include "deviceportaudio.h"

class PortAudioOutputNode : public fugio::NodeControlBase, public fugio::PlayheadInterface, public fugio::AudioProducerInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PlayheadInterface fugio::AudioProducerInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Outputs audio data to speakers/headphones via PortAudio" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/PortAudio_Output" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit PortAudioOutputNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~PortAudioOutputNode( void );

	// NodeControlInterface interface

	virtual bool initialise( void ) Q_DECL_OVERRIDE;

	virtual bool deinitialise( void ) Q_DECL_OVERRIDE;

	virtual QWidget *gui() Q_DECL_OVERRIDE;

	virtual void inputsUpdated(qint64 pTimeStamp) Q_DECL_OVERRIDE;

	virtual void loadSettings( QSettings &pSettings ) Q_DECL_OVERRIDE;
	virtual void saveSettings( QSettings &pSettings ) const Q_DECL_OVERRIDE;

	// fugio::PlayheadInterface

	virtual void playStart( qreal pTimeStamp ) Q_DECL_OVERRIDE
	{
		if( mPortAudio )
		{
			mPortAudio->playStart( pTimeStamp );
		}
	}

	virtual void playheadMove( qreal pTimeStamp ) Q_DECL_OVERRIDE
	{
		if( mPortAudio )
		{
			mPortAudio->playheadMove( pTimeStamp );
		}
	}

	virtual bool playheadPlay( qreal pTimePrev, qreal pTimeCurr ) Q_DECL_OVERRIDE
	{
		return( mPortAudio ? mPortAudio->playheadPlay( pTimePrev, pTimeCurr ) : true );
	}

	virtual void setTimeOffset( qreal pTimeOffset ) Q_DECL_OVERRIDE
	{
		if( mPortAudio )
		{
			mPortAudio->setTimeOffset( pTimeOffset );
		}
	}

	virtual qreal latency( void ) const Q_DECL_OVERRIDE
	{
		return( mPortAudio ? mPortAudio->latency() : 0 );
	}

	// InterfaceAudioProducer interface
public:
	virtual void audio(qint64 pSamplePosition, qint64 pSampleCount, int pChannelOffset, int pChannelCount, float **pBuffers, qint64 pLatency, void *pInstanceData ) const Q_DECL_OVERRIDE;
	virtual void *allocAudioInstance( qreal pSampleRate, fugio::AudioSampleFormat pSampleFormat, int pChannels ) Q_DECL_OVERRIDE;
	virtual void freeAudioInstance(void *pInstanceData) Q_DECL_OVERRIDE;

signals:
	void audioUpdated( void );

	void audioDeviceChanged( const QString &pDeviceName );

protected slots:
	void audioDeviceSelected( const QString &pDeviceName );

	void clicked( void );

private:
	QSharedPointer<fugio::PinInterface>	 mPinInputAudio;
	QSharedPointer<fugio::PinInterface>	 mPinInputVolume;

	QSharedPointer<DevicePortAudio>		 mPortAudio;

	mutable QMutex						 mProducerMutex;
	fugio::AudioProducerInterface		*mProducer;
	void								*mInstance;

	QString								 mDeviceName;

	float								 mVolume;
};

#endif // PORTAUDIOOUTPUTNODE_H

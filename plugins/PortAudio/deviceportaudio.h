#ifndef DEVICEPORTAUDIO_H
#define DEVICEPORTAUDIO_H

#include <QObject>
#include <QSettings>
#include <QList>

#include <portaudio.h>

#include <fugio/playhead_interface.h>
#include <fugio/node_interface.h>
#include <fugio/audio/audio_producer_interface.h>

class DevicePortAudio : public QObject, public fugio::PlayheadInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PlayheadInterface )

public:
	static void deviceInitialise( void );
	static void deviceDeinitialise( void );
	static void devicePacketStart( qint64 pTimeStamp );
	static void devicePacketEnd( void );

	static void deviceCfgSave( QSettings &pDataStream );
	static void deviceCfgLoad( QSettings &pDataStream );

	static QSharedPointer<DevicePortAudio> newDevice( PaDeviceIndex pDevIdx );

	static QStringList deviceOutputNameList( void );

	static QString deviceOutputDefaultName( void );

	static PaDeviceIndex deviceOutputNameIndex( const QString &pDeviceName );

	///static void delDevice( DevicePortAudio *pDelDev );

//	static QList<DevicePortAudio *> devices( void )
//	{
//		return( mDeviceList );
//	}

protected:
	explicit DevicePortAudio( PaDeviceIndex pDeviceIndex );

	void deviceOutputOpen( const PaDeviceInfo *DevInf );
	void deviceInputOpen( const PaDeviceInfo *DevInf );

	void deviceOutputClose( void );
	void deviceInputClose( void );

public:
	virtual ~DevicePortAudio( void );

	qreal sampleRate( void ) const;

	inline int inputChannelCount( void ) const
	{
		return( mInputChannelCount );
	}

	inline int outputChannelCount( void ) const
	{
		return( mOutputChannelCount );
	}

//	inline qreal timeoffset( void ) const
//	{
//		return( mTimeOffset );
//	}

//	void setCurrentNode( QSharedPointer<fugio::NodeInterface> pNode );

	void addProducer( fugio::AudioProducerInterface *pAudioProducer );
	void remProducer( fugio::AudioProducerInterface *pAudioProducer );

	//-------------------------------------------------------------------------
	// fugio::PlayheadInterface

	virtual void playStart( qreal )
	{
	}

	virtual void playheadMove( qreal ) {}

	virtual bool playheadPlay( qreal, qreal )
	{
		return( true );
	}

	virtual qreal latency( void ) const
	{
		return( mOutputTimeLatency );
	}

	virtual void setTimeOffset( qreal pTimeOffset );

	//-------------------------------------------------------------------------

private:

	static int streamCallbackStatic( const void *input, void *output,
									 unsigned long frameCount,
									 const PaStreamCallbackTimeInfo* timeInfo,
									 PaStreamCallbackFlags statusFlags,
									 void *userData );

	int streamCallbackOutput( void *output,
						unsigned long frameCount,
						const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags );

	int streamCallbackInput( const void *input,
						unsigned long frameCount,
						const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags );

signals:
	void audio( const float **pData, quint64 pSampleCount, int pChannelCount, qint64 pSamplePosition );

public slots:

private:
	static QList<QWeakPointer<DevicePortAudio>>			 mDeviceList;

	typedef struct AudioInstanceData
	{
		fugio::AudioProducerInterface		*mProducer;
		void								*mInstance;
	} AudioInstanceData;

	QMutex									 mProducerMutex;
	QList<AudioInstanceData>				 mProducers;

	PaDeviceIndex							 mDeviceIndex;
	PaStream								*mStreamOutput;
	PaStream								*mStreamInput;

	PaTime									 mOutputTimeLatency;
	PaTime									 mInputTimeLatency;

	PaStreamCallbackTimeInfo				 mOutputTimeInfo;
	PaStreamCallbackTimeInfo				 mInputTimeInfo;

	qint64									 mOutputAudioOffset;
	PaTime									 mInputAudioOffset;

	qreal									 mSampleRate;
	int										 mOutputChannelCount;
	int										 mInputChannelCount;
};

#endif // DEVICEPORTAUDIO_H

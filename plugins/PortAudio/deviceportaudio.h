#ifndef DEVICEPORTAUDIO_H
#define DEVICEPORTAUDIO_H

#include <QObject>
#include <QSettings>
#include <QList>
#include <QMutex>

#if defined( PORTAUDIO_SUPPORTED )
#include <portaudio.h>
#endif

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

#if defined( PORTAUDIO_SUPPORTED )
	static QSharedPointer<DevicePortAudio> newDevice( PaDeviceIndex pDevIdx );

	static QStringList deviceOutputNameList( void );

	static QString deviceOutputDefaultName( void );

	static PaDeviceIndex deviceOutputNameIndex( const QString &pDeviceName );

	static QStringList deviceInputNameList( void );

	static QString deviceInputDefaultName( void );

	static PaDeviceIndex deviceInputNameIndex( const QString &pDeviceName );

	///static void delDevice( DevicePortAudio *pDelDev );

//	static QList<DevicePortAudio *> devices( void )
//	{
//		return( mDeviceList );
//	}
#endif

protected:
#if defined( PORTAUDIO_SUPPORTED )
	explicit DevicePortAudio( PaDeviceIndex pDeviceIndex );

	void deviceOutputOpen( const PaDeviceInfo *DevInf );
	void deviceInputOpen( const PaDeviceInfo *DevInf );

	void deviceOutputClose( void );
	void deviceInputClose( void );
#endif

public:
	virtual ~DevicePortAudio( void );

	qreal outputSampleRate( void ) const;

	inline int inputChannelCount( void ) const
	{
		return( mInputChannelCount );
	}

	inline int outputChannelCount( void ) const
	{
		return( mOutputChannelCount );
	}

	inline qreal inputSampleRate( void ) const
	{
		return( mInputSampleRate );
	}

	inline fugio::AudioSampleFormat inputSampleFormat( void ) const
	{
		return( mInputSampleFormat );
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
#if defined( PORTAUDIO_SUPPORTED )
		return( mOutputTimeLatency );
#else
		return( 0 );
#endif
	}

	virtual void setTimeOffset( qreal pTimeOffset );

	//-------------------------------------------------------------------------

private:
#if defined( PORTAUDIO_SUPPORTED )

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
#endif
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

#if defined( PORTAUDIO_SUPPORTED )
	PaDeviceIndex							 mDeviceIndex;
	PaStream								*mStreamOutput;
	PaStream								*mStreamInput;

	PaTime									 mOutputTimeLatency;
	PaTime									 mInputTimeLatency;

	PaStreamCallbackTimeInfo				 mOutputTimeInfo;
	PaStreamCallbackTimeInfo				 mInputTimeInfo;

	qint64									 mOutputAudioOffset;
	PaTime									 mInputAudioOffset;
#endif

	qreal									 mOutputSampleRate;
	int										 mOutputChannelCount;
	int										 mInputChannelCount;
	qreal									 mInputSampleRate;
	fugio::AudioSampleFormat				 mInputSampleFormat;
};

#endif // DEVICEPORTAUDIO_H

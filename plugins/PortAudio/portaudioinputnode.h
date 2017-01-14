#ifndef PORTAUDIOINPUTNODE_H
#define PORTAUDIOINPUTNODE_H

#include <QObject>
#include <QVector>

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>
#include <fugio/audio/audio_instance_base.h>

#include <fugio/nodecontrolbase.h>

#include <fugio/portaudio/uuid.h>
#include <fugio/audio/audio_producer_interface.h>

#include "deviceportaudio.h"

class PortAudioInputNode : public fugio::NodeControlBase, public fugio::AudioProducerInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::AudioProducerInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Inputs audio data from PortAudio" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Audio_Input_(PortAudio)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit PortAudioInputNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~PortAudioInputNode( void );

	// NodeControlInterface interface
public:
	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

	virtual void loadSettings(QSettings &pSettings) Q_DECL_OVERRIDE;
	virtual void saveSettings( QSettings &pSettings ) const Q_DECL_OVERRIDE;

	virtual QWidget *gui() Q_DECL_OVERRIDE;

	// AudioProducerInterface interface
public:
	virtual fugio::AudioInstanceBase *audioAllocInstance(qreal pSampleRate, fugio::AudioSampleFormat pSampleFormat, int pChannels) Q_DECL_OVERRIDE;
	virtual qint64 audioLatency() const Q_DECL_OVERRIDE;
	virtual int audioChannels() const Q_DECL_OVERRIDE;
	virtual qreal audioSampleRate() const Q_DECL_OVERRIDE;
	virtual fugio::AudioSampleFormat audioSampleFormat() const Q_DECL_OVERRIDE;

	virtual bool isValid( fugio::AudioInstanceBase *pInstance ) const Q_DECL_OVERRIDE;

signals:
	void audioDeviceChanged( const QString &pDeviceName );

protected slots:
	void audioDeviceSelected( const QString &pDeviceName );

	void clicked( void );

	void rebuildDeviceList( void );

private:
	QSharedPointer<fugio::PinInterface>		 mPinAudio;
	fugio::AudioProducerInterface			*mValAudio;

	QSharedPointer<DevicePortAudio>			 mPortAudio;
	fugio::AudioInstanceBase				*mInstance;

	QStringList								 mDeviceList;

	QString									 mDeviceName;

#if defined( PORTAUDIO_SUPPORTED )
	PaDeviceIndex							 mDeviceIndex;
#endif
};

#endif // PORTAUDIOINPUTNODE_H

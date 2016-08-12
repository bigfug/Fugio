#ifndef DEVICEMIDI_H
#define DEVICEMIDI_H

#include <QObject>
#include <QList>
#include <QVector>
#include <QSettings>
#include <QUuid>
#include <QMutex>

#include <portmidi.h>

#include <fugio/node_control_interface.h>
#include <fugio/midi/midi_interface.h>
#include <fugio/midi/midi_input_interface.h>

FUGIO_NAMESPACE_BEGIN
class NodeInterface;
class PinInterface;
class VariantInterface;
FUGIO_NAMESPACE_END

class DeviceMidi : public QObject
{
	Q_OBJECT

public:
	static bool deviceInitialise( void );
	static void deviceDeinitialise( void );
	static void devicePacketStart( void );
	static void devicePacketEnd( void );

	static QStringList deviceInputNames( void );

	static QStringList deviceOutputNames( void );

	static QString deviceOutputDefaultName( void );
	static QString deviceInputDefaultName( void );

	static QSharedPointer<DeviceMidi> newDevice( PmDeviceID pDevIdx );

	static PmDeviceID deviceOutputNameIndex( const QString &pDeviceName );
	static PmDeviceID deviceInputNameIndex( const QString &pDeviceName );

private:
	static QString nameFromDeviceInfo( const PmDeviceInfo *pDevInf );

	static PmDeviceID findDeviceInputId( const QString &pName );
	static PmDeviceID findDeviceOutputId( const QString &pName );

	static QString deviceName( PmDeviceID pDevIdx );

private:
	explicit DeviceMidi( PmDeviceID pDevIdx );

	void packetStart( void );
	void packetEnd( void );

public:
	virtual ~DeviceMidi( void );

	inline QString name( void ) const
	{
		return( mDeviceName );
	}

	bool isActive( void ) const
	{
		return( mStreamInput || mStreamOutput );
	}

	void output( PmEvent pEvent );

	void output( const QVector<PmEvent> &pEvents );

	void output( const QVector<fugio::MidiEvent> &pEvents );

	void output( const int32_t *pEvents, int pCount );

	void outputSysEx( const QByteArray pArray );

	void registerMidiInputNode( fugio::MidiInputInterface *pMII )
	{
		QMutexLocker		Lock( &mMidiInputNodesMutex );

		mMidiInputNodes.removeAll( pMII );

		mMidiInputNodes.append( pMII );
	}

	void unregisterMidiInputNode( fugio::MidiInputInterface *pMII )
	{
		QMutexLocker		Lock( &mMidiInputNodesMutex );

		mMidiInputNodes.removeAll( pMII );
	}

private:
	static QList<QWeakPointer<DeviceMidi>>			 mDeviceList;

	const QString						 mDeviceName;
	PmDeviceID							 mDeviceId;

	PortMidiStream						*mStreamInput;
	PortMidiStream						*mStreamOutput;

	QVector<PmEvent>					 mEvents;
	QList<QByteArray>					 mSysExBuffer;
	QByteArray							 mSysExCurrent;

	QMutex								 mMidiInputNodesMutex;
	QList<fugio::MidiInputInterface *>	 mMidiInputNodes;
};

#endif // DEVICEMIDI_H

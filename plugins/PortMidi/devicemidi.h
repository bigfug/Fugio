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
	typedef enum MidiDirection
	{
		INPUT, OUTPUT
	} MidiDirection;

	static bool deviceInitialise( void );
	static void deviceDeinitialise( void );
	static void devicePacketStart( void );
	static void devicePacketEnd( void );

	static void updateDeviceList( void );

	static QStringList deviceInputNames( void )
	{
		QStringList		DevLst = mMidiInputDevices.keys();

		std::sort( DevLst.begin(), DevLst.end() );

		return( DevLst );
	}

	static QStringList deviceOutputNames( void )
	{
		QStringList		DevLst = mMidiOutputDevices.keys();

		std::sort( DevLst.begin(), DevLst.end() );

		return( DevLst );
	}

	static QSharedPointer<DeviceMidi> findDeviceInput( const QString &pName );
	static QSharedPointer<DeviceMidi> findDeviceOutput( const QString &pName );

private:
	static QString nameFromDeviceInfo( const PmDeviceInfo *pDevInf );

	static PmDeviceID findDeviceInputId( const QString &pName );
	static PmDeviceID findDeviceOutputId( const QString &pName );

private:
	explicit DeviceMidi( const QString &pDeviceName, MidiDirection pDirection );

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
	typedef QMap<QString,QWeakPointer<DeviceMidi>>	DeviceMap;

	static DeviceMap					 mMidiInputDevices;
	static DeviceMap					 mMidiOutputDevices;

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

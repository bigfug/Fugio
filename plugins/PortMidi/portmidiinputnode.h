#ifndef PORTMIDIINPUTNODE_H
#define PORTMIDIINPUTNODE_H

#include <QObject>

#include <fugio/portmidi/uuid.h>

#include <fugio/nodecontrolbase.h>

#include <fugio/midi/midi_input_interface.h>

FUGIO_NAMESPACE_BEGIN
class VariantInterface;
class MidiInterface;
FUGIO_NAMESPACE_END

class DeviceMidi;
class QPushButton;

class PortMidiInputNode : public fugio::NodeControlBase, public fugio::MidiInputInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::MidiInputInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Receives MIDI information from an external source." )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/PortMidiInput" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit PortMidiInputNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~PortMidiInputNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual bool initialise( void ) Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

	virtual QWidget *gui() Q_DECL_OVERRIDE;

	virtual void loadSettings( QSettings &pSettings ) Q_DECL_OVERRIDE;
	virtual void saveSettings( QSettings &pSettings ) const Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------
	// MidiInputInterface interface

public:
	virtual void midiProcessInput( const fugio::MidiEvent *pMessages, quint32 pMessageCount ) Q_DECL_OVERRIDE;

signals:
	void deviceChanged( const QString &pDeviceName );

private slots:
	void setDeviceName( const QString &pDeviceName );

private:
	QString									 mDeviceName;
	QSharedPointer<DeviceMidi>				 mDevice;

	QSharedPointer<fugio::PinInterface>		 mPinMidi;
	fugio::MidiInterface					*mValMidi;
};

#endif // PORTMIDIINPUTNODE_H

#ifndef PORTMIDIOUTPUTNODE_H
#define PORTMIDIOUTPUTNODE_H

#include <QObject>

#include <fugio/portmidi/uuid.h>
#include <fugio/choice_interface.h>

#include <fugio/nodecontrolbase.h>

class DeviceMidi;

class PortMidiOutputNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Sends MIDI messages and timing information to an external destination." )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/PortMidiOutput" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit PortMidiOutputNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~PortMidiOutputNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual bool initialise( void ) Q_DECL_OVERRIDE;
	virtual bool deinitialise( void ) Q_DECL_OVERRIDE;

	virtual QWidget *gui() Q_DECL_OVERRIDE;

	virtual void loadSettings( QSettings &pSettings ) Q_DECL_OVERRIDE;
	virtual void saveSettings( QSettings &pSettings ) const Q_DECL_OVERRIDE;

	virtual QList<QUuid> pinAddTypesInput() const Q_DECL_OVERRIDE;
	virtual bool canAcceptPin(fugio::PinInterface *pPin) const Q_DECL_OVERRIDE;

signals:
	void midiDeviceChanged( const QString &pDeviceName );

protected slots:
	void onFrameStart();
	void onFrameEnd( qint64 pTimeStamp );

	void midiDeviceSelected( const QString &pDeviceName );

	void rebuildDeviceList( void );

private:
	QStringList								 mDeviceList;
	QString									 mDeviceName;

	QSharedPointer<DeviceMidi>				 mDevice;

	QSharedPointer<fugio::PinInterface>		 mPinClock;
	QSharedPointer<fugio::PinInterface>		 mPinMidi;

	qreal									 mClockValue;
	qreal									 mClockLast;

	bool									 mPlayState;
};

#endif // PORTMIDIOUTPUTNODE_H

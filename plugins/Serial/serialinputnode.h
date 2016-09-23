#ifndef SERIALINPUTNODE_H
#define SERIALINPUTNODE_H

#include <fugio/pin_interface.h>
#include <fugio/core/variant_interface.h>

#include <fugio/nodecontrolbase.h>

#include <fugio/serial/uuid.h>

#include "deviceserial.h"

class QComboBox;

class SerialInputNode : public fugio::NodeControlBase
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit SerialInputNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~SerialInputNode( void )
	{

	}

	// NodeControlInterface interface
public:
	virtual bool initialise( void ) Q_DECL_OVERRIDE;

	virtual bool deinitialise( void ) Q_DECL_OVERRIDE;

	virtual QWidget *gui() Q_DECL_OVERRIDE;

	virtual void loadSettings(QSettings &pSettings) Q_DECL_OVERRIDE;
	virtual void saveSettings( QSettings &pSettings ) const Q_DECL_OVERRIDE;

private slots:
	void onFrameStart( void );

	void deviceSelection( int pDeviceIndex );

signals:
	void deviceSelected( int pDeviceIndex );

private:
	DeviceSerial							*mDevice;

	QSharedPointer<fugio::PinInterface>		 mPinOutput;
	fugio::VariantInterface					*mValOutput;

	QUuid									 mDeviceUuid;
};

#endif // SERIALINPUTNODE_H

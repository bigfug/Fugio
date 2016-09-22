#ifndef SERIALOUTPUTNODE_H
#define SERIALOUTPUTNODE_H

#include <fugio/pin_interface.h>
#include <fugio/core/variant_interface.h>

#include <fugio/nodecontrolbase.h>

#include <fugio/serial/uuid.h>

#include "deviceserial.h"

class QComboBox;

class SerialOutputNode : public fugio::NodeControlBase
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit SerialOutputNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~SerialOutputNode( void ){}

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QWidget *gui() Q_DECL_OVERRIDE;

	virtual void loadSettings(QSettings &pSettings) Q_DECL_OVERRIDE;
	virtual void saveSettings( QSettings &pSettings ) const Q_DECL_OVERRIDE;

private slots:
	void deviceSelection( int pDeviceIndex );

signals:
	void deviceSelected( int pDeviceIndex );

private:
	DeviceSerial							*mDevice;

	QSharedPointer<fugio::PinInterface>		 mPinInput;
	qreal									 mLastSend;

	QUuid									 mDeviceUuid;
};

#endif // SERIALOUTPUTNODE_H

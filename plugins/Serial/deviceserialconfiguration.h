#ifndef DEVICESERIALCONFIGURATION_H
#define DEVICESERIALCONFIGURATION_H

#include <QDialog>
#include <QUuid>

#include "deviceserial.h"

namespace Ui {
class DeviceSerialConfiguration;
}

class QListWidgetItem;

class DeviceSerialConfiguration : public QDialog
{
	Q_OBJECT

public:
	explicit DeviceSerialConfiguration(QWidget *parent = 0);

	~DeviceSerialConfiguration();

protected:
	void widgetsUpdate();

	DeviceSerial *findDevice( QUuid pDeviceId );

	QListWidgetItem *findItem( const QUuid &pDeviceId );

private slots:
	void on_mButtonAdd_clicked();

	void on_mDeviceList_itemActivated( QListWidgetItem *item );

	void on_mDeviceList_itemChanged( QListWidgetItem *item );

	void onDeviceSelected( QUuid pDeviceName );

	void on_mName_textChanged(const QString &arg1);

	void on_mPort_currentTextChanged(const QString &arg1);

	void on_mBaud_currentIndexChanged(const QString &arg1);

	void on_mDeviceList_itemClicked(QListWidgetItem *item);

	void on_mButtonRemove_clicked();

	void on_mButtonEnable_toggled(bool checked);

private:
	Ui::DeviceSerialConfiguration *ui;

	DeviceSerial		*mDevice;
	QUuid				 mDeviceName;
};

#endif // DEVICESERIALCONFIGURATION_H

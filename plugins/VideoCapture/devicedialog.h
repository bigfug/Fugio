#ifndef DEVICEDIALOG_H
#define DEVICEDIALOG_H

#include <QDialog>
#include <QMap>

namespace Ui {
class DeviceDialog;
}

#if defined( VIDEOCAPTURE_SUPPORTED )
#include <videocapture/Capture.h>
#else
namespace ca { typedef int Capture; }
#endif

class DeviceDialog : public QDialog
{
	Q_OBJECT

public:
	explicit DeviceDialog( ca::Capture &pCapture, int pDevIdx, int pCfgIdx, QWidget *parent = 0);

	virtual ~DeviceDialog();

	inline int deviceIdx( void ) const
	{
		return( mDevIdx );
	}

	inline int formatIdx( void ) const
	{
		return( mCfgIdx );
	}

	void updateFormatList( void );

private slots:
	void deviceChanged( int index );
	void formatChanged( int index );

private:
	Ui::DeviceDialog	*ui;

#if defined( VIDEOCAPTURE_SUPPORTED )
	ca::Capture			&mCapture;
#endif

	int					 mDevIdx;
	int					 mCfgIdx;
};

#endif // DEVICEDIALOG_H

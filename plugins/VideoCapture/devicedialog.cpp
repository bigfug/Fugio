#include "devicedialog.h"
#include "ui_devicedialog.h"

DeviceDialog::DeviceDialog( int pDevIdx, int pCfgIdx, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DeviceDialog),
#if defined( VIDEOCAPTURE_SUPPORTED )
	mCapture( nullptr, nullptr ),
#endif
	mDevIdx( pDevIdx ), mCfgIdx( pCfgIdx )
{
	ui->setupUi( this );

#if defined( VIDEOCAPTURE_SUPPORTED )
	const std::vector<ca::Device>	DevLst = mCapture.getDevices();

	int		DevIdx = -1;

	for( const ca::Device &Dev : DevLst )
	{
		if( Dev.index == mDevIdx )
		{
			DevIdx = ui->mDevice->count();
		}

		ui->mDevice->addItem( QString::fromStdString( Dev.name ), Dev.index );
	}

	if( DevIdx >= 0 )
	{
		ui->mDevice->setCurrentIndex( DevIdx );
	}

	updateFormatList();
#endif

	connect( ui->mDevice, SIGNAL(currentIndexChanged(int)), this, SLOT(deviceChanged(int)) );

	connect( ui->mFormat, SIGNAL(currentIndexChanged(int)), this, SLOT(formatChanged(int)) );
}

DeviceDialog::~DeviceDialog()
{
	delete ui;
}

void DeviceDialog::updateFormatList( void )
{
	static QStringList FmtNam =
	{
		"UNKNOWN",
		"UYVY422",
		"YUYV422",
		"YUV422P",
		"YUV420P",
		"YUV420BP",
		"YUVJ420P",
		"YUVJ420BP",
		"ARGB32",
		"BGRA32",
		"RGBA32",
		"RGB24",
		"JPEG_OPENDML",
		"H264",
		"MJPEG"
	};

	ui->mFormat->clear();

#if defined( VIDEOCAPTURE_SUPPORTED )
	const std::vector<ca::Capability>	CapLst = mCapture.getCapabilities( mDevIdx );

	int		CapIdx = -1;

	for( const ca::Capability &Cap : CapLst )
	{
		if( Cap.capability_index == mCfgIdx )
		{
			CapIdx = ui->mFormat->count();
		}

		QString FN = ( Cap.pixel_format >= 0 && Cap.pixel_format < FmtNam.size() ? FmtNam.at( Cap.pixel_format ) : FmtNam.at( 0 ) );

		ui->mFormat->addItem( QString( "%1x%2 @ %3 fps (%4)" ).arg( Cap.width ).arg( Cap.height ).arg( float( Cap.fps ) / 100.0f ).arg( FN ), Cap.capability_index );
	}

	if( CapIdx >= 0 )
	{
		ui->mFormat->setCurrentIndex( CapIdx );
	}
	else
	{
		mCfgIdx = CapLst.empty() ? -1 : 0;
	}
#endif
}

void DeviceDialog::deviceChanged( int index )
{
	int	DevIdx = ui->mDevice->itemData( index ).toInt();

	if( DevIdx != mDevIdx )
	{
		mDevIdx = DevIdx;

		updateFormatList();
	}
}

void DeviceDialog::formatChanged( int index )
{
	mCfgIdx = ui->mFormat->itemData( index ).toInt();
}

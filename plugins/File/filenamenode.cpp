#include "filenamenode.h"

#include <QPushButton>
#include <QFileDialog>
#include <QMainWindow>

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>
#include <fugio/node_interface.h>

FilenameNode::FilenameNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mValFilename = pinOutput<fugio::FilenameInterface *>( "Filename", mPinFilename, PID_FILENAME );
}

QWidget *FilenameNode::gui()
{
	QPushButton	*GUI = new QPushButton( tr( "..." ) );

	connect( GUI, SIGNAL(clicked()), this, SLOT(onClick()) );

	return( GUI );
}

void FilenameNode::onClick()
{
	QSharedPointer<fugio::PinInterface>	PinDat;
	QString							DiaDir;
	QString							DiaFlt;

	if( mPinFilename->isConnected() )
	{
		if( ( PinDat = mPinFilename->connectedPins().first() ) )
		{
			DiaDir = PinDat->setting( PIN_SETTING_DIRECTORY, QString() ).toString();
			DiaFlt = PinDat->setting( PIN_SETTING_FILE_PATTERN, QString() ).toString();
		}
	}

#if defined( Q_OS_MACX )
	bool		GlobalPause = mNode->context()->global()->isPaused();

	if( !GlobalPause )
	{
		mNode->context()->global()->pause();
	}
#endif

	QString			FilNam = QFileDialog::getOpenFileName( mNode->context()->global()->mainWindow(), QString(), DiaDir, DiaFlt );

	if( !FilNam.isEmpty() )
	{
		QFileInfo		FilInf( FilNam );

		if( PinDat )
		{
			PinDat->setSetting( PIN_SETTING_DIRECTORY, FilInf.absolutePath() );
		}

		mValFilename->setFilename( FilInf.absoluteFilePath() );

		mNode->context()->updateNode( mNode->context()->findNode( mNode->uuid() ) );
	}

#if defined( Q_OS_MACX )
	if( !GlobalPause )
	{
		mNode->context()->global()->unpause();
	}
#endif
}

void FilenameNode::inputsUpdated( qint64 pTimeStamp )
{
	if( pTimeStamp )
	{
		pinUpdated( mPinFilename );
	}
}

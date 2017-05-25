#include "filenamenode.h"

#include <QPushButton>
#include <QFileDialog>
#include <QMainWindow>

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>
#include <fugio/node_interface.h>
#include <fugio/editor_interface.h>

FilenameNode::FilenameNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	mValFilename = pinOutput<fugio::FilenameInterface *>( "Filename", mPinFilename, PID_FILENAME );
}

bool FilenameNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	return( updateStatus() );
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

	fugio::EditorInterface	*EI = qobject_cast<fugio::EditorInterface *>( mNode->context()->global()->findInterface( IID_EDITOR ) );

	QString			FilNam = QFileDialog::getOpenFileName( EI ? EI->mainWindow() : nullptr, QString(), DiaDir, DiaFlt );

	if( !FilNam.isEmpty() )
	{
		QFileInfo		FilInf( FilNam );

		if( PinDat )
		{
			PinDat->setSetting( PIN_SETTING_DIRECTORY, FilInf.absolutePath() );
		}

		if( mValFilename->filename() != FilInf.absoluteFilePath() )
		{
			mValFilename->setFilename( FilInf.absoluteFilePath() );

			mNode->context()->updateNode( mNode );
		}
	}

#if defined( Q_OS_MACX )
	if( !GlobalPause )
	{
		mNode->context()->global()->unpause();
	}
#endif
}

bool FilenameNode::updateStatus()
{
	if( mValFilename->filename().isEmpty() )
	{
		mNode->setStatus( fugio::NodeInterface::Initialising );

		mNode->setStatusMessage( tr( "Click the button to choose a file" ) );

		return( false );
	}

	if( !QFileInfo( mValFilename->filename() ).exists() )
	{
		mNode->setStatus( fugio::NodeInterface::Error );

		mNode->setStatusMessage( tr( "%1: file not found" ).arg( mValFilename->filename() ) );

		return( false );
	}

	mNode->setStatus( fugio::NodeInterface::Initialised );

	mNode->setStatusMessage( "" );

	return( true );
}

void FilenameNode::inputsUpdated( qint64 pTimeStamp )
{
	if( pTimeStamp && updateStatus() )
	{
		pinUpdated( mPinFilename );
	}
}

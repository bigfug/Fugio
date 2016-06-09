#ifndef CMDPINIMPORT_H
#define CMDPINIMPORT_H

#include <QUndoCommand>
#include <QTemporaryFile>
#include <QSettings>

#include <fugio/global_interface.h>
#include <fugio/global.h>

#include "app.h"

class CmdPinImport : public QUndoCommand
{
public:
	explicit CmdPinImport( QSharedPointer<fugio::PinInterface> pPin, const QString &pValNew )
		: mPin( pPin ), mValNew( pValNew )
	{
		setText( QObject::tr( "Pin Import" ) );

		if( true )
		{
			QTemporaryFile						 mTmpFil;

			mTmpFil.setAutoRemove( false );

			mValOld = mTmpFil.fileName();
		}

		QSettings	Settings( mValOld, QSettings::IniFormat );

		if( Settings.isWritable() )
		{
			Settings.clear();

			mPin->control()->saveSettings( Settings );
		}
	}

	virtual ~CmdPinImport( void )
	{

	}

	virtual void undo( void )
	{
		QSettings	Settings( mValOld, QSettings::IniFormat );

		if( Settings.format() == QSettings::IniFormat )
		{
			mPin->control()->loadSettings( Settings );

			mPin->node()->context()->pinUpdated( mPin );
		}
	}

	virtual void redo( void )
	{
		QSettings	Settings( mValNew, QSettings::IniFormat );

		if( Settings.format() == QSettings::IniFormat )
		{
			mPin->control()->loadSettings( Settings );

			mPin->node()->context()->pinUpdated( mPin );
		}
	}

private:
	QSharedPointer<fugio::PinInterface>		 mPin;
	QString								 mValOld, mValNew;
};

#endif // CMDPINIMPORT_H

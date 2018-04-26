#ifndef CMDSETALWAYSUPDATE_H
#define CMDSETALWAYSUPDATE_H

#include <QUndoCommand>

#include <fugio/global_interface.h>
#include <fugio/global.h>

#include "app.h"

class CmdSetAlwaysUpdate : public QUndoCommand
{
public:
	explicit CmdSetAlwaysUpdate( QSharedPointer<fugio::PinInterface> pPin, const bool pValNew )
		: mPin( pPin ), mValOld( pPin->alwaysUpdate() ), mValNew( pValNew )
	{
		setText( QObject::tr( "Set always update" ) );
	}

	virtual ~CmdSetAlwaysUpdate( void ) {}

	virtual void undo( void )
	{
		mPin->setAlwaysUpdate( mValOld );
	}

	virtual void redo( void )
	{
		mPin->setAlwaysUpdate( mValNew );
	}

private:
	QSharedPointer<fugio::PinInterface>		 mPin;
	const bool								 mValOld, mValNew;
};

#endif // CMDSETALWAYSUPDATE_H

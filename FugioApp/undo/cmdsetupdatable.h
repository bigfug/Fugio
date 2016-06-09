#ifndef CMDSETUPDATABLE_H
#define CMDSETUPDATABLE_H

#include <QUndoCommand>

#include <fugio/global_interface.h>
#include <fugio/global.h>

#include "app.h"

class CmdSetUpdatable : public QUndoCommand
{
public:
	explicit CmdSetUpdatable( QSharedPointer<fugio::PinInterface> pPin, const bool pValNew )
		: mPin( pPin ), mValOld( pPin->updatable() ), mValNew( pValNew )
	{
		setText( QObject::tr( "Set updatable" ) );
	}

	virtual ~CmdSetUpdatable( void )
	{

	}

	virtual void undo( void )
	{
		mPin->setUpdatable( mValOld );
	}

	virtual void redo( void )
	{
		mPin->setUpdatable( mValNew );
	}

private:
	QSharedPointer<fugio::PinInterface>		 mPin;
	const bool							 mValOld, mValNew;
};

#endif // CMDSETUPDATABLE_H

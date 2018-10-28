#ifndef CMDNEWCONTEXT_H
#define CMDNEWCONTEXT_H

#include <QUndoCommand>

#include <fugio/global_interface.h>

#include <fugio/global.h>

#include "app.h"

class CmdNewContext : public QUndoCommand
{
public:
	explicit CmdNewContext( void )
	{
		setText( QObject::tr( "New Context" ) );

		gApp->incrementStatistic( "context-new" );
	}

	virtual ~CmdNewContext( void )
	{

	}

	virtual void undo( void )
	{
		gApp->global().delContext( mContext );
	}

	virtual void redo( void )
	{
		mContext = gApp->global().newContext();
	}

private:
	fugio::ContextInterface			*mContext;
};

#endif // CMDNEWCONTEXT_H

#ifndef CMDPINREMOVE_H
#define CMDPINREMOVE_H

#include <QUndoCommand>

#include <fugio/global_interface.h>
#include <fugio/global.h>

#include "app.h"

class CmdPinRemove : public QUndoCommand
{
public:
	explicit CmdPinRemove( QSharedPointer<fugio::PinInterface> pPin )
		: mPin( pPin ), mNode( mPin->node() ), mLinks( mPin->connectedPins() )
	{
		setText( QObject::tr( "Pin Remove" ) );
	}

	virtual ~CmdPinRemove( void )
	{

	}

	virtual void undo( void )
	{
		mNode->addPin( mPin );

		for( QSharedPointer<fugio::PinInterface> P : mLinks )
		{
			mNode->context()->connectPins( mPin->globalId(), P->globalId() );
		}
	}

	virtual void redo( void )
	{
		for( QSharedPointer<fugio::PinInterface> P : mLinks )
		{
			mNode->context()->disconnectPins( mPin->globalId(), P->globalId() );
		}

		mNode->removePin( mPin );
	}

private:
	QSharedPointer<fugio::PinInterface>		 mPin;
	QSharedPointer<fugio::NodeInterface>		 mNode;
	QList<QSharedPointer<fugio::PinInterface>>	 mLinks;
};

#endif // CMDPINREMOVE_H

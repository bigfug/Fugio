#ifndef CMDSETDEFAULTVALUE_H
#define CMDSETDEFAULTVALUE_H

#include <QUndoCommand>

#include <fugio/global_interface.h>
#include <fugio/global.h>

#include "app.h"

class CmdSetDefaultValue : public QUndoCommand
{
public:
	explicit CmdSetDefaultValue( QSharedPointer<fugio::PinInterface> pPin, const QVariant &pValNew )
		: mPin( pPin ), mValOld( pPin->value() ), mValNew( pValNew )
	{
		setText( QObject::tr( "Set pin default value" ) );
	}

	virtual ~CmdSetDefaultValue( void )
	{

	}

	virtual void undo( void )
	{
		mPin->setValue( mValOld );

		fugio::ContextInterface		*Context = ( mPin->node() && mPin->node()->context() ? mPin->node()->context() : nullptr );

		if( Context )
		{
			Context->updateNode( Context->findNode( mPin->node()->uuid() ) );
		}
	}

	virtual void redo( void )
	{
		mPin->setValue( mValNew );

		fugio::ContextInterface		*Context = ( mPin->node() && mPin->node()->context() ? mPin->node()->context() : nullptr );

		if( Context )
		{
			Context->updateNode( Context->findNode( mPin->node()->uuid() ) );
		}
	}

private:
	QSharedPointer<fugio::PinInterface>		 mPin;
	const QVariant							 mValOld, mValNew;
};

#endif // CMDSETDEFAULTVALUE_H

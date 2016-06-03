#ifndef CMDSETPINNAME_H
#define CMDSETPINNAME_H

#include <QUndoCommand>
#include <QUuid>

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>
#include <fugio/node_interface.h>

class CmdSetPinName : public QUndoCommand
{
public:
	explicit CmdSetPinName( QSharedPointer<fugio::PinInterface> pPin, const QString &pNewVal )
		: mPin( pPin ), mValOld( pPin->name() ), mValNew( pNewVal )
	{
		setText( QObject::tr( "Rename Pin" ) );
	}

	virtual ~CmdSetPinName( void )
	{

	}

	virtual void undo( void )
	{
		QSharedPointer<fugio::PinInterface>		PIN = mPin.toStrongRef();

		if( PIN )
		{
			PIN->setName( mValOld );
		}
	}

	virtual void redo( void )
	{
		QSharedPointer<fugio::PinInterface>		PIN = mPin.toStrongRef();

		if( PIN )
		{
			PIN->setName( mValNew );
		}
	}

private:
	QWeakPointer<fugio::PinInterface>			 mPin;
	QString								 mValOld, mValNew;
};

#endif // CMDSETPINNAME_H


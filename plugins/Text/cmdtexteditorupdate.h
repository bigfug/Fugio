#ifndef CMDTEXTEDITORUPDATE_H
#define CMDTEXTEDITORUPDATE_H

#include <QUndoCommand>

#include <fugio/node_interface.h>
#include <fugio/context_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/core/variant_interface.h>

//#include <fugiolib_global.h>

//#include "app.h"

class CmdTextEditorUpdate : public QUndoCommand
{
public:
	explicit CmdTextEditorUpdate( QSharedPointer<fugio::PinInterface> pPin, const QString &pString )
		: mPin( pPin ), mNewTxt( pString )
	{
		//setText( QObject::tr( "Add link to new pin" ) );

		fugio::VariantInterface	*V = qobject_cast<fugio::VariantInterface *>( mPin->control()->qobject() );

		if( V )
		{
			mOldTxt = V->variant().toString();
		}
	}

	virtual ~CmdTextEditorUpdate( void ) Q_DECL_OVERRIDE {}

	virtual void undo( void ) Q_DECL_OVERRIDE
	{
		fugio::VariantInterface	*V = qobject_cast<fugio::VariantInterface *>( mPin->control()->qobject() );

		if( V )
		{
			V->setVariant( mOldTxt );

			mPin->node()->context()->pinUpdated( mPin );
		}
	}

	virtual void redo( void ) Q_DECL_OVERRIDE
	{
		fugio::VariantInterface	*V = qobject_cast<fugio::VariantInterface *>( mPin->control()->qobject() );

		if( V )
		{
			V->setVariant( mNewTxt );

			mPin->node()->context()->pinUpdated( mPin );
		}
	}

private:
	QSharedPointer<fugio::PinInterface>		 mPin;
	QString									 mOldTxt;
	QString									 mNewTxt;
};

#endif // CMDTEXTEDITORUPDATE_H

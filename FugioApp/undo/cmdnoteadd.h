#ifndef CMDNOTEADD_H
#define CMDNOTEADD_H

#include <QUndoCommand>
#include <QUuid>

#include "contextwidgetprivate.h"
#include "contextview.h"
#include "noteitem.h"
#include "app.h"

class CmdNoteAdd : public QUndoCommand
{
public:
	explicit CmdNoteAdd( ContextWidgetPrivate *pContextWidget )
		: mContextWidget( pContextWidget )
	{
		setText( QObject::tr( "Add Note" ) );

		gApp->incrementStatistic( "note-added" );
	}

	virtual ~CmdNoteAdd( void )
	{

	}

	virtual void undo( void )
	{
		QSharedPointer<NoteItem>	Note = mNoteItem.toStrongRef();

		if( Note )
		{
			mContextWidget->view()->scene()->removeItem( Note.data() );
		}
	}

	virtual void redo( void )
	{
		QSharedPointer<NoteItem>	Note = mNoteItem.toStrongRef();

		if( !Note )
		{
			mContextWidget->view()->scene()->clearSelection();

			mNoteItem = mContextWidget->view()->noteAdd( QObject::tr( "New Note" ), QUuid() );
		}
	}

private:
	ContextWidgetPrivate		*mContextWidget;
	QWeakPointer<NoteItem>		 mNoteItem;
};

#endif // CMDNOTEADD_H

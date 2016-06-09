#ifndef CMDCONTEXTVIEWPASTE_H
#define CMDCONTEXTVIEWPASTE_H

#include <QUndoCommand>

#include <fugio/global_interface.h>
#include <fugio/global.h>

#include "app.h"
#include "contextview.h"

class CmdContextViewPaste : public QUndoCommand
{
public:
	explicit CmdContextViewPaste( ContextView *pContext, const QByteArray &pPasteData, const QPointF &pPastePoint = QPointF() )
		: mContext( pContext ), mPasteData( pPasteData ), mPastePoint( pPastePoint )
	{
		setText( QObject::tr( "Paste" ) );
	}

	virtual ~CmdContextViewPaste( void )
	{

	}

	virtual void undo( void )
	{
		for( QUuid U : mPasteNodes )
		{
			mContext->context()->unregisterNode( U );
		}

		mContext->decreasePasteOffset();
	}

	virtual void redo( void )
	{
		const QString		TempFile = QDir( QDir::tempPath() ).absoluteFilePath( "fugio-copy.tmp" );

		if( true )
		{
			QFile		FH( TempFile );

			if( !FH.open( QIODevice::WriteOnly ) )
			{
				return;
			}

			if( FH.write( mPasteData ) )
			{
				FH.close();
			}
		}

		mContext->scene()->clearSelection();

		mContext->clearPasteNodes();

		mContext->setUndoNodeUpdates( false );

		mContext->setPastePoint( mPastePoint );

		mContext->context()->load( TempFile, true );

		mContext->setUndoNodeUpdates( true );

		QFile::remove( TempFile );

		mPasteNodes = mContext->pasteNodes();

		mContext->increasePasteOffset();
	}

private:
	ContextView				*mContext;
	const QByteArray		 mPasteData;
	QList<QUuid>			 mPasteNodes;
	const QPointF			 mPastePoint;
};

#endif // CMDCONTEXTVIEWPASTE_H


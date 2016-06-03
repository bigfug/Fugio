#include "contextsubwindow.h"

#include <QMessageBox>
#include <QMdiArea>
#include <QFileInfo>

#include "contextwidgetprivate.h"
#include "contextview.h"

ContextSubWindow::ContextSubWindow( QWidget *pParent, Qt::WindowFlags pFlags )
	: QMdiSubWindow( pParent, pFlags )
{

}

ContextSubWindow::~ContextSubWindow()
{

}

ContextWidgetPrivate *ContextSubWindow::contextWidget()
{
	return( qobject_cast<ContextWidgetPrivate *>( widget() ) );
}

void ContextSubWindow::closeEvent( QCloseEvent *pEvent )
{
	if( contextWidget()->undoStack()->isClean() )
	{
		pEvent->accept();

		return;
	}

	//mdiArea()->setActiveSubWindow( this );

	QMessageBox		MsgBox;

	MsgBox.setWindowTitle( QFileInfo( contextWidget()->filename() ).fileName() );
	MsgBox.setText( "The patch has been modified." );
	MsgBox.setInformativeText( "Do you want to save your changes?" );
	MsgBox.setStandardButtons( QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel );
	MsgBox.setDefaultButton( QMessageBox::Save );

	switch( MsgBox.exec() )
	{
		case QMessageBox::Save:
			contextWidget()->userSave();

			if( !contextWidget()->undoStack()->isClean() )
			{
				pEvent->ignore();

				return;
			}
			break;

		case QMessageBox::Discard:
			break;

		case QMessageBox::Cancel:
			pEvent->ignore();
			return;

		default:
			break;
	}

	pEvent->accept();
}


void ContextSubWindow::focusInEvent( QFocusEvent *pEvent )
{
	QMdiSubWindow::focusInEvent( pEvent );
}

void ContextSubWindow::focusOutEvent( QFocusEvent *pEvent )
{
	QMdiSubWindow::focusOutEvent( pEvent );
}

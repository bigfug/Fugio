#include "texteditorgui.h"

#include <QMainWindow>
#include <QPushButton>
#include <QSettings>
#include <QCoreApplication>

#include "fugio/global_interface.h"
#include "fugio/context_interface.h"
#include <fugio/context_widget_interface.h>
#include <fugio/text/syntax_highlighter_instance_interface.h>
#include <fugio/node_signals.h>
#include <fugio/pin_signals.h>
#include <fugio/global_interface.h>
#include <fugio/context_signals.h>
#include <fugio/editor_interface.h>

#include "texteditorgui.h"
#include "texteditorform.h"
#include "cmdtexteditorupdate.h"
#include "textplugin.h"
#include <fugio/text/syntax_error_interface.h>
#include <fugio/text/syntax_error_signals.h>

#include <fugio/utils.h>

void TextEditorGui::initialise( void )
{
	QSharedPointer<fugio::NodeInterface>		Node = mNode->node();

	fugio::EditorInterface	*EI = qobject_cast<fugio::EditorInterface *>( Node->context()->global()->findInterface( IID_EDITOR ) );

	if( !EI )
	{
		return;
	}

	if( ( mDockWidget = new QDockWidget( "TextEditor", EI->mainWindow() ) ) == Q_NULLPTR )
	{
		return;
	}

	mDockWidget->setObjectName( Node->uuid().toString() );

	connect( mDockWidget, SIGNAL( visibilityChanged( bool ) ), this, SLOT( dockSetVisible( bool ) ) );

	if( ( mTextEdit = new TextEditorForm( mDockWidget ) ) == Q_NULLPTR )
	{
		return;
	}

	setupTextEditor( mTextEdit->textEdit() );

	connect( mNode->qobject(), SIGNAL( nameChanged( QString ) ), mTextEdit, SLOT( updateNodeName( QString ) ) );

	mDockWidget->setWidget( mTextEdit );

	EI->mainWindow()->addDockWidget( mDockArea, mDockWidget );

	mTextEdit->updateNodeName( Node->name() );

	checkHighlighter();

	if( !mDockVisible )
	{
		mDockWidget->hide();
	}
}

void TextEditorGui::checkHighlighter()
{
	QUuid			 DefaultHighlighterUuid = ( mSyntaxInterface ? mSyntaxInterface->highlighterUuid() : QUuid() );

	if( mHighlighter )
	{
		bool		DeleteHighlighter = false;

		if( mHighlighterType == HIGHLIGHT_NONE )
		{
			DeleteHighlighter = true;
		}
		else if( !mPinOutputString->isConnected() && mHighlighterType == HIGHLIGHT_DEFAULT )
		{
			DeleteHighlighter = true;
		}
		else if( mPinOutputString->isConnected() && mHighlighterType == HIGHLIGHT_DEFAULT && mHighlighterUuid != DefaultHighlighterUuid )
		{
			DeleteHighlighter = true;
		}

		if( DeleteHighlighter )
		{
			delete mHighlighter;

			mHighlighter = nullptr;
		}
	}

	if( !mHighlighter )
	{
		if( mHighlighterType == HIGHLIGHT_DEFAULT && !DefaultHighlighterUuid.isNull() )
		{
			mHighlighter = TextPlugin::instance()->syntaxHighlighterInstance( DefaultHighlighterUuid );
		}
		else if( mHighlighterType == HIGHLIGHT_CUSTOM && !mHighlighterUuid.isNull() )
		{
			mHighlighter = TextPlugin::instance()->syntaxHighlighterInstance( mHighlighterUuid );
		}

		if( mHighlighter && mSyntaxInterface )
		{
			mHighlighter->updateErrors( mSyntaxInterface->syntaxErrors() );
		}
	}

	if( mHighlighter && mTextEdit && mHighlighter->highlighter()->document() != mTextEdit->textEdit()->document() )
	{
		mHighlighter->highlighter()->setDocument( mTextEdit->textEdit()->document() );
	}
}

bool TextEditorGui::isBuffered() const
{
	return( mBuffered );
}

void TextEditorGui::modificationUpdated( bool pModified )
{
	if( mDockWidget )
	{
		if( pModified && !isBuffered() )
		{
			mDockWidget->setWindowTitle( QString( "TextEditor: %1*" ).arg( mNode->node()->name() ) );
		}
		else
		{
			mDockWidget->setWindowTitle( QString( "TextEditor: %1" ).arg( mNode->node()->name() ) );
		}
	}
}

void TextEditorGui::editorUpdated( void )
{
	if( !mTextEdit )
	{
		return;
	}

	checkHighlighter();

	if( !mCurrentText.isEmpty() && mCurrentText != mTextEdit->textEdit()->document()->toPlainText() )
	{
		mTextEdit->textEdit()->document()->setPlainText( mCurrentText );

		emit modifiedChanged( false );
	}
}

void TextEditorGui::syntaxErrorsUpdated( QList<SyntaxError> pSyntaxErrors )
{
	if( mHighlighter )
	{
		mHighlighter->updateErrors( pSyntaxErrors );
	}

	if( mTextEdit )
	{
		mTextEdit->setSyntaxErrors( pSyntaxErrors );
	}
}

void TextEditorGui::bufferedUpdated( bool pBuffered )
{
	mBuffered = pBuffered;
}

void TextEditorGui::show()
{
	if( !mTextEdit )
	{
		return;
	}

	if( mDockWidget->isHidden() )
	{
		mDockWidget->show();
	}

	mTextEdit->textEdit()->activateWindow();
}

void TextEditorGui::textUpdated( QString pText )
{
	if( isBuffered() )
	{
		if( mTextEdit->textEdit()->document()->isModified() )
		{
			fugio::ContextWidgetInterface	*ICW = qobject_cast<fugio::ContextWidgetInterface *>( mNode->node()->context()->findInterface( IID_CONTEXT_WIDGET ) );

			if( ICW )
			{
				CmdTextEditorUpdate		*CMD = new CmdTextEditorUpdate( mPinOutputString, mTextEdit->textEdit()->document()->toPlainText() );

				ICW->undoStack()->push( CMD );
			}

			modificationUpdated( false );
		}
		else
		{
			pinUpdated( mPinOutputString );
		}
	}

	mCurrentText = pText;
}

void TextEditorGui::syntaxInterfaceUpdated( SyntaxErrorInterface *pSEI )
{
	if( mSyntaxInterface && mSyntaxInterface == pSEI )
	{
		return;
	}

	if( mSyntaxInterface )
	{
		disconnect( mSyntaxInterface->syntaxErrorSignals(), SIGNAL(syntaxErrorsUpdated(QList<fugio::SyntaxError>)), this, SLOT(syntaxErrorsUpdated(QList<fugio::SyntaxError>)) );

		mSyntaxInterface = Q_NULLPTR;
	}

	if( pSEI )
	{
		connect( pSEI->syntaxErrorSignals(), SIGNAL(syntaxErrorsUpdated(QList<fugio::SyntaxError>)), this, SLOT(syntaxErrorsUpdated(QList<fugio::SyntaxError>)) );

		if( mTextEdit && mTextEdit->textEdit()->document()->isEmpty() )
		{
			QString		NewTxt = pPin->value().toString();

			mValOutputString->setVariant( NewTxt );

			mTextEdit->textEdit()->document()->setPlainText( NewTxt );
		}

	}

	checkHighlighter();
}

void TextEditorGui::setupTextEditor( QPlainTextEdit *pTextEdit )
{
	QFont font;
	font.setFamily("Courier");
	font.setFixedPitch(true);
	font.setPointSize(10);

	pTextEdit->setFont(font);

	pTextEdit->setTabStopWidth( pTextEdit->fontMetrics().width( QLatin1Char( ' ' ) ) * 4 );
	pTextEdit->setLineWrapMode( QPlainTextEdit::NoWrap );

	pTextEdit->document()->setPlainText( mCurrentText );

	pTextEdit->document()->setModified( false );

	connect( pTextEdit, &QPlainTextEdit::modificationChanged, this, &TextEditorGui::modificationUpdated );

	connect( pTextEdit, &QPlainTextEdit::textChanged, this, &TextEditorGui::editorUpdated );

	connect( this, &TextEditorGui::modifiedChanged, pTextEdit->document(), &QTextDocument::setModified );
}

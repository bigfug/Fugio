
#include "texteditornode.h"

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

#include "texteditorform.h"
#include "cmdtexteditorupdate.h"
#include "textplugin.h"
#include <fugio/text/syntax_error_interface.h>
#include <fugio/text/syntax_error_signals.h>

#include <fugio/utils.h>

TextEditorNode::TextEditorNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mDockArea( Qt::BottomDockWidgetArea ), mDockVisible( true ),
	  mHighlighterType( HIGHLIGHT_DEFAULT )
{
	FUGID( PIN_INPUT_BUFFER, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_OUTPUT_STRING, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );

	pinInput( "Trigger", PID_FUGIO_NODE_TRIGGER );

	mPinInputBuffer = pinInput( "Buffer", PIN_INPUT_BUFFER );

	mPinInputBuffer->setValue( true );

	mValOutputString = pinOutput<fugio::VariantInterface *>( "Text", mPinOutputString, PID_STRING, PIN_OUTPUT_STRING );

	connect( mPinOutputString->qobject(), SIGNAL(linked(QSharedPointer<fugio::PinInterface>)), this, SLOT(outputLinked(QSharedPointer<fugio::PinInterface>)) );
	connect( mPinOutputString->qobject(), SIGNAL(unlinked(QSharedPointer<fugio::PinInterface>)), this, SLOT(outputUninked(QSharedPointer<fugio::PinInterface>)) );
}

TextEditorNode::~TextEditorNode( void )
{
}

QWidget *TextEditorNode::gui()
{
	QPushButton		*GUI = new QPushButton( "Edit..." );

	connect( GUI, SIGNAL(clicked()), this, SLOT(onEditClicked()) );

	return( GUI );
}

void TextEditorGui::checkHighlighter()
{
	fugio::SyntaxErrorInterface				*SEI = output<fugio::SyntaxErrorInterface *>( mPinOutputString );
	QUuid									 DefaultHighlighterUuid = ( SEI ? SEI->highlighterUuid() : QUuid() );

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

		if( mHighlighter && SEI )
		{
			mHighlighter->updateErrors( SEI->syntaxErrors() );
		}
	}

	if( mHighlighter && mTextEdit && mHighlighter->highlighter()->document() != mTextEdit->textEdit()->document() )
	{
		mHighlighter->highlighter()->setDocument( mTextEdit->textEdit()->document() );
	}
}

bool TextEditorNode::isBuffered() const
{
	return( variant( mPinInputBuffer ).toBool() );
}

void TextEditorNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	emit checkHighlighter();

	pinUpdated( mPinOutputString );
}

void TextEditorGui::initialise( void )
{
	QSharedPointer<fugio::NodeInterface>		Node = mNode->node();

	fugio::EditorInterface	*EI = qobject_cast<fugio::EditorInterface *>( Node->context()->global()->findInterface( IID_EDITOR ) );

	if( !EI )
	{
		return;
	}

	if( ( mDockWidget = new QDockWidget( "TextEditor", EI->mainWindow() ) ) == 0 )
	{
		return;
	}

	mDockWidget->setObjectName( Node->uuid().toString() );

	connect( mDockWidget, SIGNAL( visibilityChanged( bool ) ), this, SLOT( dockSetVisible( bool ) ) );

	if( ( mTextEdit = new TextEditorForm( mDockWidget ) ) == 0 )
	{
		return;
	}

	setupTextEditor( mTextEdit->textEdit() );

	connect( mTextEdit, SIGNAL( updateText() ), this, SLOT( onTextUpdate() ) );

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

bool TextEditorNode::initialise( void )
{
	TextEditorGui	*GUI = new TextEditorGui( this );

	GUI->moveToThread( QCoreApplication::instance()->thread() );

	connect( this, &TextEditorNode::closeEditor, GUI, &TextEditorGui::deleteLater );

	QMetaObject::invokeMethod( GUI, "initialise", Qt::QueuedConnection );

	return( true );
}

bool TextEditorNode::deinitialise( void )
{
	emit closeEditor();

	return( true );
}

void TextEditorNode::loadSettings( QSettings &pSettings )
{
	mValOutputString->setVariant( QString::fromUtf8( pSettings.value( "value" ).toByteArray() ) );

	mDockArea = (Qt::DockWidgetArea)pSettings.value( "dockarea", int( mDockArea ) ).toInt();

	mDockVisible = pSettings.value( "visible", mDockVisible ).toBool();

	mHighlighterType = HighlighterType( pSettings.value( "highlight-type", int( mHighlighterType ) ).toInt() );

	mHighlighterUuid = fugio::utils::string2uuid( pSettings.value( "highlight-uuid", fugio::utils::uuid2string( mHighlighterUuid ) ).toString() );
}

void TextEditorNode::saveSettings( QSettings &pSettings ) const
{
	pSettings.setValue( "value", mValOutputString->variant() );

	pSettings.setValue( "dockarea", int( mDockArea ) );

	if( !mDockVisible )
	{
		pSettings.setValue( "visible", mDockVisible );
	}

	if( mHighlighterType != HIGHLIGHT_DEFAULT )
	{
		pSettings.setValue( "highlight-type", int( mHighlighterType ) );
	}

	if( mHighlighterType == HIGHLIGHT_CUSTOM )
	{
		pSettings.setValue( "highlight-uuid", fugio::utils::uuid2string( mHighlighterUuid ) );
	}
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

	pTextEdit->document()->setPlainText( mValOutputString->variant().toString() );

	pTextEdit->document()->setModified( false );

	connect( pTextEdit, SIGNAL(modificationChanged(bool)), this, SLOT(onTextModified(bool)) );

	connect( pTextEdit, SIGNAL(textChanged()), this, SLOT(textChanged()) );

	connect( this, SIGNAL(modified(bool)), pTextEdit->document(), SLOT(setModified(bool)) );
}

void TextEditorNode::onEditClicked( void )
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

void TextEditorNode::onTextModified( bool pModified )
{
	if( mDockWidget )
	{
		if( pModified && !isBuffered() )
		{
			mDockWidget->setWindowTitle( QString( "TextEditor: %1*" ).arg( mNode->name() ) );
		}
		else
		{
			mDockWidget->setWindowTitle( QString( "TextEditor: %1" ).arg( mNode->name() ) );
		}
	}
}

void TextEditorNode::onTextUpdate()
{
	if( isBuffered() )
	{
		if( mTextEdit->textEdit()->document()->isModified() )
		{
			fugio::ContextWidgetInterface	*ICW = qobject_cast<fugio::ContextWidgetInterface *>( mNode->context()->findInterface( IID_CONTEXT_WIDGET ) );

			if( ICW )
			{
				CmdTextEditorUpdate		*CMD = new CmdTextEditorUpdate( mPinOutputString, mTextEdit->textEdit()->document()->toPlainText() );

				ICW->undoStack()->push( CMD );
			}

			emit modified( false );
		}
		else
		{
			pinUpdated( mPinOutputString );
		}
	}
}

void TextEditorNode::onTextPinUpdated()
{
	if( !mTextEdit )
	{
		return;
	}

	emit checkHighlighter();

	const QString	NewTxt = mValOutputString->variant().toString();

	if( NewTxt != mTextEdit->textEdit()->document()->toPlainText() && !NewTxt.isEmpty() )
	{
		mTextEdit->textEdit()->document()->setPlainText( NewTxt );

		emit modified( false );
	}
}

void TextEditorNode::dockSetVisible( bool pVisible )
{
	mDockVisible = pVisible;
}

void TextEditorNode::textChanged()
{
	QPlainTextEdit		*TextEdit = qobject_cast<QPlainTextEdit *>( sender() );

	if( TextEdit )
	{
		if( !isBuffered() )
		{
			const QString	TextData = TextEdit->document()->toPlainText();

			if( TextData != mValOutputString->variant().toString() )
			{
				mValOutputString->setVariant( TextData );

				pinUpdated( mPinOutputString );
			}

			TextEdit->document()->setModified( false );
		}
	}
}

void TextEditorNode::outputLinked( QSharedPointer<PinInterface> pPin )
{
	fugio::SyntaxErrorInterface	*SEI = ( pPin->hasControl() ? qobject_cast<fugio::SyntaxErrorInterface *>( pPin->control()->qobject() ) : nullptr );

	if( SEI )
	{
		connect( SEI->syntaxErrorSignals(), SIGNAL(syntaxErrorsUpdated(QList<fugio::SyntaxError>)), this, SLOT(syntaxErrorsUpdated(QList<fugio::SyntaxError>)) );
	}

	emit checkHighlighter();

	if( mTextEdit && mTextEdit->textEdit()->document()->isEmpty() )
	{
		QString		NewTxt = pPin->value().toString();

		mValOutputString->setVariant( NewTxt );

		mTextEdit->textEdit()->document()->setPlainText( NewTxt );
	}
}

void TextEditorNode::outputUninked( QSharedPointer<PinInterface> pPin )
{
	fugio::SyntaxErrorInterface	*SEI = ( pPin->hasControl() ? qobject_cast<fugio::SyntaxErrorInterface *>( pPin->control()->qobject() ) : nullptr );

	if( SEI )
	{
		disconnect( SEI->syntaxErrorSignals(), SIGNAL(syntaxErrorsUpdated(QList<fugio::SyntaxError>)), this, SLOT(syntaxErrorsUpdated(QList<fugio::SyntaxError>)) );
	}

	emit checkHighlighter();
}

void TextEditorNode::syntaxErrorsUpdated( QList<fugio::SyntaxError> pSyntaxErrors )
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


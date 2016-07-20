#include "texteditorform.h"
#include "ui_texteditorform.h"
#include <QFileDialog>
#include <QDockWidget>
#include <QToolBar>
#include <QToolButton>
#include <QMenu>

TextEditorForm::TextEditorForm(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::TextEditorForm), mHighlighter( 0 )
{
	ui->setupUi(this);

	ui->mErrorText->hide();

	connect( ui->mTextEdit, SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));

	QToolBar		*ToolBar = new QToolBar( this );

	ui->verticalLayout->insertWidget( 0, ToolBar );

	QMenu			*FileMenu = new QMenu();

	QAction			*FileOpen   = new QAction( tr( "&Open..." ), this );
	QAction			*FileSave   = new QAction( tr( "&Save..." ), this );
	QAction			*FileSaveAs = new QAction( tr( "&Save as..." ), this );

	FileMenu->addAction( FileOpen );
	FileMenu->addAction( FileSave );
	FileMenu->addAction( FileSaveAs );

	QToolButton		*ButtonMenu = new QToolButton( ToolBar );

	ButtonMenu->setMenu( FileMenu );
	ButtonMenu->setPopupMode( QToolButton::InstantPopup );
	ButtonMenu->setText( "&File" );

	ToolBar->addWidget( ButtonMenu );

	connect( FileOpen,   SIGNAL(triggered(bool)), this, SLOT(textOpen()) );
	connect( FileSave,   SIGNAL(triggered(bool)), this, SLOT(textSave()) );
	connect( FileSaveAs, SIGNAL(triggered(bool)), this, SLOT(textSaveAs()) );

	QToolButton		*ButtonUpdate = new QToolButton( ToolBar );

	ButtonUpdate->setText( tr( "Update" ) );

	ToolBar->addWidget( ButtonUpdate );

	connect( ButtonUpdate, SIGNAL(released()), this, SLOT(updateClicked()) );
}

TextEditorForm::~TextEditorForm()
{
	delete ui;
}

QPlainTextEdit *TextEditorForm::textEdit( void )
{
	return( ui->mTextEdit );
}

void TextEditorForm::setHighlighter(fugio::SyntaxHighlighterInterface *pHighlighter)
{
	mHighlighter = pHighlighter;

	ui->mTextEdit->setHighlighter( pHighlighter );
}

void TextEditorForm::errorsUpdated()
{
	ui->mTextEdit->update();

	ui->mTextEdit->highlightCurrentLine();

	cursorPositionChanged();
}

void TextEditorForm::updateNodeName( const QString &pName )
{
	QDockWidget		*W = qobject_cast<QDockWidget *>( parent() );

	if( W )
	{
		W->setWindowTitle( QString( "TextEditor: %1" ).arg( pName ) );
	}
}

void TextEditorForm::updateClicked()
{
	emit updateText();
}

void TextEditorForm::textOpen()
{
	QString		FileName = QFileDialog::getOpenFileName( this );

	if( FileName.isEmpty() )
	{
		return;
	}

	QFile		FileHandle( FileName );

	if( !FileHandle.open( QIODevice::ReadOnly | QIODevice::Text ) )
	{
		return;
	}

	ui->mTextEdit->document()->setPlainText( FileHandle.readAll() );

	mFileName = FileName;
}

void TextEditorForm::textSave()
{
	if( mFileName.isEmpty() )
	{
		textSaveAs();
	}
	else
	{
		QFile		FileHandle( mFileName );

		if( !FileHandle.open( QIODevice::WriteOnly | QIODevice::Text ) )
		{
			return;
		}

		FileHandle.write( qPrintable( ui->mTextEdit->document()->toPlainText() ) );
	}
}

void TextEditorForm::textSaveAs()
{
	QString		FileName = QFileDialog::getSaveFileName( this );

	if( FileName.isEmpty() )
	{
		return;
	}

	QFile		FileHandle( FileName );

	if( !FileHandle.open( QIODevice::WriteOnly | QIODevice::Text ) )
	{
		return;
	}

	FileHandle.write( qPrintable( ui->mTextEdit->document()->toPlainText() ) );

	mFileName = FileName;
}

void TextEditorForm::cursorPositionChanged()
{
	QStringList	ErrLst;
	QString		ErrMsg;

	if( mHighlighter )
	{
		if( !( ErrLst = mHighlighter->errorList( ui->mTextEdit->textCursor().blockNumber() + 1 ) ).isEmpty() )
		{
			ErrMsg = ErrLst.join( "\n" );
		}
		else if( !( ErrLst = mHighlighter->errorList( 0 ) ).isEmpty() )
		{
			ErrMsg = ErrLst.join( "\n" );
		}
	}

	if( ErrMsg.isEmpty() )
	{
		ui->mErrorText->hide();
	}
	else
	{
		ui->mErrorText->show();

		ui->mErrorText->setText( ErrMsg );
	}
}

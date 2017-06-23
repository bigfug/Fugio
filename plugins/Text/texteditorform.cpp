#include "texteditorform.h"
#include "ui_texteditorform.h"
#include <QFileDialog>
#include <QDockWidget>
#include <QToolBar>
#include <QToolButton>
#include <QMenu>

#include <fugio/text/syntax_highlighter_factory_interface.h>

#include "textplugin.h"

TextEditorForm::TextEditorForm(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::TextEditorForm)
{
	ui->setupUi(this);

	ui->mErrorText->hide();

	connect( ui->mTextEdit, SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));

	QToolBar		*ToolBar = new QToolBar( this );

	ui->verticalLayout->insertWidget( 0, ToolBar );

	//-------------------------------------------------------------------------

	QMenu			*FileMenu = new QMenu();

	QAction			*FileOpen   = new QAction( tr( "&Open..." ), this );
	QAction			*FileSave   = new QAction( tr( "&Save..." ), this );
	QAction			*FileSaveAs = new QAction( tr( "&Save as..." ), this );

	FileMenu->addAction( FileOpen );
	FileMenu->addAction( FileSave );
	FileMenu->addAction( FileSaveAs );

	//-------------------------------------------------------------------------

	QToolButton		*ButtonMenu = new QToolButton( ToolBar );

	ButtonMenu->setMenu( FileMenu );
	ButtonMenu->setPopupMode( QToolButton::InstantPopup );
	ButtonMenu->setText( "&File" );

	ToolBar->addWidget( ButtonMenu );

	connect( FileOpen,   SIGNAL(triggered(bool)), this, SLOT(textOpen()) );
	connect( FileSave,   SIGNAL(triggered(bool)), this, SLOT(textSave()) );
	connect( FileSaveAs, SIGNAL(triggered(bool)), this, SLOT(textSaveAs()) );

	//-------------------------------------------------------------------------

	QToolButton		*ButtonUpdate = new QToolButton( ToolBar );

	ButtonUpdate->setText( tr( "&Update" ) );

	ToolBar->addWidget( ButtonUpdate );

	connect( ButtonUpdate, SIGNAL(released()), this, SLOT(updateClicked()) );

	//-------------------------------------------------------------------------

#if 0 // TODO
	QAction			*A = nullptr;

	QList<SyntaxHighlighterInterface::SyntaxHighlighterIdentity>	HL = TextPlugin::instance()->syntaxHighlighters();

	QMenu			*SyntaxMenu = new QMenu();

	if( ( A = SyntaxMenu->addAction( tr( "None" ), this, SLOT(setSyntaxNone()) ) ) != nullptr )
	{
		A->setCheckable( true );
	}

	if( ( A = SyntaxMenu->addAction( tr( "Default" ), this, SLOT(setSyntaxDefault()) ) ) != nullptr )
	{
		A->setCheckable( true );
	}

	for( auto SH : HL )
	{
		QAction *A = SyntaxMenu->addAction( SH.second, [=]()
		{
			setSyntax( SH.first );
		} );

		A->setCheckable( true );
		A->setData( SH.first );
	}

	//-------------------------------------------------------------------------

	QToolButton		*SyntaxButton = new QToolButton( ToolBar );

	SyntaxButton->setMenu( SyntaxMenu );
	SyntaxButton->setPopupMode( QToolButton::InstantPopup );
	SyntaxButton->setText( "Syntax" );

	ToolBar->addWidget( SyntaxButton );
#endif
}

TextEditorForm::~TextEditorForm()
{
	delete ui;
}

QPlainTextEdit *TextEditorForm::textEdit( void )
{
	return( ui->mTextEdit );
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
	QStringList			ErrLst;
	QString				ErrMsg;

	int		LinNum = ui->mTextEdit->textCursor().blockNumber() + 1;

	for( fugio::SyntaxError SE : mSyntaxErrors )
	{
		if( SE.mLineStart <= LinNum && SE.mLineEnd >= LinNum )
		{
			ErrLst << SE.mError;
		}
	}

	if( ErrLst.isEmpty() )
	{
		for( fugio::SyntaxError SE : mSyntaxErrors )
		{
			if( SE.mLineStart <= 0 )
			{
				ErrLst << SE.mError;
			}
		}
	}

	ErrMsg = ErrLst.join( "\n" );

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

void TextEditorForm::setSyntaxNone()
{
	update();
}

void TextEditorForm::setSyntaxDefault()
{
	update();
}

void TextEditorForm::setSyntax( const QUuid &pUuid )
{
	SyntaxHighlighterFactoryInterface *Factory = TextPlugin::instance()->syntaxHighlighterFactory( pUuid );

	if( Factory )
	{
		fugio::SyntaxHighlighterInstanceInterface *Instance = Factory->syntaxHighlighterInstance();

		if( Instance )
		{

		}
	}
}

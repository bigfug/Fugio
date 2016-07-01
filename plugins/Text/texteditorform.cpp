#include "texteditorform.h"
#include "ui_texteditorform.h"
#include <QFileDialog>
#include <QDockWidget>

TextEditorForm::TextEditorForm(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::TextEditorForm), mHighlighter( 0 )
{
	ui->setupUi(this);

	ui->mErrorText->hide();

	connect( ui->mTextEdit, SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));
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

void TextEditorForm::on_mButtonUpdate_clicked()
{
	emit updateText();
}

void TextEditorForm::on_mButtonLoad_clicked()
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
}

void TextEditorForm::on_mButtonSave_clicked()
{
}

void TextEditorForm::on_mButtonSaveAs_clicked()
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

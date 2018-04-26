#include "stylesheetform.h"
#include "ui_stylesheetform.h"
#include "app.h"

#include <QFileDialog>
#include <QDir>
#include <QSettings>

StyleSheetForm::StyleSheetForm(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::StyleSheetForm)
{
	ui->setupUi(this);

	QFont font;
	font.setFamily("Courier");
	font.setStyleHint(QFont::Monospace);
	font.setFixedPitch(true);
	font.setPointSize(10);

	ui->mStyleSheet->setFont( font );

	const int tabStop = 4;  // 4 characters

	QFontMetrics metrics(font);

	ui->mStyleSheet->setTabStopWidth( tabStop * metrics.width( ' ' ) );

	QSettings		Settings;

	mFileName = Settings.value( "stylesheet-path", gApp->global().sharedDataPath() + "/stylesheets/default.css" ).toString();

	stylesLoad( mFileName );

	stylesApply();

	mChanged = false;
}

StyleSheetForm::~StyleSheetForm()
{
	if( mChanged )
	{
		on_mButtonStyleSave_clicked();
	}

	delete ui;
}

void StyleSheetForm::on_mButtonStyleUpdate_clicked()
{
	stylesApply();
}

void StyleSheetForm::on_mButtonStyleLoad_clicked()
{
	QString		FileName = QFileDialog::getOpenFileName( this, tr( "Open Stylesheet" ), mFileName, tr( "Stylesheets (*.css)" ) );

	if( FileName.isEmpty() )
	{
		return;
	}

	QSettings		Settings;

	mFileName = FileName;

	Settings.setValue( "stylesheet-path", mFileName );

	stylesLoad( mFileName );

	stylesApply();

	mChanged = false;
}

void StyleSheetForm::on_mButtonStyleSave_clicked()
{
	QString		FileName = QFileDialog::getSaveFileName( this, tr( "Save Stylesheet" ), mFileName, tr( "Stylesheets (*.css)" ) );

	if( !FileName.isEmpty() )
	{
		QSettings		Settings;

		mFileName = FileName;

		Settings.setValue( "stylesheet-path", mFileName );

		stylesSave( FileName );

		mChanged = false;
	}
}

void StyleSheetForm::stylesApply( void )
{
	QString		CurDir = QDir::currentPath();
	QString		NewDir = QFileInfo( mFileName ).path();

	if( !QDir::setCurrent( NewDir ) )
	{
		return;
	}

	gApp->setStyleSheet( ui->mStyleSheet->toPlainText() );

	QDir::setCurrent( CurDir );
}

void StyleSheetForm::stylesLoad( QString pFileName )
{
	QFile		ss( pFileName );

	if( ss.open( QIODevice::ReadOnly | QIODevice::Text ) )
	{
		QTextStream	ts( &ss );

		ui->mStyleSheet->setPlainText( ts.readAll() );

		mFileName = pFileName;

		ss.close();
	}
	else
	{
		qWarning() << "Couldn't open stylesheet" << pFileName;
	}
}

void StyleSheetForm::stylesSave( QString pFileName )
{
	QFile		ss( pFileName );

	if( ss.open( QIODevice::WriteOnly | QIODevice::Text ) )
	{
		QTextStream	ts( &ss );

		ts << ui->mStyleSheet->toPlainText();

		ss.close();

		mFileName = pFileName;
	}
	else
	{
		qWarning() << "Couldn't open stylesheet" << pFileName;
	}
}

void StyleSheetForm::on_mStyleSheet_textChanged()
{
	mChanged = true;
}

#include "contextwidgetprivate.h"
#include "ui_contextwidgetprivate.h"

#include <QFileDialog>
#include <QStylePainter>
#include <QStandardPaths>

#include "app.h"
#include "contextview.h"
#include "nodeitem.h"

#include <QPushButton>
#include <QDateTime>

#include <fugio/utils.h>

ContextWidgetPrivate::ContextWidgetPrivate(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ContextWidgetPrivate), mUndoStack( &gApp->undoGroup() ), mContextWidget( this )
{
	ui->setupUi(this);

	// ContextInterface doesn't (currently) have a 'save to file handle' method, so generate a temporary filename here

	generateRecoveryFilename();

	ui->mButtonPlay->setIcon( gApp->style()->standardIcon( QStyle::SP_MediaPlay ) );
	ui->mButtonStop->setIcon( gApp->style()->standardIcon( QStyle::SP_MediaStop ) );

	mButtonGroup = new QButtonGroup( this );

	if( ( mSplitter = new QSplitter( this ) ) == 0 )
	{
		return;
	}

	mSplitter->setOrientation( Qt::Horizontal );

	ui->mVerticalLayout->insertWidget( 1, mSplitter, 1 );

	if( ( mContextView = new ContextView( mSplitter ) ) == 0 )
	{
		return;
	}

	if( mContextView )
	{
		mSplitter->addWidget( mContextView );

		QPushButton		*B = new QPushButton( tr( "Node Editor" ), this );

		if( B != 0 )
		{
			// Add node editor with id 0

			mButtonGroup->addButton( B, 0 );

			B->setCheckable( true );
			B->setChecked( true );

			B->setShortcut( Qt::Key_F1 );

			ui->mLayoutButtons->insertWidget( ui->mLayoutButtons->count() - 1, B );

			connect( B, SIGNAL(toggled(bool)), mContextView, SLOT(setVisible(bool)) );
		}
	}

//	if( ( mInspector = new NodeInspectorForm( mSplitter ) ) != 0 )
//	{
//		mSplitter->addWidget( mInspector );
//	}

//	QList<int>		SzeLst;

//	SzeLst.append( width() );
//	SzeLst.append( 0 );

//	mSplitter->setSizes( SzeLst );

//	connect( mContextView, SIGNAL(nodeInspection(NodeItem*)), this, SLOT(onNodeInspection(NodeItem*)) );
}

ContextWidgetPrivate::~ContextWidgetPrivate()
{
	gApp->global().delContext( mContext );

	if( !mRecoveryFilename.isEmpty() && QFile::exists( mRecoveryFilename ) )
	{
		QFile::remove( mRecoveryFilename );
	}

	delete ui;
}

void ContextWidgetPrivate::generateRecoveryFilename()
{
	mRecoveryFilename = QString::number( int( QDateTime::currentMSecsSinceEpoch() % 0xffffffff ), 16 );

	mRecoveryFilename = QString( "fugio_tmp_%1.fug" ).arg( mRecoveryFilename );

	mRecoveryFilename = QDir( QStandardPaths::writableLocation( QStandardPaths::DataLocation ) ).absoluteFilePath( mRecoveryFilename );
}

void ContextWidgetPrivate::setContext( QSharedPointer<fugio::ContextInterface> pContext )
{
	mContext = pContext;

	mContextView->setContext( pContext );

	pContext->registerInterface( IID_CONTEXT_WIDGET, this );

	onContextDurationChanged( mContext->duration() );

	connect( mContext->qobject(), SIGNAL(durationChanged(qreal)), this, SLOT(onContextDurationChanged(qreal)) );
}

QSharedPointer<fugio::ContextInterface> ContextWidgetPrivate::context()
{
	return( mContextView->context() );
}

void ContextWidgetPrivate::timeout()
{
	if( mContextView->context()->isPlaying() )
	{
		if( ui->mTimeSlider->isEnabled() )
		{
			ui->mTimeSlider->setEnabled( false );
			ui->mTimeText->setEnabled( false );
		}

		ui->mTimeSlider->setValue( mContext->position() * 1000.0 );

		ui->mTimeText->setText( fugio::utils::timeToString( mContext->position() ) );
	}
	else
	{
		if( !ui->mTimeSlider->isEnabled() )
		{
			ui->mTimeSlider->setEnabled( true );
			ui->mTimeText->setEnabled( true );
		}

		if( !ui->mTimeSlider->hasFocus() )
		{
			ui->mTimeSlider->setValue( mContext->position() * 1000.0 );
		}

		if( !ui->mTimeText->hasFocus() )
		{
			ui->mTimeText->setText( fugio::utils::timeToString( mContext->position() ) );
		}
	}

	emit mContextWidget.updateGui();
}

void ContextWidgetPrivate::addWidget(QWidget *pWidget)
{
	//QList<int>	Sizes = mSplitter->sizes();

	QPushButton		*B = new QPushButton( pWidget->objectName(), this );

	if( B != 0 )
	{
		B->setCheckable( true );
		B->setChecked( false );

		mButtonGroup->addButton( B );

		B->setShortcut( Qt::Key( quint32( Qt::Key_F1 ) + mSplitter->count() ) );

		//B->setShortcut( Qt::Key_F2 );

		ui->mLayoutButtons->insertWidget( ui->mLayoutButtons->count() - 1, B );

		connect( B, SIGNAL(toggled(bool)), pWidget, SLOT(setVisible(bool)) );
	}

	mSplitter->addWidget( pWidget );

	pWidget->setVisible( false );

//	if( Sizes.size() > 0 )
//	{
//		Sizes.append( 0 );

//		mSplitter->setSizes( Sizes );
	//	}
}

void ContextWidgetPrivate::setWindowTitleFromFileName(const QString &pFileName)
{
	QStringList	FileNameParts = QDir::fromNativeSeparators( pFileName ).split( '/' );

	while( FileNameParts.size() > 2 )
	{
		FileNameParts.removeFirst();
	}

	setWindowTitle( QDir::toNativeSeparators( QString( "%1/%2" ).arg( FileNameParts.first() ).arg( FileNameParts.last() ) ) );
}

void ContextWidgetPrivate::setGroupWidgetText(const QString pText)
{
	ui->mGroupLinks->setText( pText );
}

void ContextWidgetPrivate::userSave( void )
{
	if( filename().isEmpty() )
	{
		userSaveAs();
	}
	else if( context()->save( filename() ) )
	{
		undoStack()->setClean();

		emit contextFilenameChanged( filename() );
	}
}

void ContextWidgetPrivate::userSaveAs( void )
{
	QSettings				 Settings;
	const QString	DatDir = QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation );

	QString					 PatchDirectory = Settings.value( "patch-directory", QDir( DatDir ).absoluteFilePath( "Fugio" ) ).toString();

	QString		FileName = QFileDialog::getSaveFileName( this, tr( "Save Patch" ), PatchDirectory, tr( "Fugio Patches (*.fug)" ) );

	if( FileName.isEmpty() )
	{
		return;
	}

	mFileName = FileName;

	PatchDirectory = QFileInfo( FileName ).absoluteDir().path();

	Settings.setValue( "patch-directory", PatchDirectory );

	setWindowTitleFromFileName( FileName );

	context()->save( FileName );

	undoStack()->setClean();

	emit contextFilenameChanged( FileName );
}

void ContextWidgetPrivate::load(const QString &pFileName)
{
	mFileName = pFileName;

	setWindowTitleFromFileName( mFileName );

	mContextView->clearPasteOffset();

	context()->load( mFileName );

	mContextView->resetPasteOffset();
}

void ContextWidgetPrivate::cut()
{
	int		ButtonId = mButtonGroup->checkedId();

	if( ButtonId == -1 )
	{
		return;
	}

	if( !ButtonId )
	{
		mContextView->cut();
	}
}

void ContextWidgetPrivate::copy()
{
	int		ButtonId = mButtonGroup->checkedId();

	if( ButtonId == -1 )
	{
		return;
	}

	if( !ButtonId )
	{
		mContextView->copy();
	}
}

void ContextWidgetPrivate::paste()
{
	int		ButtonId = mButtonGroup->checkedId();

	if( ButtonId == -1 )
	{
		return;
	}

	if( !ButtonId )
	{
		mContextView->paste();
	}
}

void ContextWidgetPrivate::saveSelectedTo(const QString &pFileName)
{
	int		ButtonId = mButtonGroup->checkedId();

	if( ButtonId == -1 )
	{
		return;
	}

	if( !ButtonId )
	{
		mContextView->saveSelectedTo( pFileName );
	}
}

void ContextWidgetPrivate::on_mButtonPlay_clicked()
{
	context()->play();
}

void ContextWidgetPrivate::on_mButtonStop_clicked()
{
	static qint64	LastStop = 0;
	qint64			CurrStop = QDateTime::currentMSecsSinceEpoch();

	if( context()->isPlaying() )
	{
		context()->stop();
	}

	if( CurrStop - LastStop <= QApplication::doubleClickInterval() )
	{
		context()->setPlayheadPosition( 0 );
	}

	LastStop = CurrStop;
}

void ContextWidgetPrivate::onContextDurationChanged( qreal pDuration )
{
	ui->mTimeSlider->setRange( 0, pDuration * 1000.0 );

	timeout();
}

//void ContextWidgetPrivate::onNodeInspection( NodeItem *pNodeItem )
//{
//	QList<int>		SzeLst;

//	if( pNodeItem == 0 )
//	{
//		SzeLst.append( width() );
//		SzeLst.append( 0 );

//		//mInspector->inspectNode( QSharedPointer<fugio::NodeInterface>() );
//	}
//	else
//	{
//		SzeLst.append( ( width() * 3 ) / 4 );
//		SzeLst.append( ( width() * 1 ) / 4 );

//		//mInspector->inspectNode( mContext->findNode( pNodeItem->id() ) );
//	}

//	//mSplitter->setSizes( SzeLst );
//}

void ContextWidgetPrivate::on_mTimeText_textEdited( const QString &arg1 )
{
	if( !mContextView->context()->isPlaying() )
	{
		mContextView->context()->setPlayheadPosition( fugio::utils::stringToTime( arg1 ) );

		ui->mTimeSlider->setValue( mContext->position() * 1000.0 );
	}
}

void ContextWidgetPrivate::on_mTimeSlider_sliderMoved( int position )
{
	if( !mContextView->context()->isPlaying() )
	{
		qreal		NewTim = qreal( position ) / 1000.0;

		mContext->setPlayheadPosition( NewTim );

		ui->mTimeText->setText( fugio::utils::timeToString( mContext->position() ) );
	}
}


QUndoStack *ContextWidgetPrivate::undoStack()
{
	return( &mUndoStack );
}

fugio::ContextWidgetSignals *ContextWidgetPrivate::qobject()
{
	return( &mContextWidget );
}

void ContextWidgetPrivate::paintEvent(QPaintEvent *)
{
	QStyleOption opt;

	opt.initFrom( this );

	QStylePainter( this ).drawPrimitive( QStyle::PE_Widget, opt );
}

void ContextWidgetPrivate::groupSelected()
{
	int		ButtonId = mButtonGroup->checkedId();

	if( ButtonId == -1 )
	{
		return;
	}

	if( ButtonId == 0 )
	{
		mContextView->groupSelected();
	}
}

void ContextWidgetPrivate::ungroupSelected()
{
	if( !mButtonGroup->checkedId() )
	{
		mContextView->ungroupSelected();
	}
}

void ContextWidgetPrivate::groupParent()
{
	if( !mButtonGroup->checkedId() )
	{
		mContextView->groupParent();
	}
}

void ContextWidgetPrivate::loadRecovery( const QString &pFileName )
{
	mRecoveryFilename = pFileName;

	mContext->load( mRecoveryFilename );

	mFileName = mContext->metaInfo( fugio::ContextInterface::Filename );
}

void ContextWidgetPrivate::saveRecovery()
{
	mContext->setMetaInfo( fugio::ContextInterface::Filename, mFileName );

	mContext->save( mRecoveryFilename );
}

void ContextWidgetPrivate::on_mGroupLinks_linkActivated( const QString &link )
{
	emit groupLinkActivated( link );
}

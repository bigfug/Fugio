#include "numbermonitorform.h"
#include "ui_numbermonitorform.h"

#include <QPainter>
#include <QPaintEvent>
#include <QDockWidget>
#include <QMenu>
#include <QFileDialog>
#include <QImageWriter>

NumberMonitorForm::NumberMonitorForm(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::NumberMonitorForm), mIndex( 0 ), mMin( 0 ), mMax( 1 ), mY( 0 )
{
	ui->setupUi(this);
}

NumberMonitorForm::~NumberMonitorForm()
{
	delete ui;
}

void NumberMonitorForm::value( const QList< QPair<QColor,qreal> > &pValLst )
{
	for( const QPair<QColor,qreal> &V : pValLst )
	{
		if( V.second < mMin )
		{
			mMin = V.second;
		}

		if( V.second > mMax )
		{
			mMax = V.second;
		}
	}

	if( mY.size() != pValLst.size() )
	{
		mY.resize( pValLst.size() );
	}

	if( mImage.size() != size() )
	{
		mImage = QImage( size(), QImage::Format_ARGB32_Premultiplied );

		QPainter	Painter( &mImage );

		Painter.setBackground( Qt::white );

		Painter.eraseRect( mImage.rect() );

		mIndex = 0;

		update();
	}

	if( mIndex >= mImage.width() )
	{
		mIndex = 0;
	}

	QPainter	Painter( &mImage );

	Painter.setPen( Qt::white );

	Painter.drawLine( mIndex, 0, mIndex, height() );

	for( int i = 0 ; i < pValLst.size() ; i++ )
	{
		const QPair<QColor,qreal> &V = pValLst.at( i );

		int		y = ( 1.0 - qBound( 0.0, ( ( V.second - mMin ) / ( mMax - mMin ) ), 1.0 ) ) * ( height() - 1 );

		Painter.setPen( V.first );

		if( !mIndex || abs( y - mY[ i ] ) == 1 )
		{
			Painter.drawPoint( mIndex, y );
		}
		else
		{
			Painter.drawLine( mIndex - 1, mY[ i ], mIndex, y );
		}

		mY[ i ] = y;
	}

	update( mIndex - 1, 0, 1, height() );

	mIndex++;
}

void NumberMonitorForm::setNodeName(const QString &pName)
{
	QDockWidget			*DW = qobject_cast<QDockWidget *>( parent() );

	if( DW )
	{
		DW->setWindowTitle( tr( "Monitor: %1" ).arg( pName ) );
	}
}

void NumberMonitorForm::paintEvent( QPaintEvent *pEvent )
{
	QWidget::paintEvent( pEvent );

	QPainter		Painter( this );

	Painter.drawImage( pEvent->rect(), mImage, pEvent->rect() );

	Painter.setPen( Qt::red );

	Painter.drawLine( mIndex, 0, mIndex, height() );
}

void NumberMonitorForm::contextMenuEvent( QContextMenuEvent *pEvent )
{
	QMenu	Menu;

	Menu.addAction( "Save image...", this, SLOT(saveImage()) );

	Menu.exec( pEvent->globalPos() );
}

void NumberMonitorForm::saveImage()
{
	QString		Filename = QFileDialog::getSaveFileName( this, tr( "Save Image" ), mSaveImageDir, tr( "PNG Images (*.png);;JPG Images (*jpg)" ) );

	if( !Filename.isEmpty() )
	{
		QFileInfo		FileInfo( Filename );

		if( FileInfo.suffix().toLower() != "png" && FileInfo.suffix().toLower() != "jpg" )
		{
			Filename += ".png";

			FileInfo = QFileInfo( Filename );
		}

		mSaveImageDir = FileInfo.absolutePath();

		mImage.save( Filename );
	}
}

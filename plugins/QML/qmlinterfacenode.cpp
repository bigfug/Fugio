#include "qmlinterfacenode.h"

#include <fugio/core/uuid.h>

#include <QUrl>
#include <QQmlContext>
#include <QDockWidget>
#include <QMainWindow>
#include <QQmlEngine>
#include <QTemporaryFile>
#include <QDir>
#include <QQuickItem>
#include <QStandardPaths>
#include <QQmlError>

#include <fugio/context_interface.h>
#include <fugio/context_widget_signals.h>
#include <fugio/pin_signals.h>
#include <fugio/pin_control_interface.h>
#include <fugio/file/filename_interface.h>
#include <fugio/node_signals.h>
#include <fugio/editor_interface.h>

#include <fugio/image/image.h>

#include <fugio/text/uuid.h>

#include <fugio/qml/uuid.h>

#include <fugio/core/variant_interface.h>

#include <fugio/text/syntax_highlighter_factory_interface.h>
#include <fugio/text/syntax_highlighter_instance_interface.h>

#include "qmlwidget.h"
#include "qmlplugin.h"

QImage ImageProvider::requestImage( const QString &id, QSize *size, const QSize &requestedSize )
{
	return( mNode->imageRequest( id, size, requestedSize ) );
}

QMLInterfaceNode::QMLInterfaceNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mDockWidget( nullptr ), mWidget( nullptr ), mQuickView( Q_NULLPTR ), mQML( pNode ), mDockArea( Qt::BottomDockWidgetArea )
{
	FUGID( PIN_INPUT_QML, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );

	mValInputSource = pinInput<fugio::SyntaxErrorInterface *>( "QML", mPinSource, PID_SYNTAX_ERROR, PIN_INPUT_QML );

	mPinSource->registerPinInputType( PID_FILENAME );
	mPinSource->registerPinInputType( PID_STRING );

	mValInputSource->setHighlighterUuid( SYNTAX_HIGHLIGHTER_QML );

	const char *Source =
		"import QtQuick 2.6\n\n"
		"Item {\n\n"
		"}\n";

	mPinSource->setValue( Source );
}

bool QMLInterfaceNode::initialise( void )
{
	fugio::EditorInterface	*EI = qobject_cast<fugio::EditorInterface *>( mNode->context()->global()->findInterface( IID_EDITOR ) );

	QQmlEngine		*Engine = nullptr;

	if( EI )
	{
		if( ( mDockWidget = new QDockWidget( "QML", EI->mainWindow() ) ) == 0 )
		{
			return( false );
		}

		connect( mDockWidget, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)), this, SLOT(dockWidgetLocationUpdated(Qt::DockWidgetArea)) );

		mDockWidget->setObjectName( mNode->uuid().toString() );

		mWidget = new QMLWidget();

		if( mWidget )
		{
			connect( mWidget, &QQuickWidget::statusChanged, this, &QMLInterfaceNode::widgetStatusChanged );

			Engine = mWidget->engine();

			mWidget->rootContext()->setContextProperty( "fugio", &mQML );

			mWidget->rootContext()->setContextObject( &mQML );

			mWidget->setObjectName( mNode->uuid().toString() );

			mWidget->setResizeMode( QQuickWidget::SizeRootObjectToView );

			mDockWidget->setWidget( mWidget );

			EI->mainWindow()->addDockWidget( mDockArea, mDockWidget );

			mWidget->updateNodeName( mNode->name() );
		}
	}
	else
	{
		mQuickView = new QQuickView();

		if( mQuickView )
		{
			Engine = mQuickView->engine();

			connect( mQuickView, &QQuickView::statusChanged, this, &QMLInterfaceNode::viewStatusChanged );

			mQuickView->rootContext()->setContextProperty( "fugio", &mQML );

			mQuickView->rootContext()->setContextObject( &mQML );

			mQuickView->setObjectName( mNode->uuid().toString() );

	//		mQuickView->setResizeMode( QQuickView::SizeRootObjectToView );

			mQuickView->setTitle( mNode->name() );

			mQuickView->show();
		}
	}

	if( Engine )
	{
		QDir			UsrDir = QDir( QStandardPaths::writableLocation( QStandardPaths::DataLocation ) );

		if( !UsrDir.cd( "qml" ) )
		{
			UsrDir.mkdir( "qml" );
			UsrDir.cd( "qml" );
		}

		Engine->addImportPath( QDir( QMLPlugin::app()->sharedDataPath() ).filePath( "qml" ) );
		Engine->addImportPath( UsrDir.path() );

		qDebug() << Engine->importPathList();
		qDebug() << Engine->pluginPathList();

		Engine->addImageProvider( "fugio", new ImageProvider( this ) );
	}

	connect( this, &QMLInterfaceNode::signalInputsUpdated, &mQML, &QMLNode::signalInputsUpdated );

	connect( mNode->qobject(), SIGNAL(nameChanged(QString)), this, SLOT(nodeNameUpdated(QString)) );

	return( true );
}

bool QMLInterfaceNode::deinitialise( void )
{
	if( mWidget )
	{
		delete mWidget;

		mWidget = nullptr;
	}

	if( mDockWidget )
	{
		delete mDockWidget;

		mDockWidget = nullptr;
	}

	if( mQuickView )
	{
		mQuickView->deleteLater();

		mQuickView = Q_NULLPTR;
	}

	return( true );
}

void QMLInterfaceNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	if( mPinSource->isUpdated( pTimeStamp ) )
	{
		if( fugio::FilenameInterface *I = input<fugio::FilenameInterface *>( mPinSource ) )
		{
			if( mWidget )
			{
				mWidget->setSource( QUrl::fromLocalFile( I->filename() ) );
			}
			else if( mQuickView )
			{
				mQuickView->setSource( QUrl::fromLocalFile( I->filename() ) );

				mQuickView->show();
			}
		}

		if( fugio::VariantInterface *I = input<fugio::VariantInterface *>( mPinSource ) )
		{
			QTemporaryFile		FH;

			if( FH.open() )
			{
				FH.write( I->variant().toByteArray() );

				FH.close();

				if( mWidget )
				{
					mWidget->setSource( QUrl::fromLocalFile( FH.fileName() ) );
				}
				else if( mQuickView )
				{
					mQuickView->setSource( QUrl::fromLocalFile( FH.fileName() ) );
				}
			}
		}

		if( mWidget )
		{
//			qDebug() << mWidget->initialSize();
		}
		else if( mQuickView && mQuickView->rootObject() )
		{
//			qDebug() << mQuickView->initialSize() << mQuickView->rootObject()->width() << mQuickView->rootObject()->height();

			processObject( mQuickView->rootObject() );
		}
	}

	emit signalInputsUpdated( pTimeStamp );
}

QList<QUuid> QMLInterfaceNode::pinAddTypesInput() const
{
	return( mNode->context()->global()->pinIds().keys() );
}

bool QMLInterfaceNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_OUTPUT );
}

void QMLInterfaceNode::registerOwnership( QObject *O )
{
	QQmlEngine::setObjectOwnership( O, QQmlEngine::CppOwnership );
}

QImage QMLInterfaceNode::imageRequest( const QString &pId, QSize *pSize, const QSize &pRequestedSize )
{
	QSharedPointer<fugio::PinInterface>		P = mNode->findInputPinByName( pId );

	if( !P )
	{
		return( QImage() );
	}

	fugio::Image	SrcDat = variant<fugio::Image>( P );

	if( !SrcDat.isValid() )
	{
		return( QImage() );
	}

	QImage			SrcImg = SrcDat.image();

	if( SrcImg.isNull() )
	{
		return( QImage() );
	}

	if( pSize )
	{
		*pSize = SrcImg.size();
	}

	QSize		NewSze( pRequestedSize.width() > 0 ? pRequestedSize.width() : SrcImg.width(), pRequestedSize.height() > 0 ? pRequestedSize.height() : SrcImg.height() );

	if( NewSze != SrcImg.size() )
	{
		return( SrcImg.scaled( NewSze ) );
	}

	return( SrcImg );
}

void QMLInterfaceNode::nodeNameUpdated(const QString &pName)
{
	if( mQuickView )
	{
		mQuickView->setTitle( pName );
	}
}

void QMLInterfaceNode::processObject( QObject *O )
{
	for( QObject *C : O->children() )
	{
		processObject( C );
	}
}

QList<QUuid> QMLInterfaceNode::pinAddTypesOutput() const
{
	return( mNode->context()->global()->pinIds().keys() );
}

void QMLInterfaceNode::loadSettings( QSettings &pSettings )
{
	mDockArea = (Qt::DockWidgetArea)pSettings.value( "dockarea", int( mDockArea ) ).toInt();

	//		mDockVisible = pSettings.value( "visible", mDockVisible ).toBool();
}

void QMLInterfaceNode::saveSettings( QSettings &pSettings ) const
{
	pSettings.setValue( "dockarea", int( mDockArea ) );

	//		if( !mDockVisible )
	//		{
	//			pSettings.setValue( "visible", mDockVisible );
	//		}
}

void QMLInterfaceNode::processErrors( const QList<QQmlError> &pErrLst ) const
{
	QList<fugio::SyntaxError> ErrDat;

	for( QQmlError QmlErr : pErrLst )
	{
		fugio::SyntaxError	SE;

		SE.mColumnStart = SE.mColumnEnd = QmlErr.column();
		SE.mLineStart = SE.mLineEnd = QmlErr.line();

#if QT_VERSION >= QT_VERSION_CHECK( 5, 9, 0 )
		switch( QmlErr.messageType() )
		{
			case QtDebugMsg:
				SE.mError = "DBUG: ";
				break;

			case QtInfoMsg:
				SE.mError = "INFO: ";
				break;

			case QtWarningMsg:
				SE.mError = "WARN: ";
				break;

			case QtCriticalMsg:
				SE.mError = "CRIT: ";
				break;

			case QtFatalMsg:
				SE.mError = "FATL: ";
				break;
		}
#endif

		SE.mError.append( QmlErr.description() );

		ErrDat << SE;
	}

	mValInputSource->setSyntaxErrors( ErrDat );
}

void QMLInterfaceNode::viewStatusChanged( QQuickView::Status pStatus )
{
	if( pStatus == QQuickView::Error )
	{
		processErrors( mQuickView->errors() );

		mNode->setStatus( fugio::NodeInterface::Error );
	}
	else
	{
		mValInputSource->clearSyntaxErrors();

		mNode->setStatus( fugio::NodeInterface::Initialised );
	}
}

void QMLInterfaceNode::widgetStatusChanged( QQuickWidget::Status pStatus )
{
	if( pStatus == QQuickWidget::Error )
	{
		processErrors( mWidget->errors() );

		mNode->setStatus( fugio::NodeInterface::Error );
	}
	else
	{
		mValInputSource->clearSyntaxErrors();

		mNode->setStatus( fugio::NodeInterface::Initialised );
	}
}

void QMLInterfaceNode::dockWidgetLocationUpdated( Qt::DockWidgetArea pArea )
{
	mDockArea = pArea;
}


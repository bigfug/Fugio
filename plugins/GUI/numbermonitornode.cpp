#include "numbermonitornode.h"

#include <QMainWindow>
#include <QPushButton>

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>
//#include <fugio/colour/colour_interface.h>
#include <fugio/core/uuid.h>

#include <fugio/context_signals.h>

#include "numbermonitorform.h"

NumberMonitorNode::NumberMonitorNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mDockWidget( nullptr ), mMonitor( nullptr ), mDockArea( Qt::BottomDockWidgetArea ), mLastTime( 0 )
{
	mPinValue = pinInput( "Number" );
}

NumberMonitorNode::~NumberMonitorNode( void )
{
	QMainWindow		*MainWindow = mNode->context()->global()->mainWindow();

	if( mDockWidget != 0 )
	{
		MainWindow->removeDockWidget( mDockWidget );

		delete mDockWidget;

		mDockWidget = 0;
	}
}

QWidget *NumberMonitorNode::gui( void )
{
	QPushButton		*GUI = new QPushButton( "Show..." );

	connect( GUI, SIGNAL(clicked()), this, SLOT(onShowClicked()) );

	return( GUI );
}

bool NumberMonitorNode::initialise( void )
{
	if( !fugio::NodeControlBase::initialise() )
	{
		return( false );
	}

	QMainWindow		*MainWindow = mNode->context()->global()->mainWindow();

	if( MainWindow != 0 )
	{
		if( ( mDockWidget = new QDockWidget( QString( "Monitor: %1" ).arg( mNode->name() ), MainWindow ) ) == 0 )
		{
			return( false );
		}

		mDockWidget->setObjectName( mNode->name() );

		if( ( mMonitor = new NumberMonitorForm( mDockWidget ) ) == 0 )
		{
			return( false );
		}

		//setupTextEditor( mTextEdit->textEdit() );

		//connect( mTextEdit, SIGNAL(updateText()), this, SLOT(onTextUpdate()) );

		mDockWidget->setWidget( mMonitor );

		MainWindow->addDockWidget( mDockArea, mDockWidget );

		connect( mNode->context()->qobject(), SIGNAL(frameStart(qint64)), this, SLOT(onContextFrame(qint64)) );
	}

	return( true );
}

bool NumberMonitorNode::deinitialise( void )
{
	if( mDockWidget != 0 )
	{
		mDockWidget->deleteLater();

		mDockWidget = nullptr;

		mMonitor = nullptr;
	}

	return( true );
}

void NumberMonitorNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );
}

void NumberMonitorNode::onShowClicked( void )
{
	if( !mMonitor )
	{
		return;
	}

	if( mDockWidget->isHidden() )
	{
		mDockWidget->show();
	}

	mMonitor->activateWindow();
}

void NumberMonitorNode::onContextFrame( qint64 pTimeStamp )
{
	if( !mLastTime )
	{
		mLastTime = pTimeStamp - 40;
	}

	if( pTimeStamp - mLastTime < 40 )
	{
		return;
	}

	mLastTime += 40;

	QList< QPair<QColor,qreal> >	ValLst;
	QPair<QColor,qreal>				CurVal;

	CurVal.first  = QColor( Qt::black );
	CurVal.second = 0.0;

	for( QSharedPointer<fugio::PinInterface> P : mNode->enumInputPins() )
	{
		if( P->isConnected() )
		{
			fugio::VariantInterface	*V = qobject_cast<fugio::VariantInterface *>( P->connectedPin()->control()->qobject() );

			if( V )
			{
				CurVal.second = V->variant().toDouble();

				ValLst.append( CurVal );

				continue;
			}

//			InterfaceColour		*C = qobject_cast<InterfaceColour *>( P->connectedPin()->control()->qobject() );

//			if( C )
//			{
//				CurVal.first = C->colour();

//				continue;
//			}
		}
	}

	mMonitor->value( ValLst );
}


QList<QUuid> NumberMonitorNode::pinAddTypesInput( void ) const
{
	static QList<QUuid>	PinLst;

	if( PinLst.isEmpty() )
	{
		PinLst << PID_BOOL;
		PinLst << PID_INTEGER;
		PinLst << PID_FLOAT;
		PinLst << PID_STRING;
//		PinLst << PID_COLOUR;
	}

	return( PinLst );
}

bool NumberMonitorNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	if( pPin->direction() != PIN_OUTPUT )
	{
		return( false );
	}

	return( true );
}

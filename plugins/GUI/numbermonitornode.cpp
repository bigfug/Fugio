#include "numbermonitornode.h"

#include <QMainWindow>
#include <QPushButton>

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>
#include <fugio/colour/colour_interface.h>
#include <fugio/core/uuid.h>

#include <fugio/context_signals.h>

#include "numbermonitorform.h"

NumberMonitorNode::NumberMonitorNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mDockWidget( nullptr ), mMonitor( nullptr ), mDockArea( Qt::BottomDockWidgetArea )
{
	FUGID( PIN_INPUT_NUMBER, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );

	mPinInputTrigger = pinInput( "Trigger", PID_FUGIO_NODE_TRIGGER );

	mPinInputValue = pinInput( "Number", PIN_INPUT_NUMBER );

	mPinInputValue->setAutoRename( true );
}

NumberMonitorNode::~NumberMonitorNode( void )
{
	if( mDockWidget )
	{
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

	if( MainWindow )
	{
		if( ( mDockWidget = new QDockWidget( QString( "Monitor: %1" ).arg( mNode->name() ), MainWindow ) ) == 0 )
		{
			return( false );
		}

		mDockWidget->setObjectName( mNode->uuid().toString() );

		if( ( mMonitor = new NumberMonitorForm( mDockWidget ) ) == 0 )
		{
			return( false );
		}

		mDockWidget->setWidget( mMonitor );

		MainWindow->addDockWidget( mDockArea, mDockWidget );

		connect( mNode->qobject(), SIGNAL(nameChanged(QString)), this, SLOT(updateNodeName(QString)) );

		connect( this, SIGNAL(nodeNameUpdated(QString)), mMonitor, SLOT(setNodeName(QString)) );
	}

	return( true );
}

bool NumberMonitorNode::deinitialise( void )
{
	if( mDockWidget )
	{
		delete mDockWidget;

		mDockWidget = nullptr;

		mMonitor = nullptr;
	}

	return( NodeControlBase::deinitialise() );
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

void NumberMonitorNode::updateNodeName( const QString &pName )
{
	emit nodeNameUpdated( pName );
}

void NumberMonitorNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	if( !mPinInputTrigger->isUpdated( pTimeStamp ) )
	{
		return;
	}

	QList< QPair<QColor,qreal> >	ValLst;
	QPair<QColor,qreal>				CurVal;

	CurVal.first  = QColor( Qt::black );
	CurVal.second = 0.0;

	for( QSharedPointer<fugio::PinInterface> P : mNode->enumInputPins() )
	{
		if( P->isConnected() )
		{
			// Colour before Variant, because Colours are Variants too!

			fugio::ColourInterface		*C = qobject_cast<fugio::ColourInterface *>( P->connectedPin()->control()->qobject() );

			if( C )
			{
				CurVal.first = C->colour();

				continue;
			}

			fugio::VariantInterface	*V = qobject_cast<fugio::VariantInterface *>( P->connectedPin()->control()->qobject() );

			if( V )
			{
				CurVal.second = V->variant().toDouble();

				ValLst.append( CurVal );

				continue;
			}
		}
	}

	mMonitor->value( ValLst );
}

QList<QUuid> NumberMonitorNode::pinAddTypesInput( void ) const
{
	static QList<QUuid>	PinLst =
	{
		PID_BOOL,
		PID_INTEGER,
		PID_FLOAT,
		PID_STRING,
		PID_COLOUR
	};

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

bool NumberMonitorNode::pinShouldAutoRename( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_INPUT );
}

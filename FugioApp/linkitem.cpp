#include "linkitem.h"

#include <QPainterPath>
#include <QPen>
#include <QBrush>
#include <QLineF>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QPainterPathStroker>
#include <QStyleOptionGraphicsItem>
#include <QColorDialog>
#include <QMenu>

#include <QDebug>

#include <fugio/utils.h>

//#include "nodeitem.h"
#include "pinitem.h"
#include "contextview.h"
#include "undo/cmdsetcolour.h"

LinkItem::LinkItem( QGraphicsItem *pParent ) :
	QGraphicsPathItem( pParent ), mSrcPin( 0 ), mDstPin( 0 ), mColour( Qt::darkGray )
{
	setFlags( flags() | QGraphicsItem::ItemIsSelectable );

	setBrush( mColour );

	setZValue( -1 );
}

LinkItem::~LinkItem()
{
	if( mSrcPin )
	{
		mSrcPin->linkRem( this );
	}

	if( mDstPin )
	{
		mDstPin->linkRem( this );
	}
}

void LinkItem::setDstPnt( const QPointF &P )
{
	P2 = P;

	updatePath();
}

void LinkItem::setSrcPin( PinItem *pSrcPin )
{
	mSrcPin = pSrcPin;

	if( mSrcPin )
	{
		mColour = mSrcPin->colour();

		if( mDstPin )
		{
			mDstPin->setColour( mColour );
		}
	}
	else
	{
		if( mDstPin )
		{
			mDstPin->setColour( Qt::lightGray );
		}
	}

	updateSrcPin();

	updateColour();

	updateToolTip();
}

void LinkItem::setDstPin( PinItem *pDstPin )
{
	if( mDstPin == pDstPin )
	{
		return;
	}

	if( mDstPin )
	{
		mDstPin->setColour( Qt::lightGray );

		mDstPin->linkRem( this );
	}

	mDstPin = pDstPin;

	updateToolTip();

	if( mDstPin )
	{
		mDstPin->setColour( mColour );
	}

	if( !mSrcPin || !mDstPin )
	{
		return;
	}

	updateDstPin();

	updateColour();
}

void LinkItem::updateSrcPin()
{
	if( mSrcPin != 0 )
	{
		P1 = mSrcPin->mapToScene( 5, 5 );

		updatePath();
	}
}

void LinkItem::updateDstPin()
{
	if( mDstPin != 0 )
	{
		P2 = mDstPin->mapToScene( 5, 5 );

		updatePath();
	}
}

void LinkItem::paint( QPainter *pPainter, const QStyleOptionGraphicsItem *pOption, QWidget *pWidget)
{
	QStyleOptionGraphicsItem myoption = (*pOption);

	myoption.state &= !QStyle::State_Selected;

	setPen( Qt::NoPen );

	if( pOption->state.testFlag( QStyle::State_Selected ) )
	{
		setBrush( Qt::yellow );
	}
	else if( mStatusColour.isValid() )
	{
		setBrush( mStatusColour );
	}
	else
	{
		setBrush( mColour );
	}

	//setBrush( pOption->palette.window() );

	QGraphicsPathItem::paint( pPainter, &myoption, pWidget );
}

void LinkItem::setColour( const QColor &pColor )
{
	if( mColour == pColor )
	{
		return;
	}

	mColour = pColor;

	setBrush( pColor );

	if( mDstPin )
	{
		mDstPin->setColour( mColour );
	}
}

void LinkItem::setStatusColour( const QColor &pColour )
{
	mStatusColour = pColour;

	setBrush( mStatusColour );
}

void LinkItem::clrStatusColour()
{
	mStatusColour = QColor();

	setBrush( mColour );
}

void LinkItem::updatePath()
{
	if( P1.isNull() || P2.isNull() )
	{
		return;
	}

	QPainterPath		Path;

	Path.moveTo( P1 );

	if( mSrcPin && mSrcPin->pin()->direction() == PIN_OUTPUT )
	{
		Path.cubicTo( P1.x() + 50.0, P1.y(), P2.x() - 50.0, P2.y(), P2.x(), P2.y() );
	}
	else
	{
		Path.cubicTo( P1.x() - 50.0, P1.y(), P2.x() + 50.0, P2.y(), P2.x(), P2.y() );
	}

	QPainterPathStroker	Stroke;

	Stroke.setWidth( 5 );

	setPath( Stroke.createStroke( Path ) );
}

void LinkItem::updateColour()
{
	if( !mSrcPin || !mSrcPin->pin() || !mDstPin || !mDstPin->pin() )
	{
		return;
	}

	QColor	C = QColor( mSrcPin->pin()->setting( QString( "colours/%1" ).arg( fugio::utils::uuid2string( mDstPin->pin()->globalId() ) ), brush().color().name() ).toString() );

	if( C != brush().color() )
	{
		setBrush( C );
	}
}

void LinkItem::updateToolTip()
{
	QStringList		ToolTips;

	if( mSrcPin && mSrcPin->pin() && mSrcPin->pin()->node() )
	{
		ToolTips << mSrcPin->pin()->node()->name() << " / " << mSrcPin->pin()->name();
	}

	ToolTips << "->";

	if( mDstPin && mDstPin->pin() && mDstPin->pin()->node() )
	{
		ToolTips << mDstPin->pin()->node()->name() << " / " << mDstPin->pin()->name();
	}

	setToolTip( ToolTips.join( ' ' ) );
}

void LinkItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *pEvent)
{
	QMenu		Menu;

	QAction		*actionSetColour = Menu.addAction( "Set Colour" );

	QAction		*actionSelected = Menu.exec( pEvent->screenPos() );

	if( actionSelected == actionSetColour )
	{
		QColor C = QColorDialog::getColor( brush().color() );

		if( C.isValid() && C != brush().color() )
		{
//			CmdNodeSetColour		*Cmd = new CmdNodeSetColour( this, C );

//			if( Cmd != 0 )
//			{
//				mContextView->widget()->undoStack().push( Cmd );
//			}

			setColour( C );
		}
	}

}

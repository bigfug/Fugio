#include "pinitem.h"

#include <QGraphicsLineItem>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QAction>
#include <QInputDialog>
#include <QToolTip>
#include <QDateTime>
#include <QUrl>
#include <QMimeData>
#include <QFileDialog>
#include <QTextStream>
#include <QStyleOptionGraphicsItem>
#include <QColorDialog>
#include <QSignalMapper>

#include <QDebug>

#include "app.h"

#include <fugio/pin_interface.h>
#include <fugio/global_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/choice_interface.h>

#include "linkitem.h"
#include "contextview.h"

#include "contextwidgetprivate.h"

#include "undo/cmdsetdefaultvalue.h"
#include "undo/cmdsetupdatable.h"
#include "undo/cmdpinimport.h"
#include "undo/cmdpinremove.h"
#include "undo/cmdlinkadd.h"

#include "undo/cmdaddpinlink.h"
#include "undo/cmdsetpinname.h"
#include "undo/cmdpinsplit.h"
#include "undo/cmdpinjoin.h"

#include "undo/cmdgroupaddoutput.h"
#include "undo/cmdgroupaddinput.h"
#include "undo/cmdgroupreminput.h"
#include "undo/cmdgroupremoutput.h"

PinItem::PinItem( ContextView *pContextView, QSharedPointer<fugio::PinInterface> pPin, NodeItem *pParent ) :
	QGraphicsObject( pParent ), mContextView( pContextView ), mPin( pPin ), mLink( 0 ), mPinId( pPin->globalId() )
{
	setAcceptHoverEvents( true );

	setAcceptDrops( true );

	mPinName = mPin->name();

	mPinColour = QColor( Qt::lightGray );
}

PinItem::PinItem( ContextView *pContextView, const QUuid &pId, NodeItem *pParent )
	: QGraphicsObject( pParent ), mContextView( pContextView ), mLink( 0 ), mPinId( pId )
{
	setAcceptHoverEvents( true );

	setAcceptDrops( true );

	mPinColour = QColor( Qt::lightGray );
}

PinItem::~PinItem( void )
{
	QList<LinkItem*>		LinkTemp = mLinks;

	for( LinkItem *L : LinkTemp )
	{
		if( L->srcPin() == this )
		{
			if( !L->dstPin() )
			{
				delete L;
			}
			else
			{
				L->setSrcPin( 0 );
			}
		}
		else if( L->dstPin() == this )
		{
			if( !L->srcPin() )
			{
				delete L;
			}
			else
			{
				L->setDstPin( 0 );
			}
		}
	}

	//qDebug() << "~PinItem" << mPin->uuid();
}

QRectF PinItem::boundingRect() const
{
	return( QRectF( 0, 0, 10, 10 ) );
}

void PinItem::paint( QPainter *pPainter, const QStyleOptionGraphicsItem *pOption, QWidget *pWidget )
{
	Q_UNUSED( pOption )
	Q_UNUSED( pWidget )

	pPainter->setPen( Qt::black );
	pPainter->setBrush( mPinColour );
	pPainter->drawRect( boundingRect() );

	NodeItem			*Node  = qobject_cast<NodeItem *>( parentObject() );
	NodeItem			*Group = mContextView->findNodeItem( Node->groupId() ).data();

	if( mPin->direction() == PIN_INPUT )
	{
		// Are we in a group?  Do we want to add/remove this pin from the group's pins?

		if( Group )
		{
			if( Group->findPinInput( mPin->globalId() ) )
			{
				QRectF				R( -50, 2, 50, 6 );

				QLinearGradient		Gradient( 0, 0, 1, 0 );

				Gradient.setColorAt( 0, Qt::transparent );
				Gradient.setColorAt( 1, mPinColour );

				Gradient.setCoordinateMode( QGradient::ObjectBoundingMode );

				pPainter->setPen( Qt::transparent );
				pPainter->setBrush( Gradient );

				pPainter->drawRect( R );
			}
		}
	}
	else
	{
		if( Group )
		{
			if( Group->findPinOutput( mPin->globalId() ) )
			{
				QRectF				R( 11, 2, 50, 6 );

				QLinearGradient		Gradient( 0, 0, 1, 0 );

				Gradient.setColorAt( 0, mPinColour );
				Gradient.setColorAt( 1, Qt::transparent );

				Gradient.setCoordinateMode( QGradient::ObjectBoundingMode );

				pPainter->setPen( Qt::transparent );
				pPainter->setBrush( Gradient );

				pPainter->drawRect( R );
			}
		}
	}
}

void PinItem::contextMenuEvent( QGraphicsSceneContextMenuEvent *pEvent )
{
	QSignalMapper		SigMap;

	NodeItem			*Node  = qobject_cast<NodeItem *>( parentObject() );
	NodeItem			*Group = mContextView->findNodeItem( Node->groupId() ).data();

	QMenu		Menu( mPinName );

	QAction		*actionTitle = Menu.addAction( mPinName );

	actionTitle->setEnabled( false );

	Menu.addSeparator();

	Menu.addAction( tr( "Rename" ), this, SLOT(menuRename()) );

	Menu.addAction( tr( "Edit Default" ), this, SLOT(menuEditDefault()) );

	if( QAction *A = Menu.addAction( tr( "Remove" ), this, SLOT(menuRemove()) ) )
	{
		A->setEnabled( mPin->removable() );
	}

	if( QAction *A = Menu.addAction( tr( "Hide" ), this, SLOT(menuHide()) ) )
	{
		A->setEnabled( false );
	}

	if( QAction *A = Menu.addAction( tr( "Updatable" ), this, SLOT(menuUpdatable()) ) )
	{
		A->setCheckable( true );
		A->setChecked( mPin->updatable() );
	}

	Menu.addAction( tr( "Import" ), this, SLOT(menuImport()) );
	Menu.addAction( tr( "Export" ), this, SLOT(menuExport()) );

	if( mPin->direction() == PIN_INPUT )
	{
		// Are we in a group?  Do we want to add/remove this pin from the group's pins?

		if( Group )
		{
			if( Group->findPinInput( mPin->globalId() ) )
			{
				if( QAction *A = Menu.addAction( tr( "Remove Group Input" ), this, SLOT(menuRemGroupInput()) ) )
				{
					if( mPin->isConnected() && mContextView->findNodeItem( mPin->connectedNode()->uuid() )->groupId() != Group->id() )
					{
						A->setEnabled( false );
					}
				}
			}
			else
			{
				Menu.addAction( tr( "Add Group Input" ), this, SLOT(menuAddGroupInput()) );
			}
		}

		// Lets see if we have any nodes that are available to create the datatype for this pin

		QList<QUuid>	TypeList;

		for( const QUuid &ID : mPin->inputTypes() )
		{
			TypeList.append( gApp->global().pinJoiners( ID ) );
		}

		if( !TypeList.empty() )
		{
			Menu.addSection( tr( "Joiners" ) );

			for( const QUuid &ID : TypeList )
			{
				if( QAction *A = Menu.addAction( gApp->global().nodeName( ID ), &SigMap, SLOT(map()) ) )
				{
					SigMap.setMapping( A, ID.toString() );

					A->setData( ID );
				}
			}

			connect( &SigMap, SIGNAL(mapped(QString)), this, SLOT(menuJoin(QString)) );
		}
	}
	else
	{
		Menu.addAction( tr( "Set Colour..." ), this, SLOT(menuSetColour()) );

		// Are we in a group?  Do we want to add/remove this pin from the group's pins?

		if( Group )
		{
			if( Group->findPinOutput( mPin->globalId() ) )
			{
				if( QAction *A = Menu.addAction( tr( "Remove Group Output" ), this, SLOT(menuRemGroupOutput()) ) )
				{
					// We don't want to remove the pin if it's connected to pins outside the group

					for( QSharedPointer<fugio::PinInterface> P : mPin->connectedPins() )
					{
						if( mContextView->findNodeItem( P->node()->uuid() )->groupId() != Group->id() )
						{
							A->setEnabled( false );

							break;
						}
					}
				}
			}
			else
			{
				Menu.addAction( tr( "Add Group Output" ), this, SLOT(menuAddGroupOutput()) );
			}
		}

		// Lets see if we have any nodes that are available to create the datatype for this pin

		QList<QUuid>	TypeList = gApp->global().pinSplitters( mPin->controlUuid() );

		if( !TypeList.empty() )
		{
			Menu.addSection( tr( "Splitters" ) );

			for( const QUuid &ID : TypeList )
			{
				if( QAction *A = Menu.addAction( gApp->global().nodeName( ID ), &SigMap, SLOT(map())) )
				{
					SigMap.setMapping( A, ID.toString() );

					A->setData( ID );
				}
			}

			connect( &SigMap, SIGNAL(mapped(QString)), this, SLOT(menuSplit(QString)) );
		}
	}

	Menu.exec( pEvent->screenPos() );
}

void PinItem::hoverEnterEvent( QGraphicsSceneHoverEvent *pEvent )
{
	QStringList		PinDat;

	PinDat << QString( "<b>%1</b>" ).arg( mPin->name() );

	QMap<QString,QString>	PinMap;

	if( mPin->control() )
	{
		QString		PinTyp = gApp->global().pinName( mPin->controlUuid() );

		if( !PinTyp.isEmpty() )
		{
			PinDat << PinTyp;
		}

		QString		CtlVal = mPin->control()->toString();

		if( !CtlVal.isEmpty() )
		{
			PinDat << CtlVal;
		}
	}
	else if( !mPin->value().toString().isEmpty() )
	{
		PinMap.insert( tr( "Value" ), mPin->value().toString() );
	}

	if( !mPin->description().isEmpty() )
	{
		PinMap.insert( tr( "Description" ), mPin->description() );
	}

	for( QString &S : PinDat )
	{
		S.prepend( "<p>" );
		S.append( "</p>" );
	}

	for( auto K : PinMap )
	{
		PinDat << "<h3>" << PinMap.key( K ) << "</h3>";
		PinDat << "<p>" << K << "</p>";
	}

	setToolTip( PinDat.join( "\n" ) );

	QToolTip::showText( pEvent->screenPos(), toolTip(), 0 );

//	QGraphicsObject::hoverEnterEvent( pEvent );
}

void PinItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *pEvent)
{
//	QGraphicsObject::hoverLeaveEvent( pEvent );
	Q_UNUSED( pEvent )

	QToolTip::hideText();
}

void PinItem::dragEnterEvent( QGraphicsSceneDragDropEvent *pEvent )
{
	pEvent->acceptProposedAction();
}

void PinItem::dropEvent( QGraphicsSceneDragDropEvent *pEvent )
{
	if( pEvent->mimeData()->hasText() )
	{
		CmdSetDefaultValue		*Cmd = new CmdSetDefaultValue( mPin, pEvent->mimeData()->text() );

		if( Cmd != 0 )
		{
			mContextView->widget()->undoStack()->push( Cmd );
		}
	}
	else if( pEvent->mimeData()->hasUrls() )
	{
		QList<QUrl>		URL_LST = pEvent->mimeData()->urls();

		if( URL_LST.size() > 0 )
		{
			CmdSetDefaultValue		*Cmd = new CmdSetDefaultValue( mPin, URL_LST.first().toLocalFile() );

			if( Cmd != 0 )
			{
				mContextView->widget()->undoStack()->push( Cmd );
			}
		}
	}

	pEvent->acceptProposedAction();
}

void PinItem::linkMove( const QPointF &pMovement )
{
	Q_UNUSED( pMovement )

	foreach( LinkItem *LI, mLinks )
	{
		LI->updateSrcPin();
		LI->updateDstPin();
	}
}

void PinItem::linkAdd( LinkItem *pLinkItem )
{
	mLinks.append( pLinkItem );
}

void PinItem::linkRem( LinkItem *pLinkItem )
{
	mLinks.removeAll( pLinkItem );
}

LinkItem *PinItem::findLink( PinItem *pDst )
{
	foreach( LinkItem *LI, mLinks )
	{
		if( LI->dstPin() == pDst )
		{
			return( LI );
		}
	}

	return( 0 );
}

bool PinItem::hasLink( PinItem *pDst ) const
{
	foreach( LinkItem *LI, mLinks )
	{
		if( LI->dstPin() == pDst )
		{
			return( true );
		}
	}

	return( false );
}

QUuid PinItem::uuid() const
{
	return( mPin ? mPin->globalId() : mPinId );
}

void PinItem::setColour( const QColor &pColour )
{
	if( mPinColour != pColour )
	{
		mPinColour = pColour;

		for( LinkItem *LI : mLinks )
		{
			LI->setColour( mPinColour );
		}

		emit colourUpdated( mPinColour );
	}
}

void PinItem::mousePressEvent( QGraphicsSceneMouseEvent *pEvent )
{
	if( pEvent->button() == Qt::LeftButton )
	{
		if( mLink != 0 )
		{
			delete mLink;

			mLink = 0;
		}

		if( mPin->direction() == PIN_INPUT && !mPin->connectedPin().isNull() )
		{
			pEvent->accept();

			return;
		}

		if( ( mLink = new LinkItem() ) != 0 )
		{
			scene()->addItem( mLink );

			mLink->setZValue( 1.0 );

			mLink->setSrcPin( this );

			mLink->setDstPnt( mapToScene( 5, 5 ) );
		}
	}

	pEvent->accept();
}

void PinItem::mouseMoveEvent( QGraphicsSceneMouseEvent *pEvent )
{
	if( mLink != 0 )
	{
		PinItem		*PI = findDest( pEvent->scenePos() );

		if( PI )
		{
			if( PI == this )
			{
				mLink->setStatusColour( Qt::red );
			}
			else if( pin()->direction() == PI->pin()->direction() )
			{
				mLink->setStatusColour( Qt::red );
			}
			else if( PI->pin()->direction() == PIN_INPUT && !PI->pin()->connectedPin().isNull() )
			{
				mLink->setStatusColour( Qt::red );
			}
			else
			{
				mLink->clrStatusColour();
			}
		}
		else
		{
			QList<QGraphicsItem *>	Items = scene()->items( pEvent->scenePos() );

			QSharedPointer<fugio::NodeInterface>	IntNod;

			for( int i = 0 ; i < Items.size() && IntNod.isNull() ; i++ )
			{
				NodeItem	*Node = qgraphicsitem_cast<NodeItem *>( Items.at( i ) );

				if( Node == 0 )
				{
					continue;
				}

				IntNod = mContextView->context()->findNode( Node->id() );
			}

			if( IntNod.isNull() || IntNod->control().isNull() || !IntNod->control()->canAcceptPin( pin().data() ) )
			{
				mLink->setStatusColour( Qt::red );
			}
			else
			{
				mLink->clrStatusColour();
			}
		}
	}

	pEvent->accept();
}

void PinItem::mouseReleaseEvent( QGraphicsSceneMouseEvent *pEvent )
{
	if( mLink )
	{
		mLink->setZValue( -1.0 );
		mLink->unsetCursor();
		mLink->clrStatusColour();

		mContextView->scene()->clearSelection();

		PinItem		*PI = findDest( pEvent->scenePos() );

		if( PI )
		{
			if( PI->pin()->direction() == PIN_INPUT && !PI->pin()->connectedPin().isNull() )
			{
				// Don't connect to input pins if they're already connected to something else
			}
			else
			{
				QSharedPointer<fugio::PinInterface>	SrcPin = mLink->srcPin()->pin();
				QSharedPointer<fugio::PinInterface>	DstPin = PI->pin();

				if( SrcPin != 0 && DstPin != 0 && SrcPin->direction() != DstPin->direction() )
				{
					CmdLinkAdd				*Cmd = new CmdLinkAdd( mContextView->widget(), SrcPin, DstPin );

					if( Cmd != 0 )
					{
						mContextView->widget()->undoStack()->push( Cmd );
					}
				}
			}
		}
		else
		{
			QList<QGraphicsItem *>	Items = scene()->items( pEvent->scenePos() );

			QSharedPointer<fugio::NodeInterface>	IntNod;

			for( int i = 0 ; i < Items.size() && IntNod.isNull() ; i++ )
			{
				NodeItem	*Node = qgraphicsitem_cast<NodeItem *>( Items.at( i ) );

				if( Node == 0 )
				{
					continue;
				}

				IntNod = mContextView->context()->findNode( Node->id() );
			}

			if( IntNod && IntNod->control()->canAcceptPin( mLink->srcPin()->pin().data() ) )
			{
				QSharedPointer<fugio::PinInterface>	SrcPin = mLink->srcPin()->pin();
				//QSharedPointer<fugio::PinInterface>	DstPin;

				//if( SrcPin->direction() == PIN_OUTPUT )
				{
					CmdAddPinLink			*Cmd = new CmdAddPinLink( IntNod, SrcPin );

					if( Cmd != 0 )
					{
						mContextView->widget()->undoStack()->push( Cmd );
					}
				}
			}
		}

		delete mLink;

		mLink = nullptr;
	}

	pEvent->accept();
}


PinItem *PinItem::findDest( const QPointF &pPoint )
{
	QList<QGraphicsItem *>	Items = scene()->items( pPoint );

	PinItem			*PinDst = 0;

	for( int i = 0 ; i < Items.size() && PinDst == 0 ; i++ )
	{
		PinDst = qgraphicsitem_cast<PinItem *>( Items.at( i ) );

		if( PinDst == 0 )
		{
			continue;
		}

		if( PinDst == this )
		{
			continue;
		}

		if( pin()->direction() == PinDst->pin()->direction() )
		{
			return( PinDst );
		}
	}

	if( PinDst == 0 )
	{
		mLink->setDstPnt( pPoint );
	}
	else
	{
		mLink->setDstPnt( PinDst->mapToScene( 5, 5 ) );
	}

	return( PinDst );
}

void PinItem::menuRename()
{
	bool	OK;

	QString	New = QInputDialog::getText( nullptr, mPin->name(), "Edit Name", QLineEdit::Normal, mPin->name(), &OK ).trimmed();

	if( OK && !New.isEmpty() && New != mPin->name() )
	{
		CmdSetPinName		*Cmd = new CmdSetPinName( mPin, New );

		if( Cmd != 0 )
		{
			mContextView->widget()->undoStack()->push( Cmd );
		}
	}
}

void PinItem::menuEditDefault()
{
	fugio::ChoiceInterface	*CI = mPin->hasControl() ? qobject_cast<fugio::ChoiceInterface *>( mPin->control()->qobject() ) : nullptr;

	if( CI )
	{
		QStringList		ChoiceLabels = CI->choices();
		QString			CurrentLabel = mPin->value().toString();
		int				CurrentIndex = ChoiceLabels.indexOf( CurrentLabel );

		QString			NewLabel = QInputDialog::getItem( nullptr, mPin->name(), tr( "Choose Default Value" ), ChoiceLabels, CurrentIndex, false );

		if( NewLabel != CurrentLabel )
		{
			CmdSetDefaultValue		*Cmd = new CmdSetDefaultValue( mPin, NewLabel );

			if( Cmd )
			{
				mContextView->widget()->undoStack()->push( Cmd );
			}
		}
	}
	else
	{
		bool	OK;

		QString	New = QInputDialog::getText( nullptr, mPin->name(), "Edit Default Value", QLineEdit::Normal, mPin->value().toString(), &OK );

		if( OK )
		{
			CmdSetDefaultValue		*Cmd = new CmdSetDefaultValue( mPin, New );

			if( Cmd != 0 )
			{
				mContextView->widget()->undoStack()->push( Cmd );
			}
		}
	}
}

void PinItem::menuRemove()
{
	CmdPinRemove			*Cmd = new CmdPinRemove( mPin );

	if( Cmd != 0 )
	{
		mContextView->widget()->undoStack()->push( Cmd );
	}
}

void PinItem::menuHide()
{

}

void PinItem::menuUpdatable()
{
	CmdSetUpdatable				*Cmd = new CmdSetUpdatable( mPin, !mPin->updatable() );

	if( Cmd != 0 )
	{
		mContextView->widget()->undoStack()->push( Cmd );
	}
}

void PinItem::menuImport()
{
	QString		FileName = QFileDialog::getOpenFileName( nullptr, tr( "Import Data" ) );

	if( !FileName.isEmpty() )
	{
		CmdPinImport				*Cmd = new CmdPinImport( mPin, FileName );

		if( Cmd != 0 )
		{
			mContextView->widget()->undoStack()->push( Cmd );
		}
	}
}

void PinItem::menuExport()
{
	QString		FileName = QFileDialog::getSaveFileName( nullptr, tr( "Export Data" ) );

	if( !FileName.isEmpty() )
	{
		QSettings	Settings( FileName, QSettings::IniFormat );

		if( Settings.isWritable() )
		{
			Settings.clear();

			mPin->control()->saveSettings( Settings );
		}
	}
}

void PinItem::menuJoin( const QString &pUuid )
{
	if( CmdPinJoin *Cmd = new CmdPinJoin( mContextView->context(), mPin, QUuid( pUuid ) ) )
	{
		mContextView->widget()->undoStack()->push( Cmd );
	}
}

void PinItem::menuSplit( const QString &pUuid )
{
	if( CmdPinSplit *Cmd = new CmdPinSplit( mContextView->context(), mPin, QUuid( pUuid ) ) )
	{
		mContextView->widget()->undoStack()->push( Cmd );
	}
}

void PinItem::menuSetColour( void )
{
	QColor C = QColorDialog::getColor( mPinColour );

	if( C.isValid() && C != mPinColour )
	{
		setColour( C );
	}
}

void PinItem::menuAddGroupInput()
{
	NodeItem			*Node = qobject_cast<NodeItem *>( parentObject() );

	Q_ASSERT( Node );

	if( CmdGroupAddInput *Cmd = new CmdGroupAddInput( mContextView, Node->groupId(), mPin ) )
	{
		mContextView->widget()->undoStack()->push( Cmd );
	}
}

void PinItem::menuRemGroupInput()
{
	NodeItem			*Node = qobject_cast<NodeItem *>( parentObject() );

	Q_ASSERT( Node );

	if( CmdGroupRemInput *Cmd = new CmdGroupRemInput( mContextView, Node->groupId(), mPin ) )
	{
		mContextView->widget()->undoStack()->push( Cmd );
	}
}

void PinItem::menuAddGroupOutput()
{
	NodeItem			*Node = qobject_cast<NodeItem *>( parentObject() );

	Q_ASSERT( Node );

	if( CmdGroupAddOutput *Cmd = new CmdGroupAddOutput( mContextView, Node->groupId(), mPin ) )
	{
		mContextView->widget()->undoStack()->push( Cmd );
	}
}

void PinItem::menuRemGroupOutput()
{
	NodeItem			*Node = qobject_cast<NodeItem *>( parentObject() );

	Q_ASSERT( Node );

	if( CmdGroupRemOutput *Cmd = new CmdGroupRemOutput( mContextView, Node->groupId(), mPin ) )
	{
		mContextView->widget()->undoStack()->push( Cmd );
	}
}

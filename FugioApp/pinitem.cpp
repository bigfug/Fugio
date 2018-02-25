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
#include <QFontDialog>
#include <QTextStream>
#include <QStyleOptionGraphicsItem>
#include <QColorDialog>
#include <QSignalMapper>
#include <QMessageBox>
#include <QDesktopServices>
#include <QMetaClassInfo>
#include <QColorDialog>
#include <QMetaClassInfo>
#include <QFileDialog>

#include <QDebug>

#include "app.h"

#include <fugio/pin_interface.h>
#include <fugio/global_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/choice_interface.h>
#include <fugio/colour/colour_interface.h>
#include <fugio/file/filename_interface.h>

#include "linkitem.h"
#include "contextview.h"

#include "contextwidgetprivate.h"
#include "pinprivate.h"

#include "undo/cmdsetdefaultvalue.h"
#include "undo/cmdsetupdatable.h"
#include "undo/cmdsetalwaysupdate.h"
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

#include "undo/cmdpinmakeglobal.h"
#include "undo/cmdpinremoveglobal.h"
#include "undo/cmdpinconnectglobal.h"
#include "undo/cmdpindisconnectglobal.h"

#include "pinpopupfloat.h"

PinItem::PinItem( ContextView *pContextView, QSharedPointer<fugio::PinInterface> pPin, NodeItem *pParent ) :
	QGraphicsObject( pParent ), mContextView( pContextView ), mPin( pPin ), mPinId( pPin->globalId() )
{
	setAcceptHoverEvents( true );

	setAcceptDrops( true );

	mPinName = mPin->name();

	mPinColour = QColor( Qt::lightGray );

	longPressTimer().setSingleShot( true );
}

PinItem::PinItem( ContextView *pContextView, const QUuid &pId, NodeItem *pParent )
	: QGraphicsObject( pParent ), mContextView( pContextView ), mPinId( pId )
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

	QColor		PinColour = mPinColour;

	if( mPin->direction() == PIN_INPUT && mPin->isConnected() )
	{
		QSharedPointer<fugio::PinInterface>	ConPin = mPin->connectedPin();

		if( !ConPin || !ConPin->node() )
		{
			return;
		}

		NodeItem		*SrcNod = mContextView->findNodeItem( ConPin->node()->uuid() ).data();
		PinItem			*SrcPin = SrcNod->findPinOutput( ConPin->globalId() );

		if( SrcPin )
		{
			PinColour = SrcPin->colour();
		}
	}

	NodeItem			*Node  = qobject_cast<NodeItem *>( parentObject() );
	NodeItem			*Group = mContextView->findNodeItem( Node->groupId() ).data();

	if( mPin->direction() == PIN_INPUT )
	{
		if( mPin->isConnected() && mPin->connectedPin()->setting( "global", false ).toBool() )
		{
			QRectF				R( -20, 2, 20, 6 );

			pPainter->setPen( Qt::transparent );
			pPainter->setBrush( PinColour );

			pPainter->drawRect( R );

			pPainter->setPen( QPen( Qt::black ) );

			pPainter->drawEllipse( QPointF( -20, 5 ), 5.0f, 5.0f );
		}
		else if( Group && Group->findPinInput( mPin->globalId() ) )
		{
			// Are we in a group?  Do we want to add/remove this pin from the group's pins?

			QRectF				R( -50, 2, 50, 6 );

			QLinearGradient		Gradient( 0, 0, 1, 0 );

			Gradient.setColorAt( 0, Qt::transparent );
			Gradient.setColorAt( 1, PinColour );

			Gradient.setCoordinateMode( QGradient::ObjectBoundingMode );

			pPainter->setPen( Qt::transparent );
			pPainter->setBrush( Gradient );

			pPainter->drawRect( R );
		}
	}
	else
	{
		if( isGlobal() )
		{
			QRectF				R( 10, 2, 20, 6 );

			pPainter->setPen( Qt::transparent );
			pPainter->setBrush( mPinColour );

			pPainter->drawRect( R );

			pPainter->setPen( QPen( Qt::black ) );

			pPainter->drawEllipse( QPointF( 30, 5 ), 5.0f, 5.0f );
		}
		else if( Group && Group->findPinOutput( mPin->globalId() ) )
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

	pPainter->setPen( Qt::black );
	pPainter->setBrush( PinColour );

	pPainter->drawRect( boundingRect() );
}

void PinItem::contextMenuEvent( QGraphicsSceneContextMenuEvent *pEvent )
{
	QSignalMapper		SigMap;

	NodeItem			*Node  = qobject_cast<NodeItem *>( parentObject() );
	NodeItem			*Group = mContextView->findNodeItem( Node->groupId() ).data();

	QString				 HelpUrl = helpUrl();

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
		if( mPin->isConnected() && mPin->connectedPin()->setting( "global", false ).toBool() )
		{
			Menu.addAction( tr( "Disconnect Global" ), this, SLOT(menuDisconnectGlobal()) );
		}
		else
		{
			if( !mPin->isConnected() )
			{
				Menu.addAction( tr( "Connect Global..." ), this, SLOT(menuConnectGlobal()) );
			}

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

			if( !mPin->isConnected() )
			{
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
		}
	}
	else
	{
		if( QAction *A = Menu.addAction( tr( "Always Update" ), this, SLOT(menuAlwaysUpdate()) ) )
		{
			A->setCheckable( true );
			A->setChecked( mPin->alwaysUpdate() );
		}

		Menu.addAction( tr( "Set Colour..." ), this, SLOT(menuSetColour()) );

		if( isGlobal() )
		{
			Menu.addAction( tr( "Remove Global" ), this, SLOT(menuRemoveGlobal()) );
		}
		else
		{
			if( !mPin->isConnected() )
			{
				Menu.addAction( tr( "Make Global" ), this, SLOT(menuMakeGlobal()) );
			}

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

			if( !mPin->isConnected() )
			{
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
		}
	}

	if( !HelpUrl.isEmpty() )
	{
		Menu.addSeparator();

		if( QAction *A = Menu.addAction( tr( "Help..." ) ) )
		{
			connect( A, SIGNAL(triggered()), this, SLOT(menuHelp()) );
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
		QString		CtlDsc = mPin->control()->description();

		if( !CtlDsc.isEmpty() )
		{
			PinDat << CtlDsc;
		}
		else if( !PinTyp.isEmpty() )
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

	QString	PinDsc = mPin->description();

	if( !PinDsc.isEmpty() )
	{
		PinMap.insert( tr( "Description" ), PinDsc );
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

	disconnect( &longPressTimer(), SIGNAL(timeout()), this, SLOT(longPressTimeout()) );

	longPressTimer().stop();

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

	for( LinkItem *LI : mLinks )
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
	for( LinkItem *LI : mLinks )
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
	for( LinkItem *LI : mLinks )
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

void PinItem::longPressTimeout()
{
	PinPopupFloat	*Popup = new PinPopupFloat( mDragStartPoint, mPin );

	scene()->addItem( Popup );

	Popup->grabMouse();

	QToolTip::hideText();

	mLink.reset();
}

void PinItem::mousePressEvent( QGraphicsSceneMouseEvent *pEvent )
{
	if( pEvent->button() == Qt::LeftButton )
	{
		mLink.reset();

		if( mPin->direction() == PIN_OUTPUT || !mPin->isConnected() )
		{
			mDragStartPoint = pEvent->pos();
		}

		if( mPin->direction() == PIN_INPUT && !mPin->isConnected() )
		{
			connect( &longPressTimer(), SIGNAL(timeout()), this, SLOT(longPressTimeout()) );

			mDragStartPoint = pEvent->scenePos();

			longPressTimer().start( 750 );
		}
	}
}

void PinItem::mouseMoveEvent( QGraphicsSceneMouseEvent *pEvent )
{
	if( !mLink )
	{
		if( !mDragStartPoint.isNull() )
		{
			if( ( pEvent->pos() - mDragStartPoint ).manhattanLength() >= QApplication::startDragDistance() )
			{
				disconnect( &longPressTimer(), SIGNAL(timeout()), this, SLOT(longPressTimeout()) );

				longPressTimer().stop();

				mLink = std::unique_ptr<LinkItem>( new LinkItem() );

				if( mLink )
				{
					scene()->addItem( mLink.get() );

					mLink->setZValue( 1.0 );

					mLink->setSrcPin( this );

					mLink->setDstPnt( mapToScene( 5, 5 ) );
				}
			}
		}
	}

	if( mLink )
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

				if( !Node )
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
}

void PinItem::mouseReleaseEvent( QGraphicsSceneMouseEvent *pEvent )
{
	disconnect( &longPressTimer(), SIGNAL(timeout()), this, SLOT(longPressTimeout()) );

	longPressTimer().stop();

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

		mLink.reset();
	}

	mDragStartPoint = QPointF();
}

void PinItem::mouseDoubleClickEvent( QGraphicsSceneMouseEvent *pEvent )
{
	PinPrivate	*PP = qobject_cast<PinPrivate *>( mPin->qobject() );

	if( PP && PP->direction() == PIN_INPUT )
	{
		PP->update( std::numeric_limits<qint64>::max() );
	}
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

QString PinItem::helpUrl()
{
	QString		HelpUrl;

	if( mPin->hasControl() )
	{
		const QMetaObject	*DMO = mPin->control()->qobject()->metaObject();

		if( DMO )
		{
			int			 HelpClassInfoIndex = DMO->indexOfClassInfo( "URL" );

			if( HelpClassInfoIndex != -1 )
			{
				HelpUrl = QString( DMO->classInfo( HelpClassInfoIndex ).value() );
			}
		}
	}

	return( HelpUrl );
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

		return;
	}

	fugio::FilenameInterface	*FNI = mPin->hasControl() ? qobject_cast<fugio::FilenameInterface *>( mPin->control()->qobject() ) : nullptr;

	if( FNI )
	{
		QString	FN = QFileDialog::getOpenFileName( Q_NULLPTR, tr( "Select file..." ), FNI->filename() );

		if( !FN.isEmpty() )
		{
			CmdSetDefaultValue		*Cmd = new CmdSetDefaultValue( mPin, FN );

			if( Cmd )
			{
				mContextView->widget()->undoStack()->push( Cmd );
			}
		}

		return;
	}

	const QVariant		PinVal = mPin->value();

	if( QMetaType::Type( PinVal.type() ) == QMetaType::QColor )
	{
		QColor			CurrentColour = mPin->value().value<QColor>();

		QColor			NewColour = QColorDialog::getColor( CurrentColour, nullptr, tr( "Choose Colour" ), QColorDialog::ShowAlphaChannel );

		if( NewColour.isValid() )
		{
			CmdSetDefaultValue		*Cmd = new CmdSetDefaultValue( mPin, NewColour );

			if( Cmd )
			{
				mContextView->widget()->undoStack()->push( Cmd );
			}
		}

		return;
	}

	if( QMetaType::Type( PinVal.type() ) == QMetaType::QFont )
	{
		bool			OK;
		QFont			CurVal = mPin->value().value<QFont>();
		QFont			NewVal = QFontDialog::getFont( &OK, CurVal, nullptr, tr( "Choose Font" ) );

		if( OK )
		{
			CmdSetDefaultValue		*Cmd = new CmdSetDefaultValue( mPin, NewVal );

			if( Cmd )
			{
				mContextView->widget()->undoStack()->push( Cmd );
			}
		}

		return;
	}

	{
		bool	OK;

		QString	New = QInputDialog::getText( nullptr, mPin->name(), tr( "Edit Default Value" ), QLineEdit::Normal, PinVal.toString(), &OK );

		if( OK )
		{
			CmdSetDefaultValue		*Cmd = new CmdSetDefaultValue( mPin, New );

			if( Cmd )
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

void PinItem::menuAlwaysUpdate()
{
	CmdSetAlwaysUpdate				*Cmd = new CmdSetAlwaysUpdate( mPin, !mPin->alwaysUpdate() );

	if( Cmd )
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

void PinItem::menuMakeGlobal()
{
	if( CmdPinMakeGlobal *Cmd = new CmdPinMakeGlobal( mContextView, mPin ) )
	{
		mContextView->widget()->undoStack()->push( Cmd );
	}
}

void PinItem::menuRemoveGlobal()
{
	if( CmdPinRemoveGlobal *Cmd = new CmdPinRemoveGlobal( mContextView, mPin ) )
	{
		mContextView->widget()->undoStack()->push( Cmd );
	}
}

void PinItem::menuConnectGlobal()
{
	const QList<QUuid>		PidLst = mContextView->globalPins();

	if( PidLst.isEmpty() )
	{
		QMessageBox::information( nullptr, tr( "No Global Pins" ), tr( "There are no global pins defined" ) );

		return;
	}

	QMap<QString,QUuid>		PinLst;

	for( const QUuid Pid : PidLst )
	{
		QSharedPointer<fugio::PinInterface> PI = mContextView->context()->findPin( Pid );

		if( PI )
		{
			PinLst.insert( PI->name(), Pid );
		}
	}

	QString	PinNam = QInputDialog::getItem( nullptr, tr( "Choose Global" ), tr( "Choose Global" ), PinLst.keys(), 0, false );

	if( !PinNam.isEmpty() )
	{
		if( CmdPinConnectGlobal *Cmd = new CmdPinConnectGlobal( mContextView, mPin, PinLst.value( PinNam ) ) )
		{
			mContextView->widget()->undoStack()->push( Cmd );
		}
	}
}

void PinItem::menuDisconnectGlobal()
{
	if( CmdPinDisconnectGlobal *Cmd = new CmdPinDisconnectGlobal( mContextView, mPin, mPin->connectedPin()->globalId() ) )
	{
		mContextView->widget()->undoStack()->push( Cmd );
	}
}

void PinItem::menuHelp()
{
	QString		HelpUrl = helpUrl();

	if( !HelpUrl.isEmpty() )
	{
		QDesktopServices::openUrl( QUrl( HelpUrl.append( "?utm_source=fugio&utm_medium=pin-help" ) ) );
	}
}

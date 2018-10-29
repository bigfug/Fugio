
#include "texteditornode.h"

#include <QMainWindow>
#include <QPushButton>
#include <QSettings>
#include <QCoreApplication>

#include "fugio/global_interface.h"
#include "fugio/context_interface.h"
#include <fugio/context_widget_interface.h>
#include <fugio/text/syntax_highlighter_instance_interface.h>
#include <fugio/node_signals.h>
#include <fugio/pin_signals.h>
#include <fugio/global_interface.h>
#include <fugio/context_signals.h>
#include <fugio/editor_interface.h>

#include "texteditorgui.h"
#include "texteditorform.h"
#include "cmdtexteditorupdate.h"
#include "textplugin.h"
#include <fugio/text/syntax_error_signals.h>

#include <fugio/utils.h>

TextEditorNode::TextEditorNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode ), mDockArea( Qt::BottomDockWidgetArea ), mDockVisible( true ),
	  mHighlighterType( HIGHLIGHT_DEFAULT ), mSyntaxInterface( Q_NULLPTR ), mHighlighter( Q_NULLPTR )
{
	FUGID( PIN_INPUT_BUFFER, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_OUTPUT_STRING, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );

	pinInput( "Trigger", PID_FUGIO_NODE_TRIGGER );

	mPinInputBuffer = pinInput( "Buffer", PIN_INPUT_BUFFER );

	mPinInputBuffer->setValue( true );

	mValOutputString = pinOutput<fugio::VariantInterface *>( "Text", mPinOutputString, PID_STRING, PIN_OUTPUT_STRING );

	connect( mPinOutputString->qobject(), SIGNAL(linked(QSharedPointer<fugio::PinInterface>)), this, SLOT(outputLinked(QSharedPointer<fugio::PinInterface>)) );
	connect( mPinOutputString->qobject(), SIGNAL(unlinked(QSharedPointer<fugio::PinInterface>)), this, SLOT(outputUninked(QSharedPointer<fugio::PinInterface>)) );
}

QWidget *TextEditorNode::gui()
{
	QPushButton		*GUI = new QPushButton( "Edit..." );

	connect( GUI, &QPushButton::clicked, this, &TextEditorNode::editClicked );

	return( GUI );
}

void TextEditorNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	if( mPinInputBuffer->isUpdated( pTimeStamp ) )
	{
		emit bufferedChanged( isBuffered() );
	}

	pinUpdated( mPinOutputString );
}

bool TextEditorNode::initialise( void )
{
	fugio::EditorInterface	*EI = qobject_cast<fugio::EditorInterface *>( node()->context()->global()->findInterface( IID_EDITOR ) );

	if( EI )
	{
		TextEditorGui	*GUI = new TextEditorGui( this, mDockArea, mDockVisible );

		GUI->bufferedUpdated( variant( mPinInputBuffer ).toBool() );

		GUI->moveToThread( QCoreApplication::instance()->thread() );

		connect( this, &TextEditorNode::closeEditor, GUI, &TextEditorGui::deleteLater );

		connect( this, &TextEditorNode::bufferedChanged, GUI, &TextEditorGui::bufferedUpdated );

		connect( this, &TextEditorNode::showEditor, GUI, &TextEditorGui::show );

		connect( this, &TextEditorNode::textChanged, GUI, &TextEditorGui::textUpdated );

		connect( this, &TextEditorNode::syntaxHighlighterChanged, GUI, &TextEditorGui::syntaxHighlighterUpdated );

		connect( this, &TextEditorNode::syntaxErrorsChanged, GUI, &TextEditorGui::syntaxErrorsUpdated );

		connect( GUI, &TextEditorGui::textChanged, this, &TextEditorNode::textUpdated );
		connect( GUI, &TextEditorGui::textModified, this, &TextEditorNode::textModified );

		QMetaObject::invokeMethod( GUI, "initialise", Qt::QueuedConnection );
	}

	return( true );
}

bool TextEditorNode::deinitialise( void )
{
	emit closeEditor();

	return( true );
}

void TextEditorNode::loadSettings( QSettings &pSettings )
{
	mValOutputString->setVariant( QString::fromUtf8( pSettings.value( "value" ).toByteArray() ) );

	mDockArea = (Qt::DockWidgetArea)pSettings.value( "dockarea", int( mDockArea ) ).toInt();

	mDockVisible = pSettings.value( "visible", mDockVisible ).toBool();

	mHighlighterType = HighlighterType( pSettings.value( "highlight-type", int( mHighlighterType ) ).toInt() );

	mHighlighterUuid = fugio::utils::string2uuid( pSettings.value( "highlight-uuid", fugio::utils::uuid2string( mHighlighterUuid ) ).toString() );
}

void TextEditorNode::saveSettings( QSettings &pSettings ) const
{
	pSettings.setValue( "value", mValOutputString->variant() );

	pSettings.setValue( "dockarea", int( mDockArea ) );

	if( !mDockVisible )
	{
		pSettings.setValue( "visible", mDockVisible );
	}

	if( mHighlighterType != HIGHLIGHT_DEFAULT )
	{
		pSettings.setValue( "highlight-type", int( mHighlighterType ) );
	}

	if( mHighlighterType == HIGHLIGHT_CUSTOM )
	{
		pSettings.setValue( "highlight-uuid", fugio::utils::uuid2string( mHighlighterUuid ) );
	}
}

void TextEditorNode::editClicked( void )
{
	emit showEditor();
}

void TextEditorNode::setDockVisible( bool pVisible )
{
	mDockVisible = pVisible;
}

void TextEditorNode::textModified( QString pText )
{
	if( !isBuffered() )
	{
		if( pText != mValOutputString->variant().toString() )
		{
			mValOutputString->setVariant( pText );

			pinUpdated( mPinOutputString );
		}
	}
}

void TextEditorNode::textUpdated( QString pText )
{
	mValOutputString->setVariant( pText );

	pinUpdated( mPinOutputString );
}

void TextEditorNode::outputLinked( QSharedPointer<PinInterface> pPin )
{
	fugio::SyntaxErrorInterface	*SEI = ( pPin->hasControl() ? qobject_cast<fugio::SyntaxErrorInterface *>( pPin->control()->qobject() ) : nullptr );

	if( SEI )
	{
		connect( SEI->syntaxErrorSignals(), SIGNAL(syntaxErrorsUpdated(QList<fugio::SyntaxError>)), this, SLOT(syntaxErrorsUpdated(QList<fugio::SyntaxError>)) );
	}

	checkHighlighter();

	if( mValOutputString->variant().toString().isEmpty() )
	{
		QString		NewTxt = pPin->value().toString();

		mValOutputString->setVariant( NewTxt );

		emit textChanged( NewTxt );
	}
}

void TextEditorNode::outputUninked( QSharedPointer<PinInterface> pPin )
{
	fugio::SyntaxErrorInterface	*SEI = ( pPin->hasControl() ? qobject_cast<fugio::SyntaxErrorInterface *>( pPin->control()->qobject() ) : nullptr );

	if( SEI )
	{
		disconnect( SEI->syntaxErrorSignals(), SIGNAL(syntaxErrorsUpdated(QList<fugio::SyntaxError>)), this, SLOT(syntaxErrorsUpdated(QList<fugio::SyntaxError>)) );
	}

	checkHighlighter();
}

void TextEditorNode::syntaxErrorsUpdated( QList<SyntaxError> pSyntaxErrors )
{
	emit syntaxErrorsChanged( pSyntaxErrors );
}

bool TextEditorNode::isBuffered( void ) const
{
	return( variant( mPinInputBuffer ).toBool() );
}

void TextEditorNode::checkHighlighter( void )
{
	QUuid			 DefaultHighlighterUuid = ( mSyntaxInterface ? mSyntaxInterface->highlighterUuid() : QUuid() );
	bool			 HighlighterUpdate = false;

	if( mHighlighter )
	{
		bool		DeleteHighlighter = false;

		if( mHighlighterType == HIGHLIGHT_NONE )
		{
			DeleteHighlighter = true;
		}
		else if( !mPinOutputString->isConnected() && mHighlighterType == HIGHLIGHT_DEFAULT )
		{
			DeleteHighlighter = true;
		}
		else if( mPinOutputString->isConnected() && mHighlighterType == HIGHLIGHT_DEFAULT && mHighlighterUuid != DefaultHighlighterUuid )
		{
			DeleteHighlighter = true;
		}

		if( DeleteHighlighter )
		{
			delete mHighlighter;

			mHighlighter = nullptr;

			HighlighterUpdate = true;
		}
	}

	if( !mHighlighter )
	{
		if( mHighlighterType == HIGHLIGHT_DEFAULT && !DefaultHighlighterUuid.isNull() )
		{
			mHighlighter = TextPlugin::instance()->syntaxHighlighterInstance( DefaultHighlighterUuid );
		}
		else if( mHighlighterType == HIGHLIGHT_CUSTOM && !mHighlighterUuid.isNull() )
		{
			mHighlighter = TextPlugin::instance()->syntaxHighlighterInstance( mHighlighterUuid );
		}

		if( mHighlighter )
		{
			HighlighterUpdate = true;
		}

		if( mHighlighter && mSyntaxInterface )
		{
			mHighlighter->updateErrors( mSyntaxInterface->syntaxErrors() );
		}
	}

	if( HighlighterUpdate )
	{
		emit syntaxHighlighterChanged( mHighlighter );
	}
}

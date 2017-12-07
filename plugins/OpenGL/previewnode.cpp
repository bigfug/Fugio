#include "previewnode.h"

#include <QMainWindow>
#include <QMatrix4x4>
#include <QSettings>

#include <fugio/global_interface.h>
#include <fugio/context_interface.h>
#include <fugio/opengl/state_interface.h>
#include <fugio/opengl/texture_interface.h>
#include <fugio/render_interface.h>
#include <fugio/performance.h>
#include <fugio/editor_interface.h>

#include "preview.h"
#include "openglplugin.h"

PreviewNode::PreviewNode( QSharedPointer<fugio::NodeInterface> pNode ) :
	NodeControlBase( pNode ), mDockWidget( 0 ), mDockArea( Qt::BottomDockWidgetArea ), mOutput( Q_NULLPTR )
{
	FUGID( PIN_INPUT_STATE, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
	FUGID( PIN_INPUT_RENDER, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
	FUGID( PIN_OUTPUT_SIZE, "5c8f8f4e-58ce-4e47-9e1e-4168d17e1863" );

	mPinInputState  = pinInput( "State", PIN_INPUT_STATE );
	mPinInputRender = pinInput( "Render", PIN_INPUT_RENDER );

	mValOutputSize = pinOutput<fugio::VariantInterface *>( "Size", mPinOutputSize, PID_SIZE, PIN_OUTPUT_SIZE );

	mPinInputState->setDescription( tr( "The OpenGL rendering state to apply" ) );

	mPinInputRender->setDescription( tr( "The input 3D Geometry to render" ) );
}

bool PreviewNode::initialise()
{
	if( !fugio::NodeControlBase::initialise() )
	{
		return( false );
	}

	if( !mOutput && !mDockWidget )
	{
		fugio::EditorInterface	*EI = qobject_cast<fugio::EditorInterface *>( mNode->context()->global()->findInterface( IID_EDITOR ) );

		if( EI )
		{
			if( ( mDockWidget = new QDockWidget( QString( "Preview: %1" ).arg( mNode->name() ), EI->mainWindow() ) ) == 0 )
			{
				return( false );
			}

			mDockWidget->setObjectName( mNode->uuid().toString() );

			mOutput = new Preview( mNode, mDockWidget );

			if( mOutput )
			{
				mDockWidget->setWidget( mOutput );

				connect( mOutput, &Preview::resized, this, &PreviewNode::sizeChanged );
			}

			EI->mainWindow()->addDockWidget( mDockArea, mDockWidget );

			mDockWidget->show();
		}
	}

	if( !OpenGLPlugin::hasContextStatic() )
	{
		return( false );
	}

	OPENGL_PLUGIN_DEBUG;

	return( true );
}

bool PreviewNode::deinitialise()
{
	if( mDockWidget )
	{
		mDockWidget->deleteLater();

		mDockWidget = 0;

		mOutput = 0;
	}

	return( true );
}

void PreviewNode::loadSettings( QSettings &pSettings )
{
	NodeControlBase::loadSettings( pSettings );

	mDockArea = (Qt::DockWidgetArea)pSettings.value( "dockarea", int( mDockArea ) ).toInt();
}

void PreviewNode::saveSettings( QSettings &pSettings ) const
{
	NodeControlBase::saveSettings( pSettings );

	pSettings.setValue( "dockarea", int( mDockArea ) );
}

void PreviewNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	if( mOutput )
	{
		if( mOutput->size() != mValOutputSize->variant().toSize() )
		{
			mValOutputSize->setVariant( mOutput->size() );

			pinUpdated( mPinOutputSize );
		}

		mOutput->update();
	}
}

void PreviewNode::render( qint64 pTimeStamp )
{
	if( !mOutput ) //|| !mOutput->renderInit() )
	{
		return;
	}

	OpenGLStateInterface		*State = input<OpenGLStateInterface *>( mPinInputState );

	if( State )
	{
		State->stateBegin();
	}

	fugio::RenderInterface		*Render = input<fugio::RenderInterface *>( mPinInputRender );

	if( Render )
	{
		Render->render( pTimeStamp );
	}

	if( State )
	{
		State->stateEnd();
	}

	OPENGL_PLUGIN_DEBUG;
}

void PreviewNode::sizeChanged( const QSize &pSize )
{
	Q_UNUSED( pSize )

	mNode->context()->updateNode( mNode );
}

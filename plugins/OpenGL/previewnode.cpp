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

#include "preview.h"
#include "openglplugin.h"

PreviewNode::PreviewNode( QSharedPointer<fugio::NodeInterface> pNode ) :
	NodeControlBase( pNode ), mDockWidget( 0 ), mDockArea( Qt::BottomDockWidgetArea ), mOutput( nullptr )
{
	QSharedPointer<fugio::PinInterface>	PinState    = pinInput( "State" );
	QSharedPointer<fugio::PinInterface>	PinGeometry = pinInput( "Geometry" );

	PinState->setDescription( tr( "The OpenGL rendering state to apply" ) );

	PinGeometry->setDescription( tr( "The input 3D Geometry to render" ) );

	mTexture = pinOutput<OpenGLTextureInterface *>( "Texture", mPinTexture, PID_OPENGL_TEXTURE );
}

bool PreviewNode::initialise()
{
	if( !fugio::NodeControlBase::initialise() )
	{
		return( false );
	}

	if( !mOutput && !mDockWidget )
	{
		QMainWindow		*MainWindow = mNode->context()->global()->mainWindow();

		if( MainWindow != 0 )
		{
			if( ( mDockWidget = new QDockWidget( QString( "Preview: %1" ).arg( mNode->name() ), MainWindow ) ) == 0 )
			{
				return( false );
			}

			mDockWidget->setObjectName( mNode->uuid().toString() );

			mDockWidget->setWidget( QWidget::createWindowContainer( mOutput = new Preview( mNode ) ) );

			MainWindow->addDockWidget( mDockArea, mDockWidget );

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

	//mPinStringInterface->setVariant( QString::fromUtf8( pSettings.value( "value" ).toByteArray() ) );

	mDockArea = (Qt::DockWidgetArea)pSettings.value( "dockarea", int( mDockArea ) ).toInt();
}

void PreviewNode::saveSettings( QSettings &pSettings ) const
{
	NodeControlBase::saveSettings( pSettings );

	//pSettings.setValue( "value", mPinStringInterface->variant() );

	pSettings.setValue( "dockarea", int( mDockArea ) );
}

void PreviewNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );

	if( mOutput )
	{
//		mOutput->renderLater();
	}
}

QList<QUuid> PreviewNode::pinAddTypesInput() const
{
	static QList<QUuid>		PinLst;

	if( PinLst.isEmpty() )
	{
		PinLst << PID_RENDER;
		PinLst << PID_OPENGL_STATE;
	}

	return( PinLst );
}

bool PreviewNode::canAcceptPin( fugio::PinInterface *pPin ) const
{
	return( pPin->direction() == PIN_OUTPUT );
}

void PreviewNode::render( qint64 pTimeStamp )
{
	if( !mOutput ) //|| !mOutput->renderInit() )
	{
		return;
	}

	OPENGL_PLUGIN_DEBUG;

//	mOutput->renderStart();

	OPENGL_PLUGIN_DEBUG;

	OpenGLStateInterface		*CurrentState = 0;

	for( QSharedPointer<fugio::PinInterface> P : mNode->enumInputPins() )
	{
		if( !P->isConnected() )
		{
			continue;
		}

		if( P->connectedPin().isNull() )
		{
			continue;
		}

		if( P->connectedPin()->control().isNull() )
		{
			continue;
		}

		QObject					*O = P->connectedPin()->control()->qobject();

		if( O == 0 )
		{
			continue;
		}

		if( true )
		{
			fugio::RenderInterface		*Geometry = qobject_cast<fugio::RenderInterface *>( O );

			if( Geometry != 0 )
			{
				Geometry->render( pTimeStamp );

				continue;
			}
		}

		if( true )
		{
			OpenGLStateInterface		*NextState = qobject_cast<OpenGLStateInterface *>( O );

			if( NextState != 0 )
			{
				if( CurrentState != 0 )
				{
					CurrentState->stateEnd();
				}

				CurrentState = NextState;

				CurrentState->stateBegin();

				continue;
			}
		}
	}

	if( CurrentState != 0 )
	{
		CurrentState->stateEnd();
	}

	if( mPinTexture->isConnectedToActiveNode() )
	{
		QVector3D		TexSze = mTexture->size();

		if( !mTexture->dstTexId() || mOutput->size() != QSize( TexSze.x(), TexSze.y() ) )
		{
			mTexture->setFilter( GL_LINEAR, GL_LINEAR );
			mTexture->setFormat( GL_RGBA );
			mTexture->setGenMipMaps( false );
			mTexture->setInternalFormat( GL_RGBA );
			mTexture->setSize( mOutput->size().width(), mOutput->size().height() );
			mTexture->setTarget( GL_TEXTURE_2D );
			mTexture->setType( GL_UNSIGNED_BYTE );
//			mTexture->setWrap( GL_CLAMP, GL_CLAMP, GL_CLAMP );

			mTexture->update( 0, 0 );
		}

		if( mTexture->dstTexId() )
		{
			mTexture->dstBind();

			glCopyTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, 0, 0, mOutput->size().width(), mOutput->size().height() );

			mTexture->release();

			pinUpdated( mPinTexture );
		}
	}

//	mOutput->renderEnd();

	OPENGL_PLUGIN_DEBUG;
}

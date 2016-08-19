#include "outputglnode.h"

#include <QtGlobal>

#include <QMatrix4x4>
#include <QMouseEvent>
#include <QApplication>
#include <QSettings>
#include <QDateTime>
#include <QAction>

#include <QOpenGLDebugLogger>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFramebufferObjectFormat>

#include "fugio/interface_texture.h"
#include "fugio/interface_app.h"
#include "fugio/interface_variant.h"
#include "fugio/interface_context.h"
#include "fugio/fugio-base.h"
#include <fugio/fugio-dev.h>
#include <fugio/fugio-image.h>

#include <fugio/interface_geometry.h>
#include <fugio/interface_opengl_state.h>
#include <fugio/performance.h>

#include "openglplugin.h"

static QUuid	PID_INPUT_TEXTURE( "{13f2ee6f-8b61-4cfc-a028-f9e3b895b431}" );
static QUuid	PID_OUTPUT_WIDTH( "{43b50bc3-3ef2-49e8-97d2-3daafae1c3f2}" );
static QUuid	PID_OUTPUT_HEIGHT( "{364fa323-96a6-4ee0-8818-15d4f6a95326}" );

//QGLWidget( QGLFormat( QGL::AlphaChannel ), 0, pNode->context()->global()->shareWidget() ),

OutputGLNode::OutputGLNode( QSharedPointer<InterfaceNode> pNode )
	: NodeControlBase( pNode ), mTimeStamp( 0 ), mGrabScreenId( 0 ), mFBO( 0 ), mFBOAA( 0 )
{
	if( DeviceOpenGLOutput::devices().isEmpty() || ( mOutput = DeviceOpenGLOutput::devices().first() ) == 0 )
	{
		mOutput = DeviceOpenGLOutput::newDevice();
	}

	if( mOutput != 0 )
	{
		mOutput->setCurrentNode( mNode );
	}

	mPinState = pinInput( "State" );

	mTexturePin = pinInput( "InputTexture", PID_INPUT_TEXTURE );

	mPinOutputWidth = pinInput( "OutputWidth", PID_OUTPUT_WIDTH );

	mPinOutputHeight = pinInput( "OutputHeight", PID_OUTPUT_HEIGHT );

	mPinMouseShowCursor = pinInput( "Mouse Show Cursor" );

	mPinInputWindowWidth  = pinInput( "Window Width" );
	mPinInputWindowHeight = pinInput( "Window Height" );

	mPinWidthValue = pinOutput<InterfaceVariant *>( "Width", mPinWidth, PID_INTEGER );

	mPinHeightValue = pinOutput<InterfaceVariant *>( "Height", mPinHeight, PID_INTEGER );

	mPinOutputImageValue = pinOutput<InterfaceImage *>( "Image", mPinOutputImage, PID_IMAGE );

	mMouseButtonLeft = pinOutput<InterfaceVariant *>( "Mouse Left", mPinMouseButtonLeft, PID_BOOL );

	mMousePosition = pinOutput<InterfaceVariant *>( "Mouse Position", mPinMousePosition, PID_POINT );

	mTexture = pinOutput<InterfaceTexture *>( "Texture", mPinTexture, PID_TEXTURE );

	//setWindowFlags( windowFlags() | Qt::WindowStaysOnTopHint );

	if( ( mActionSaveImage = new QAction( this ) ) != 0 )
	{
		mActionSaveImage->setShortcuts( QKeySequence::Save );

		connect( mActionSaveImage, SIGNAL(triggered()), this, SLOT(screengrabTimer()) );
	}

	connect( &mTimer, SIGNAL(timeout()), this, SLOT(screengrabTimer()) );

#if !defined( QT_DEBUG )
	QTimer::singleShot( 5000, mOutput, SLOT(setFullScreen()) );
#endif
}

OutputGLNode::~OutputGLNode( void )
{
	if( mTimer.isActive() )
	{
		mTimer.stop();
	}

	deinitialise();
}

void OutputGLNode::inputsUpdated( qint64 pTimeStamp )
{
	fugio::Performance	Perf( mNode, "inputsUpdated" );

	if( !QOpenGLContext::currentContext() )
	{
		return;
	}

	if( showCursor() )
	{

	}

	if( !mPinInputWindowWidth->defaultValue().isEmpty() )
	{
		bool			 O;
		int				 W;

		W = mPinInputWindowWidth->defaultValue().toInt( &O );

		if( O )
		{
			if( W > 0 )
			{
				mOutput->setWidth( W );
			}
		}
	}

	if( !mPinInputWindowHeight->defaultValue().isEmpty() )
	{
		bool			 O;
		int				 H;

		H = mPinInputWindowHeight->defaultValue().toInt( &O );

		if( O )
		{
			if( H > 0 )
			{
				mOutput->setHeight( H );
			}
		}
	}

	mTimeStamp = pTimeStamp;

	if( !mOutput->renderInit() )
	{
		return;
	}

	const bool CaptureWindow = ( !mGrabFileNames.isEmpty() || mPinOutputImage->isConnectedToActiveNode() );

	if( CaptureWindow && ( mFBO == 0 || mFBO->width() != mOutput->width() || mFBO->height() != mOutput->height() ) )
	{
		if( QOpenGLFramebufferObject::hasOpenGLFramebufferObjects() )
		{
			if( mFBOAA != 0 )
			{
				delete mFBOAA;

				mFBOAA = 0;
			}

			if( mFBO != 0 )
			{
				delete mFBO;

				mFBO = 0;
			}

			if( QOpenGLContext::currentContext()->format().samples() > 0 )
			{
				QOpenGLFramebufferObjectFormat	Format;

				Format.setAttachment( QOpenGLFramebufferObject::Depth );
				Format.setSamples( QOpenGLContext::currentContext()->format().samples() );

				if( ( mFBOAA = new QOpenGLFramebufferObject( mOutput->size(), Format ) ) == 0 )
				{
					return;
				}
			}

			QOpenGLFramebufferObjectFormat	Format;

			Format.setAttachment( QOpenGLFramebufferObject::Depth );

			if( ( mFBO = new QOpenGLFramebufferObject( mOutput->size(), Format ) ) == 0 )
			{
				return;
			}
		}
	}

	if( CaptureWindow )
	{
		if( mFBOAA != 0 )
		{
			mFBOAA->bind();
		}
		else if( mFBO != 0 )
		{
			mFBO->bind();
		}
	}

	mOutput->renderStart();

	int		PinIdx = 0;

	InterfaceOpenGLState		*CurrentState = 0;

	foreach( QSharedPointer<InterfacePin> P, mNode->enumInputPins() )
	{
		Q_ASSERT( PinIdx == P->order() );

		PinIdx++;

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

		QObject					*O = P->connectedPin()->control()->object();

		if( O == 0 )
		{
			continue;
		}

		if( true )
		{
			InterfaceTexture		*Texture = qobject_cast<InterfaceTexture *>( O );

			if( Texture != 0 )
			{
				if( Texture->textureId() != 0 )
				{
					drawTexture( Texture );
				}

				continue;
			}
		}

		if( true )
		{
			InterfaceGeometry		*Geometry = qobject_cast<InterfaceGeometry *>( O );

			if( Geometry != 0 )
			{
				Geometry->drawGeometry();

				continue;
			}
		}

		if( true )
		{
			InterfaceOpenGLState		*NextState = qobject_cast<InterfaceOpenGLState *>( O );

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

	if( CaptureWindow && mFBO != 0 )
	{
		if( mFBOAA != 0 )
		{
			mFBOAA->release();

			QOpenGLFramebufferObject::blitFramebuffer( mFBO, mFBOAA );
		}
		else
		{
			mFBO->release();
		}

		glViewport( 0, 0, mOutput->width(), mOutput->height() );
#if 0
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();

		//Initialize Modelview Matrix
		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();

		glColor4f( 1.0, 1.0, 1.0, 1.0 );

		glEnable( GL_TEXTURE_2D );

		glBindTexture( GL_TEXTURE_2D, mFBO->texture() );

		glBegin( GL_QUADS );
			glTexCoord2i( 0, 0 );	glVertex2i( -1, -1 );
			glTexCoord2i( 1, 0 );	glVertex2i(  1, -1 );
			glTexCoord2i( 1, 1 );	glVertex2i(  1,  1 );
			glTexCoord2i( 0, 1 );	glVertex2i( -1,  1 );
		glEnd();

		glBindTexture( GL_TEXTURE_2D, 0 );

		glDisable( GL_TEXTURE_2D );
#endif
	}

#if 0
	glViewport( 0, 0, mOutput->width(),  mOutput->height() );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	//Initialize Modelview Matrix
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glColor4f( 0.0, 1.0, 0.0, 1.0 );

	glBegin( GL_LINES );
		glVertex2i( -1, -1 );
		glVertex2i(  1,  1 );
		glVertex2i( -1,  1 );
		glVertex2i(  1, -1 );
	glEnd();
#endif

	if( mPinTexture->isConnectedToActiveNode() )
	{
		QVector<quint32>		TexSze = mTexture->size();

		if( !mTexture->textureId() || mOutput->size() != QSize( TexSze[ 0 ], TexSze[ 1 ] ) )
		{
			mTexture->setFilter( GL_LINEAR, GL_LINEAR );
			mTexture->setFormat( GL_RGBA );
			mTexture->setGenMipMaps( false );
			mTexture->setInternalFormat( GL_RGBA );
			mTexture->setSize( mOutput->size().width(), mOutput->size().height() );
			mTexture->setTarget( GL_TEXTURE_2D );
			mTexture->setType( GL_UNSIGNED_BYTE );
			mTexture->setWrap( GL_CLAMP, GL_CLAMP, GL_CLAMP );

			mTexture->update( 0, 0 );
		}

		if( mTexture->textureId() )
		{
			mTexture->bind();

			glCopyTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, 0, 0, mOutput->size().width(), mOutput->size().height() );

			mTexture->release();

			pinUpdated( mPinTexture );
		}
	}

	mOutput->renderEnd();

	if( ( !mGrabFileNames.isEmpty() || mPinOutputImage->isConnectedToActiveNode() ) && mFBO != 0 )
	{
		mBufferImage = mFBO->toImage();

		mPinOutputImageValue->setSize( mBufferImage.width(), mBufferImage.height() );
		mPinOutputImageValue->setLineSize( 0, mBufferImage.bytesPerLine() );

		switch( mBufferImage.format() )
		{
			case QImage::Format_RGB32:
				mPinOutputImageValue->setFormat( InterfaceImage::FORMAT_BGRA8 );
				break;

			case QImage::Format_ARGB32_Premultiplied:
			case QImage::Format_ARGB32:
				mPinOutputImageValue->setFormat( InterfaceImage::FORMAT_RGBA8 );
				break;

			default:
				break;
		}

		mPinOutputImageValue->setBuffer( 0, mBufferImage.constBits() );

		pinUpdated( mPinOutputImage );

		if( !mGrabFileNames.isEmpty() )
		{
			if( mBufferImage.format() == QImage::Format_ARGB32_Premultiplied )
			{
				QImage		TmpImg( mBufferImage.constBits(), mBufferImage.width(), mBufferImage.height(), QImage::Format_ARGB32 );

				foreach( const QString &FN, mGrabFileNames )
				{
					TmpImg.save( FN, 0, 100 );
				}
			}
			else
			{
				foreach( const QString &FN, mGrabFileNames )
				{
					mBufferImage.save( FN, 0, 100 );
				}
			}

			mGrabFileNames.clear();
		}
	}

	OPENGL_PLUGIN_DEBUG
}

bool OutputGLNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	if( !QOpenGLContext::currentContext() )
	{
		return( false );
	}

	initializeOpenGLFunctions();

	return( true );
}

bool OutputGLNode::deinitialise()
{
	if( mOutput )
	{
		mOutput->setCurrentNode( QSharedPointer<InterfaceNode>() );

		mOutput = nullptr;
	}

	return( NodeControlBase::deinitialise() );
}

void OutputGLNode::drawTexture( InterfaceTexture *Texture )
{
	QRectF		VP = QRectF( 0, 0, mOutput->width(), mOutput->height() );

	QMatrix4x4 pmvMatrix;
	pmvMatrix.ortho( VP );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	glLoadMatrixf( pmvMatrix.constData() );

	//Initialize Modelview Matrix
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glColor4f( 1.0, 1.0, 1.0, 1.0 );

	if( mPinOutputWidth->isConnected() )
	{
		QSharedPointer<InterfacePinControl>		C = mPinOutputWidth->connectedPin()->control();

		if( C != 0 )
		{
			InterfaceVariant	*V = qobject_cast<InterfaceVariant *>( C->object() );

			if( V != 0 )
			{
				bool			 O;
				double			 W;

				W = V->variant().toDouble( &O );

				if( O )
				{
					W *= mOutput->geometry().width();

					VP.setLeft( ( mOutput->geometry().width() / 2 ) - ( W / 2 ) );
					VP.setRight( ( mOutput->geometry().width() / 2 ) + ( W / 2 ) );
				}
			}
		}
	}
	else if( !mPinOutputWidth->defaultValue().isEmpty() )
	{
		bool			 O;
		double			 W;

		W = mPinOutputWidth->defaultValue().toDouble( &O );

		if( O )
		{
			W *= mOutput->geometry().width();

			VP.setLeft( ( mOutput->geometry().width() / 2 ) - ( W / 2 ) );
			VP.setRight( ( mOutput->geometry().width() / 2 ) + ( W / 2 ) );
		}
	}

	if( mPinOutputHeight->isConnected() )
	{
		QSharedPointer<InterfacePinControl>		C = mPinOutputHeight->connectedPin()->control();

		if( C != 0 )
		{
			InterfaceVariant	*V = qobject_cast<InterfaceVariant *>( C->object() );

			if( V != 0 )
			{
				bool			 O;
				double			 H;

				H = V->variant().toDouble( &O );

				if( O )
				{
					H *= mOutput->geometry().height();

					VP.setTop( ( mOutput->geometry().height() / 2 ) - ( H / 2 ) );
					VP.setBottom( ( mOutput->geometry().height() / 2 ) + ( H / 2 ) );
				}
			}
		}
	}
	else if( !mPinOutputHeight->defaultValue().isEmpty() )
	{
		bool			 O;
		double			 H;

		H = mPinOutputHeight->defaultValue().toDouble( &O );

		if( O )
		{
			H *= mOutput->geometry().height();

			VP.setTop( ( mOutput->geometry().height() / 2 ) - ( H / 2 ) );
			VP.setBottom( ( mOutput->geometry().height() / 2 ) + ( H / 2 ) );
		}
	}

	if( openGLFeatures().testFlag( QOpenGLFunctions::Multitexture ) )
	{
		glActiveTexture( GL_TEXTURE0 );
	}

	glEnable( Texture->target() );

	Texture->bind();

	switch( Texture->target() )
	{
		case GL_TEXTURE_1D:
			{
				QVector<quint32>		S = Texture->size();

				GLint	s = S.at( 0 );

				glBegin( GL_QUADS );
					glTexCoord1f( 0 );		glVertex2i( VP.left(), VP.top() );
					glTexCoord1f( s );		glVertex2i( VP.right(), VP.top() );
					glTexCoord1f( s );		glVertex2i( VP.right(), VP.bottom() );
					glTexCoord1f( 0 );		glVertex2i( VP.left(), VP.bottom() );
				glEnd();
			}
			break;

		case GL_TEXTURE_2D:
			{
				glBegin( GL_QUADS );
					glTexCoord2f( 0, 0 );	glVertex2i( VP.left(), VP.top() );
					glTexCoord2f( 1, 0 );	glVertex2i( VP.right(), VP.top() );
					glTexCoord2f( 1, 1 );	glVertex2i( VP.right(), VP.bottom() );
					glTexCoord2f( 0, 1 );	glVertex2i( VP.left(), VP.bottom() );
				glEnd();
			}
			break;

		case GL_TEXTURE_RECTANGLE:
			{
				QVector<quint32>		S = Texture->size();

				GLint	s = S.at( 0 );
				GLint	t = S.at( 1 );

				glBegin( GL_QUADS );
					glTexCoord2i( 0, 0 );	glVertex2i( VP.left(), VP.top() );
					glTexCoord2i( s, 0 );	glVertex2i( VP.right(), VP.top() );
					glTexCoord2i( s, t );	glVertex2i( VP.right(), VP.bottom() );
					glTexCoord2i( 0, t );	glVertex2i( VP.left(), VP.bottom() );
				glEnd();
			}
			break;
	}

	Texture->release();

	glDisable( Texture->target() );

#if 0
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glColor4f( 1.0, 0.0, 1.0, 1.0 );

	glBegin( GL_LINES );
		glVertex2i(  0, -1 );
		glVertex2i(  0,  1 );
		glVertex2i( -1,  0 );
		glVertex2i(  1,  0 );
	glEnd();
#endif
}

void OutputGLNode::screengrab( const QString &pFileName )
{
	if( pFileName.isEmpty() )
	{
		mGrabFileNames.append( QString( "../screenshots/%1.%2.png" ).arg( QDate::currentDate().toString( "yyyy-MM-dd" ) ).arg( QTime::currentTime().toString( "hh-mm-ss" ) ) );
	}
	else
	{
		mGrabFileNames.append( pFileName );
	}
}

void OutputGLNode::resize( int pWidth, int pHeight )
{
	if( mPinWidthValue->variant().toInt() != pWidth )
	{
		mPinWidthValue->setVariant( pWidth );

		mNode->context()->pinUpdated( mPinWidth );
	}

	if( mPinHeightValue->variant().toInt() != pHeight )
	{
		mPinHeightValue->setVariant( pHeight );

		mNode->context()->pinUpdated( mPinHeight );
	}
}

void OutputGLNode::buttonLeftPressed( bool pPressed )
{
	mMouseButtonLeft->setVariant( pPressed );

	mNode->context()->pinUpdated( mPinMouseButtonLeft );
}

void OutputGLNode::cursorPosition( const QPointF &pCurPos )
{
	mMousePosition->setVariant( pCurPos );

	mNode->context()->pinUpdated( mPinMousePosition );
}

bool OutputGLNode::showCursor() const
{
	InterfaceVariant		*V;

	if( mPinMouseShowCursor->isConnected() && ( V = qobject_cast<InterfaceVariant *>( mPinMouseShowCursor->connectedPin()->control()->object() ) ) != 0 )
	{
		return( V->variant().toBool() );
	}

	return( mPinMouseShowCursor->defaultValue().toInt() != 0 );
}

void OutputGLNode::screengrabTimer( void )
{
	screengrab();
}


QList<QUuid> OutputGLNode::pinAddTypesInput() const
{
	QList<QUuid>	PinLst;

	PinLst << PID_GEOMETRY;

	return( PinLst );
}

bool OutputGLNode::canAcceptPin(InterfacePin *pPin) const
{
	if( pPin->direction() == PIN_OUTPUT )
	{
		return( true );
	}

	return( false );
}

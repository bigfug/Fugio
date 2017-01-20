#include "imagetotexturenode.h"

#include <QApplication>
#include <QSettings>
#include <QDebug>
#include <QComboBox>
#include <QDateTime>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMainWindow>
#include <QtConcurrent>
#include <QMessageBox>

#include <fugio/global_interface.h>
#include "fugio/context_interface.h"

#include <fugio/core/variant_interface.h>
#include <fugio/performance.h>

#include "openglplugin.h"

#include "texturenodeform.h"

#if defined( Q_OS_RASPBERRY_PI )
#include "deviceopengloutputrpi.h"
#else
#include "deviceopengloutput.h"
#endif

ImageToTextureNode::ImageToTextureNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	static const QUuid	PIN_OUTPUT_SIZE = QUuid( "{b98dfce9-2107-4ff2-bc7d-91d67a890a5d}" );

	if( ( mPinInputImage = pinInput( "Image" ) ) == 0 )
	{
		return;
	}

	mTexture = pinOutput<OpenGLTextureInterface *>( "Texture", mPinOutput, PID_OPENGL_TEXTURE );

	mTexture->setTarget( GL_TEXTURE_2D );

	mOutputTextureSize = pinOutput<VariantInterface *>( "Texture Size", mPinOutputTextureSize, PID_SIZE_3D, PIN_OUTPUT_SIZE );

	mPinInputImage->setDescription( tr( "An input image that is copied to the graphics card as a Texture" ) );

	mPinOutput->setDescription( tr( "The allocated OpenGL texture" ) );
}

ImageToTextureNode::~ImageToTextureNode( void )
{
}

QWidget *ImageToTextureNode::gui()
{
	QPushButton		*GUI = new QPushButton( "Edit..." );

	connect( GUI, SIGNAL(clicked()), this, SLOT(onEditClicked()) );

	return( GUI );
}

bool ImageToTextureNode::initialise()
{
	if( !NodeControlBase::initialise() )
	{
		return( false );
	}

	if( !OpenGLPlugin::hasContextStatic() )
	{
		return( false );
	}

	return( true );
}

void ImageToTextureNode::loadSettings( QSettings &pSettings )
{
	QString		CurVal;
	int			CurInt;

	CurVal = OpenGLPlugin::mMapTargets.key( mTexture->target() );
	CurVal = pSettings.value( "Target", CurVal ).toString();
	CurInt = OpenGLPlugin::mMapTargets.value( CurVal, mTexture->target() );

	mTexture->setTarget( CurInt );

	CurVal = OpenGLPlugin::mMapFormat.key( mTexture->format() );
	CurVal = pSettings.value( "Format", CurVal ).toString();
	CurInt = OpenGLPlugin::mMapFormat.value( CurVal, mTexture->format() );

	mTexture->setFormat( CurInt );

	CurVal = OpenGLPlugin::mMapInternal.key( mTexture->internalFormat() );
	CurVal = pSettings.value( "Internal", CurVal ).toString();
	CurInt = OpenGLPlugin::mMapInternal.value( CurVal, mTexture->internalFormat() );

	mTexture->setInternalFormat( CurInt );

	CurVal = OpenGLPlugin::mMapType.key( mTexture->type() );
	CurVal = pSettings.value( "Type", CurVal ).toString();
	CurInt = OpenGLPlugin::mMapType.value( CurVal, mTexture->type() );

	mTexture->setType( CurInt );

	int		CurMin = mTexture->filterMin();
	int		CurMag = mTexture->filterMag();

	CurVal = OpenGLPlugin::mMapFilterMin.key( CurMin );
	CurVal = pSettings.value( "FilterMin", CurVal ).toString();
	CurMin = OpenGLPlugin::mMapFilterMin.value( CurVal, CurMin );

	CurVal = OpenGLPlugin::mMapFilterMag.key( CurMag );
	CurVal = pSettings.value( "FilterMag", CurVal ).toString();
	CurMag = OpenGLPlugin::mMapFilterMin.value( CurVal, CurMag );

	mTexture->setFilter( CurMin, CurMag );

	int		CurWPS = mTexture->wrapS();
	int		CurWPT = mTexture->wrapT();
	int		CurWPR = mTexture->wrapR();

	CurVal = OpenGLPlugin::mMapWrap.key( CurWPS );
	CurVal = pSettings.value( "WrapS", CurVal ).toString();
	CurWPS = OpenGLPlugin::mMapWrap.value( CurVal, CurWPS );

	CurVal = OpenGLPlugin::mMapWrap.key( CurWPT );
	CurVal = pSettings.value( "WrapT", CurVal ).toString();
	CurWPT = OpenGLPlugin::mMapWrap.value( CurVal, CurWPT );

	CurVal = OpenGLPlugin::mMapWrap.key( CurWPR );
	CurVal = pSettings.value( "WrapR", CurVal ).toString();
	CurWPR = OpenGLPlugin::mMapWrap.value( CurVal, CurWPR );

	mTexture->setWrap( CurWPS, CurWPT, CurWPR );

	mTexture->setGenMipMaps( pSettings.value( "MipMaps", mTexture->genMipMaps() ).toBool() );
}

void ImageToTextureNode::saveSettings( QSettings &pSettings ) const
{
	pSettings.setValue( "Target", OpenGLPlugin::mMapTargets.key( mTexture->target() ) );
	pSettings.setValue( "Format", OpenGLPlugin::mMapFormat.key( mTexture->format() ) );
	pSettings.setValue( "Internal", OpenGLPlugin::mMapInternal.key( mTexture->internalFormat() ) );
	pSettings.setValue( "Type", OpenGLPlugin::mMapType.key( mTexture->type() ) );
	pSettings.setValue( "FilterMin", OpenGLPlugin::mMapFilterMin.key( mTexture->filterMin() ) );
	pSettings.setValue( "FilterMag", OpenGLPlugin::mMapFilterMag.key( mTexture->filterMag() ) );
	pSettings.setValue( "WrapS", OpenGLPlugin::mMapWrap.key( mTexture->wrapS() ) );
	pSettings.setValue( "WrapT", OpenGLPlugin::mMapWrap.key( mTexture->wrapT() ) );
	pSettings.setValue( "WrapR", OpenGLPlugin::mMapWrap.key( mTexture->wrapR() ) );
	pSettings.setValue( "MipMaps", mTexture->genMipMaps() );
}

void ImageToTextureNode::onEditClicked()
{
	if( !OpenGLPlugin::hasContextStatic() )
	{
		QMessageBox::warning( nullptr, tr( "No OpenGL Context" ), tr( "Can't edit texture without OpenGL context" ), QMessageBox::Ok, QMessageBox::NoButton );

		return;
	}

	QScopedPointer<TextureNodeForm>		Form( new TextureNodeForm( mTexture, true, mNode->context()->global()->mainWindow() ) );

	if( Form )
	{
		Form->setModal( true );

		if( Form->exec() == QDialog::Accepted )
		{
			mTexture->setTarget( Form->target() );
			mTexture->setFormat( Form->format() );
			mTexture->setInternalFormat( Form->internal() );
			mTexture->setType( Form->type() );
			mTexture->setFilter( Form->filterMin(), Form->filterMag() );
			mTexture->setGenMipMaps( Form->genMipMaps() );
			mTexture->setWrap( Form->wrapS(), Form->wrapT(), Form->wrapR() );

			mNode->context()->updateNode( mNode );
		}
	}
}

void ImageToTextureNode::transferTexture( qint64 pTimeStamp )
{
	fugio::Performance	Perf( mNode, __FUNCTION__, pTimeStamp );

	fugio::ImageInterface	*I = input<fugio::ImageInterface *>( mPinInputImage );

	mTexture->update( I->buffer( 0 ), I->bufferSize( 0 ), I->lineSize( 0 ) );

	OPENGL_DEBUG( mNode->name() );

	if( mTexture->dstTexId() )
	{
		pinUpdated( mPinOutput );
	}
	else
	{
		mTexture->setSize( 0 );
	}

	OPENGL_DEBUG( mNode->name() );

	if( mTexture->size() != mOutputTextureSize->variant().value<QVector3D>() )
	{
		mOutputTextureSize->setVariant( mTexture->size() );

		pinUpdated( mPinOutputTextureSize );
	}
}

void ImageToTextureNode::inputsUpdated( qint64 pTimeStamp )
{
	if( !OpenGLPlugin::hasContextStatic() )
	{
		return;
	}

	OPENGL_DEBUG( mNode->name() );

	QVector3D		TexSze = mTexture->size();

	fugio::ImageInterface	*I = input<fugio::ImageInterface *>( mPinInputImage );

	if( !I || !I->isValid() )
	{
		if( QVector3D() != mOutputTextureSize->variant().value<QVector3D>() )
		{
			mOutputTextureSize->setVariant( QVector3D() );

			pinUpdated( mPinOutputTextureSize );
		}

		return;
	}

	QVector3D		S = QVector3D( I->size().width(), I->size().height(), 0 );

	if( TexSze != S )
	{
		mTexture->free();

		mTexture->setType( GL_UNSIGNED_BYTE );

		switch( I->format() )
		{
#if !defined( GL_ES_VERSION_2_0 )
			case fugio::ImageInterface::FORMAT_BGR8:
				mTexture->setFormat( GL_BGR );

				mTexture->setInternalFormat( GL_RGB8 );
				break;

			case fugio::ImageInterface::FORMAT_RGB8:
				mTexture->setFormat( GL_RGB );

				mTexture->setInternalFormat( GL_RGB8 );
				break;

			case fugio::ImageInterface::FORMAT_RGBA8:
				mTexture->setFormat( GL_RGBA );

				mTexture->setInternalFormat( GL_RGBA8 );
				break;

			case fugio::ImageInterface::FORMAT_BGRA8:
				mTexture->setFormat( GL_BGRA );

				mTexture->setInternalFormat( GL_RGBA8 );
				break;

			case fugio::ImageInterface::FORMAT_GRAY16:
				mTexture->setType( GL_UNSIGNED_SHORT );

				if( GLEW_VERSION_3_0 )
				{
					mTexture->setFormat( GL_RED_INTEGER );

					mTexture->setInternalFormat( GL_R16UI );
				}
				else
				{
					mTexture->setFormat( GL_LUMINANCE );

					mTexture->setInternalFormat( GL_LUMINANCE16 );
				}
				break;

			case fugio::ImageInterface::FORMAT_GRAY8:
				if( GLEW_VERSION_3_0 )
				{
					mTexture->setFormat( GL_RED_INTEGER );

					mTexture->setInternalFormat( GL_R8UI );
				}
				else
				{
					mTexture->setFormat( GL_LUMINANCE );

					mTexture->setInternalFormat( GL_LUMINANCE8 );
				}
				break;

			case fugio::ImageInterface::FORMAT_YUYV422:
				mTexture->setFormat( GL_RG );

				mTexture->setInternalFormat( GL_RG8 );
				break;

			case fugio::ImageInterface::FORMAT_RG32:
				mTexture->setFormat( GL_RG );

				mTexture->setType( GL_FLOAT );

				mTexture->setInternalFormat( GL_RG32F );
				break;

			case fugio::ImageInterface::FORMAT_DXT1:
				mTexture->setFormat( GL_RGBA );

				mTexture->setInternalFormat( GL_COMPRESSED_RGBA_S3TC_DXT1_EXT );
				break;

			case fugio::ImageInterface::FORMAT_DXT5:
			case fugio::ImageInterface::FORMAT_YCoCg_DXT5:
				mTexture->setFormat( GL_RGBA );

				mTexture->setInternalFormat( GL_COMPRESSED_RGBA_S3TC_DXT5_EXT );
				break;
#endif
			default:
				return;
		}

		mTexture->setSize( S );

		emit editable( false );
	}

	OPENGL_DEBUG( mNode->name() );

	if( false )//true )
	{
		mNode->context()->futureSync( QtConcurrent::run( this, &ImageToTextureNode::transferTexture, pTimeStamp ) );
	}
	else
	{
		transferTexture( pTimeStamp );
	}
}

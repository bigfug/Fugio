#include "texturenode.h"

#include <QSettings>
#include <QDebug>
#include <QComboBox>
#include <QDateTime>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMainWindow>
#include <QMessageBox>

#include <fugio/global_interface.h>
#include "fugio/context_interface.h"
#include <fugio/editor_interface.h>

#include <fugio/core/variant_interface.h>
#include <fugio/performance.h>

#include "openglplugin.h"

#include "texturenodeform.h"

TextureNode::TextureNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
	static const QUuid	PIN_INPUT_TEXTURE	= QUuid( "{0c6cbd5b-221d-4579-90ee-f294c7313f2a}" );
	static const QUuid	PIN_INPUT_SIZE		= QUuid( "{8603948b-cc90-4be5-9bae-116a36452758}" );
	static const QUuid	PIN_OUTPUT_SIZE		= QUuid( "{b98dfce9-2107-4ff2-bc7d-91d67a890a5d}" );
	static const QUuid	PIN_OUTPUT_TEXTURE	= QUuid( "{5a3a6525-c1e7-43e1-92f0-67d581815450}" );

	if( ( mPinInputTexture = pinInput( "Texture", PIN_INPUT_TEXTURE ) ) == 0 )
	{
		return;
	}

	mPinInputSize = pinInput( "Size", PIN_INPUT_SIZE );

	mValOutputTexture = pinOutput<OpenGLTextureInterface *>( "Texture", mPinOutput, PID_OPENGL_TEXTURE, PIN_OUTPUT_TEXTURE );

	mValOutputTexture->setTarget( QOpenGLTexture::Target2D );

	mPinInputTexture->setControl( mPinOutput->control() );

	mOutputTextureSize = pinOutput<VariantInterface *>( "Texture Size", mPinOutputTextureSize, PID_SIZE_3D, PIN_OUTPUT_SIZE );

	mPinInputTexture->setDescription( tr( "An input from a node that requires a texture, such as when rendering a Shader to a texture" ) );

	mPinOutput->setDescription( tr( "The allocated OpenGL texture" ) );
}

TextureNode::~TextureNode( void )
{
	mPinInputTexture->control().clear();
}

QWidget *TextureNode::gui()
{
	QPushButton		*GUI = new QPushButton( "Edit..." );

	connect( GUI, SIGNAL(clicked()), this, SLOT(onEditClicked()) );

	return( GUI );
}

bool TextureNode::initialise()
{
	if( !fugio::NodeControlBase::initialise() )
	{
		return( false );
	}

	if( !OpenGLPlugin::hasContextStatic() )
	{
		return( false );
	}

	return( true );
}

void TextureNode::loadSettings( QSettings &pSettings )
{
	QString		CurVal;
	int			CurInt;

	CurVal = OpenGLPlugin::mMapTargets.key( mValOutputTexture->target() );
	CurVal = pSettings.value( "Target", CurVal ).toString();
	CurInt = OpenGLPlugin::mMapTargets.value( CurVal, mValOutputTexture->target() );

	mValOutputTexture->setTarget( QOpenGLTexture::Target( CurInt ) );

	CurVal = OpenGLPlugin::mMapFormat.key( mValOutputTexture->format() );
	CurVal = pSettings.value( "Format", CurVal ).toString();
	CurInt = OpenGLPlugin::mMapFormat.value( CurVal, mValOutputTexture->format() );

	mValOutputTexture->setFormat( QOpenGLTexture::PixelFormat( CurInt ) );

	CurVal = OpenGLPlugin::mMapInternal.key( mValOutputTexture->internalFormat() );
	CurVal = pSettings.value( "Internal", CurVal ).toString();
	CurInt = OpenGLPlugin::mMapInternal.value( CurVal, mValOutputTexture->internalFormat() );

	mValOutputTexture->setInternalFormat( QOpenGLTexture::TextureFormat( CurInt ) );

	CurVal = OpenGLPlugin::mMapType.key( mValOutputTexture->type() );
	CurVal = pSettings.value( "Type", CurVal ).toString();
	CurInt = OpenGLPlugin::mMapType.value( CurVal, mValOutputTexture->type() );

	mValOutputTexture->setType( QOpenGLTexture::PixelType( CurInt ) );

	int		CurMin = mValOutputTexture->filterMin();
	int		CurMag = mValOutputTexture->filterMag();

	CurVal = OpenGLPlugin::mMapFilterMin.key( CurMin );
	CurVal = pSettings.value( "FilterMin", CurVal ).toString();
	CurMin = OpenGLPlugin::mMapFilterMin.value( CurVal, CurMin );

	CurVal = OpenGLPlugin::mMapFilterMag.key( CurMag );
	CurVal = pSettings.value( "FilterMag", CurVal ).toString();
	CurMag = OpenGLPlugin::mMapFilterMin.value( CurVal, CurMag );

	mValOutputTexture->setFilter( QOpenGLTexture::Filter( CurMin ), QOpenGLTexture::Filter( CurMag ) );

	int		CurWPS = mValOutputTexture->wrapS();
	int		CurWPT = mValOutputTexture->wrapT();
	int		CurWPR = mValOutputTexture->wrapR();

	CurVal = OpenGLPlugin::mMapWrap.key( CurWPS );
	CurVal = pSettings.value( "WrapS", CurVal ).toString();
	CurWPS = OpenGLPlugin::mMapWrap.value( CurVal, CurWPS );

	CurVal = OpenGLPlugin::mMapWrap.key( CurWPT );
	CurVal = pSettings.value( "WrapT", CurVal ).toString();
	CurWPT = OpenGLPlugin::mMapWrap.value( CurVal, CurWPT );

	CurVal = OpenGLPlugin::mMapWrap.key( CurWPR );
	CurVal = pSettings.value( "WrapR", CurVal ).toString();
	CurWPR = OpenGLPlugin::mMapWrap.value( CurVal, CurWPR );

	mValOutputTexture->setWrap( QOpenGLTexture::WrapMode( CurWPS ), QOpenGLTexture::WrapMode( CurWPT ), QOpenGLTexture::WrapMode( CurWPR ) );

	CurVal = OpenGLPlugin::mMapCompare.key( mValOutputTexture->compare() );
	CurVal = pSettings.value( "Compare", CurVal ).toString();
	CurInt = OpenGLPlugin::mMapCompare.value( CurVal, mValOutputTexture->compare() );

	mValOutputTexture->setGenMipMaps( pSettings.value( "MipMaps", mValOutputTexture->genMipMaps() ).toBool() );

	mValOutputTexture->setDoubleBuffered( pSettings.value( "DoubleBuffer", mValOutputTexture->isDoubleBuffered() ).toBool() );
}

void TextureNode::saveSettings( QSettings &pSettings ) const
{
	pSettings.setValue( "Target", OpenGLPlugin::mMapTargets.key( mValOutputTexture->target() ) );
	pSettings.setValue( "Format", OpenGLPlugin::mMapFormat.key( mValOutputTexture->format() ) );
	pSettings.setValue( "Internal", OpenGLPlugin::mMapInternal.key( mValOutputTexture->internalFormat() ) );
	pSettings.setValue( "Type", OpenGLPlugin::mMapType.key( mValOutputTexture->type() ) );
	pSettings.setValue( "FilterMin", OpenGLPlugin::mMapFilterMin.key( mValOutputTexture->filterMin() ) );
	pSettings.setValue( "FilterMag", OpenGLPlugin::mMapFilterMag.key( mValOutputTexture->filterMag() ) );
	pSettings.setValue( "WrapS", OpenGLPlugin::mMapWrap.key( mValOutputTexture->wrapS() ) );
	pSettings.setValue( "WrapT", OpenGLPlugin::mMapWrap.key( mValOutputTexture->wrapT() ) );
	pSettings.setValue( "WrapR", OpenGLPlugin::mMapWrap.key( mValOutputTexture->wrapR() ) );
	pSettings.setValue( "MipMaps", mValOutputTexture->genMipMaps() );
	pSettings.setValue( "DoubleBuffer", mValOutputTexture->isDoubleBuffered() );
	pSettings.setValue( "Compare", OpenGLPlugin::mMapCompare.key( mValOutputTexture->compare() ) );
}

void TextureNode::onEditClicked()
{
	if( !OpenGLPlugin::hasContextStatic() )
	{
		QMessageBox::warning( nullptr, tr( "No OpenGL Context" ), tr( "Can't edit texture without OpenGL context" ), QMessageBox::Ok, QMessageBox::NoButton );

		return;
	}

	fugio::EditorInterface	*EI = qobject_cast<fugio::EditorInterface *>( mNode->context()->global()->findInterface( IID_EDITOR ) );

	QScopedPointer<TextureNodeForm>		Form( new TextureNodeForm( mValOutputTexture, false, EI->mainWindow() ) );

	if( Form )
	{
		Form->setModal( true );

		if( Form->exec() == QDialog::Accepted )
		{
			mValOutputTexture->free();

			mValOutputTexture->setTarget( QOpenGLTexture::Target( Form->target() ) );
			mValOutputTexture->setFormat( Form->format() );
			mValOutputTexture->setInternalFormat( Form->internal() );
			mValOutputTexture->setType( Form->type() );
			mValOutputTexture->setFilter( Form->filterMin(), Form->filterMag() );
			mValOutputTexture->setGenMipMaps( Form->genMipMaps() );
			mValOutputTexture->setDoubleBuffered( Form->doubleBuffer() );
			mValOutputTexture->setWrap( Form->wrapS(), Form->wrapT(), Form->wrapR() );
			mValOutputTexture->setCompare( Form->compare() );

			mNode->context()->updateNode( mNode );
		}
	}
}

void TextureNode::inputsUpdated( qint64 pTimeStamp )
{
	fugio::Performance	Perf( mNode, "inputsUpdated", pTimeStamp );

	Q_UNUSED( pTimeStamp )

	if( !OpenGLPlugin::hasContextStatic() )
	{
		return;
	}

	OPENGL_PLUGIN_DEBUG

	QVector3D	NewSze;

	if( mPinInputSize->isConnected() )
	{
		VariantInterface	*V = input<VariantInterface *>( mPinInputSize );

		if( V )
		{
			if( V->variant().canConvert( QVariant::Vector3D ) )
			{
				NewSze = V->variant().value<QVector3D>();
			}
			else if( V->variant().canConvert( QVariant::Vector2D ) )
			{
				NewSze = V->variant().value<QVector2D>();
			}
			else if( V->variant().canConvert( QVariant::SizeF ) )
			{
				NewSze.setX( qMax( 0.0, V->variant().value<QSizeF>().width() ) );
				NewSze.setY( qMax( 0.0, V->variant().value<QSizeF>().height() ) );
			}
			else if( V->variant().canConvert( QVariant::Size ) )
			{
				NewSze.setX( qMax( 0, V->variant().value<QSize>().width() ) );
				NewSze.setY( qMax( 0, V->variant().value<QSize>().height() ) );
			}
			else if( V->variant().canConvert( QVariant::Double ) )
			{
				NewSze.setX( V->variant().value<double>() );
			}
		}
	}

	if( NewSze.isNull() )
	{
		return;
	}

	mValOutputTexture->setSize( NewSze );

	mValOutputTexture->update();

	if( mValOutputTexture->dstTexId() )
	{
		pinUpdated( mPinOutput );
	}

	if( mValOutputTexture->size() != mOutputTextureSize->variant().value<QVector3D>() )
	{
		mOutputTextureSize->setVariant( mValOutputTexture->size() );

		pinUpdated( mPinOutputTextureSize );
	}

	OPENGL_PLUGIN_DEBUG
}

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

	mTexture = pinOutput<OpenGLTextureInterface *>( "Texture", mPinOutput, PID_OPENGL_TEXTURE, PIN_OUTPUT_TEXTURE );

	mTexture->setTarget( GL_TEXTURE_2D );

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

	CurVal = OpenGLPlugin::mMapCompare.key( mTexture->compare() );
	CurVal = pSettings.value( "Compare", CurVal ).toString();
	CurInt = OpenGLPlugin::mMapCompare.value( CurVal, mTexture->compare() );

	mTexture->setGenMipMaps( pSettings.value( "MipMaps", mTexture->genMipMaps() ).toBool() );

	mTexture->setDoubleBuffered( pSettings.value( "DoubleBuffer", mTexture->isDoubleBuffered() ).toBool() );
}

void TextureNode::saveSettings( QSettings &pSettings ) const
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
	pSettings.setValue( "DoubleBuffer", mTexture->isDoubleBuffered() );
	pSettings.setValue( "Compare", OpenGLPlugin::mMapCompare.key( mTexture->compare() ) );
}

void TextureNode::onEditClicked()
{
	if( !OpenGLPlugin::hasContextStatic() )
	{
		QMessageBox::warning( nullptr, tr( "No OpenGL Context" ), tr( "Can't edit texture without OpenGL context" ), QMessageBox::Ok, QMessageBox::NoButton );

		return;
	}

	QScopedPointer<TextureNodeForm>		Form( new TextureNodeForm( mTexture, false, mNode->context()->global()->mainWindow() ) );

	if( Form != 0 )
	{
		Form->setModal( true );

		if( Form->exec() == QDialog::Accepted )
		{
			mTexture->free();

			mTexture->setTarget( Form->target() );
			mTexture->setFormat( Form->format() );
			mTexture->setInternalFormat( Form->internal() );
			mTexture->setType( Form->type() );
			mTexture->setFilter( Form->filterMin(), Form->filterMag() );
			mTexture->setGenMipMaps( Form->genMipMaps() );
			mTexture->setDoubleBuffered( Form->doubleBuffer() );
			mTexture->setWrap( Form->wrapS(), Form->wrapT(), Form->wrapR() );
			mTexture->setCompare( Form->compare() );

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

	mTexture->setSize( NewSze );

	mTexture->update( 0, 0 );

	if( mTexture->dstTexId() )
	{
		pinUpdated( mPinOutput );
	}

	if( mTexture->size() != mOutputTextureSize->variant().value<QVector3D>() )
	{
		mOutputTextureSize->setVariant( mTexture->size() );

		pinUpdated( mPinOutputTextureSize );
	}

	OPENGL_PLUGIN_DEBUG
}

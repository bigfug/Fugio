#include "texturenodeform.h"
#include "ui_texturenodeform.h"

#include <openglplugin.h>

#define INSERT_FORMAT(x)		mMapFormat.insert(#x,x)
#define INSERT_INTERNAL(x)		mMapInternal.insert(#x,x)
#define INSERT_TYPE(x)			mMapType.insert(#x,x)

TextureNodeForm::TextureNodeForm( OpenGLTextureInterface *pTexPin, bool pIsImage, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::TextureNodeForm), mTexPin( pTexPin ), mIsImage( pIsImage )
{
	ui->setupUi(this);

	ui->mComboTarget->addItems( OpenGLPlugin::mMapTargets.keys() );

	ui->mComboFormat->addItems( OpenGLPlugin::mMapFormat.keys() );

	ui->mComboInternalFormat->addItems( OpenGLPlugin::mMapInternal.keys() );

	ui->mComboType->addItems( OpenGLPlugin::mMapType.keys() );

	ui->mFilterMin->addItems( OpenGLPlugin::mMapFilterMin.keys() );

	ui->mFilterMag->addItems( OpenGLPlugin::mMapFilterMag.keys() );

	ui->mWrapS->addItems( OpenGLPlugin::mMapWrap.keys() );
	ui->mWrapT->addItems( OpenGLPlugin::mMapWrap.keys() );
	ui->mWrapR->addItems( OpenGLPlugin::mMapWrap.keys() );

	ui->mCompare->addItems( OpenGLPlugin::mMapCompare.keys() );

	ui->mComboTarget->setCurrentText( OpenGLPlugin::mMapTargets.key( mTexPin->target() ) );
	ui->mComboFormat->setCurrentText( OpenGLPlugin::mMapFormat.key( mTexPin->format() ) );
	ui->mComboInternalFormat->setCurrentText( OpenGLPlugin::mMapInternal.key( mTexPin->internalFormat() ) );
	ui->mComboType->setCurrentText( OpenGLPlugin::mMapType.key( mTexPin->type() ) );
	ui->mFilterMin->setCurrentText( OpenGLPlugin::mMapFilterMin.key( mTexPin->filterMin() ) );
	ui->mFilterMag->setCurrentText( OpenGLPlugin::mMapFilterMag.key( mTexPin->filterMag() ) );
	ui->mWrapS->setCurrentText( OpenGLPlugin::mMapWrap.key( mTexPin->wrapS() ) );
	ui->mWrapT->setCurrentText( OpenGLPlugin::mMapWrap.key( mTexPin->wrapT() ) );
	ui->mWrapR->setCurrentText( OpenGLPlugin::mMapWrap.key( mTexPin->wrapR() ) );
	ui->mCompare->setCurrentText( OpenGLPlugin::mMapCompare.key( mTexPin->compare() ) );

	ui->mMipMaps->setChecked( mTexPin->genMipMaps() );

	if( mIsImage )
	{
		ui->mDoubleBuffer->setEnabled( false );
		ui->mComboTarget->setEnabled( false );
		ui->mComboFormat->setEnabled( false );
		ui->mComboInternalFormat->setEnabled( false );
		ui->mComboType->setEnabled( false );
	}
	else
	{
		ui->mDoubleBuffer->setChecked( mTexPin->isDoubleBuffered() );
	}

	updateInternalFormat();
}

TextureNodeForm::~TextureNodeForm()
{
	delete ui;
}

int TextureNodeForm::target() const
{
	return( OpenGLPlugin::mMapTargets.value( ui->mComboTarget->currentText() ) );
}

int TextureNodeForm::format() const
{
	return( OpenGLPlugin::mMapFormat.value( ui->mComboFormat->currentText() ) );
}

int TextureNodeForm::internal() const
{
	return( OpenGLPlugin::mMapInternal.value( ui->mComboInternalFormat->currentText() ) );
}

int TextureNodeForm::type() const
{
	return( OpenGLPlugin::mMapType.value( ui->mComboType->currentText() ) );
}

int TextureNodeForm::filterMin() const
{
	return( OpenGLPlugin::mMapFilterMin.value( ui->mFilterMin->currentText() ) );
}

int TextureNodeForm::filterMag() const
{
	return( OpenGLPlugin::mMapFilterMag.value( ui->mFilterMag->currentText() ) );
}

int TextureNodeForm::wrapS() const
{
	return( OpenGLPlugin::mMapWrap.value( ui->mWrapS->currentText() ) );
}

int TextureNodeForm::wrapT() const
{
	return( OpenGLPlugin::mMapWrap.value( ui->mWrapT->currentText() ) );
}

int TextureNodeForm::wrapR() const
{
	return( OpenGLPlugin::mMapWrap.value( ui->mWrapR->currentText() ) );
}

bool TextureNodeForm::genMipMaps() const
{
	return( ui->mMipMaps->isChecked() );
}

bool TextureNodeForm::doubleBuffer() const
{
	return( ui->mDoubleBuffer->isChecked() );
}

int TextureNodeForm::compare() const
{
	return( OpenGLPlugin::mMapCompare.value( ui->mCompare->currentText() ) );
}

void TextureNodeForm::updateInternalFormat()
{
	mMapInternal.clear();
	mMapType.clear();

	INSERT_TYPE( GL_UNSIGNED_BYTE );
	INSERT_TYPE( GL_BYTE );
	INSERT_TYPE( GL_UNSIGNED_SHORT );
	INSERT_TYPE( GL_SHORT );
	INSERT_TYPE( GL_UNSIGNED_INT );
	INSERT_TYPE( GL_INT );
	INSERT_TYPE( GL_FLOAT );

	switch( OpenGLPlugin::mMapFormat.value( ui->mComboFormat->currentText() ) )
	{
#if !defined( GL_ES_VERSION_2_0 )
		case GL_RED:
			INSERT_INTERNAL( GL_R8 );
			INSERT_INTERNAL( GL_R8_SNORM );
			INSERT_INTERNAL( GL_R16 );
			INSERT_INTERNAL( GL_R16_SNORM );
			INSERT_INTERNAL( GL_R16F );
			INSERT_INTERNAL( GL_R32F );
			break;

		case GL_RED_INTEGER:
			INSERT_INTERNAL( GL_R8I );
			INSERT_INTERNAL( GL_R8UI );
			INSERT_INTERNAL( GL_R16I );
			INSERT_INTERNAL( GL_R16UI );
			INSERT_INTERNAL( GL_R32I );
			INSERT_INTERNAL( GL_R32UI );
			break;

		case GL_RG:
			INSERT_INTERNAL( GL_RG8 );
			INSERT_INTERNAL( GL_RG8_SNORM );
			INSERT_INTERNAL( GL_RG16 );
			INSERT_INTERNAL( GL_RG16_SNORM );
			INSERT_INTERNAL( GL_RG16F );
			INSERT_INTERNAL( GL_RG32F );
			break;

		case GL_RG_INTEGER:
			INSERT_INTERNAL( GL_RG8I );
			INSERT_INTERNAL( GL_RG8UI );
			INSERT_INTERNAL( GL_RG16I );
			INSERT_INTERNAL( GL_RG16UI );
			INSERT_INTERNAL( GL_RG32I );
			INSERT_INTERNAL( GL_RG32UI );
			break;
#endif

		case GL_RGB:
#if !defined( GL_ES_VERSION_2_0 )
			INSERT_TYPE( GL_UNSIGNED_BYTE_3_3_2 );
			INSERT_TYPE( GL_UNSIGNED_BYTE_2_3_3_REV );
			INSERT_TYPE( GL_UNSIGNED_SHORT_5_6_5_REV );
			INSERT_TYPE( GL_UNSIGNED_INT_10F_11F_11F_REV );
#endif
			INSERT_TYPE( GL_UNSIGNED_SHORT_5_6_5 );

#if !defined( GL_ES_VERSION_2_0 )
		case GL_BGR:
			INSERT_INTERNAL( GL_R3_G3_B2 );
			INSERT_INTERNAL( GL_RGB4 );
			INSERT_INTERNAL( GL_RGB5 );
			INSERT_INTERNAL( GL_RGB8 );
			INSERT_INTERNAL( GL_RGB8_SNORM );
			INSERT_INTERNAL( GL_RGB10 );
			INSERT_INTERNAL( GL_RGB12 );
			INSERT_INTERNAL( GL_RGB16_SNORM );
			INSERT_INTERNAL( GL_RGBA2 );
#endif
			INSERT_INTERNAL( GL_RGBA4 );
#if !defined( GL_ES_VERSION_2_0 )
			INSERT_INTERNAL( GL_SRGB8 );
			INSERT_INTERNAL( GL_RGB16F );
			INSERT_INTERNAL( GL_RGB32F );
			INSERT_INTERNAL( GL_R11F_G11F_B10F );
			INSERT_INTERNAL( GL_RGB9_E5 );
#endif
			break;

#if !defined( GL_ES_VERSION_2_0 )
		case GL_RGB_INTEGER:
		case GL_BGR_INTEGER:
			INSERT_INTERNAL( GL_RGB8I );
			INSERT_INTERNAL( GL_RGB8UI );
			INSERT_INTERNAL( GL_RGB16I );
			INSERT_INTERNAL( GL_RGB16UI );
			INSERT_INTERNAL( GL_RGB32I );
			INSERT_INTERNAL( GL_RGB32UI );
			break;
#endif

		case GL_RGBA:
#if !defined( GL_ES_VERSION_2_0 )
		case GL_BGRA:
			INSERT_TYPE( GL_UNSIGNED_SHORT_4_4_4_4_REV );
			INSERT_TYPE( GL_UNSIGNED_SHORT_1_5_5_5_REV );
			INSERT_TYPE( GL_UNSIGNED_INT_8_8_8_8 );
			INSERT_TYPE( GL_UNSIGNED_INT_8_8_8_8_REV );
			INSERT_TYPE( GL_UNSIGNED_INT_10_10_10_2 );
			INSERT_TYPE( GL_UNSIGNED_INT_2_10_10_10_REV );
			INSERT_TYPE( GL_UNSIGNED_INT_5_9_9_9_REV );

			INSERT_INTERNAL( GL_RGBA8 );
			INSERT_INTERNAL( GL_RGBA8_SNORM );
			INSERT_INTERNAL( GL_RGB10_A2 );
			INSERT_INTERNAL( GL_RGB10_A2UI );
			INSERT_INTERNAL( GL_RGBA12 );
			INSERT_INTERNAL( GL_RGBA16 );
			INSERT_INTERNAL( GL_SRGB8_ALPHA8 );
			INSERT_INTERNAL( GL_RGBA16F );
			INSERT_INTERNAL( GL_RGBA32F );
#endif
			INSERT_TYPE( GL_UNSIGNED_SHORT_4_4_4_4 );
			INSERT_TYPE( GL_UNSIGNED_SHORT_5_5_5_1 );

			INSERT_INTERNAL( GL_RGB5_A1 );
			break;

#if !defined( GL_ES_VERSION_2_0 )
		case GL_RGBA_INTEGER:
		case GL_BGRA_INTEGER:
			INSERT_INTERNAL( GL_RGBA8I );
			INSERT_INTERNAL( GL_RGBA8UI );
			INSERT_INTERNAL( GL_RGBA16I );
			INSERT_INTERNAL( GL_RGBA16UI );
			INSERT_INTERNAL( GL_RGBA32I );
			INSERT_INTERNAL( GL_RGBA32UI );
			break;
#endif

		case GL_DEPTH_COMPONENT:
			INSERT_INTERNAL( GL_DEPTH_COMPONENT16 );
#if !defined( GL_ES_VERSION_2_0 )
			INSERT_INTERNAL( GL_DEPTH_COMPONENT24 );
			INSERT_INTERNAL( GL_DEPTH_COMPONENT32 );
			INSERT_INTERNAL( GL_DEPTH_COMPONENT32F );
#endif
			break;

		case GL_STENCIL_INDEX:
#if !defined( GL_ES_VERSION_2_0 )
			INSERT_INTERNAL( GL_STENCIL_INDEX1 );
			INSERT_INTERNAL( GL_STENCIL_INDEX4 );
			INSERT_INTERNAL( GL_STENCIL_INDEX16 );
#endif
			INSERT_INTERNAL( GL_STENCIL_INDEX8 );
			break;

#if !defined( GL_ES_VERSION_2_0 )
		case GL_DEPTH_STENCIL:
			INSERT_INTERNAL( GL_DEPTH24_STENCIL8 );
			INSERT_INTERNAL( GL_DEPTH32F_STENCIL8 );
			break;
#endif
	}

	if( OpenGLPlugin::mMapFormat.value( ui->mComboFormat->currentText() ) == GL_DEPTH_COMPONENT )
	{
		ui->mCompare->setEnabled( true );
	}
	else
	{
		ui->mCompare->setEnabled( true );
	}

	ui->mComboInternalFormat->blockSignals( true );

	ui->mComboInternalFormat->clear();

	ui->mComboInternalFormat->addItems( mMapInternal.keys() );

	ui->mComboInternalFormat->setCurrentText( mMapInternal.key( mTexPin->internalFormat(), mMapInternal.firstKey() ) );

	ui->mComboInternalFormat->blockSignals( false );

	ui->mComboType->blockSignals( true );

	ui->mComboType->clear();

	ui->mComboType->addItems( mMapType.keys() );

	ui->mComboType->setCurrentText( mMapType.key( mTexPin->type(), mMapType.firstKey() ) );

	ui->mComboType->blockSignals( false );
}

void TextureNodeForm::on_mComboTarget_currentIndexChanged(const QString &arg1)
{
	GLenum		Target = OpenGLPlugin::mMapTargets.value( arg1 );

	mMapFormat.clear();

	INSERT_FORMAT( GL_RGB );
	INSERT_FORMAT( GL_RGBA );
#if !defined( GL_ES_VERSION_2_0 )
	INSERT_FORMAT( GL_RED );
	INSERT_FORMAT( GL_RG );
	INSERT_FORMAT( GL_BGR );
	INSERT_FORMAT( GL_BGRA );
	INSERT_FORMAT( GL_RED_INTEGER );
	INSERT_FORMAT( GL_RG_INTEGER );
	INSERT_FORMAT( GL_RGB_INTEGER );
	INSERT_FORMAT( GL_BGR_INTEGER );
	INSERT_FORMAT( GL_RGBA_INTEGER );
	INSERT_FORMAT( GL_BGRA_INTEGER );
#endif

	switch( Target )
	{
#if !defined( GL_ES_VERSION_2_0 )
		case GL_TEXTURE_1D:
			ui->mWrapS->setEnabled( true );
			ui->mWrapT->setEnabled( false );
			ui->mWrapR->setEnabled( false );
			break;
#endif

		case GL_TEXTURE_2D:
#if !defined( GL_ES_VERSION_2_0 )
		case GL_TEXTURE_RECTANGLE:
#endif
			ui->mWrapS->setEnabled( true );
			ui->mWrapT->setEnabled( true );
			ui->mWrapR->setEnabled( false );

			if( !mIsImage )
			{
				INSERT_FORMAT( GL_DEPTH_COMPONENT );
				INSERT_FORMAT( GL_STENCIL_INDEX );
#if !defined( GL_ES_VERSION_2_0 )
				INSERT_FORMAT( GL_DEPTH_STENCIL );
#endif
			}
			break;

		case GL_TEXTURE_CUBE_MAP:
			ui->mWrapS->setEnabled( true );
			ui->mWrapT->setEnabled( true );
			ui->mWrapR->setEnabled( true );

			if( !mIsImage )
			{
				INSERT_FORMAT( GL_DEPTH_COMPONENT );
			}
			break;

#if !defined( GL_ES_VERSION_2_0 )
		case GL_TEXTURE_3D:
			ui->mWrapS->setEnabled( true );
			ui->mWrapT->setEnabled( true );
			ui->mWrapR->setEnabled( true );
			break;
#endif
	}

	ui->mComboFormat->blockSignals( true );

	ui->mComboFormat->clear();

	ui->mComboFormat->addItems( mMapFormat.keys() );

	ui->mComboFormat->setCurrentText( mMapFormat.key( mTexPin->format(), mMapFormat.firstKey() ) );

	ui->mComboFormat->blockSignals( false );
}

void TextureNodeForm::on_mComboFormat_currentIndexChanged(const QString &)
{
	updateInternalFormat();
}

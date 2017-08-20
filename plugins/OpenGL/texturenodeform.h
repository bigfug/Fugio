#ifndef TEXTURENODEFORM_H
#define TEXTURENODEFORM_H

#include <QDialog>
#include <QMap>

#include "opengl_includes.h"

#include <fugio/opengl/texture_interface.h>

namespace Ui {
	class TextureNodeForm;
}

class TextureNodeForm : public QDialog
{
	Q_OBJECT

public:
	explicit TextureNodeForm( fugio::OpenGLTextureInterface *pTexPin, bool pIsImage, QWidget *parent = 0);

	virtual ~TextureNodeForm();

	int target( void ) const;
	QOpenGLTexture::PixelFormat format( void ) const;
	QOpenGLTexture::TextureFormat internal( void ) const;
	QOpenGLTexture::PixelType type( void ) const;
	QOpenGLTexture::Filter filterMin( void ) const;
	QOpenGLTexture::Filter filterMag( void ) const;
	QOpenGLTexture::WrapMode wrapS( void ) const;
	QOpenGLTexture::WrapMode wrapT( void ) const;
	QOpenGLTexture::WrapMode wrapR( void ) const;
	bool genMipMaps( void ) const;
	bool doubleBuffer( void ) const;
	QOpenGLTexture::ComparisonFunction compare( void ) const;

private slots:

	void on_mComboTarget_currentIndexChanged(const QString &arg1);

	void on_mComboFormat_currentIndexChanged(const QString &arg1);

private:
	void updateInternalFormat( void );

private:
	Ui::TextureNodeForm						*ui;
	fugio::OpenGLTextureInterface			*mTexPin;
	QMap<QString,int>						 mMapFormat;
	QMap<QString,int>						 mMapInternal;
	QMap<QString,int>						 mMapType;
	const bool								 mIsImage;
};

#endif // TEXTURENODEFORM_H

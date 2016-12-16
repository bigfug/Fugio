#ifndef TEXTURENODEFORM_H
#define TEXTURENODEFORM_H

#include <QDialog>
#include <QMap>

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
	int format( void ) const;
	int internal( void ) const;
	int type( void ) const;
	int filterMin( void ) const;
	int filterMag( void ) const;
	int wrapS( void ) const;
	int wrapT( void ) const;
	int wrapR( void ) const;
	bool genMipMaps( void ) const;
	bool doubleBuffer( void ) const;
	int compare( void ) const;

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

#ifndef IMAGEBUFFER_H
#define IMAGEBUFFER_H

#include <QObject>
#include <QImage>
#include <vector>

#include <fugio/pin_control_interface.h>

#include <fugio/image/image_interface.h>
#include <fugio/image/uuid.h>

#include <fugio/pincontrolbase.h>

class ImagePin : public fugio::PinControlBase, public fugio::ImageInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::ImageInterface )

public:
	Q_INVOKABLE explicit ImagePin( QSharedPointer<fugio::PinInterface> pPin );
	
	virtual ~ImagePin( void );

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE;

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Image" );
	}

	//-------------------------------------------------------------------------
	// InterfaceImage

	virtual QSize size( void ) const Q_DECL_OVERRIDE
	{
		return( mImageSize );
	}

	virtual int width( void ) const Q_DECL_OVERRIDE
	{
		return( mImageSize.width() );
	}

	virtual int height( void ) const Q_DECL_OVERRIDE
	{
		return( mImageSize.height() );
	}

	virtual int lineSize( int pIndex ) const Q_DECL_OVERRIDE
	{
		Q_ASSERT( pIndex >= 0 && pIndex < PLANE_COUNT );

		return( mLineWidth[ pIndex ] );
	}

	virtual const int *lineSizes( void ) const Q_DECL_OVERRIDE
	{
		return( mLineWidth );
	}

	virtual quint8 *internalBuffer( int pIndex ) Q_DECL_OVERRIDE;
	virtual quint8 *internalBuffer( int pIndex ) const Q_DECL_OVERRIDE;

	virtual quint8 **internalBuffers( void ) Q_DECL_OVERRIDE;
	virtual quint8 **internalBuffers( void ) const Q_DECL_OVERRIDE;

	virtual const quint8 *buffer( int pIndex ) Q_DECL_OVERRIDE;
	virtual const quint8 *buffer( int pIndex ) const Q_DECL_OVERRIDE;

    //virtual const quint8 * const *buffers( void ) Q_DECL_OVERRIDE;
	virtual const quint8 * const *buffers( void ) const Q_DECL_OVERRIDE;

	virtual int bufferSize( int pIndex ) const Q_DECL_OVERRIDE
	{
		return( mImageSize.height() >= 0 ? mLineWidth[ pIndex ] * mImageSize.height() : 0 );
	}

	virtual Format format( void ) const Q_DECL_OVERRIDE
	{
		return( mImageFormat );
	}

	virtual int internalFormat( void ) const Q_DECL_OVERRIDE
	{
		return( mImageInternalFormat );
	}

	virtual void unsetBuffers( void ) Q_DECL_OVERRIDE;

	virtual void setBuffer( int pIndex, const quint8 *pBuffer ) Q_DECL_OVERRIDE;

	virtual void setBuffers( quint8 * const pBuffer[ PLANE_COUNT ] ) Q_DECL_OVERRIDE;

	virtual void setSize( quint32 pWidth, quint32 pHeight ) Q_DECL_OVERRIDE;

	virtual void setFormat( Format pFormat ) Q_DECL_OVERRIDE
	{
		mImageFormat = pFormat;
	}

	virtual void setInternalFormat( int pInternalFormat ) Q_DECL_OVERRIDE
	{
		mImageInternalFormat = pInternalFormat;
	}

	virtual void setLineSize( int pIndex, int pLineSize ) Q_DECL_OVERRIDE;

	virtual void setLineSizes( const int pLineSize[ PLANE_COUNT ] ) Q_DECL_OVERRIDE;

	virtual QImage image( void ) const Q_DECL_OVERRIDE;

private:
	mutable quint8			*mImageBuffer[ 8 ];
	mutable int				 mBufferSizes[ 8 ];
	const quint8			*mImagePointer[ 8 ];
	QSize					 mImageSize;
	int						 mLineWidth[ 8 ];
	fugio::ImageInterface::Format	 mImageFormat;
	int						 mImageInternalFormat;
};

#endif // IMAGEBUFFER_H

#ifndef IMAGEBUFFER_H
#define IMAGEBUFFER_H

#include <QObject>
#include <QImage>
#include <vector>

#include <fugio/pin_control_interface.h>

#include <fugio/image/image_interface.h>
#include <fugio/image/uuid.h>
#include <fugio/core/size_interface.h>
#include <fugio/core/variant_interface.h>

#include <fugio/pincontrolbase.h>

#include <fugio/image/image.h>

class ImagePin : public fugio::PinControlBase, public fugio::ImageInterface, public fugio::SizeInterface, public fugio::VariantInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::ImageInterface fugio::SizeInterface fugio::VariantInterface )

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

	inline virtual QSize size( void ) const Q_DECL_OVERRIDE
	{
		return( mImage.size() );
	}

	inline virtual int width( void ) const Q_DECL_OVERRIDE
	{
		return( mImage.width() );
	}

	inline virtual int height( void ) const Q_DECL_OVERRIDE
	{
		return( mImage.height() );
	}

	inline virtual int lineSize( int pIndex ) const Q_DECL_OVERRIDE
	{
		return( mImage.lineSize( pIndex ) );
	}

	inline virtual const int *lineSizes( void ) const Q_DECL_OVERRIDE
	{
		return( mImage.lineSizes() );
	}

	virtual quint8 *internalBuffer( int pIndex ) Q_DECL_OVERRIDE;
	virtual quint8 *internalBuffer( int pIndex ) const Q_DECL_OVERRIDE;

	virtual quint8 **internalBuffers( void ) Q_DECL_OVERRIDE;
	virtual quint8 **internalBuffers( void ) const Q_DECL_OVERRIDE;

	virtual const quint8 *buffer( int pIndex ) Q_DECL_OVERRIDE;
	virtual const quint8 *buffer( int pIndex ) const Q_DECL_OVERRIDE;

	//virtual const quint8 * const *buffers( void ) Q_DECL_OVERRIDE;
	virtual const quint8 * const *buffers( void ) const Q_DECL_OVERRIDE;

	virtual int bufferSize( int pIndex ) const Q_DECL_OVERRIDE;

	inline virtual Format format( void ) const Q_DECL_OVERRIDE
	{
		return( mImage.format() );
	}

	inline virtual int internalFormat( void ) const Q_DECL_OVERRIDE
	{
		return( mImage.internalFormat() );
	}

	virtual void unsetBuffers( void ) Q_DECL_OVERRIDE;

	virtual void setBuffer( int pIndex, const quint8 *pBuffer ) Q_DECL_OVERRIDE;

	virtual void setBuffers( quint8 * const pBuffer[ PLANE_COUNT ] ) Q_DECL_OVERRIDE;

	virtual void setSize( quint32 pWidth, quint32 pHeight ) Q_DECL_OVERRIDE;

	virtual void setFormat( Format pFormat ) Q_DECL_OVERRIDE;

	virtual void setInternalFormat( int pInternalFormat ) Q_DECL_OVERRIDE
	{
		mImage.setInternalFormat( pInternalFormat );
	}

	virtual void setLineSize( int pIndex, int pLineSize ) Q_DECL_OVERRIDE;

	virtual void setLineSizes( const int pLineSize[ PLANE_COUNT ] ) Q_DECL_OVERRIDE;

	virtual QImage image( void ) const Q_DECL_OVERRIDE;

	virtual bool isValid( void ) const Q_DECL_OVERRIDE;

	// SizeInterface interface
public:
	virtual int sizeDimensions() const Q_DECL_OVERRIDE;
	virtual float size(int pDimension) const Q_DECL_OVERRIDE;
	virtual float sizeWidth() const Q_DECL_OVERRIDE;
	virtual float sizeHeight() const Q_DECL_OVERRIDE;
	virtual float sizeDepth() const Q_DECL_OVERRIDE;
	virtual QSizeF toSizeF() const Q_DECL_OVERRIDE;
	virtual QVector3D toVector3D() const Q_DECL_OVERRIDE;

	// VariantInterface interface
public:
	virtual void setVariant(const QVariant &pValue) Q_DECL_OVERRIDE;
	virtual QVariant variant() const Q_DECL_OVERRIDE;
	virtual void setFromBaseVariant(const QVariant &pValue) Q_DECL_OVERRIDE;
	virtual QVariant baseVariant() const Q_DECL_OVERRIDE;

private:
	fugio::Image			mImage;

};

#endif // IMAGEBUFFER_H

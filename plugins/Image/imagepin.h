#ifndef IMAGEBUFFER_H
#define IMAGEBUFFER_H

#include <QObject>
#include <QImage>
#include <vector>

#include <fugio/pin_control_interface.h>

#include <fugio/image/image.h>
#include <fugio/image/uuid.h>
#include <fugio/core/size_interface.h>
#include <fugio/core/variant_helper.h>

#include <fugio/pincontrolbase.h>

#include <fugio/image/image.h>

class ImagePin : public fugio::PinControlBase, public fugio::VariantHelper<fugio::Image>
{
	Q_OBJECT
	Q_INTERFACES( fugio::VariantInterface )

public:
	Q_INVOKABLE explicit ImagePin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~ImagePin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE;

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Image" );
	}

	//-------------------------------------------------------------------------
	// fugio::VariantInterface

	virtual QVariant variantSize( int pIndex = 0, int pOffset = 0 ) const Q_DECL_OVERRIDE
	{
		fugio::Image	SzeImg = variant( pIndex, pOffset ).value<fugio::Image>();

		return( SzeImg.size() );
	}
};

#endif // IMAGEBUFFER_H

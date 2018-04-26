#ifndef VARIANT_INTERFACE_H
#define VARIANT_INTERFACE_H

#include <fugio/global.h>

#include <QUuid>
#include <QVariant>

//#define PID_VARIANT		(QUuid("{ca0829f7-b700-4b70-a941-3518fef7785b}"))

FUGIO_NAMESPACE_BEGIN

class VariantInterface
{
public:
	virtual ~VariantInterface( void ) {}

	virtual void setVariantType( QMetaType::Type pType ) = 0;

	virtual QMetaType::Type variantType( void ) const = 0;

	virtual QUuid variantPinControl( void ) const = 0;

	virtual void setVariantCount( int pCount ) = 0;

	virtual int variantCount( void ) const = 0;

	virtual void setVariantElementCount( int pElementCount ) = 0;

	virtual int variantElementCount( void ) const = 0;

	virtual void variantReserve( int pCount ) = 0;

	virtual void variantSetStride( int pStride ) = 0;

	virtual int variantStride( void ) const = 0;

	virtual int variantArraySize( void ) const = 0;

	virtual QVariant variantSize( int pIndex = 0, int pOffset = 0 ) const = 0;


	virtual void setVariant( const QVariant &pValue ) = 0;

	virtual void setVariant( int pIndex, const QVariant &pValue ) = 0;

	virtual void setVariant( int pIndex, int pOffset, const QVariant &pValue ) = 0;

	virtual QVariant variant( int pIndex = 0, int pOffset = 0 ) const = 0;


	virtual void setFromBaseVariant( const QVariant &pValue ) = 0;

	virtual void setFromBaseVariant( int pIndex, const QVariant &pValue ) = 0;

	virtual void setFromBaseVariant( int pIndex, int pOffset, const QVariant &pValue ) = 0;

	virtual QVariant baseVariant( int pIndex = 0, int pOffset = 0 ) const = 0;


	virtual void variantClear( void ) = 0;

	virtual void variantAppend( const QVariant &pValue ) = 0;


	virtual void *variantArray( void ) = 0;

	virtual const void *variantArray( void ) const = 0;

	virtual void variantSetArray( void *pArray ) = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::VariantInterface, "com.bigfug.fugio.pin.variant/1.0" )

#endif // VARIANT_INTERFACE_H

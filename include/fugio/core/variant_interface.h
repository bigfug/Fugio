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

	virtual void setVariantCount( int pCount ) = 0;

	virtual int variantCount( void ) const = 0;

	virtual void setVariant( const QVariant &pValue ) = 0;

	virtual void setVariant( int pIndex, const QVariant &pValue ) = 0;

	virtual QVariant variant( int pIndex = 0 ) const = 0;

	virtual void setFromBaseVariant( const QVariant &pValue ) = 0;

	virtual void setFromBaseVariant( int pIndex, const QVariant &pValue ) = 0;

	virtual QVariant baseVariant( int pIndex = 0 ) const = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::VariantInterface, "com.bigfug.fugio.pin.variant/1.0" )

#endif // VARIANT_INTERFACE_H

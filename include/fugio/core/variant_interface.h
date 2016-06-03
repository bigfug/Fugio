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

	virtual void setVariant( const QVariant &pValue ) = 0;

	virtual QVariant variant( void ) const = 0;

	virtual void setFromBaseVariant( const QVariant &pValue ) = 0;

	virtual QVariant baseVariant( void ) const = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::VariantInterface, "com.bigfug.fugio.pin.variant/1.0" )

#endif // VARIANT_INTERFACE_H

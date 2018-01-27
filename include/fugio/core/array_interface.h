#ifndef ARRAY_INTERFACE_H
#define ARRAY_INTERFACE_H

#include <QByteArray>
#include <QMetaType>

#include <fugio/global.h>

FUGIO_NAMESPACE_BEGIN

class ArrayInterface
{
public:
	virtual ~ArrayInterface( void ) {}

	virtual void setType( QMetaType::Type pType ) = 0;

	virtual QMetaType::Type type( void ) const = 0;

	virtual void setStride( int pStride ) = 0;

	virtual int stride( void ) const = 0;

	virtual void setCount( int pCount ) = 0;

	virtual int count( void ) const = 0;

	virtual void reserve( int pCount ) = 0;

	virtual void setSize( int pSize ) = 0;

	virtual int size( void ) const = 0;

	virtual void *array( void ) = 0;

	virtual const void *array( void ) const = 0;

	virtual void setArray( void *pArray ) = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::ArrayInterface, "com.bigfug.fugio.array/1.0" )

#endif // ARRAY_INTERFACE_H


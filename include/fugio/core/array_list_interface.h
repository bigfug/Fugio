#ifndef ARRAY_LIST_INTERFACE_H
#define ARRAY_LIST_INTERFACE_H

#include <fugio/core/array_interface.h>

FUGIO_NAMESPACE_BEGIN

class ArrayListInterface
{
public:
	virtual ~ArrayListInterface( void ) {}

	virtual int count( void ) const = 0;

	virtual ArrayInterface *array( int pIndex ) = 0;

	virtual void appendArray( ArrayInterface *pArray ) = 0;

	virtual void setArray( int pIndex, ArrayInterface *pArray ) = 0;

	virtual void clear( void ) = 0;

	virtual void removeAll( ArrayInterface *pArray ) = 0;

	virtual void removeAt( int pIndex ) = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::ArrayListInterface, "com.bigfug.fugio.array-list/1.0" )

#endif // ARRAY_LIST_INTERFACE_H

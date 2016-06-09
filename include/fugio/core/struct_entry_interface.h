#ifndef STRUCT_ENTRY_INTERFACE_H
#define STRUCT_ENTRY_INTERFACE_H

#include <fugio/global.h>

#include <QMetaType>

FUGIO_NAMESPACE_BEGIN

class StructEntryInterface
{
public:
	virtual ~StructEntryInterface( void ) {}

	virtual QMetaType::Type type( void ) const = 0;

	virtual int entrySize( void ) const = 0;

	virtual int entryStride( void ) const = 0;

	virtual int typeSize( void ) const = 0;

	virtual int typeCount( void ) const = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::StructEntryInterface, "com.bigfug.fugio.struct.entry/1.0" )

#endif // STRUCT_ENTRY_INTERFACE_H

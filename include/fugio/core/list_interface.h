#ifndef LIST_INTERFACE_H
#define LIST_INTERFACE_H

#include <QByteArray>
#include <QMetaType>

#include <fugio/global.h>

FUGIO_NAMESPACE_BEGIN

class ListInterface
{
public:
	virtual ~ListInterface( void ) {}

	virtual int listSize( void ) const = 0;

	virtual QUuid listPinControl( void ) const = 0;

	virtual QVariant listIndex( int pIndex ) const = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::ListInterface, "com.bigfug.fugio.list/1.0" )

#endif // LIST_INTERFACE_H

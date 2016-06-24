#ifndef MENU_CONTROL_INTERFACE_H
#define MENU_CONTROL_INTERFACE_H

#include "global.h"

#include <fugio/global_interface.h>

FUGIO_NAMESPACE_BEGIN

class MenuControlInterface
{
public:
	virtual ~MenuControlInterface( void ) {}

	virtual void menuAddEntry( fugio::MenuId pMenuId, QString pEntry, QObject *pObject, const char *pSlot ) = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::MenuControlInterface, "com.bigfug.fugio.menu-control/1.0" )

#endif // MENU_CONTROL_INTERFACE_H

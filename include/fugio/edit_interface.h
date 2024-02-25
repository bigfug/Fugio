#ifndef INTERFACE_EDIT_H
#define INTERFACE_EDIT_H

#include "global.h"

FUGIO_NAMESPACE_BEGIN

class EditInterface
{
public:
	virtual ~EditInterface( void ) {}

public slots:
	virtual void cut( void ) = 0;
	virtual void copy( void ) = 0;
	virtual void paste( void ) = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::EditInterface, "com.bigfug.fugio.Edit/1.0" )

#endif // INTERFACE_EDIT_H


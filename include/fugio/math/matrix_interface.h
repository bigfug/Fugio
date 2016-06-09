#ifndef INTERFACE_MATRIX_H
#define INTERFACE_MATRIX_H

#include <fugio/global.h>

#include <QUuid>
#include <QMatrix4x4>

FUGIO_NAMESPACE_BEGIN

class MatrixInterface
{
public:
	virtual ~MatrixInterface( void ) {}

	virtual void setMatrix( const QMatrix4x4 &pValue ) = 0;

	virtual QMatrix4x4 matrix( void ) const = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::MatrixInterface, "com.bigfug.fugio.pins.matrix/1.0" )

#endif // INTERFACE_MATRIX_H

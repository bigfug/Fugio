#ifndef MATH_INTERFACE_H
#define MATH_INTERFACE_H

#include <QMetaType>
#include <QVariant>
#include <QSharedPointer>
#include <QList>

#include <fugio/global.h>

#define IID_MATH		(QUuid("{9CA93F5F-10CE-405B-85C6-4346711FB189}"))

FUGIO_NAMESPACE_BEGIN

class PinInterface;
class PinVariantIterator;
class VariantInterface;

class MathInterface
{
public:
	typedef enum MathOperator
	{
		OP_ADD,
		OP_SUBTRACT,
		OP_MULTIPLY,
		OP_DIVIDE,
		OP_SUM
	} MathOperator;

	typedef void (*MathOperatorFunction)( const QList<fugio::PinVariantIterator> &pItrLst, fugio::VariantInterface *pOutDst, int pItrMax );

	virtual ~MathInterface( void ) {}

	virtual void registerMetaTypeMathOperator( QMetaType::Type pType, MathOperator pOperator, MathOperatorFunction pFunction ) = 0;
	virtual void deregisterMetaTypeMathOperator( QMetaType::Type pType, MathOperator pOperator ) = 0;
	virtual MathOperatorFunction findMetaTypeMathOperator( QMetaType::Type pType, MathOperator pOperator ) const = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::MathInterface, "com.bigfug.fugio.math/1.0" )

#endif // MATH_INTERFACE_H

#ifndef ARRAYLISTPIN_H
#define ARRAYLISTPIN_H

#include <QObject>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>
#include <fugio/pincontrolbase.h>
#include <fugio/serialise_interface.h>

#include <fugio/core/variant_interface.h>
#include <fugio/core/array_list_interface.h>

class ArrayListPin : public fugio::PinControlBase, public fugio::ArrayListInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::ArrayListInterface )

public:
	Q_INVOKABLE explicit ArrayListPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~ArrayListPin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		return( QString( "Count: %1" ).arg( mArrayList.size() ) );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Array List" );
	}

	//-------------------------------------------------------------------------
	// fugio::ArrayListInterface
public:
	virtual int count() const Q_DECL_OVERRIDE;
	virtual fugio::ArrayInterface *array(int pIndex) Q_DECL_OVERRIDE;
	virtual void appendArray(fugio::ArrayInterface *pArray) Q_DECL_OVERRIDE;
	virtual void setArray( int pIndex, fugio::ArrayInterface *pArray ) Q_DECL_OVERRIDE;
	virtual void clear() Q_DECL_OVERRIDE;
	virtual void removeAll(fugio::ArrayInterface *pArray) Q_DECL_OVERRIDE;
	virtual void removeAt(int pIndex) Q_DECL_OVERRIDE;

private:
	QList<fugio::ArrayInterface *>		mArrayList;
};

#endif // ARRAYLISTPIN_H

#ifndef ARRAYLISTPIN_H
#define ARRAYLISTPIN_H

#include <QObject>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>
#include <fugio/pincontrolbase.h>
#include <fugio/serialise_interface.h>

#include <fugio/core/variant_interface.h>
#include <fugio/core/array_list_interface.h>
#include <fugio/core/list_interface.h>

#include "arraylistentry.h"

class ArrayListPin : public fugio::PinControlBase, public fugio::ArrayListInterface, public fugio::ListInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::ArrayListInterface fugio::ListInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_PIN_URL( "Array_List" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

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
	virtual fugio::ArrayInterface *arrayIndex(int pIndex) Q_DECL_OVERRIDE;
	virtual void setCount( int pIndex ) Q_DECL_OVERRIDE;
	virtual void clear() Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------
	// ListInterface interface
public:
	virtual int listSize() const Q_DECL_OVERRIDE;
	virtual QUuid listPinControl() const Q_DECL_OVERRIDE;
	virtual QVariant listIndex(int pIndex) const Q_DECL_OVERRIDE;
	virtual void listSetIndex(int pIndex, const QVariant &pValue) Q_DECL_OVERRIDE;
	virtual void listSetSize(int pSize) Q_DECL_OVERRIDE;
	virtual void listClear() Q_DECL_OVERRIDE;
	virtual void listAppend(const QVariant &pValue) Q_DECL_OVERRIDE;
	virtual bool listIsEmpty() const Q_DECL_OVERRIDE;

private:
	QVector<ArrayListEntry>		mArrayList;
};

#endif // ARRAYLISTPIN_H

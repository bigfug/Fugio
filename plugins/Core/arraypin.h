#ifndef ARRAYPIN_H
#define ARRAYPIN_H

#include <QObject>

#include <QByteArray>
#include <QSettings>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_interface.h>
#include <fugio/core/array_interface.h>
#include <fugio/core/list_interface.h>

#include <fugio/pincontrolbase.h>

#include <fugio/serialise_interface.h>

class ArrayPin : public fugio::PinControlBase, public fugio::VariantInterface, public fugio::ArrayInterface, public fugio::SerialiseInterface, public fugio::ListInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::VariantInterface fugio::ArrayInterface fugio::SerialiseInterface fugio::ListInterface )

public:
	Q_INVOKABLE explicit ArrayPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~ArrayPin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE;

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Array" );
	}

	virtual void loadSettings( QSettings &pSettings ) Q_DECL_OVERRIDE;

	virtual void saveSettings( QSettings &pSettings ) const Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------
	// fugio::VariantInterface

	virtual void setVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		mArray = pValue.toByteArray();
	}

	virtual QVariant variant( void ) const Q_DECL_OVERRIDE
	{
		return( mArray );
	}

	virtual void setFromBaseVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setVariant( pValue );
	}

	virtual QVariant baseVariant( void ) const Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------
	// InterfaceArray

	virtual void setType( QMetaType::Type pType ) Q_DECL_OVERRIDE
	{
		mType = pType;
	}

	virtual QMetaType::Type type( void ) const Q_DECL_OVERRIDE
	{
		return( mType );
	}

	virtual int stride( void ) const Q_DECL_OVERRIDE
	{
		return( mStride );
	}

	virtual int count( void ) const Q_DECL_OVERRIDE
	{
		return( mCount );
	}

	virtual int size( void ) const Q_DECL_OVERRIDE
	{
		return( mSize );
	}

	virtual void reserve( int pCount ) Q_DECL_OVERRIDE
	{
		mReserve = pCount;
	}

	virtual void setArray( void *pArray ) Q_DECL_OVERRIDE
	{
		mData = pArray;

		if( mData )
		{
			mArray.clear();
		}
	}

	virtual void *array( void ) Q_DECL_OVERRIDE
	{
		if( mData )
		{
			return( mData );
		}

		mArray.resize( mStride * qMax( mCount, mReserve ) );

		return( mArray.data() );
	}

	virtual void setStride( int pStride ) Q_DECL_OVERRIDE
	{
		mStride = pStride;
	}

	virtual void setCount( int pCount ) Q_DECL_OVERRIDE
	{
		mCount = pCount;
	}

	virtual void setSize( int pSize ) Q_DECL_OVERRIDE
	{
		mSize = pSize;
	}

	//-------------------------------------------------------------------------
	// fugio::SerialiseInterface

	virtual void serialise( QDataStream &pDataStream ) Q_DECL_OVERRIDE
	{
		pDataStream << int( mType );
		pDataStream << mStride;
		pDataStream << mCount;
		pDataStream << mSize;

		if( mData )
		{
			pDataStream.writeRawData( static_cast<const char *>( mData ), mSize );
		}
		else
		{
			pDataStream.writeRawData( mArray.data(), mSize );
		}
	}

	virtual void deserialise( QDataStream &pDataStream ) Q_DECL_OVERRIDE
	{
		int			TmpInt;

		pDataStream >> TmpInt;
		pDataStream >> mStride;
		pDataStream >> mCount;
		pDataStream >> mSize;

		mType = QMetaType::Type( TmpInt );

		mArray.resize( mSize );

		pDataStream.readRawData( mArray.data(), mSize );
	}

	//-------------------------------------------------------------------------
	// ListInterface interface

	virtual int listSize() const Q_DECL_OVERRIDE;
	virtual QUuid listPinControl() const Q_DECL_OVERRIDE;
	virtual QVariant listIndex(int pIndex) const Q_DECL_OVERRIDE;
	virtual void listSetIndex( int pIndex, const QVariant &pValue ) Q_DECL_OVERRIDE;
	virtual void listSetSize( int pSize ) Q_DECL_OVERRIDE;

	virtual void listClear() Q_DECL_OVERRIDE
	{
	}

	virtual void listAppend( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		Q_UNUSED( pValue )
	}

	virtual bool listIsEmpty() const Q_DECL_OVERRIDE
	{
		return( mCount == 0 );
	}

private:
	void				*mData;
	QByteArray			 mArray;
	QMetaType::Type		 mType;
	int					 mStride;
	int					 mCount;
	int					 mSize;
	int					 mReserve;
};

#endif // ARRAYPIN_H

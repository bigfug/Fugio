#ifndef ARRAYENTRY_H
#define ARRAYENTRY_H

#include <QObject>
#include <QVector>
#include <QMetaType>

#include <fugio/core/array_interface.h>
#include <fugio/core/list_interface.h>
#include <fugio/serialise_interface.h>

#include "coreplugin.h"

class ArrayListEntry : public fugio::ArrayInterface, public fugio::SerialiseInterface, public fugio::ListInterface
{
public:
	ArrayListEntry( void )
		: mData( nullptr ), mType( QMetaType::UnknownType ), mStride( 0 ),
		  mCount( 0 ), mSize( 0 ), mReserve( 0 )
	{

	}

	ArrayListEntry ( const ArrayListEntry &pSrc )
		: mArray( pSrc.mArray )
	{
		mData    = pSrc.mData;
		mType    = pSrc.mType;
		mStride  = pSrc.mStride;
		mCount   = pSrc.mCount;
		mSize    = pSrc.mSize;
		mReserve = pSrc.mReserve;
	}

	virtual ~ArrayListEntry( void ) {}

	// ArrayInterface interface
public:
	inline virtual void setType( QMetaType::Type pType ) Q_DECL_OVERRIDE
	{
		mType = pType;
	}

	inline virtual QMetaType::Type type( void ) const Q_DECL_OVERRIDE
	{
		return( mType );
	}

	inline virtual int stride( void ) const Q_DECL_OVERRIDE
	{
		return( mStride );
	}

	inline virtual int count( void ) const Q_DECL_OVERRIDE
	{
		return( mCount );
	}

	inline virtual int size( void ) const Q_DECL_OVERRIDE
	{
		return( mSize );
	}

	inline virtual void reserve( int pCount ) Q_DECL_OVERRIDE
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

		mArray.resize( ( ( mStride * qMax( mCount, mReserve ) ) / sizeof( qlonglong ) ) + 1 );

		return( mArray.data() );
	}

	inline virtual void setStride( int pStride ) Q_DECL_OVERRIDE
	{
		mStride = pStride;
	}

	inline virtual void setCount( int pCount ) Q_DECL_OVERRIDE
	{
		mCount = pCount;
	}

	inline virtual void setSize( int pSize ) Q_DECL_OVERRIDE
	{
		mSize = pSize;
	}

	//-------------------------------------------------------------------------
	// fugio::SerialiseInterface

	virtual void serialise( QDataStream &pDataStream ) const Q_DECL_OVERRIDE
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
			pDataStream.writeRawData( (const char *)mArray.data(), mSize );
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

		pDataStream.readRawData( (char *)mArray.data(), mSize );
	}

	//-------------------------------------------------------------------------
	// ListInterface interface

	virtual int listSize() const Q_DECL_OVERRIDE
	{
		return( mCount );
	}

	QUuid listPinControl() const Q_DECL_OVERRIDE
	{
		return( CorePlugin::instance()->app()->findPinForMetaType( mType ) );
	}

	QVariant listIndex( int pIndex ) const Q_DECL_OVERRIDE
	{
		const quint8	*A = (const quint8 *)( mData ? mData : ( !mArray.isEmpty() ? mArray.data() : nullptr ) );

		if( !A )
		{
			return( QVariant() );
		}

		if( pIndex < 0 || pIndex >= mCount )
		{
			return( QVariant() );
		}

		int				L = QMetaType::sizeOf( mType );

		A += L * pIndex;

		QVariant		 V( mType, (const void *)A );

		return( V );
	}

	void listSetIndex( int pIndex, const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		quint8	*A = (quint8 *)( mData ? mData : ( !mArray.isEmpty() ? mArray.data() : nullptr ) );

		if( !A )
		{
			return;
		}

		if( pIndex < 0 || pIndex >= mCount )
		{
			return;
		}

		int				L = QMetaType::sizeOf( mType );

		A += L * pIndex;

		QMetaType::construct( mType, A, pValue.constData() );
	}

	void listSetSize( int pSize ) Q_DECL_OVERRIDE
	{
		if( mData )
		{
			return;
		}

		if( pSize == mCount )
		{
			return;
		}

		mCount = pSize;

		mArray.resize( mStride * qMax( mCount, mReserve ) );
	}

	virtual void listClear() Q_DECL_OVERRIDE
	{
	}

	virtual void listAppend( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		Q_UNUSED( pValue )
	}

	virtual bool listIsEmpty() const Q_DECL_OVERRIDE
	{
		return( !mArray.count() );
	}

	virtual QMetaType::Type listType( void ) const Q_DECL_OVERRIDE
	{
		return( mType );
	}

private:
	void				*mData;
	QVector<qlonglong>	 mArray;		// 64-bit aligned
	QMetaType::Type		 mType;
	int					 mStride;
	int					 mCount;
	int					 mSize;
	int					 mReserve;
};

Q_DECLARE_METATYPE( ArrayListEntry )

#endif // ARRAYENTRY_H

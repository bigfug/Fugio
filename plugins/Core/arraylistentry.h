#ifndef ARRAYENTRY_H
#define ARRAYENTRY_H

#include <QObject>
#include <QVector>
#include <QMetaType>

#include <fugio/core/array_interface.h>

class ArrayListEntry : public QObject, public fugio::ArrayInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::ArrayInterface )

public:
	ArrayListEntry( void )
		: mData( nullptr ), mType( QMetaType::UnknownType ), mStride( 0 ),
		  mCount( 0 ), mSize( 0 ), mReserve( 0 )
	{

	}

	ArrayListEntry ( const ArrayListEntry &pSrc )
		: QObject(), mArray( pSrc.mArray )
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

		mArray.resize( ( ( mStride * qMax( mCount, mReserve ) ) / sizeof( qlonglong ) ) + 1 );

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

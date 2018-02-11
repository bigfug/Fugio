#ifndef VARIANT_HELPER_H
#define VARIANT_HELPER_H

#include <QVector>
#include <QUuid>

#include "variant_interface.h"

FUGIO_NAMESPACE_BEGIN

template <class T> class VariantHelper : public VariantInterface
{
public:
	VariantHelper( QMetaType::Type M, QUuid P )
		: mBaseType( M ), mType( M ), mUuid( P ), mElementCount( 1 ), mArray( nullptr ), mCount( 1 )
	{
		mValues.resize( 1 );

		mStride = variantElementCount() * QMetaType::sizeOf( mType );
	}

	virtual ~VariantHelper( void )
	{

	}

	virtual void setVariantType( QMetaType::Type pType ) Q_DECL_OVERRIDE
	{
		if( mBaseType == QMetaType::UnknownType )
		{
			mType = pType;
		}
	}

	virtual QMetaType::Type variantType() const Q_DECL_OVERRIDE
	{
		return( mType );
	}

	virtual void setVariantCount( int pCount ) Q_DECL_OVERRIDE
	{
		if( mArray )
		{
			mValues.clear();
		}
		else
		{
			mValues.resize( pCount * mElementCount );
		}

		mCount = pCount;
	}

	inline virtual int variantCount() const Q_DECL_OVERRIDE
	{
		return( mCount );
	}

	inline virtual void setVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setVariant( 0, pValue );
	}

	inline virtual void setVariant( int pIndex, const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setVariant( pIndex, 0, pValue );
	}

	virtual void setVariant( int pIndex, int pOffset, const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		if( mArray )
		{
			QMetaType::construct( mType, &static_cast<T *>( mArray )[ variantIndex( pIndex, pOffset ) ], pValue.constData() );
		}
		else
		{
			mValues[ variantIndex( pIndex, pOffset ) ] = pValue.value<T>();
		}
	}

	virtual QVariant variant( int pIndex, int pOffset ) const Q_DECL_OVERRIDE
	{
		if( !mCount )
		{
			return( QVariant() );
		}

		if( mArray )
		{
			return( QVariant::fromValue<T>( static_cast<T *>( mArray )[ variantIndex( pIndex, pOffset ) ] ) );
		}

		return( QVariant::fromValue<T>( mValues[ variantIndex( pIndex, pOffset ) ] ) );
	}

	inline virtual void setFromBaseVariant( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setFromBaseVariant( 0, 0, pValue );
	}

	inline virtual void setFromBaseVariant( int pIndex, const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setFromBaseVariant( pIndex, 0, pValue );
	}

	virtual void setFromBaseVariant( int pIndex, int pOffset, const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		setVariant( pIndex, pOffset, pValue );
	}

	virtual QVariant baseVariant( int pIndex, int pOffset ) const Q_DECL_OVERRIDE
	{
		return( variant( pIndex, pOffset ) );
	}

	virtual QUuid variantPinControl() const Q_DECL_OVERRIDE
	{
		return( mUuid );
	}

	virtual void variantClear() Q_DECL_OVERRIDE
	{
		if( !mArray )
		{
			mValues.clear();

			mCount = 0;
		}
	}

	virtual void variantAppend( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		if( !mArray )
		{
			mValues.append( pValue.value<T>() );

			mCount = mValues.size() / mElementCount;
		}
	}

	virtual void setVariantElementCount( int pElementCount ) Q_DECL_OVERRIDE
	{
		mElementCount = pElementCount;

		if( !mArray )
		{
			mValues.resize( mCount * mElementCount );
		}

		mStride = variantElementCount() * QMetaType::sizeOf( mType );
	}

	inline virtual int variantElementCount() const Q_DECL_OVERRIDE
	{
		return( mElementCount );
	}

	virtual void variantReserve( int pReserve ) Q_DECL_OVERRIDE
	{
		if( !mArray )
		{
			mValues.reserve( pReserve * mElementCount );
		}
	}

	virtual void variantSetStride( int pStride ) Q_DECL_OVERRIDE
	{
		mStride = pStride;
	}

	virtual int variantStride() const Q_DECL_OVERRIDE
	{
		return( mStride );
	}

	virtual void *variantArray() Q_DECL_OVERRIDE
	{
		return( mArray ? mArray : mValues.data() );
	}

	virtual const void *variantArray() const Q_DECL_OVERRIDE
	{
		return( mArray ? mArray : mValues.constData() );
	}

	virtual void variantSetArray( void *pArray ) Q_DECL_OVERRIDE
	{
		mArray = pArray;
	}

	inline int variantIndex( int pIndex, int pOffset ) const
	{
		return( ( pIndex * mElementCount ) + pOffset );
	}

	inline virtual int variantArraySize( void ) const Q_DECL_OVERRIDE
	{
		return( mStride * mCount );
	}

	virtual QVariant variantSize( int pIndex = 0, int pOffset = 0 ) const Q_DECL_OVERRIDE
	{
		Q_UNUSED( pIndex )
		Q_UNUSED( pOffset )

		return( variantCount() );
	}

protected:
	QVector<T>					 mValues;
	const QMetaType::Type		 mBaseType;
	QMetaType::Type				 mType;
	QUuid						 mUuid;
	int							 mElementCount;
	int							 mStride;
	void						*mArray;
	int							 mCount;
};

FUGIO_NAMESPACE_END

#endif // VARIANT_HELPER_H

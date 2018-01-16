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
		: mBaseType( M ), mType( M ), mUuid( P ), mElementCount( 1 )
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
		mValues.resize( pCount );
	}

	virtual int variantCount() const Q_DECL_OVERRIDE
	{
		return( mValues.size() );
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
		mValues[ variantIndex( pIndex, pOffset ) ] = pValue.value<T>();
	}

	virtual QVariant variant( int pIndex, int pOffset ) const Q_DECL_OVERRIDE
	{
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
		mValues.clear();
	}

	virtual void variantAppend( const QVariant &pValue ) Q_DECL_OVERRIDE
	{
		mValues.append( pValue.value<T>() );
	}

	virtual void setVariantElementCount( int pElementCount ) Q_DECL_OVERRIDE
	{
		mElementCount = pElementCount;
	}

	inline virtual int variantElementCount() const Q_DECL_OVERRIDE
	{
		return( mElementCount );
	}

	virtual void variantReserve( int pReserve ) Q_DECL_OVERRIDE
	{
		mValues.reserve( pReserve );
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
		return( mValues.data() );
	}

	virtual const void *variantArray() const Q_DECL_OVERRIDE
	{
		return( mValues.constData() );
	}

	virtual void variantSetArray( void * ) Q_DECL_OVERRIDE
	{

	}

	inline int variantIndex( int pIndex, int pOffset ) const
	{
		return( ( pIndex * mElementCount ) + pOffset );
	}

protected:
	QVector<T>					mValues;
	const QMetaType::Type		mBaseType;
	QMetaType::Type				mType;
	QUuid						mUuid;
	int							mElementCount;
	int							mStride;
};

FUGIO_NAMESPACE_END

#endif // VARIANT_HELPER_H

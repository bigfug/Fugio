#ifndef MODELBASE_H
#define MODELBASE_H

#include <QVariant>

class BaseModel
{
public:
	BaseModel( void );

	virtual ~BaseModel( void ) {}

	virtual int row( void ) = 0;

	virtual BaseModel *parent( void ) = 0;

	virtual int rowCount( void ) const
	{
		return( 0 );
	}

	virtual int columnCount( void ) const
	{
		return( 0 );
	}

	virtual QVariant data( int ) const
	{
		return( QVariant() );
	}
};

#endif // MODELBASE_H

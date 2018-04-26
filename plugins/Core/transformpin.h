#ifndef TRANSFORMPIN_H
#define TRANSFORMPIN_H

#include <QTransform>

#include <fugio/pincontrolbase.h>

#include <fugio/core/variant_helper.h>

#include <fugio/serialise_interface.h>

class TransformPin : public fugio::PinControlBase, public fugio::VariantHelper<QTransform>
{
	Q_OBJECT
	Q_INTERFACES( fugio::VariantInterface )
//	Q_PROPERTY( QTransform mValue READ value WRITE setValue NOTIFY valueChanged )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_PIN_URL( "QTransform" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit TransformPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~TransformPin( void ) {}

	//-------------------------------------------------------------------------
	// Q_PROPERTY

//	Q_INVOKABLE QTransform value( void ) const
//	{
//		return( mValue );
//	}

//	Q_INVOKABLE void setValue( QTransform pValue )
//	{
//		if( pValue != mValue )
//		{
//			mValue = pValue;

//			emit valueChanged( pValue );
//		}
//	}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const Q_DECL_OVERRIDE
	{
		return( QString() );
	}

	virtual QString description( void ) const Q_DECL_OVERRIDE
	{
		return( "Transform" );
	}

	//-------------------------------------------------------------------------
	// fugio::VariantInterface


signals:
	void valueChanged( QTransform pValue );
};

#endif // TRANSFORMPIN_H

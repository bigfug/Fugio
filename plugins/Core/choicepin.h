#ifndef CHOICEPIN_H
#define CHOICEPIN_H

#include <QObject>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>
#include <fugio/pincontrolbase.h>

#include <fugio/choice_interface.h>

class ChoicePin : public fugio::PinControlBase, public fugio::ChoiceInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::ChoiceInterface )
//	Q_PROPERTY( bool value READ value WRITE setValue NOTIFY valueChanged )

public:
	Q_INVOKABLE explicit ChoicePin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~ChoicePin( void ) {}

	//-------------------------------------------------------------------------
	// Q_PROPERTY

//	Q_INVOKABLE bool value( void ) const
//	{
//		return( mValue );
//	}

//	Q_INVOKABLE void setValue( bool pValue )
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
		return( "Choice" );
	}

	//-------------------------------------------------------------------------
	// ChoiceInterface interface

	Q_INVOKABLE virtual QStringList choices() const Q_DECL_OVERRIDE;

public slots:
	virtual void setChoices(QStringList pChoices) Q_DECL_OVERRIDE;

signals:
//	void valueChanged( bool pValue );

private:
	QStringList			mChoices;
};

#endif // CHOICEPIN_H

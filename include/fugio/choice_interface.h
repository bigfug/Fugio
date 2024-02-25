#ifndef CHOICE_INTERFACE_H
#define CHOICE_INTERFACE_H

#include <QStringList>
#include <QObject>

#include <fugio/global.h>

FUGIO_NAMESPACE_BEGIN

//#define IID_CHOICE		(QUuid("{56D4B43E-4B37-4B4C-A95B-82562792EA83}"))

class ChoiceInterface
{
public:
    virtual ~ChoiceInterface( void ) {}

	virtual void setChoices( QStringList pChoices ) = 0;

	virtual QStringList choices( void ) const = 0;

signals:
	virtual void choicesChanged( void ) = 0;
};

FUGIO_NAMESPACE_END

Q_DECLARE_INTERFACE( fugio::ChoiceInterface, "com.bigfug.fugio.choice/1.0" )

#endif // CHOICE_INTERFACE_H

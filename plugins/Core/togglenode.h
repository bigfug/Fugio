#ifndef TOGGLENODE_H
#define TOGGLENODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

FUGIO_NAMESPACE_BEGIN
class VariantInterface;
FUGIO_NAMESPACE_END

class ToggleNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Toggles a boolean value on and off" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Toggle" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE ToggleNode( QSharedPointer<fugio::NodeInterface> pNode);

	virtual ~ToggleNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated(qint64 pTimeStamp);

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInput;
	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::VariantInterface						*mValOutput;
};

#endif // TOGGLENODE_H

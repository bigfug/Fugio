#ifndef INERTIANODE_H
#define INERTIANODE_H

#include <QObject>
#include <QDial>

#include <fugio/nodecontrolbase.h>

FUGIO_NAMESPACE_BEGIN
class VariantInterface;
FUGIO_NAMESPACE_END

class InertiaNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Takes input triggers and generates a forward or backwards inertia speed" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Inertia" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE InertiaNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~InertiaNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

	virtual QWidget *gui();

	virtual bool initialise();

protected slots:
	void onContextFrame( qint64 pTimeStamp );

signals:
	void valueUpdated( int pValue );

protected:
	QSharedPointer<fugio::PinInterface>		 mPinOutput;
	fugio::VariantInterface					*mValOutput;

	QSharedPointer<fugio::PinInterface>		 mPinInput;

	qint64									 mTimeLast;
	double									 mValue;
	double									 mForce;
};

#endif // INERTIANODE_H

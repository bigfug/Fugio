#ifndef PWMNODE_H
#define PWMNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

#include <fugio/choice_interface.h>

#if defined( PIGPIO_SUPPORTED )
#include <pigpiod_if2.h>
#endif

class PWMNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", tr( "Raspberry Pi PWM access" ) )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "PWM" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE PWMNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~PWMNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

	// NodeControlInterface interface
public:
	virtual QStringList availableInputPins() const Q_DECL_OVERRIDE;
	virtual bool mustChooseNamedInputPin() const Q_DECL_OVERRIDE;

private slots:
	void pinAdded( QSharedPointer<fugio::PinInterface> );

	void frameEnd( qint64 pTimestamp );
};

#endif // PWMNODE_H

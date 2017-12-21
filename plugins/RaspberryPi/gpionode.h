#ifndef GPIONODE_H
#define GPIONODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

#include <fugio/choice_interface.h>

#if defined( PIGPIO_SUPPORTED )
#include <pigpiod_if2.h>
#endif

class GPIONode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", tr( "Raspberry Pi GPIO access" ) )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "GPIO" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE GPIONode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~GPIONode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

protected:
//	QSharedPointer<fugio::PinInterface>			 mPinInputFormat;

//	fugio::ChoiceInterface						*mValInputFormat;


	// NodeControlInterface interface
public:
	virtual QStringList availableInputPins() const Q_DECL_OVERRIDE;
	virtual bool mustChooseNamedInputPin() const Q_DECL_OVERRIDE;

private slots:
	void pinAdded( QSharedPointer<fugio::PinInterface> );

	void frameStart( void );
	void frameEnd( qint64 pTimestamp );

	// NodeControlInterface interface
public:
	virtual QList<AvailablePinEntry> availableOutputPins() const Q_DECL_OVERRIDE;
	virtual bool mustChooseNamedOutputPin() const Q_DECL_OVERRIDE;
};

#endif // GPIONODE_H

#ifndef SIGNALNUMBERNODE_H
#define SIGNALNUMBERNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>
#include <fugio/choice_interface.h>

class SignalNumberNode : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Simple signal generator" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Signal_(Number)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE SignalNumberNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~SignalNumberNode( void ) {}

	// NodeControlInterface interface
public:
	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;
	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

private slots:
	void onContextFrame( qint64 pTimeStamp );

protected:
	typedef enum SignalType
	{
		SINE, SQUARE, SAW, TRIANGLE
	} SignalType;

	static QMap<QString,SignalType>			 mSignalTypes;

	QSharedPointer<fugio::PinInterface>		 mPinSignalType;
	QSharedPointer<fugio::PinInterface>		 mPinInputFrequency;
	QSharedPointer<fugio::PinInterface>		 mPinInputVolume;
	QSharedPointer<fugio::PinInterface>		 mPinInputOffset;
	QSharedPointer<fugio::PinInterface>		 mPinInputBias;

	QSharedPointer<fugio::PinInterface>		 mPinOutputValue;
	fugio::VariantInterface					*mValOutputValue;

	fugio::ChoiceInterface					*mPinSignalTypeChoice;

	SignalType								 mSignalType;
	double									 mFrequency;
	double									 mVolume;
	double									 mOffset;
	double									 mPhase;
	double									 mBias;
};


#endif // SIGNALNUMBERNODE_H

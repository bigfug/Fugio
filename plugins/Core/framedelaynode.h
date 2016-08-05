#ifndef FRAMEDELAYNODE_H
#define FRAMEDELAYNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

class FrameDelayNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Takes any number of trigger inputs and fires off a trigger if any of them activate." )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Any_Trigger" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE FrameDelayNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~FrameDelayNode( void ) {}

	// NodeControlInterface interface

	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual bool pinShouldAutoRename(fugio::PinInterface *pPin) const Q_DECL_OVERRIDE;

protected slots:
	void contextFrameStart( void );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInput;

	QSharedPointer<fugio::PinInterface>			 mPinOutput;

	bool										 mSignalOutput;
};

#endif // FRAMEDELAYNODE_H

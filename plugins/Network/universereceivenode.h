#ifndef UNIVERSERECEIVENODE_H
#define UNIVERSERECEIVENODE_H

#include <fugio/nodecontrolbase.h>

class UniverseReceiveNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Receieves pins from the universe" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Universe_Receive" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE UniverseReceiveNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~UniverseReceiveNode( void ) {}

	// NodeControlInterface interface

	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QList<fugio::NodeControlInterface::AvailablePinEntry> availableOutputPins() const Q_DECL_OVERRIDE;

	virtual bool mustChooseNamedOutputPin() const Q_DECL_OVERRIDE
	{
		return( true );
	}

private slots:
	void contextFrame( void );
};

#endif // UNIVERSERECEIVENODE_H

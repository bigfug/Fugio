#ifndef LOCALTIMENODE_H
#define LOCALTIMENODE_H

#include <fugio/nodecontrolbase.h>

#include <fugio/core/variant_interface.h>

class LocalTimeNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Local time" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Local_Time" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit LocalTimeNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~LocalTimeNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

protected slots:
	void contextFrameStart( void );

protected:
	void updateTime( void );

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputTrigger;

	QSharedPointer<fugio::PinInterface>			 mPinOutputTime;
	fugio::VariantInterface						*mValOutputTime;
};

#endif // LOCALTIMENODE_H

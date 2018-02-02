#ifndef UTCTIMENODE_H
#define UTCTIMENODE_H

#include <fugio/nodecontrolbase.h>

#include <fugio/core/variant_interface.h>

class UTCTimeNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "UTC time" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "UTC_Time" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit UTCTimeNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~UTCTimeNode( void ) {}

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

#endif // UTCTIMENODE_H

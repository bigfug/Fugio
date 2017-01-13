#ifndef PLAYHEADNODE_H
#define PLAYHEADNODE_H

#include <QObject>

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_interface.h>

#include <fugio/core/uuid.h>
#include <fugio/nodecontrolbase.h>
#include <fugio/playhead_interface.h>

class PlayheadNode : public fugio::NodeControlBase, public fugio::PlayheadInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PlayheadInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Outputs the current patch time in seconds." )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Playhead" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit PlayheadNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~PlayheadNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual bool initialise( void );

	virtual bool deinitialise( void );

	//-------------------------------------------------------------------------
	// fugio::PlayheadInterface

	virtual void playStart( qreal )
	{
	}

	virtual void playheadMove( qreal pTimeStamp );

	virtual bool playheadPlay( qreal pTimePrev, qreal pTimeCurr );

	virtual void setTimeOffset( qreal ) {}

	virtual qreal latency( void ) const
	{
		return( 0 );
	}

private slots:

private:
	QSharedPointer<fugio::PinInterface>		 mPinTime;
	fugio::VariantInterface					*mValTime;
};

#endif // PLAYHEADNODE_H

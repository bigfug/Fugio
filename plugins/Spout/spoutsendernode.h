#ifndef SPOUTSENDERNODE_H
#define SPOUTSENDERNODE_H

#include "opengl_includes.h"

#include <QObject>

#include <fugio/nodecontrolbase.h>

#if defined( SPOUT_SUPPORTED )
#include <SpoutSender.h>
#endif

class SpoutSenderNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Spout_Sender" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE SpoutSenderNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~SpoutSenderNode( void ) {}

	// InterfaceNodeControl interface
public:
	virtual bool initialise();
	virtual bool deinitialise();

	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinTexture;
	QSharedPointer<fugio::PinInterface>			 mPinName;
	QSharedPointer<fugio::PinInterface>			 mPinInputFlip;

#if defined( SPOUT_SUPPORTED )
	SpoutSender								 mSender;
#endif

	char									 mName[ 256 ];
	quint32									 mWidth;
	quint32									 mHeight;
};

#endif // SPOUTSENDERNODE_H

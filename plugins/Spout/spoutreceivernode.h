#ifndef SPOUTRECEIVERNODE_H
#define SPOUTRECEIVERNODE_H

#include "opengl_includes.h"

#include <QObject>

#include <fugio/nodecontrolbase.h>

#if defined( SPOUT_SUPPORTED )
#include <SpoutReceiver.h>
#endif

class SpoutPin;

class SpoutReceiverNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Spout_Receiver" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE SpoutReceiverNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~SpoutReceiverNode( void ) {}

	// InterfaceNodeControl interface
public:
	virtual bool initialise();
	virtual bool deinitialise();

	virtual void inputsUpdated( qint64 pTimeStamp );

	virtual QWidget *gui();

protected slots:
	void onContextFrame( qint64 pTimeStamp );

	void onReceiverName( const QString &pName );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputName;

	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	SpoutPin								*mOutput;

	QSharedPointer<fugio::PinInterface>			 mPinName;
	fugio::VariantInterface						*mName;

	QSharedPointer<fugio::PinInterface>			 mPinSize;
	fugio::VariantInterface						*mSize;
};

#endif // SPOUTRECEIVERNODE_H

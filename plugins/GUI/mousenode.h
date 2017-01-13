#ifndef MOUSENODE_H
#define MOUSENODE_H

#include <QObject>
#include <QScreen>

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_interface.h>
#include <fugio/gui/input_events_interface.h>

#include <fugio/gui/uuid.h>

#include <fugio/nodecontrolbase.h>

class MouseNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Mouse" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Mouse_(GUI)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit MouseNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~MouseNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface interface

public:
	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

private slots:
	void contextFrameStart( void );

private:
	QSharedPointer<fugio::PinInterface>		 mPinInputEvents;

	QSharedPointer<fugio::PinInterface>		 mPinOutputPosition;
	fugio::VariantInterface					*mValOutputPosition;

	QSharedPointer<fugio::PinInterface>		 mPinOutputLeftButton;
	fugio::VariantInterface					*mValOutputLeftButton;
};

#endif // MOUSENODE_H

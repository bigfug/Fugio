#ifndef BUTTONNODE_H
#define BUTTONNODE_H

#include <QObject>

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_interface.h>

#include <fugio/gui/uuid.h>

#include <fugio/nodecontrolbase.h>

class ButtonNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Provides a simple GUI button that can be clicked on to generate a trigger, boolean, and toggle." )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Button" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit ButtonNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~ButtonNode( void );

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual QWidget *gui( void ) Q_DECL_OVERRIDE;

	virtual bool initialise( void ) Q_DECL_OVERRIDE;

protected slots:
	void onClick( void );
	void onPressed( void );
	void onReleased( void );

	void onContextFrameStart( void );

private:
	QSharedPointer<fugio::PinInterface>			 mPinTrigger;
	fugio::VariantInterface						*mValTrigger;

	QSharedPointer<fugio::PinInterface>			 mPinBoolean;
	fugio::VariantInterface						*mValBoolean;

	QSharedPointer<fugio::PinInterface>			 mPinToggle;
	fugio::VariantInterface						*mValToggle;

	bool										 mToggle;
	bool										 mClicked;
	bool										 mPressed;
	bool										 mReleased;
};

#endif // BUTTONNODE_H

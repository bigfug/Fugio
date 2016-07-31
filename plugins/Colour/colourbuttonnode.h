#ifndef COLOUR_BUTTON_NODE_H
#define COLOUR_BUTTON_NODE_H

#include <QObject>
#include <QColor>

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_interface.h>
#include <fugio/colour/colour_interface.h>

#include <fugio/core/uuid.h>
#include <fugio/nodecontrolbase.h>

class ColourButtonNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "A simple GUI colour selector." )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Colour_(GUI)" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit ColourButtonNode( QSharedPointer<fugio::NodeInterface> pNode );
	
	virtual ~ColourButtonNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual QWidget *gui( void ) Q_DECL_OVERRIDE;

	virtual void loadSettings( QSettings &pSettings ) Q_DECL_OVERRIDE;

	virtual void saveSettings( QSettings &pSettings ) const Q_DECL_OVERRIDE;

signals:
	void valueUpdated( QColor pValue );

protected slots:
	void valueChanged( QColor pValue );

private:
	QSharedPointer<fugio::PinInterface>			 mPinValue;
	fugio::ColourInterface						*mColour;
};

#endif // COLOUR_BUTTON_NODE_H

#ifndef JOIN_COLOUR_HSLA_NODE_H
#define JOIN_COLOUR_HSLA_NODE_H

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

class JoinColourHSLANode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Creates a colour from hue, saturation, lightness, and alpha values." )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Colour_HSLA" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit JoinColourHSLANode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~JoinColourHSLANode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual void loadSettings( QSettings &pSettings ) Q_DECL_OVERRIDE;

	virtual void saveSettings( QSettings &pSettings ) const Q_DECL_OVERRIDE;

private:
	static qreal value( QSharedPointer<fugio::PinInterface> pPin, qreal pVal );

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputHue;
	QSharedPointer<fugio::PinInterface>			 mPinInputSaturation;
	QSharedPointer<fugio::PinInterface>			 mPinInputLightness;
	QSharedPointer<fugio::PinInterface>			 mPinInputAlpha;

	QSharedPointer<fugio::PinInterface>			 mPinOutputColour;
	fugio::ColourInterface						*mValOutputColour;

	QColor										 mColour;
	qreal										 h, s, l, a;
};

#endif // JOIN_COLOUR_HSLA_NODE_H

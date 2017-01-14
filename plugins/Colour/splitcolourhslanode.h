#ifndef SPLITCOLOURHSLANODE_H
#define SPLITCOLOURHSLANODE_H

#include <QObject>

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_interface.h>
#include <fugio/colour/colour_interface.h>

#include <fugio/core/uuid.h>

#include <fugio/nodecontrolbase.h>

class SplitColourHSLANode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Split an input colour into hue, saturation, lightness, and alpha" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Split_HSLA_(Colour)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit SplitColourHSLANode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~SplitColourHSLANode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

private:
	QSharedPointer<fugio::PinInterface>			 mPinInput;

	QSharedPointer<fugio::PinInterface>			 mPinOutH;
	fugio::VariantInterface						*mOutH;

	QSharedPointer<fugio::PinInterface>			 mPinOutS;
	fugio::VariantInterface						*mOutS;

	QSharedPointer<fugio::PinInterface>			 mPinOutL;
	fugio::VariantInterface						*mOutL;

	QSharedPointer<fugio::PinInterface>			 mPinOutA;
	fugio::VariantInterface						*mOutA;
};

#endif // SPLITCOLOURHSLANODE_H

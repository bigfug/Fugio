#ifndef SPLIT_COLOUR_RGBA_NODE_H
#define SPLIT_COLOUR_RGBA_NODE_H

#include <QObject>

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_interface.h>
#include <fugio/colour/colour_interface.h>

#include <fugio/core/uuid.h>

#include <fugio/nodecontrolbase.h>

class SplitColourRGBANode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Split an input colour into red, green, blue, and alpha" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Colour_Split_RGBA" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit SplitColourRGBANode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~SplitColourRGBANode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual bool initialise() Q_DECL_OVERRIDE;

private:
	QSharedPointer<fugio::PinInterface>			 mPinInput;

	QSharedPointer<fugio::PinInterface>			 mPinOutR;
	fugio::VariantInterface						*mOutR;

	QSharedPointer<fugio::PinInterface>			 mPinOutG;
	fugio::VariantInterface						*mOutG;

	QSharedPointer<fugio::PinInterface>			 mPinOutB;
	fugio::VariantInterface						*mOutB;

	QSharedPointer<fugio::PinInterface>			 mPinOutA;
	fugio::VariantInterface						*mOutA;
};

#endif // SPLIT_COLOUR_RGBA_NODE_H

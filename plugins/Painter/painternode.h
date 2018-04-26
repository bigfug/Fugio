#ifndef EXAMPLENODE1_H
#define EXAMPLENODE1_H

#include <fugio/nodecontrolbase.h>
#include <fugio/core/variant_interface.h>

class PainterNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Painter Node" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Painter" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE PainterNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~PainterNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputTrigger;

	QSharedPointer<fugio::PinInterface>			 mPinInputSize;

	QSharedPointer<fugio::PinInterface>			 mPinInputPainter;

	QSharedPointer<fugio::PinInterface>			 mPinOutputImage;
	fugio::VariantInterface						*mValOutputImage;
};


#endif // EXAMPLENODE1_H

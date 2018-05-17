#ifndef EXAMPLENODE1_H
#define EXAMPLENODE1_H

#include <QObject>

#include <fugio/nodecontrolbase.h>
#include <fugio/core/variant_interface.h>

#include <quirc.h>

class QRScannerNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "QR Scanner" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "QR_Scanner" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE QRScannerNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~QRScannerNode( void ) {}

	// NodeControlInterface interface

	// NodeControlInterface interface
public:
	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputImage;

//	QSharedPointer<fugio::PinInterface>			 mPinOutput;

//	fugio::VariantInterface						*mOutput;

	struct quirc								*mQR;
	int											 mW, mH;
};


#endif // EXAMPLENODE1_H

#ifndef ROTATIONFROMVECTORSNODE_H
#define ROTATIONFROMVECTORSNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

#include <fugio/core/variant_interface.h>

class RotationFromVectorsNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Adds all the input numbers together." )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Add_Number" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE RotationFromVectorsNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~RotationFromVectorsNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInput1;
	QSharedPointer<fugio::PinInterface>			 mPinInput2;

	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::VariantInterface						*mValOutput;
};

#endif // ROTATIONFROMVECTORSNODE_H

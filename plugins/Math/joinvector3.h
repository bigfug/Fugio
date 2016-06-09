#ifndef JOINVECTOR3_H
#define JOINVECTOR3_H

#include <QObject>

#include <fugio/nodecontrolbase.h>
#include <fugio/core/variant_interface.h>

class JoinVector3Node : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Makes a Vector3 from raw values." )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Join_Vector3" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE JoinVector3Node( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~JoinVector3Node( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinX;
	QSharedPointer<fugio::PinInterface>			 mPinY;
	QSharedPointer<fugio::PinInterface>			 mPinZ;

	QSharedPointer<fugio::PinInterface>			 mPinVector3;

	fugio::VariantInterface						*mVector3;
};

#endif // JOINVECTOR3_H

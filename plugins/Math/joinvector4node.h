#ifndef JOINVECTOR4NODE_H
#define JOINVECTOR4NODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>
#include <fugio/core/variant_interface.h>

class JoinVector4Node : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Makes a Vector4 from raw values." )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Join_Vector3" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE JoinVector4Node( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~JoinVector4Node( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinX;
	QSharedPointer<fugio::PinInterface>			 mPinY;
	QSharedPointer<fugio::PinInterface>			 mPinZ;
	QSharedPointer<fugio::PinInterface>			 mPinW;

	QSharedPointer<fugio::PinInterface>			 mPinVector4;

	fugio::VariantInterface						*mVector4;
};
#endif // JOINVECTOR4NODE_H

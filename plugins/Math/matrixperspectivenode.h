#ifndef MATRIXPERSPECTIVENODE_H
#define MATRIXPERSPECTIVENODE_H

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>

#include <fugio/core/variant_interface.h>

#include <fugio/nodecontrolbase.h>

class MatrixPerspectiveNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Create a perspective projection Matrix4" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Perspective_(Matrix4)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit MatrixPerspectiveNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~MatrixPerspectiveNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp );

private:
	QSharedPointer<fugio::PinInterface>			 mPinAngle;
	QSharedPointer<fugio::PinInterface>			 mPinAspectRatio;
	QSharedPointer<fugio::PinInterface>			 mPinNearPlane;
	QSharedPointer<fugio::PinInterface>			 mPinFarPlane;

	QSharedPointer<fugio::PinInterface>			 mPinOutputMatrix;
	fugio::VariantInterface						*mOutputMatrix;
};

#endif // MATRIXPERSPECTIVENODE_H

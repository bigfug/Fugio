#ifndef MATRIXLOOKATNODE_H
#define MATRIXLOOKATNODE_H

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>

#include <fugio/core/variant_interface.h>

#include <fugio/nodecontrolbase.h>

class MatrixLookAtNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Create a 'Look At' Matrix4" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Look_At_(Matrix4)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit MatrixLookAtNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~MatrixLookAtNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputMatrix;
	QSharedPointer<fugio::PinInterface>			 mPinInputPosition;
	QSharedPointer<fugio::PinInterface>			 mPinInputCenter;
	QSharedPointer<fugio::PinInterface>			 mPinInputUp;

	QSharedPointer<fugio::PinInterface>			 mPinOutputMatrix;
	fugio::VariantInterface						*mOutputMatrix;
};

#endif // MATRIXLOOKATNODE_H

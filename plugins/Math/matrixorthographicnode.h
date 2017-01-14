#ifndef MATRIXORTHOGRAPHICNODE_H
#define MATRIXORTHOGRAPHICNODE_H

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>

#include <fugio/core/variant_interface.h>

#include <fugio/nodecontrolbase.h>

class MatrixOrthographicNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Create an orthographic projection Matrix4" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Orthographic_(Matrix4)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit MatrixOrthographicNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~MatrixOrthographicNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp );

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputLeft;
	QSharedPointer<fugio::PinInterface>			 mPinInputRight;
	QSharedPointer<fugio::PinInterface>			 mPinInputBottom;
	QSharedPointer<fugio::PinInterface>			 mPinInputTop;
	QSharedPointer<fugio::PinInterface>			 mPinInputNear;
	QSharedPointer<fugio::PinInterface>			 mPinInputFar;

	QSharedPointer<fugio::PinInterface>			 mPinOutputMatrix;
	fugio::VariantInterface						*mOutputMatrix;
};

#endif // MATRIXORTHOGRAPHICNODE_H

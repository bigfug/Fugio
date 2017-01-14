#ifndef MATRIXINVERSENODE_H
#define MATRIXINVERSENODE_H

#include <fugio/nodecontrolbase.h>

class MatrixInverseNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Creates the inverse of a Matrix4" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Inverse_(Matrix4)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit MatrixInverseNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~MatrixInverseNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp );

private:
	QSharedPointer<fugio::PinInterface>			 mPinMatrixInput;

	QSharedPointer<fugio::PinInterface>			 mPinMatrixOutput;
	fugio::VariantInterface						*mValMatrixOutput;
};

#endif // MATRIXINVERSENODE_H

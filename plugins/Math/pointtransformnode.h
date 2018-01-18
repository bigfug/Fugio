#ifndef POINTTRANSFORMNODE_H
#define POINTTRANSFORMNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

#include <fugio/core/variant_interface.h>

class PointTransformNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Point_Transform" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE PointTransformNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~PointTransformNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputMatrix;
	QSharedPointer<fugio::PinInterface>			 mPinInputPoints;

	QSharedPointer<fugio::PinInterface>			 mPinOutputPoints;
	fugio::VariantInterface						*mValOutputPoints;
};

#endif // POINTTRANSFORMNODE_H

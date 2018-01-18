#ifndef POLYGONNODE_H
#define POLYGONNODE_H

#include <fugio/nodecontrolbase.h>

#include <fugio/core/variant_interface.h>

class PolygonNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Polygon_(Geometry)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit PolygonNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~PolygonNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputSides;
	QSharedPointer<fugio::PinInterface>			 mPinInputRadius;

	QSharedPointer<fugio::PinInterface>			 mPinOutputPoints;
	fugio::VariantInterface						*mValOutputPoints;

	float										 mRadius;
};

#endif // POLYGONNODE_H

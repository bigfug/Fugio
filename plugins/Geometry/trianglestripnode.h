#ifndef TRIANGLESTRIPNODE_H
#define TRIANGLESTRIPNODE_H

#include <fugio/nodecontrolbase.h>

#include <fugio/core/variant_interface.h>

class TriangleStripNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Triangle_Strip_(Geometry)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit TriangleStripNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~TriangleStripNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputCount;

	QSharedPointer<fugio::PinInterface>			 mPinOutputPoints;
	fugio::VariantInterface						*mValOutputPoints;

	QSharedPointer<fugio::PinInterface>			 mPinOutputUV;
	fugio::VariantInterface						*mValOutputUV;
};

#endif // TRIANGLESTRIPNODE_H

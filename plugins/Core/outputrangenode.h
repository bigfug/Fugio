#ifndef OUTPUTRANGENODE_H
#define OUTPUTRANGENODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

FUGIO_NAMESPACE_BEGIN
class VariantInterface;
FUGIO_NAMESPACE_END

class OutputRangeNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Output_Range" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE OutputRangeNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~OutputRangeNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual bool canAcceptPin( fugio::PinInterface *pPin ) const Q_DECL_OVERRIDE;

	virtual bool pinShouldAutoRename(fugio::PinInterface *pPin) const Q_DECL_OVERRIDE;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputSource;
	QSharedPointer<fugio::PinInterface>			 mPinInputRange;
	QSharedPointer<fugio::PinInterface>			 mPinInputIndex;

	int											 mPinCnt;
	int											 mPinIdx;
};

#endif // OUTPUTRANGENODE_H

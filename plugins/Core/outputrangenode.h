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
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Output_Range" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE OutputRangeNode( QSharedPointer<fugio::NodeInterface> pNode);

	virtual ~OutputRangeNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated(qint64 pTimeStamp);

	virtual bool canAcceptPin( fugio::PinInterface *pPin ) const;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInput;

	QSharedPointer<fugio::PinInterface>			 mPinNumber;
	fugio::VariantInterface						*mValNumber;

	int										 mPinCnt;
	int										 mPinIdx;
};

#endif // OUTPUTRANGENODE_H

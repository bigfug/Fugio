#ifndef MINMAXNODE_H
#define MINMAXNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

#include <fugio/core/variant_interface.h>

class MinMaxNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Min_Max_(Number)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE MinMaxNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~MinMaxNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputValue;
	QSharedPointer<fugio::PinInterface>			 mPinInputReset;

	QSharedPointer<fugio::PinInterface>			 mPinOutputOutput;
	fugio::VariantInterface						*mValOutputOutput;

	QSharedPointer<fugio::PinInterface>			 mPinOutputMin;
	fugio::VariantInterface						*mValOutputMin;

	QSharedPointer<fugio::PinInterface>			 mPinOutputMax;
	fugio::VariantInterface						*mValOutputMax;

	float										 mMin, mMax;
};

#endif // MINMAXNODE_H

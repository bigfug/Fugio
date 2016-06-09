#ifndef TYPESIZENODE_H
#define TYPESIZENODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>
#include <fugio/choice_interface.h>

class TypeSizeNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Type_Size" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE TypeSizeNode( QSharedPointer<fugio::NodeInterface> pNode);

	virtual ~TypeSizeNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputType;
	fugio::ChoiceInterface						*mValInputType;

	QSharedPointer<fugio::PinInterface>			 mPinOutputSize;
	fugio::VariantInterface						*mValOutputSize;
};

#endif // TYPESIZENODE_H

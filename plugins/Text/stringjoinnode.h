#ifndef STRINGJOINNODE_H
#define STRINGJOINNODE_H

#include <QObject>

#include <fugio/core/uuid.h>
#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/core/variant_interface.h>
#include <fugio/nodecontrolbase.h>

class StringJoinNode : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Joins two or more strings together" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Join_String" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit StringJoinNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~StringJoinNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QList<QUuid> pinAddTypesInput() const Q_DECL_OVERRIDE;
	virtual bool canAcceptPin(fugio::PinInterface *pPin) const Q_DECL_OVERRIDE;
	virtual bool pinShouldAutoRename(fugio::PinInterface *pPin) const Q_DECL_OVERRIDE;

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputJoinChar;

	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::VariantInterface						*mValOutput;
};

#endif // STRINGJOINNODE_H

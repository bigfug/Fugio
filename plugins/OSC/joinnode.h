#ifndef JOINNODE_H
#define JOINNODE_H

#include <fugio/nodecontrolbase.h>
#include <fugio/osc/uuid.h>
#include <fugio/osc/join_interface.h>

class JoinNode : public fugio::NodeControlBase, public fugio::osc::JoinInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::osc::JoinInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/OSC_Join" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit JoinNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~JoinNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QList<QUuid> pinAddTypesInput() const Q_DECL_OVERRIDE;

	virtual bool canAcceptPin( fugio::PinInterface *pPin ) const Q_DECL_OVERRIDE;

	virtual QUuid pinAddControlUuid(fugio::PinInterface *pPin) const Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------
	// JoinInterface interface

	virtual void oscJoin( QStringList pPath, const QVariant &pValue ) Q_DECL_OVERRIDE;

	virtual void oscPath( QStringList &pPath ) const Q_DECL_OVERRIDE;

private:
	QSharedPointer<fugio::PinInterface>			 mPinOutput;
	fugio::osc::JoinInterface					*mValOutput;
};

#endif // JOINNODE_H

#ifndef PROJECTORNODE_H
#define PROJECTORNODE_H

#include <fugio/nodecontrolbase.h>

#include <fugio/choice_interface.h>
#include <fugio/core/variant_interface.h>

class ProjectorNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Projector" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit ProjectorNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~ProjectorNode( void ) Q_DECL_OVERRIDE {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

private slots:
	void clientListUpdated( void );

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputProjector;
	fugio::ChoiceInterface						*mValInputProjector;

	QSharedPointer<fugio::PinInterface>			 mPinInputPower;

	QSharedPointer<fugio::PinInterface>			 mPinOutputData;
	fugio::VariantInterface						*mValOutputData;
};

#endif // PROJECTORNODE_H

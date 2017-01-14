#ifndef RANDOMNUMBERNODE_H
#define RANDOMNUMBERNODE_H

#include <QObject>

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_interface.h>

#include <fugio/core/uuid.h>

#include <fugio/nodecontrolbase.h>

#include <random>

class RandomNumberNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Generate one or more random numbers when triggered." )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Random_(Number)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit RandomNumberNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~RandomNumberNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp );

	virtual QList<QUuid> pinAddTypesOutput( void ) const;

	virtual bool canAcceptPin( fugio::PinInterface *pPin ) const;

private:
	std::default_random_engine				 mGenerator;
	std::uniform_real_distribution<qreal>	 mDistribution;
	QSharedPointer<fugio::PinInterface>			 mPinTrigger;
};

#endif // RANDOMNUMBERNODE_H

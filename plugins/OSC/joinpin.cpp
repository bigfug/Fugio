#include "joinpin.h"

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>

JoinPin::JoinPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin )
{

}

QString JoinPin::toString() const
{
	QStringList		StrLst;

	oscPath( StrLst );

	return( QString( "/%1" ).arg( StrLst.join( '/' ) ) );
}

void JoinPin::oscJoin( QStringList pPath, const QVariant &pValue )
{
	pPath.prepend( mPin->name() );

	if( mPin->direction() == PIN_OUTPUT )
	{
		QSharedPointer<fugio::NodeInterface>	 NI = mPin->connectedNode();

		fugio::osc::JoinInterface				*JI = qobject_cast<fugio::osc::JoinInterface *>( NI && NI->hasControl() ? NI->control()->qobject() : nullptr );

		if( JI )
		{
			JI->oscJoin( pPath, pValue );
		}
	}
}

void JoinPin::oscPath( QStringList &pPath ) const
{
	pPath.prepend( mPin->name() );

	QSharedPointer<fugio::NodeInterface>	 NI;

	if( mPin->direction() == PIN_OUTPUT )
	{
		NI = mPin->connectedNode();
	}
	else
	{
		NI = mPin->node();
	}

	fugio::osc::JoinInterface				*JI = qobject_cast<fugio::osc::JoinInterface *>( NI && NI->hasControl() ? NI->control()->qobject() : nullptr );

	if( JI )
	{
		JI->oscPath( pPath );
	}
}

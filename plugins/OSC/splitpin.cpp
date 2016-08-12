#include "splitpin.h"

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>

SplitPin::SplitPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin )
{

}

QString SplitPin::toString() const
{
	QStringList		StrLst;

	oscPath( StrLst );

	return( QString( "/%1" ).arg( StrLst.join( '/' ) ) );
}

void SplitPin::oscSplit( QStringList pPath, const QVariant &pValue )
{
	if( mPin->direction() == PIN_OUTPUT )
	{
		fugio::NodeInterface			*NI = mPin->connectedNode();

		fugio::osc::SplitInterface		*SI = qobject_cast<fugio::osc::SplitInterface *>( NI && NI->hasControl() ? NI->control()->qobject() : nullptr );

		if( SI )
		{
			SI->oscSplit( pPath, pValue );
		}
	}
}

void SplitPin::oscPath( QStringList &pPath ) const
{
	QStringList	PinNam = mPin->name().split( '/', QString::SkipEmptyParts );

	std::reverse( PinNam.begin(), PinNam.end() );

	for( QStringList::iterator it = PinNam.begin() ; it != PinNam.end() ; it++ )
	{
		pPath.prepend( *it );
	}

	fugio::NodeInterface	*NI;

	if( mPin->direction() == PIN_INPUT )
	{
		NI = mPin->connectedNode();
	}
	else
	{
		NI = mPin->node();
	}

	fugio::osc::SplitInterface					*SI = qobject_cast<fugio::osc::SplitInterface *>( NI && NI->hasControl() ? NI->control()->qobject() : nullptr );

	if( SI )
	{
		SI->oscPath( pPath );
	}
}


QStringList SplitPin::oscNamespace()
{
	fugio::NodeInterface	*NI;

	if( mPin->direction() == PIN_INPUT )
	{
		NI = mPin->connectedNode();
	}
	else
	{
		NI = mPin->node();
	}

	fugio::osc::NamespaceInterface		*SI = qobject_cast<fugio::osc::NamespaceInterface *>( NI && NI->hasControl() ? NI->control()->qobject() : nullptr );

	if( SI )
	{
		return( SI->oscNamespace() );
	}

	return( QStringList() );
}

QList<fugio::NodeControlInterface::AvailablePinEntry> SplitPin::oscPins( const QStringList &pCurDir ) const
{
	fugio::NodeInterface	*NI;

	if( mPin->direction() == PIN_INPUT )
	{
		NI = mPin->connectedNode();
	}
	else
	{
		NI = mPin->node();
	}

	fugio::osc::NamespaceInterface		*SI = qobject_cast<fugio::osc::NamespaceInterface *>( NI && NI->hasControl() ? NI->control()->qobject() : nullptr );

	if( SI )
	{
		return( SI->oscPins( pCurDir ) );
	}

	return( QList<fugio::NodeControlInterface::AvailablePinEntry>() );
}


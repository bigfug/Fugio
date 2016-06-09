#include "splitlistnode.h"

#include <fugio/core/uuid.h>

#include <fugio/context_interface.h>

SplitListNode::SplitListNode( QSharedPointer<fugio::NodeInterface> pNode )
	: NodeControlBase( pNode )
{
}

void SplitListNode::inputsUpdated( qint64 pTimeStamp )
{
	NodeControlBase::inputsUpdated( pTimeStamp );
}

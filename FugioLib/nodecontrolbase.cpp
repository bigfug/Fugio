
#include <fugio/nodecontrolbase.h>

fugio::NodeControlBase::NodeControlBase( QSharedPointer<fugio::NodeInterface> pNode )
	: mNode( pNode ), mInitialisedCalled( false ), mDeinitialisedCalled( false ), mIsPairing( false )
{
}

bool fugio::NodeControlBase::initialise()
{
	PairedPinsHelperInterface *PPHI = qobject_cast<PairedPinsHelperInterface *>(this);

	if (PPHI) {
		pairedPinConnect();
	}

	mInitialisedCalled = true;

	return (true);
}

bool fugio::NodeControlBase::deinitialise()
{
	PairedPinsHelperInterface *PPHI = qobject_cast<PairedPinsHelperInterface *>(this);

	if (PPHI) {
		pairedPinDisconnect();
	}

	mDeinitialisedCalled = true;

	return (true);
}

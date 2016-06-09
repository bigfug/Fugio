#ifndef PERFORMANCE_H
#define PERFORMANCE_H

#include <QDateTime>
#include <QSharedPointer>
#include <QElapsedTimer>

#include "global.h"
#include "node_interface.h"
#include "context_interface.h"

FUGIO_NAMESPACE_BEGIN

class Performance
{
public:
	Performance( QSharedPointer<fugio::NodeInterface> pNode, const QString &pName, qint64 pTimeStamp )
		: mNode( pNode ), mName( pName ), mTimeStamp( pTimeStamp )
	{
		mTimer.start();
	}

	virtual ~Performance( void )
	{
		mNode->context()->performance( mNode, mName, mTimeStamp, mTimeStamp + mTimer.elapsed() );
	}

private:
	QSharedPointer<fugio::NodeInterface>		 mNode;
	const QString						 mName;
	QElapsedTimer						 mTimer;
	qint64								 mTimeStamp;
};

FUGIO_NAMESPACE_END

#endif // PERFORMANCE_H


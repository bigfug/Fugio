#include "testglobal.h"

#include <fugio/global.h>
#include <fugio/global_interface.h>
#include <fugio/context_interface.h>

void TestGlobal::fugioGlobal()
{
	QVERIFY( fugio::fugio() );
}

void TestGlobal::newContext()
{
	QSharedPointer<fugio::ContextInterface>	C = fugio::fugio()->newContext();

	QVERIFY( C );

	fugio::fugio()->delContext( C );

	fugio::fugio()->clear();
}

QTEST_MAIN( TestGlobal )

#include "testglobal.moc"

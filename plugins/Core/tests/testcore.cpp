#include "testcore.h"

#include <fugio/core/uuid.h>

TestCore::TestCore()
{
	fugio::GlobalInterface	*G = fugio::fugio();

	QVERIFY( G );

	G->enablePlugin( "fugio-core" );
}

void TestCore::newContext()
{
	QSharedPointer<fugio::ContextInterface>	C;

	C = fugio::fugio()->newContext();

	QVERIFY( C );
}

void TestCore::createNode()
{
	QSharedPointer<fugio::ContextInterface>	C = fugio::fugio()->newContext();

	QVERIFY( C );

	QSharedPointer<fugio::NodeInterface>	NI = C->createNode( "Test", QUuid::createUuid(), NID_ANY_TRIGGER );

	QVERIFY( NI );
	QVERIFY( NI->hasControl() );
	QVERIFY( NI->controlUuid() == NID_ANY_TRIGGER );

	fugio::fugio()->clear();
}

QTEST_GUILESS_MAIN( TestCore )

#include "testcore.moc"

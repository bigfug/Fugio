#include "testcore.h"

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

//		if( C->load( mPatchFileName ) )
//		{
//			G->start();

//			R = pApp.exec();

//			G->stop();
//		}

	fugio::fugio()->clear();
}

QTEST_MAIN( TestCore )

#include "testcore.moc"

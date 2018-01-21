#include "testglobal.h"

void TestGlobal::toUpper()
{
    QString str = "Hello";
    QCOMPARE(str.toUpper(), QString("HELLO"));
}

QTEST_MAIN( TestGlobal )

#include "testglobal.moc"

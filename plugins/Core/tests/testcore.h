#ifndef TESTCORE_H
#define TESTCORE_H

#include <fugio/test_helper.h>

class TestCore : public TestHelper
{
	Q_OBJECT

public:
	TestCore( void );

private slots:
	void newContext();
};

#endif // TESTCORE_H

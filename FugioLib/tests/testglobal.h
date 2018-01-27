#ifndef TESTGLOBAL_H
#define TESTGLOBAL_H

#include <QtTest/QtTest>

class TestGlobal : public QObject
{
	Q_OBJECT

private slots:
	void fugioGlobal();

	void newContext();
};

#endif // TESTGLOBAL_H

#ifndef TESTGLOBAL_H
#define TESTGLOBAL_H

#include <QtTest/QtTest>

class TestGlobal : public QObject
{
	Q_OBJECT

private slots:
	void toUpper();
};

#endif // TESTGLOBAL_H

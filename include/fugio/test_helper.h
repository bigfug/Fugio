#ifndef TEST_HELPER_H
#define TEST_HELPER_H

#include <QtTest/QtTest>

#include <QObject>
#include <QDir>

#include <fugio/global.h>
#include <fugio/global_interface.h>

#include "globalprivate.h"
#include "contextprivate.h"

class TestHelper : public QObject
{
	Q_OBJECT

private slots:
	void initTestCase()
	{
		G = qobject_cast<GlobalPrivate *>( fugio::fugio()->qobject() );

		QVERIFY( G );

		QDir	PluginsDir = QDir( qApp->applicationDirPath() );

		PluginsDir.cdUp();
		PluginsDir.cdUp();

		G->loadPlugins( PluginsDir );

		G->initialisePlugins();

		G->start();
	}

	void cleanupTestCase()
	{
		G->stop();

		G->clear();

		G->unloadPlugins();

		delete G;
	}

private:
	GlobalPrivate			*G;
};

#endif // TEST_HELPER_H

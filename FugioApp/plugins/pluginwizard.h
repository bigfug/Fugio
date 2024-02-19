#ifndef PLUGINWIZARD_H
#define PLUGINWIZARD_H

#include <QObject>

#include <pluginmanager.h>

class PluginWizard : public QObject
{
	Q_OBJECT
public:
	explicit PluginWizard(QObject *parent = nullptr);

	static bool checkPluginWizard( void );
signals:

};

#endif // PLUGINWIZARD_H

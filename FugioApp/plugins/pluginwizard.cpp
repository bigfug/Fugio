#include "pluginwizard.h"

#include "app.h"

PluginWizard::PluginWizard(QObject *parent)
: QObject{parent}
{

}

bool PluginWizard::checkPluginWizard()
{
	SettingsHelper	Helper;
	PluginConfig	PC( Helper );

	return( PC.installedPlugins().isEmpty() );
}


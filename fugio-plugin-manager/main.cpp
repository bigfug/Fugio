#include <QCoreApplication>
#include <QLocale>
#include <QTranslator>

#include "globalprivate.h"
#include "pluginmanager.h"

int main(int argc, char *argv[])
{
//    QCoreApplication a(argc, argv);

//    QTranslator translator;
//    const QStringList uiLanguages = QLocale::system().uiLanguages();
//    for (const QString &locale : uiLanguages) {
//        const QString baseName = "fugio-plugin-manager_" + QLocale(locale).name();
//        if (translator.load(":/i18n/" + baseName)) {
//            a.installTranslator(&translator);
//            break;
//        }
//    }

//    return a.exec();

    GlobalPrivate   *G = qobject_cast<GlobalPrivate *>( fugio::fugio()->qobject() );

    PluginManager   &PM = G->pluginManager();

    return( 0 );
}

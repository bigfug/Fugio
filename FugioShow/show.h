#ifndef SHOW_H
#define SHOW_H

#include <QObject>

#include <QApplication>
#include <QSharedPointer>
//#include <QWidget>
#include <QTimer>
//#include <QWindow>
#include <QCommandLineParser>

#include <fugio/global_interface.h>
//#include <fugio/interface_output.h>
#include <fugio/node_control_interface.h>

#include "globalprivate.h"
#include "contextprivate.h"
#include <fugio/global.h>

//#include <fugio/fugio-opengl.h>

FUGIO_NAMESPACE_BEGIN

class Show : public QObject
{
    Q_OBJECT

public:
    explicit Show( void );

    virtual ~Show( void );

    int exec (QApplication &pApp );

public slots:
	//void onWindowCreate( QWindow *pWindow );

private:
    bool parseCommandLine( GlobalPrivate *G, QCoreApplication &pApp );

private:
    QString		mPatchFileName;
};

FUGIO_NAMESPACE_END

#endif // SHOW_H

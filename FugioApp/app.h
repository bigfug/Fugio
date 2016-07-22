#ifndef APP_H
#define APP_H

#include <QApplication>

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QList>
#include <QPair>
#include <QMutex>
#include <QUndoGroup>

#include "globalprivate.h"

#include <fugio/global_interface.h>

FUGIO_NAMESPACE_BEGIN
class PinInterface;
FUGIO_NAMESPACE_END

class MainWindow;
class QOpenGLContext;

class App : public QApplication
{
	Q_OBJECT

public:
	explicit App( int &argc, char **argv );
	
	virtual ~App( void );

	void setMainWindow( MainWindow *pMainWindow );

	MainWindow *mainWindow( void );

	inline GlobalPrivate &global( void )
	{
		return( *mGlobal );
	}

	inline QUndoGroup &undoGroup( void )
	{
		return( mUndoGroup );
	}

	static void incrementStatistic( const QString &pName );
	static void recordData( const QString &pName, const QString &pValue );

private:
	MainWindow				*mMainWindow;
	GlobalPrivate			*mGlobal;
	QUndoGroup				 mUndoGroup;
};

#define gApp (static_cast<App *>(QCoreApplication::instance()))

#endif // APP_H

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
		return( *qobject_cast<GlobalPrivate *>( fugio::fugio()->qobject() ) );
	}

	inline QUndoGroup &undoGroup( void )
	{
		return( mUndoGroup );
	}

	void incrementStatistic( const QString &pName );

	void recordData( const QString &pName, const QString &pValue );

	QString userSnippetsDirectory( void ) const;

	void setUserSnippetsDirectory( const QString &pDirectory );

	typedef struct LogMessage
	{
		QtMsgType type;
		QString msg;
	} LogMessage;

	static void log_file( const QString &pLogDat )
	{
		QFile		FH( mLogFileName );

		if( FH.open( QFile::Append ) )
		{
			QTextStream	TS( &FH );

			TS << pLogDat << "\n";

			FH.close();
		}
	}

	static void logger_static( QtMsgType type, const QMessageLogContext &context, const QString &msg )
	{
		Q_UNUSED( context )

		LogMessage		LogMsg;

		LogMsg.type = type;
		LogMsg.msg = msg;

		mLogMessages << LogMsg;

		QByteArray localMsg = msg.toLocal8Bit();

		QString		LogDat;

		LogDat.append( QDateTime::currentDateTime().toString( Qt::ISODate ) );
		LogDat.append( ": " );
		LogDat.append( localMsg );

		switch (type)
		{
			case QtDebugMsg:
				fprintf(stderr, "DBUG: %s\n", localMsg.constData() );
				break;
	#if ( QT_VERSION >= QT_VERSION_CHECK( 5, 5, 0 ) )
			case QtInfoMsg:
				fprintf(stderr, "INFO: %s\n", localMsg.constData() );
				break;
	#endif
			case QtWarningMsg:
				fprintf(stderr, "WARN: %s\n", localMsg.constData() );
				break;
			case QtCriticalMsg:
				fprintf(stderr, "CRIT: %s\n", localMsg.constData() );
				break;
			case QtFatalMsg:
				fprintf(stderr, "FATL: %s\n", localMsg.constData() );
				break;
		}

		fflush( stderr );

		log_file( LogDat );

		if( type == QtFatalMsg )
		{
			abort();
		}
	}

	static void setLogFileName( const QString &pFileName )
	{
		mLogFileName = pFileName;
	}

	static QList<LogMessage> logMessages( void )
	{
		return( mLogMessages );
	}

signals:
	void userSnippetsDirectoryChanged( const QString &pDirectory );

private:
	static QList<LogMessage>		 mLogMessages;
	static QString					 mLogFileName;

	MainWindow				*mMainWindow;
	QUndoGroup				 mUndoGroup;
	QNetworkAccessManager	 mNetworkAccessManager;
	QString					 mUserSnippetsDirectory;	
};

#define gApp (static_cast<App *>(QCoreApplication::instance()))

#endif // APP_H

#ifndef GLOBAL_SIGNALS_H
#define GLOBAL_SIGNALS_H

#include <QObject>

#include "global.h"
#include "context_interface.h"

class GlobalPrivate;

FUGIO_NAMESPACE_BEGIN

class GlobalSignals : public QObject
{
	Q_OBJECT

	//friend class ::GlobalPrivate;

public:

signals:
	// This is the only point where Global time can be changed
	void frameInitialise( void );
	void frameInitialise( qint64 pTimeStamp );

	void frameStart( void );
	void frameStart( qint64 pTimeStamp );

	void frameEnd( void );
	void frameEnd( qint64 pTimeStamp );

	void fps( qreal pFPS );

	void contextAdded( QSharedPointer<fugio::ContextInterface> );
	void contextRemoved( QSharedPointer<fugio::ContextInterface> );

	void configLoad( QSettings &pSettings );
	void configSave( QSettings &pSettings ) const;

	void nodeClassAdded( const ClassEntry &pClassEntry );

protected:
	GlobalSignals( GlobalPrivate *p );

	virtual ~GlobalSignals( void );

private:
	GlobalPrivate		*mPrivate;
};

FUGIO_NAMESPACE_END

#endif // GLOBAL_SIGNALS_H

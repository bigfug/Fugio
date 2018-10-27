#ifndef CONTEXT_SIGNALS_H
#define CONTEXT_SIGNALS_H

#include <QObject>
#include <QSettings>
#include <QUuid>

#include "global.h"
#include "context_interface.h"

FUGIO_NAMESPACE_BEGIN

class ContextSignals : public QObject
{
	Q_OBJECT

signals:
	// This is the only point where Context time can be changed
	void frameInitialise( void );
	void frameInitialise( qint64 pTimeStamp );

	void frameStart( void );
	void frameStart( qint64 pTimeStamp );

	void frameProcess( void );
	void frameProcess( qint64 pTimeStamp );

	void frameFinalise( void );
	void frameFinalise( qint64 pTimeStamp );

	void frameEnd( void );
	void frameEnd( qint64 pTimeStamp );

	void aboutToPlay( void );

	void stateChanged( ContextInterface::TimeState pState );

	void playheadPosition( qreal pTimePrev, qreal pTimeCurr );

	void durationChanged( qreal pDuration );

	void filenameChanged( const QString &pFileName );

	void metaInfoChanged( ContextInterface::MetaInfo pMetaType, const QString &pMetaInfo );

	void activeStateChanged( bool pActive );

	void contextStart( void );

	void loadStart( QSettings &pSettings, bool pPartial );
	void loading( QSettings &pSettings, bool pPartial );
	void loadEnd( QSettings &pSettings, bool pPartial );

	void saveStart( QSettings &pSettings ) const;
	void saving( QSettings &pSettings ) const;
	void saveEnd( QSettings &pSettings ) const;

	void clearContext( void );

	void nodeAdded( QUuid pNodeId );
	void nodeRemoved( QUuid pNodeId );
	void nodeRenamed( QUuid pNodeId1, QUuid pNodeId2 );
	void nodeRelabled( QUuid pOriginalNodeId, QUuid pNewNodeId );
	void nodeUpdated( QUuid pNodeId );
	void nodeActivated( QUuid pNodeId );
	void nodeComplete( QUuid pNodeId );

	// Pin id's are PinInterface::globalId()

	void pinAdded( QUuid pNodeId, QUuid pPinId );
	void pinRenamed( QUuid pNodeId, QUuid pPinId1, QUuid pPinId2 );
	void pinRelabled( QUuid pNodeId, QUuid pOriginalPinId, QUuid pNewPinId );
	void pinRemoved( QUuid pNodeId, QUuid pPinId );

	void linkAdded( QUuid pPinId1, QUuid pPinId2 );
	void linkRemoved( QUuid pPinId1, QUuid pPinId2 );

protected:
	ContextSignals( QObject *pParent = Q_NULLPTR );

	virtual ~ContextSignals( void );
};

FUGIO_NAMESPACE_END

#endif // CONTEXT_SIGNALS_H

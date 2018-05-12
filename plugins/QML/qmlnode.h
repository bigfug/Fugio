#ifndef QMLCONNECTOR_H
#define QMLCONNECTOR_H

#include <QObject>
#include <QQmlListProperty>

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>

#include "qmlpin.h"

class QMLNode : public QObject
{
	Q_OBJECT

	Q_PROPERTY( QObject *control READ control )
//	Q_PROPERTY( QUuid globalId READ globalId )
	Q_PROPERTY( QString name READ name WRITE setName NOTIFY nameChanged )
	Q_PROPERTY( QQmlListProperty<QMLPin> ouputs READ outputs )

public:
	QMLNode( QSharedPointer<fugio::NodeInterface> pNode );

//	QMLNode( void ) {}

	virtual ~QMLNode( void ) {}

	Q_INVOKABLE QObject *input( const QString &pName );

	Q_INVOKABLE QObject *output( const QString &pName );

	Q_INVOKABLE void pinUpdated( const QString &pName );

	Q_INVOKABLE void pinUpdated( QObject *pObject );

    QQmlListProperty<QMLPin> outputs( void );

	// Not Q_INVOLKABLE to retain ownership of returned pointer

	QObject *control( void ) const;

	QString name() const;

signals:
	void inputsUpdated( qint64 timestamp );

	void nameChanged( QString name );

public slots:
	void signalInputsUpdated( qint64 pTimeStamp );

	void setName( QString name );

private slots:
	void signalNameChanged( QString name );

private:
	static QMLPin *outputIndex( QQmlListProperty<QMLPin> *pListProp, int pIndex );
	static int outputCount( QQmlListProperty<QMLPin> *pListProp );

private:
	QSharedPointer<fugio::NodeInterface>		 mNode;
};

#endif // QMLCONNECTOR_H

#ifndef QMLPIN_H
#define QMLPIN_H

#include <QObject>
#include <QQmlListProperty>

#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

class QMLPin : public QObject
{
	Q_OBJECT

	Q_PROPERTY( QObject *control READ control )
	Q_PROPERTY( QObject *connectedPin READ connectedPin )
	Q_PROPERTY( QVariant value READ value WRITE setValue NOTIFY valueChanged )
	Q_PROPERTY( QString name READ name WRITE setName NOTIFY nameChanged )
	Q_PROPERTY( int count READ count WRITE setCount )

public:
	explicit QMLPin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~QMLPin( void ) {}

	// QObjects returned here must deletable or marked as Cpp owned

	Q_INVOKABLE QObject *findInterface( const QString &pUUID );

	// Not Q_INVOLKABLE to retain ownership of returned pointer

	QObject *control( void ) const;

	QObject *connectedPin( void );

	QString name() const;

	QVariant value() const;

	Q_INVOKABLE bool isUpdated( qint64 pTimeStamp );

	Q_INVOKABLE QVariant variant( int pIndex = 0 );

	Q_INVOKABLE int variantCount( void );

	Q_INVOKABLE void setVariant( QVariant pValue );

	Q_INVOKABLE void setVariant( int pIndex, QVariant pValue );

	Q_INVOKABLE void trigger( void );

	int count( void );

	void setCount( int pCount );

signals:
	void nameChanged( QString name );

	void valueChanged( QVariant value );

	void updated( qint64 timestamp );

public slots:
	void setName( QString name );

	void setValue( QVariant value );

private slots:
	void signalNameChanged( QString name );

	void signalValueChanged( QVariant value );

private:
	QSharedPointer<fugio::PinInterface>		mPin;
};

#endif // QMLPIN_H

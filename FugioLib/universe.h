#ifndef UNIVERSE_H
#define UNIVERSE_H

#include <QObject>
#include <QMap>
#include <QByteArray>
#include <QList>
#include <QMutex>
#include <QUuid>

#include <fugio/global_interface.h>

#include <QUdpSocket>
#include <QHostAddress>

class Universe : public QObject
{
	Q_OBJECT
public:
	explicit Universe( QObject *pParent = nullptr );

	qint64 data( qint64 pTime, const QUuid &pUuid, QString &pName, QUuid &pType, QByteArray &pData ) const;

	QList<fugio::GlobalInterface::UniverseEntry> entries( void ) const
	{
		return( mEntries.values() );
	}

signals:

public slots:
	void addData( qint64 pTime, const QUuid &pUuid, const QString &pName, const QUuid &pType, const QByteArray &pData );

	void addPacket( const QByteArray &pArray )
	{
		mMutex.lock();

		mPackets.append( pArray );

		mMutex.unlock();
	}

	void clearData( qint64 pTime );

	void cast( qint64 pTime );

private slots:
	void updateCasters( void );

	void readyRead( void );

private:
	typedef struct InterfaceCaster
	{
		QUdpSocket			*mSocket;
        QHostAddress		 mAddress;
		int					 mPort;

        InterfaceCaster( QObject *pParent );

		InterfaceCaster( const InterfaceCaster &pIC )
			: mSocket( pIC.mSocket ), mAddress( pIC.mAddress ), mPort( pIC.mPort )
		{

		}

	} InterfaceCaster;

	typedef struct DataCast
	{
		qint64				mTime;
		QString				mName;
		QUuid				mUuid;
		QUuid				mType;
		QByteArray			mData;

		bool isValid( void ) const
		{
			return( mTime >= 0 && !mUuid.isNull() && !mType.isNull() && !mData.isEmpty() );
		}
	} DataCast;

	int								 mPort;

	QMap<int,InterfaceCaster>		 mCasters;

	QByteArrayList					 mPackets;

	mutable QMutex					 mMutex;

	QUdpSocket						*mSocket;

	QList<DataCast>					 mData;

	QMap<QUuid,fugio::GlobalInterface::UniverseEntry>				 mEntries;
};

#endif // UNIVERSE_H

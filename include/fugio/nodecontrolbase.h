#ifndef NODECONTROLBASE_H
#define NODECONTROLBASE_H

#include <QObject>
#include <QDebug>
#include <QSharedPointer>

#include <fugio/context_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/node_interface.h>
#include <fugio/pin_interface.h>

#include <fugio/fugio.h>
#include <fugio/core/variant_interface.h>

#define FUGID(x,y) 	static const QUuid x = QUuid( "{" y "}" );

FUGIO_NAMESPACE_BEGIN

class NodeControlBase : public QObject, public fugio::NodeControlInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::NodeControlInterface )

public:
	explicit NodeControlBase( QSharedPointer<fugio::NodeInterface> pNode )
		: mNode( pNode ), mPidIdx( 0 )
	{
		if( PID_UUID.isEmpty() )
		{
			PID_UUID.append( QUuid( "{9e154e12-bcd8-4ead-95b1-5a59833bcf4e}" ) );
			PID_UUID.append( QUuid( "{1b5e9ce8-acb9-478d-b84b-9288ab3c42f5}" ) );
			PID_UUID.append( QUuid( "{261cc653-d7fa-4c34-a08b-3603e8ae71d5}" ) );
			PID_UUID.append( QUuid( "{249f2932-f483-422f-b811-ab679f006381}" ) );
			PID_UUID.append( QUuid( "{ce8d578e-c5a4-422f-b3c4-a1bdf40facdb}" ) );
			PID_UUID.append( QUuid( "{e6bf944e-5f46-4994-bd51-13c2aa6415b7}" ) );
			PID_UUID.append( QUuid( "{a2bbf374-0dc8-42cb-b85a-6a43b58a348f}" ) );
			PID_UUID.append( QUuid( "{51297977-7b4b-4e08-9dea-89a8add4abe0}" ) );
			PID_UUID.append( QUuid( "{c997473a-2016-466b-9128-beacb99870a2}" ) );
			PID_UUID.append( QUuid( "{e27ce75e-fb9f-4a57-97b8-1c2d966f053b}" ) );
			PID_UUID.append( QUuid( "{3d995a98-0f56-4371-a1fa-67ecaefeba1c}" ) );
			PID_UUID.append( QUuid( "{d77f1053-0f88-4b4a-bfc6-57be3bb1eddd}" ) );
			PID_UUID.append( QUuid( "{1ceaeb2d-0463-44bc-9b80-10129cd8f4eb}" ) );
			PID_UUID.append( QUuid( "{5064e449-8b0b-4447-9009-c81997f754ef}" ) );
			PID_UUID.append( QUuid( "{5c8f8f4e-58ce-4e47-9e1e-4168d17e1863}" ) );
			PID_UUID.append( QUuid( "{a9b8d8d8-e4aa-4a99-b4eb-ea22f1945c63}" ) );
			PID_UUID.append( QUuid( "{da55f50c-1dee-47c9-bee5-6a2e48672a13}" ) );
			PID_UUID.append( QUuid( "{0fb3ba87-ff71-41bc-84ee-4f488a18068f}" ) );
			PID_UUID.append( QUuid( "{b13398e2-4e71-460f-a3b1-499ce9af224d}" ) );
			PID_UUID.append( QUuid( "{fe19f444-7a5f-47e6-ae83-ceb43b8ae915}" ) );
		}
	}

	virtual ~NodeControlBase( void ) {}

	//-------------------------------------------------------------------------
	// fugio::NodeInterface

	virtual QObject *qobject( void )
	{
		return( this );
	}

	virtual QWidget *gui( void )
	{
		return( 0 );
	}

	virtual QSharedPointer<fugio::NodeInterface> node( void )
	{
		return( mNode );
	}

	virtual void inputsUpdated( qint64 )
	{
	}

	virtual bool initialise( void )
	{
		return( true );
	}

	virtual bool deinitialise( void )
	{
		return( true );
	}

	virtual void loadSettings( QSettings & )
	{
	}

	virtual void saveSettings( QSettings & )
	{
	}

	void pinUpdated( QSharedPointer<fugio::PinInterface> &pPin )
	{
		mNode->context()->pinUpdated( pPin );
	}

	QSharedPointer<fugio::PinInterface> pinInput( const QString &pName )
	{
		return( mNode->createPin( pName, PIN_INPUT, next_uuid() ) );
	}

	QSharedPointer<fugio::PinInterface> pinInput( const QString &pName, const QUuid &pUuid )
	{
		return( mNode->createPin( pName, PIN_INPUT, pUuid ) );
	}

	template <class T> T pinInput( const QString &pName, QSharedPointer<fugio::PinInterface> &mPinInterface, const QUuid &pControlUUID )
	{
		Q_ASSERT( mPinInterface.isNull() );

		return( qobject_cast<T>( mNode->createPin( pName, PIN_INPUT, next_uuid(), mPinInterface, pControlUUID ) ) );
	}

	template <class T> T pinInput( const QString &pName, QSharedPointer<fugio::PinInterface> &mPinInterface, const QUuid &pControlUUID, const QUuid &pUuid )
	{
		Q_ASSERT( mPinInterface.isNull() );

		return( qobject_cast<T>( mNode->createPin( pName, PIN_INPUT, pUuid, mPinInterface, pControlUUID ) ) );
	}

	template <class T> T pinOutput( const QString &pName, QSharedPointer<fugio::PinInterface> &mPinInterface, const QUuid &pControlUUID )
	{
		Q_ASSERT( mPinInterface.isNull() );

		return( qobject_cast<T>( mNode->createPin( pName, PIN_OUTPUT, next_uuid(), mPinInterface, pControlUUID ) ) );
	}

	template <class T> T pinOutput( const QString &pName, QSharedPointer<fugio::PinInterface> &mPinInterface, const QUuid &pControlUUID, const QUuid &pUuid )
	{
		Q_ASSERT( mPinInterface.isNull() );

		return( qobject_cast<T>( mNode->createPin( pName, PIN_OUTPUT, pUuid, mPinInterface, pControlUUID ) ) );
	}

	template <class T> T input( QSharedPointer<fugio::PinInterface> &pPin )
	{
		if( !pPin->isConnected() || !pPin->connectedPin()->hasControl() )
		{
			return( 0 );
		}

		return( qobject_cast<T>( pPin->connectedPin()->control()->qobject() ) );
	}

	template <class T> T input( const QSharedPointer<fugio::PinInterface> &pPin ) const
	{
		if( !pPin->isConnected() || !pPin->connectedPin()->hasControl() )
		{
			return( 0 );
		}

		return( qobject_cast<T>( pPin->connectedPin()->control()->qobject() ) );
	}

	template <class T> T output( QSharedPointer<fugio::PinInterface> &pPin )
	{
		if( !pPin->isConnected() || !pPin->connectedPin()->hasControl() )
		{
			return( 0 );
		}

		return( qobject_cast<T>( pPin->connectedPin()->control()->qobject() ) );
	}

	template <class T> T output( const QSharedPointer<fugio::PinInterface> &pPin ) const
	{
		if( !pPin->isConnected() || !pPin->connectedPin()->hasControl() )
		{
			return( 0 );
		}

		return( qobject_cast<T>( pPin->connectedPin()->control()->qobject() ) );
	}

	static QVariant variantStatic( QSharedPointer<fugio::PinInterface> &pPin )
	{
		if( !pPin->isConnected() || !pPin->connectedPin()->hasControl() )
		{
			return( 0 );
		}

		VariantInterface	*V = qobject_cast<VariantInterface *>( pPin->connectedPin()->control()->qobject() );

		return( V ? V->variant() : pPin->value() );
	}

	QVariant variant( QSharedPointer<fugio::PinInterface> &pPin ) const
	{
		VariantInterface	*V = input<VariantInterface *>( pPin );

		return( V ? V->variant() : pPin->value() );
	}

	QVariant variant( const QSharedPointer<fugio::PinInterface> &pPin ) const
	{
		VariantInterface	*V = input<VariantInterface *>( pPin );

		return( V ? V->variant() : pPin->value() );
	}

	QUuid next_uuid( void )
	{
		Q_ASSERT( mPidIdx < PID_UUID.size() );

		return( PID_UUID[ mPidIdx++ ] );
	}

	virtual QStringList availableInputPins( void ) const
	{
		return( QStringList() );
	}

	virtual QList<AvailablePinEntry> availableOutputPins( void ) const
	{
		return( QList<NodeControlInterface::AvailablePinEntry>() );
	}

	virtual QString helpUrl( void ) const
	{
		return( QString() );
	}

	virtual QList<QUuid> pinAddTypesInput( void ) const
	{
		return( QList<QUuid>() );
	}

	virtual QList<QUuid> pinAddTypesOutput( void ) const
	{
		return( QList<QUuid>() );
	}

	virtual bool canAcceptPin( fugio::PinInterface *pPin ) const
	{
		Q_UNUSED( pPin )

		return( false );
	}

	virtual bool mustChooseNamedInputPin( void ) const
	{
		return( false );
	}

	virtual bool mustChooseNamedOutputPin( void ) const
	{
		return( false );
	}

protected:
	QSharedPointer<fugio::NodeInterface>	 mNode;
	int										 mPidIdx;
	static QList<QUuid>						 PID_UUID;

private:
	Q_DISABLE_COPY( NodeControlBase )
};

FUGIO_NAMESPACE_END

#endif // NODECONTROLBASE_H

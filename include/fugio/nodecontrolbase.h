#ifndef NODECONTROLBASE_H
#define NODECONTROLBASE_H

#include <QObject>
#include <QDebug>
#include <QSharedPointer>
#include <QList>

#include <fugio/context_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/node_interface.h>
#include <fugio/node_signals.h>
#include <fugio/pin_interface.h>
#include <fugio/paired_pins_helper_interface.h>

#include <fugio/fugio.h>
#include <fugio/core/variant_interface.h>

// Default wiki node link

#define WIKI_NODE_URL(x)	"http://wiki.bigfug.com/view/Nodes:" ## x

#define FUGID(x,y) 	static const QUuid x = QUuid( "{" y "}" );

// A section of copy and paste id's for updating legacy code based on PID_UUID

//FUGID( PIN_XXX_XXX, "9e154e12-bcd8-4ead-95b1-5a59833bcf4e" );
//FUGID( PIN_XXX_XXX, "1b5e9ce8-acb9-478d-b84b-9288ab3c42f5" );
//FUGID( PIN_XXX_XXX, "261cc653-d7fa-4c34-a08b-3603e8ae71d5" );
//FUGID( PIN_XXX_XXX, "249f2932-f483-422f-b811-ab679f006381" );
//FUGID( PIN_XXX_XXX, "ce8d578e-c5a4-422f-b3c4-a1bdf40facdb" );
//FUGID( PIN_XXX_XXX, "e6bf944e-5f46-4994-bd51-13c2aa6415b7" );
//FUGID( PIN_XXX_XXX, "a2bbf374-0dc8-42cb-b85a-6a43b58a348f" );
//FUGID( PIN_XXX_XXX, "51297977-7b4b-4e08-9dea-89a8add4abe0" );
//FUGID( PIN_XXX_XXX, "c997473a-2016-466b-9128-beacb99870a2" );
//FUGID( PIN_XXX_XXX, "e27ce75e-fb9f-4a57-97b8-1c2d966f053b" );
//FUGID( PIN_XXX_XXX, "3d995a98-0f56-4371-a1fa-67ecaefeba1c" );
//FUGID( PIN_XXX_XXX, "d77f1053-0f88-4b4a-bfc6-57be3bb1eddd" );
//FUGID( PIN_XXX_XXX, "1ceaeb2d-0463-44bc-9b80-10129cd8f4eb" );
//FUGID( PIN_XXX_XXX, "5064e449-8b0b-4447-9009-c81997f754ef" );
//FUGID( PIN_XXX_XXX, "5c8f8f4e-58ce-4e47-9e1e-4168d17e1863" );
//FUGID( PIN_XXX_XXX, "a9b8d8d8-e4aa-4a99-b4eb-ea22f1945c63" );
//FUGID( PIN_XXX_XXX, "da55f50c-1dee-47c9-bee5-6a2e48672a13" );
//FUGID( PIN_XXX_XXX, "0fb3ba87-ff71-41bc-84ee-4f488a18068f" );
//FUGID( PIN_XXX_XXX, "b13398e2-4e71-460f-a3b1-499ce9af224d" );
//FUGID( PIN_XXX_XXX, "fe19f444-7a5f-47e6-ae83-ceb43b8ae915" );

FUGIO_NAMESPACE_BEGIN

class NodeControlBase : public QObject, public fugio::NodeControlInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::NodeControlInterface )

public:
	explicit NodeControlBase( QSharedPointer<fugio::NodeInterface> pNode )
		: mNode( pNode ), mPidIdx( 0 ), mInitialisedCalled( false ), mDeinitialisedCalled( false )
	{
		// this will be going soon

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

	inline virtual QObject *qobject( void ) Q_DECL_OVERRIDE
	{
		return( this );
	}

	inline virtual QGraphicsItem *guiItem( void ) Q_DECL_OVERRIDE
	{
		return( Q_NULLPTR );
	}

	inline virtual QWidget *gui( void ) Q_DECL_OVERRIDE
	{
		return( Q_NULLPTR );
	}

	inline virtual QSharedPointer<fugio::NodeInterface> node( void ) Q_DECL_OVERRIDE
	{
		return( mNode );
	}

	inline virtual QSharedPointer<fugio::NodeInterface> node( void ) const Q_DECL_OVERRIDE
	{
		return( mNode );
	}

	inline virtual void inputsUpdated( qint64 ) Q_DECL_OVERRIDE
	{
	}

	virtual bool initialise( void ) Q_DECL_OVERRIDE
	{
		PairedPinsHelperInterface		*PPHI = qobject_cast<PairedPinsHelperInterface *>( this );

		if( PPHI )
		{
			pairedPinConnect();
		}

		mInitialisedCalled = true;

		return( true );
	}

	virtual bool deinitialise( void ) Q_DECL_OVERRIDE
	{
		PairedPinsHelperInterface		*PPHI = qobject_cast<PairedPinsHelperInterface *>( this );

		if( PPHI )
		{
			pairedPinDisconnect();
		}

		mDeinitialisedCalled = true;

		return( true );
	}

	inline virtual bool wasInitialiseCalled( void ) const Q_DECL_OVERRIDE
	{
		return( mInitialisedCalled );
	}

	inline virtual bool wasDeinitialiseCalled( void ) const Q_DECL_OVERRIDE
	{
		return( mDeinitialisedCalled );
	}

	inline virtual void loadSettings( QSettings & ) Q_DECL_OVERRIDE
	{
	}

	inline virtual void saveSettings( QSettings & ) const Q_DECL_OVERRIDE
	{
	}

	inline virtual QStringList availableInputPins( void ) const Q_DECL_OVERRIDE
	{
		return( QStringList() );
	}

	inline virtual QList<AvailablePinEntry> availableOutputPins( void ) const Q_DECL_OVERRIDE
	{
		return( QList<NodeControlInterface::AvailablePinEntry>() );
	}

	inline virtual QList<QUuid> pinAddTypesInput( void ) const Q_DECL_OVERRIDE
	{
		return( QList<QUuid>() );
	}

	inline virtual QList<QUuid> pinAddTypesOutput( void ) const Q_DECL_OVERRIDE
	{
		return( QList<QUuid>() );
	}

	inline virtual bool canAcceptPin( fugio::PinInterface * ) const Q_DECL_OVERRIDE
	{
		return( false );
	}

	inline virtual bool mustChooseNamedInputPin( void ) const Q_DECL_OVERRIDE
	{
		return( false );
	}

	inline virtual bool mustChooseNamedOutputPin( void ) const Q_DECL_OVERRIDE
	{
		return( false );
	}

	inline virtual QUuid pinAddControlUuid( fugio::PinInterface * ) const Q_DECL_OVERRIDE
	{
		return( QUuid() );
	}

	inline virtual bool pinShouldAutoRename( fugio::PinInterface * ) const Q_DECL_OVERRIDE
	{
		return( false );
	}

	//-------------------------------------------------------------------------

	void pinUpdated( QSharedPointer<fugio::PinInterface> &pPin, qint64 pGlobalTimestamp = -1 )
	{
		mNode->context()->pinUpdated( pPin, pGlobalTimestamp );
	}

	//-------------------------------------------------------------------------
	// support methods for creating input pins

	// dont use this version in new code

	QSharedPointer<fugio::PinInterface> pinInput( const QString &pName ) // soon... Q_DECL_DEPRECATED
	{
		return( mNode->createPin( pName, PIN_INPUT, QUuid::createUuid(), next_uuid() ) );
	}

	QSharedPointer<fugio::PinInterface> pinInput( const QString &pName, const QUuid &pLocalId )
	{
		return( mNode->createPin( pName, PIN_INPUT, QUuid::createUuid(), pLocalId ) );
	}

	template <class T> T pinInput( const QString &pName, QSharedPointer<fugio::PinInterface> &mPinInterface, const QUuid &pControlUUID ) // soon... Q_DECL_DEPRECATED
	{
		Q_ASSERT( mPinInterface.isNull() );

		return( qobject_cast<T>( mNode->createPin( pName, PIN_INPUT, QUuid::createUuid(), next_uuid(), mPinInterface, pControlUUID ) ) );
	}

	template <class T = fugio::PinInterface *> T pinInput( const QString &pName, QSharedPointer<fugio::PinInterface> &mPinInterface, const QUuid &pControlUUID, const QUuid &pLocalId )
	{
		Q_ASSERT( mPinInterface.isNull() );

		return( qobject_cast<T>( mNode->createPin( pName, PIN_INPUT, QUuid::createUuid(), pLocalId, mPinInterface, pControlUUID ) ) );
	}

	//-------------------------------------------------------------------------
	// support methods for creating output pins

	QSharedPointer<fugio::PinInterface> pinOutput( const QString &pName, const QUuid &pLocalId )
	{
		QSharedPointer<fugio::PinInterface>		PinInt;

		mNode->createPin( pName, PIN_OUTPUT, QUuid::createUuid(), pLocalId, PinInt, QUuid() );

		return( PinInt );
	}

	QSharedPointer<fugio::PinInterface> pinOutput( const QString &pName, const QUuid &pControlUUID, const QUuid &pLocalId )
	{
		QSharedPointer<fugio::PinInterface>		PinInt;

		mNode->createPin( pName, PIN_OUTPUT, QUuid::createUuid(), pLocalId, PinInt, pControlUUID );

		return( PinInt );
	}

	template <class T> T pinOutput( const QString &pName, QSharedPointer<fugio::PinInterface> &mPinInterface, const QUuid &pControlUUID ) // soon... Q_DECL_DEPRECATED
	{
		Q_ASSERT( mPinInterface.isNull() );

		return( qobject_cast<T>( mNode->createPin( pName, PIN_OUTPUT, QUuid::createUuid(), next_uuid(), mPinInterface, pControlUUID ) ) );
	}

	template <class T> T pinOutput( const QString &pName, QSharedPointer<fugio::PinInterface> &mPinInterface, const QUuid &pControlUUID, const QUuid &pLocalId )
	{
		Q_ASSERT( mPinInterface.isNull() );

		return( qobject_cast<T>( mNode->createPin( pName, PIN_OUTPUT, QUuid::createUuid(), pLocalId, mPinInterface, pControlUUID ) ) );
	}

	//-------------------------------------------------------------------------
	// template based access of connected pin controls

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

	template <class T> QList<T> outputs( QSharedPointer<fugio::PinInterface> &pPin )
	{
		QList<T>		CtlLst;

		for( QSharedPointer<fugio::PinInterface> P : pPin->connectedPins() )
		{
			if( !P->hasControl() )
			{
				continue;
			}

			T PinCtl = qobject_cast<T>( P->control()->qobject() );

			if( !PinCtl )
			{
				continue;
			}

			CtlLst << PinCtl;
		}

		return( CtlLst );
	}

	template <class T> T outputs( const QSharedPointer<fugio::PinInterface> &pPin ) const
	{
		QList<T>		CtlLst;

		for( QSharedPointer<fugio::PinInterface> P : pPin->connectedPins() )
		{
			if( !P->hasControl() )
			{
				continue;
			}

			T PinCtl = qobject_cast<T>( P->control()->qobject() );

			if( !PinCtl )
			{
				continue;
			}

			CtlLst << PinCtl;
		}

		return( CtlLst );
	}

	//-------------------------------------------------------------------------
	// Helper methods for accessing fugio::VariantInterface based connected pins

	static QVariant variantStatic( QSharedPointer<fugio::PinInterface> &pPin )
	{
		if( !pPin->isConnected() || !pPin->connectedPin()->hasControl() )
		{
			return( pPin->value() );
		}

		VariantInterface	*V = qobject_cast<VariantInterface *>( pPin->connectedPin()->control()->qobject() );

		return( V ? V->variant() : pPin->value() );
	}

	template <class T> static T variantStatic( QSharedPointer<fugio::PinInterface> &pPin )
	{
		if( !pPin->isConnected() || !pPin->connectedPin()->hasControl() )
		{
			return( pPin->value().value<T>() );
		}

		VariantInterface	*V = qobject_cast<VariantInterface *>( pPin->connectedPin()->control()->qobject() );

		return( V ? V->variant().value<T>() : pPin->value().value<T>() );
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

	template <class T> T variant( const QSharedPointer<fugio::PinInterface> &pPin ) const
	{
		VariantInterface	*V = input<VariantInterface *>( pPin );

		return( V ? V->variant().value<T>() : pPin->value().value<T>() );
	}

	//-------------------------------------------------------------------------
	// Helper methods for getting lists of inputs based on type

	template <class T> QList<T> enumInputs( void ) const
	{
		QList<T>		IntLst;

		for( QSharedPointer<fugio::PinInterface> P : mNode->enumInputPins() )
		{
			T	PinInt = input<T>( P );

			if( PinInt )
			{
				IntLst << PinInt;
			}
		}

		return( IntLst );
	}

	template <class T> QList<T> enumOutputs( void ) const
	{
		QList<T>		IntLst;

		for( QSharedPointer<fugio::PinInterface> P : mNode->enumOutputPins() )
		{
			T	PinInt = qobject_cast<T>( P->hasControl() ? P->control()->qobject() : nullptr );

			if( PinInt )
			{
				IntLst << PinInt;
			}
		}

		return( IntLst );
	}

	//-------------------------------------------------------------------------
	// fugio::VariantInterface helpers

	void variantSetCount( fugio::VariantInterface *I, int C, bool &U )
	{
		if( I->variantCount() != C )
		{
			I->setVariantCount( C );

			U = true;
		}
	}

	template <typename T> void variantSetValue( fugio::VariantInterface *I, int pIndex, const T &D, bool &U )
	{
		QVariant	V;

		V.setValue( D );

		if( I->variant( pIndex ) != V )
		{
			I->setVariant( pIndex, V );

			U = true;
		}
	}

	//-------------------------------------------------------------------------
	// Helper methods for working with pin pairs

private:
	void pairedPinConnect( void )
	{
		connect( mNode->qobject(), SIGNAL(pinAdded(QSharedPointer<fugio::PinInterface>)), this, SLOT(pairedPinAddedHelper(QSharedPointer<fugio::PinInterface>)) );
		connect( mNode->qobject(), SIGNAL(pinRemoved(QSharedPointer<fugio::PinInterface>)), this, SLOT(pairedPinRemovedHelper(QSharedPointer<fugio::PinInterface>)) );
	}

	void pairedPinDisconnect( void )
	{
		disconnect( mNode->qobject(), SIGNAL(pinAdded(QSharedPointer<fugio::PinInterface>)), this, SLOT(pairedPinAddedHelper(QSharedPointer<fugio::PinInterface>)) );
		disconnect( mNode->qobject(), SIGNAL(pinRemoved(QSharedPointer<fugio::PinInterface>)), this, SLOT(pairedPinRemovedHelper(QSharedPointer<fugio::PinInterface>)) );
	}

private slots:
	void pairedPinAddedHelper( QSharedPointer<fugio::PinInterface> pPin )
	{
		if( pPin->direction() == PIN_INPUT )
		{
			fugio::PairedPinsHelperInterface		*PPHI = qobject_cast<fugio::PairedPinsHelperInterface *>( this );

			if( !PPHI )
			{
				qWarning() << "Paired pin helper doesn't have fugio::PairedPinsHelperInterface as interface";

				return;
			}

			QSharedPointer<fugio::PinInterface> DstPin;

			if( !pPin->pairedUuid().isNull() )
			{
				DstPin = mNode->findPinByLocalId( pPin->pairedUuid() );

				if( DstPin )
				{
					return;
				}
			}

			if( !DstPin )
			{
				pinOutput<fugio::VariantInterface *>( pPin->name(), DstPin, PPHI->pairedPinControlUuid( pPin ), QUuid::createUuid() );

				if( DstPin )
				{
					mNode->pairPins( pPin, DstPin );
				}
			}
		}
	}

	void pairedPinRemovedHelper( QSharedPointer<fugio::PinInterface> pPin )
	{
		if( !pPin->pairedUuid().isNull() )
		{
			QSharedPointer<fugio::PinInterface>		PP = mNode->findPinByLocalId( pPin->pairedUuid() );

			if( PP )
			{
				mNode->removePin( PP );
			}
		}
	}

protected:
	//-------------------------------------------------------------------------
	// don't use this in new code!

	QUuid next_uuid( void ) // soon... Q_DECL_DEPRECATED
	{
		Q_ASSERT( mPidIdx < PID_UUID.size() );

		return( PID_UUID[ mPidIdx++ ] );
	}

protected:
	QSharedPointer<fugio::NodeInterface>	 mNode;

	// don't use this in new code

	int										 mPidIdx;
	static QList<QUuid>						 PID_UUID;

private:
	bool									 mInitialisedCalled;
	bool									 mDeinitialisedCalled;

private:
	Q_DISABLE_COPY( NodeControlBase )
};

FUGIO_NAMESPACE_END

#endif // NODECONTROLBASE_H

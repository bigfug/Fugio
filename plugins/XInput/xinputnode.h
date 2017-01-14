#ifndef XINPUTNODE_H
#define XINPUTNODE_H

#include <QObject>

#include <fugio/xinput/uuid.h>

#include <fugio/nodecontrolbase.h>
#include <fugio/pin_interface.h>
#include <fugio/core/variant_interface.h>

#if defined( XINPUT_SUPPORTED )
#include <windows.h>
#include <Xinput.h>
#endif

class XInputNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Reads the current state of an XInput controller" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Xinput" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit XInputNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~XInputNode( void );

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual bool initialise( void ) Q_DECL_OVERRIDE;

protected slots:
	void frameStart( void );

private:
#if defined( XINPUT_SUPPORTED )
	XINPUT_STATE		mLastState;
#endif

	QSharedPointer<fugio::PinInterface>		 mPinInputIndex;

	QSharedPointer<fugio::PinInterface>		 mPinStickLeftX;
	fugio::VariantInterface					*mStickLeftX;

	QSharedPointer<fugio::PinInterface>		 mPinStickLeftY;
	fugio::VariantInterface					*mStickLeftY;

	QSharedPointer<fugio::PinInterface>		 mPinStickLeftM;
	fugio::VariantInterface					*mStickLeftM;

	QSharedPointer<fugio::PinInterface>		 mPinStickRightX;
	fugio::VariantInterface					*mStickRightX;

	QSharedPointer<fugio::PinInterface>		 mPinStickRightY;
	fugio::VariantInterface					*mStickRightY;

	QSharedPointer<fugio::PinInterface>		 mPinStickRightM;
	fugio::VariantInterface					*mStickRightM;

	QSharedPointer<fugio::PinInterface>		 mPinButtonA;
	fugio::VariantInterface					*mButtonA;

	QSharedPointer<fugio::PinInterface>		 mPinButtonB;
	fugio::VariantInterface					*mButtonB;

	QSharedPointer<fugio::PinInterface>		 mPinButtonX;
	fugio::VariantInterface					*mButtonX;

	QSharedPointer<fugio::PinInterface>		 mPinButtonY;
	fugio::VariantInterface					*mButtonY;

	QSharedPointer<fugio::PinInterface>		 mPinDPadUp;
	fugio::VariantInterface					*mDPadUp;

	QSharedPointer<fugio::PinInterface>		 mPinDPadRight;
	fugio::VariantInterface					*mDPadRight;

	QSharedPointer<fugio::PinInterface>		 mPinDPadDown;
	fugio::VariantInterface					*mDPadDown;

	QSharedPointer<fugio::PinInterface>		 mPinDPadLeft;
	fugio::VariantInterface					*mDPadLeft;

	QSharedPointer<fugio::PinInterface>		 mPinTriggerL;
	fugio::VariantInterface					*mTriggerL;

	QSharedPointer<fugio::PinInterface>		 mPinTriggerR;
	fugio::VariantInterface					*mTriggerR;

	QSharedPointer<fugio::PinInterface>		 mPinStart;
	fugio::VariantInterface					*mStart;

	QSharedPointer<fugio::PinInterface>		 mPinBack;
	fugio::VariantInterface					*mBack;

	QSharedPointer<fugio::PinInterface>		 mPinThumbL;
	fugio::VariantInterface					*mThumbL;

	QSharedPointer<fugio::PinInterface>		 mPinThumbR;
	fugio::VariantInterface					*mThumbR;

	QSharedPointer<fugio::PinInterface>		 mPinShoulderL;
	fugio::VariantInterface					*mShoulderL;

	QSharedPointer<fugio::PinInterface>		 mPinShoulderR;
	fugio::VariantInterface					*mShoulderR;
};

#endif // XINPUTNODE_H

#ifndef ENVELOPENODE_H
#define ENVELOPENODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>
#include <fugio/core/variant_interface.h>
#include <fugio/paired_pins_helper_interface.h>

class EnvelopeNode : public fugio::NodeControlBase, public fugio::PairedPinsHelperInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::PairedPinsHelperInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Models a value envelope" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Envelope" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE EnvelopeNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~EnvelopeNode( void ) {}

	// NodeControlInterface interface

	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

	virtual QList<QUuid> pinAddTypesInput() const Q_DECL_OVERRIDE;
	virtual bool canAcceptPin(fugio::PinInterface *pPin) const Q_DECL_OVERRIDE;

	// PairedPinsHelperInterface interface
public:
	virtual QUuid pairedPinControlUuid(QSharedPointer<fugio::PinInterface> pPin) const Q_DECL_OVERRIDE;

protected slots:
	void contextFrame( qint64 pTimeStamp );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinAttack;
	QSharedPointer<fugio::PinInterface>			 mPinDecay;
	QSharedPointer<fugio::PinInterface>			 mPinSustain;
	QSharedPointer<fugio::PinInterface>			 mPinRelease;

	typedef enum EnvelopeState
	{
		START, ATTACK, DECAY, SUSTAIN, RELEASE
	} EnvelopeState;

	EnvelopeState								 mEnvState;

	typedef struct PinInf
	{
		bool									 mInputState;
		qint64									 mInputTime;
		float									 mValue;
		float									 mValueAtSwitch;

		PinInf( void ) : mInputState( false ), mInputTime( 0 ), mValue( 0 ), mValueAtSwitch( 0 ) {}
	} PinInf;

	QMap<QUuid,PinInf>							 mPinInf;
};

#endif // ENVELOPENODE_H

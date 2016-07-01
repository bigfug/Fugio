#ifndef BEATTAPNODE_H
#define BEATTAPNODE_H

#include <QObject>

#include <fugio/core/variant_interface.h>

#include <fugio/nodecontrolbase.h>

class BeatTapNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Calculates the beats per minute (BPM) tempo from a series of input triggers" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Beat_Tap" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit BeatTapNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~BeatTapNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual bool initialise( void ) Q_DECL_OVERRIDE;

	virtual void loadSettings( QSettings &pSettings ) Q_DECL_OVERRIDE;
	virtual void saveSettings( QSettings &pSettings ) const Q_DECL_OVERRIDE;

private slots:
	void frameStart( qint64 pTimeStamp );

private:
	QSharedPointer<fugio::PinInterface>	 mPinTrigger;

	QSharedPointer<fugio::PinInterface>	 mPinBPM;
	fugio::VariantInterface				*mValBPM;

	QSharedPointer<fugio::PinInterface>	 mPinSync;
	fugio::VariantInterface				*mValSync;

	QSharedPointer<fugio::PinInterface>	 mPinBeat;

	QList<qint64>					 mBeatTime;

	double							 mBeatDuration;
	double							 mBeatStart;
	double							 mBeatSync;
	double							 mBeatLast;
};

#endif // BEATTAPNODE_H

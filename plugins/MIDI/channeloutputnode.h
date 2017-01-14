#ifndef CHANNELNODE_H
#define CHANNELNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>
#include <fugio/core/variant_interface.h>
#include <fugio/midi/midi_interface.h>
#include <fugio/choice_interface.h>

class ChannelOutputNode : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Midi_Channel_Input" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE ChannelOutputNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~ChannelOutputNode( void ) {}

	// NodeControlInterface interface

	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

	virtual QList<QUuid> pinAddTypesInput() const Q_DECL_OVERRIDE;

	virtual bool canAcceptPin( fugio::PinInterface *pPin ) const Q_DECL_OVERRIDE;

	virtual QStringList availableInputPins() const Q_DECL_OVERRIDE;
	virtual bool mustChooseNamedInputPin() const Q_DECL_OVERRIDE;

private slots:
	void frameFinalised( qint64 pTimeStamp );

	void pinAdded( QSharedPointer<fugio::PinInterface> pPin );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputChannel;

	QSharedPointer<fugio::PinInterface>			 mPinInputProgram;
	fugio::ChoiceInterface						*mValInputProgram;

	QSharedPointer<fugio::PinInterface>			 mPinOutputMidi;
	fugio::MidiInterface						*mValOutputMidi;

	qint64										 mLastUpdate;

	static QStringList							 PrgLst;
};


#endif // CHANNELNODE_H

#include "midioutputpin.h"

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>

QDataStream &operator <<( QDataStream &DS, const fugio::MidiEvent &ME )
{
	DS << ME.timestamp << ME.message;

	return( DS );
}

QDataStream &operator >>( QDataStream &DS, fugio::MidiEvent &ME )
{
	DS >> ME.timestamp >> ME.message;

	return( DS );
}

MidiOutputPin::MidiOutputPin( QSharedPointer<fugio::PinInterface> pPin )
	: PinControlBase( pPin )
{

}

void MidiOutputPin::registerMetaType()
{
	qRegisterMetaTypeStreamOperators<fugio::MidiEvent>( "fugio::MidiEvent" );
}

void MidiOutputPin::addMessages( const QVector<fugio::MidiEvent> &pMessages )
{
	mMessages << pMessages;
}

QVector<fugio::MidiEvent> MidiOutputPin::messages() const
{
	return( mMessages );
}

QByteArray MidiOutputPin::systemExclusive() const
{
	return( mSysEx );
}

void MidiOutputPin::clearData()
{
	mMessages.clear();

	mSysEx.clear();
}

void MidiOutputPin::serialise( QDataStream &pDataStream ) const
{
	pDataStream << mMessages;
	pDataStream << mSysEx;
}

void MidiOutputPin::deserialise( QDataStream &pDataStream )
{
	pDataStream >> mMessages;
	pDataStream >> mSysEx;
}

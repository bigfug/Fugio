#ifndef FIRMATADECODENODE_H
#define FIRMATADECODENODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>
#include <fugio/core/variant_interface.h>

class FirmataNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "First Example" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Firmata" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

	void updateDigitalReport();

public:
	Q_INVOKABLE FirmataNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~FirmataNode( void ) {}

	// NodeControlInterface interface

	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

	virtual QWidget *gui() Q_DECL_OVERRIDE;

protected:
	void processSysEx( const QByteArray &pSysEx );

	void setAnalogValue( int pPinIdx, int pPinVal );
	void setDigitalValue( int pPinIdx, bool pPinVal );

	void beginPinStateQuery( void );
	void nextPinStateQuery( void );

protected slots:
	void pinRemoved( QSharedPointer<fugio::PinInterface> P );

	void pinLinked( QSharedPointer<fugio::PinInterface> S, QSharedPointer<fugio::PinInterface> D );
	void pinUnlinked( QSharedPointer<fugio::PinInterface> S, QSharedPointer<fugio::PinInterface> D );

	void editPins( void );

	void contextFrameProcess( qint64 pTimeStamp );
	void contextFrameFinalise( qint64 pTimeStamp );

	void addPin( int pIdx, int pType );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputTrigger;

	QSharedPointer<fugio::PinInterface>			 mPinInputData;
	QSharedPointer<fugio::PinInterface>			 mPinInputReset;

	QSharedPointer<fugio::PinInterface>			 mPinOutputData;
	fugio::VariantInterface						*mValOutputData;

	typedef QPair<QSharedPointer<fugio::PinInterface>,fugio::VariantInterface*>		PinOut;
	typedef QMap<int,PinOut>														PinMapOutput;
	typedef QMap<int,QSharedPointer<fugio::PinInterface>>			PinMapInput;

	PinMapInput									 mPinMapInput;
	PinMapOutput								 mPinMapOutput;

	QList<int>									 mPinIdxList;
	QMap<int,int>								 mPinMapType;
	QMultiMap<int,QPair<int,int>>				 mPinMapData;
	QByteArray									 mPinAnalogMap;
	QStringList									 mPinNames;

	qint64										 mLastResponse;

	QByteArray									 mBuffer;
	bool										 mInSysEx;
	quint8										 mMessageType;
	quint8										 mMessageChannel;
	quint8										 mMessageData1;
	quint8										 mMessageData2;
	quint8										 mMessageCount;

	quint8										 mPinStateQueryIdx;

	quint16										 mDigitalOutputValues;
	quint16										 mDigitalOutputReport;

	QByteArray									 mOutputCommands;

	qint64										 mLastUpdate;
};

#endif // FIRMATADECODENODE_H

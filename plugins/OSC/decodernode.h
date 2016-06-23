#ifndef DECODERNODE_H
#define DECODERNODE_H

#include <fugio/nodecontrolbase.h>
#include <fugio/osc/uuid.h>
#include <fugio/osc/split_interface.h>
#include <fugio/osc/namespace_interface.h>

#include "namespacepin.h"

class DecoderNode : public fugio::NodeControlBase, public fugio::osc::NamespaceInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::osc::NamespaceInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Decoder_OSC" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit DecoderNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~DecoderNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

public:
	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QList<AvailablePinEntry> availableOutputPins() const Q_DECL_OVERRIDE;
	virtual QList<QUuid> pinAddTypesOutput() const Q_DECL_OVERRIDE;
	virtual bool canAcceptPin(fugio::PinInterface *pPin) const Q_DECL_OVERRIDE;

	//-------------------------------------------------------------------------
	// NamespaceInterface interface
public:
	virtual QStringList oscNamespace() Q_DECL_OVERRIDE;
	virtual QList<fugio::NodeControlInterface::AvailablePinEntry> oscPins(const QStringList &pCurDir) const Q_DECL_OVERRIDE;

private:
	void processByteArray( const QByteArray pByteArray );
	void processDatagram( const QByteArray &pDatagram );
	void processBundle( const QByteArray &pDatagram );

private:
	QSharedPointer<fugio::PinInterface>			 mPinInput;

	QSharedPointer<fugio::PinInterface>			 mPinOutputNamespace;
	NamespacePin								*mValOutputNamespace;

	QHash<QString,QVariantList>					 mDataInput;
};

#endif // DECODERNODE_H

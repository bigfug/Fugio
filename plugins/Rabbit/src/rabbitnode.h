#ifndef GAMEPADNODE_H
#define GAMEPADNODE_H

#include <QObject>

#include <QMutex>

#include <fugio/nodecontrolbase.h>

#include <fugio/core/variant_interface.h>

#include "rcp.h"

#include <fugio/paired_pins_helper_interface.h>

class RabbitNode : public fugio::NodeControlBase, public fugio::PairedPinsHelperInterface, protected rcp::ServerTransporter
{
	Q_OBJECT
	Q_INTERFACES( fugio::PairedPinsHelperInterface )

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Rabbit" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Rabbit" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE RabbitNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~RabbitNode( void ) Q_DECL_OVERRIDE {}

	// NodeControlInterface interface

	virtual bool initialise() Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QList<QUuid> pinAddTypesOutput() const Q_DECL_OVERRIDE;

	virtual bool canAcceptPin( fugio::PinInterface *pPin ) const Q_DECL_OVERRIDE;

	virtual bool pinShouldAutoRename( fugio::PinInterface *pPin ) const Q_DECL_OVERRIDE;

	// PairedPinsHelperInterface interface
public:
	virtual QUuid pairedPinControlUuid(QSharedPointer<fugio::PinInterface> pPin) const Q_DECL_OVERRIDE;

protected slots:
	void pinAdded( QSharedPointer<fugio::NodeInterface> pNode, QSharedPointer<fugio::PinInterface> pPin );
	void pinRemoved( QSharedPointer<fugio::NodeInterface> pNode, QSharedPointer<fugio::PinInterface> pPin );
	void pinLinked( QSharedPointer<fugio::PinInterface> pPinSrc, QSharedPointer<fugio::PinInterface> pPinDst );
	void pinUnlinked( QSharedPointer<fugio::PinInterface> pPinSrc, QSharedPointer<fugio::PinInterface> pPinDst );

	void frameFinalise( void );

protected:
	rcp::ParameterServer						 mParamServer;

	QSharedPointer<fugio::PinInterface>			 mPinInputData;

	QSharedPointer<fugio::PinInterface>			 mPinOutputData;
	fugio::VariantInterface						*mValOutputData;

	QMap<QUuid,int>								 mOutputParamMap;

	// ServerTransporter interface
public:
	virtual void bind(int port) Q_DECL_OVERRIDE;
	virtual void unbind() Q_DECL_OVERRIDE;
	virtual void sendToOne(std::istream &data, void *id) Q_DECL_OVERRIDE;
	virtual void sendToAll(std::istream &data, void *excludeId) Q_DECL_OVERRIDE;
	virtual int getConnectionCount() Q_DECL_OVERRIDE;
};

#endif // GAMEPADNODE_H

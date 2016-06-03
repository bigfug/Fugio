#ifndef DUPLICATEPINSNODE_H
#define DUPLICATEPINSNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

#include <fugio/core/variant_interface.h>

class DuplicatePinsNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Duplicates an input pin to multiple outputs" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Duplicate_Pins" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE DuplicatePinsNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~DuplicatePinsNode( void ) {}

	// NodeControlInterface interface

	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual bool canAcceptPin( fugio::PinInterface *pPin ) const Q_DECL_OVERRIDE;

private slots:
	void pinAdded( QSharedPointer<fugio::NodeInterface> pNode, QSharedPointer<fugio::PinInterface> pPin );

	void inputLinked( QSharedPointer<fugio::PinInterface> pNewPin );
	void inputUnlinked( QSharedPointer<fugio::PinInterface> pOldPin );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInput;
	QSharedPointer<fugio::PinInterface>			 mPinOutput;
};


#endif // DUPLICATEPINSNODE_H

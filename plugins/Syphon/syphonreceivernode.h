#ifndef SYPHONRECEIVERNODE_H
#define SYPHONRECEIVERNODE_H

#include "opengl_includes.h"

#include <QObject>

#include <fugio/nodecontrolbase.h>

class SyphonPin;

class SyphonReceiverNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Syphon_Receiver" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE SyphonReceiverNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~SyphonReceiverNode( void ) {}

	// InterfaceNodeControl interface
public:
	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QWidget *gui() Q_DECL_OVERRIDE;

protected slots:
	void onContextFrame( void );

	void selectServer( void );

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputName;

	QSharedPointer<fugio::PinInterface>			 mPinOutputTexture;
	SyphonPin									*mValOutputTexture;

	QSharedPointer<fugio::PinInterface>			 mPinOutputName;
	fugio::VariantInterface						*mValOutputName;
};

#endif // SYPHONRECEIVERNODE_H

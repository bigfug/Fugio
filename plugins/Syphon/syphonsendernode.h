#ifndef SYPHONSENDERNODE_H
#define SYPHONSENDERNODE_H

#include "opengl_includes.h"

#include <QObject>

#include <fugio/nodecontrolbase.h>

#include "syphonsender.h"

class SyphonSenderNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Syphon_Sender" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE SyphonSenderNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~SyphonSenderNode( void ) {}

	// InterfaceNodeControl interface
public:
	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputTexture;
	QSharedPointer<fugio::PinInterface>			 mPinInputName;

#if defined( SYPHON_SUPPORTED )
	SyphonSender								 mSender;
#endif

	char										 mName[ 256 ];
	quint32										 mWidth;
	quint32										 mHeight;
};


#endif // SYPHONSENDERNODE_H

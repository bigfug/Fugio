#ifndef SYPHONRENDERNODE_H
#define SYPHONRENDERNODE_H

#include "opengl_includes.h"

#include <QObject>

#include <fugio/nodecontrolbase.h>

#include "syphonsender.h"

class SyphonRenderNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Syphon_Render" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE SyphonRenderNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~SyphonRenderNode( void ) {}

	// InterfaceNodeControl interface
public:
	virtual bool initialise() Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputRender;
	QSharedPointer<fugio::PinInterface>			 mPinInputName;
	QSharedPointer<fugio::PinInterface>			 mPinInputSize;

#if defined( SYPHON_SUPPORTED )
	SyphonSender								 mSender;
#endif
};

#endif // SYPHONRENDERNODE_H

#ifndef PARAMLOADNODE_H
#define PARAMLOADNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

#include <fugio/artoolkit/param_interface.h>

class ParamLoadNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "arParamLoad" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE ParamLoadNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~ParamLoadNode( void ) {}

	// NodeControlInterface interface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

protected:
	QSharedPointer<fugio::PinInterface>			 mPinInputFilename;

	QSharedPointer<fugio::PinInterface>			 mPinOutputParam;
	fugio::ar::ParamInterface					*mValOutputParam;
};

#endif // PARAMLOADNODE_H

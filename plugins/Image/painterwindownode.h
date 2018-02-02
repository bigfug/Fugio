#ifndef PAINTERWINDOWNODE_H
#define PAINTERWINDOWNODE_H

#include <fugio/nodecontrolbase.h>

#include <fugio/image/image.h>
#include <fugio/core/variant_interface.h>

#include "painterwindow.h"

class PainterWindowNode : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Painter Window" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Painter_(Window)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit PainterWindowNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~PainterWindowNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual bool initialise() Q_DECL_OVERRIDE;
	virtual bool deinitialise() Q_DECL_OVERRIDE;

	virtual QWidget *gui( void ) Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputImage;
	QSharedPointer<fugio::PinInterface>			 mPinInputGeometry;

	PainterWindow								*mPainterWindow;
};


#endif // PAINTERWINDOWNODE_H

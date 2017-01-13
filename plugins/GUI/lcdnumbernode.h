#ifndef LCDNUMBERNODE_H
#define LCDNUMBERNODE_H

#include <QObject>

#include <fugio/core/variant_interface.h>
#include <fugio/nodecontrolbase.h>

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/uuid.h>

class LcdNumberNode : public fugio::NodeControlBase
{
	Q_OBJECT
	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Simple LCD number display" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "LCD_Number_(GUI)" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit LcdNumberNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~LcdNumberNode( void );

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual bool initialise( void );

	virtual QWidget *gui( void );

	virtual void inputsUpdated( qint64 pTimeStamp );

signals:
	void valueUpdated( double pValue );

private:
	QSharedPointer<fugio::PinInterface>			 mPinInput;
	double									 mValInput;
};


#endif // LCDNUMBERNODE_H

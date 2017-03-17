#ifndef LOGGERNODE_H
#define LOGGERNODE_H

#include <QObject>

#include <fugio/nodecontrolbase.h>

class LoggerNode : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Writes the input string to the log file" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "Logger" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit LoggerNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~LoggerNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputString;
};

#endif // LOGGERNODE_H

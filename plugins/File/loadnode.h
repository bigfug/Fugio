#ifndef LOADNODE_H
#define LOADNODE_H

#include <QObject>
#include <QDateTime>

#include <fugio/file/filename_interface.h>

#include <fugio/core/uuid.h>

#include <fugio/nodecontrolbase.h>

class LoadNode : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Loads a file into a Byte Array" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Load" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit LoadNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~LoadNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface interface
public:
	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

private:
	QSharedPointer<fugio::PinInterface>			 mPinInputTrigger;
	QSharedPointer<fugio::PinInterface>			 mPinInputFilename;

	QSharedPointer<fugio::PinInterface>			 mPinOutputByteArray;
	fugio::VariantInterface						*mValOutputByteArray;

	QString										 mLastFilename;
	QDateTime									 mLastModified;
};

#endif // LOADNODE_H

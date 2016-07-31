#ifndef FILENAMENODE_H
#define FILENAMENODE_H

#include <QObject>

#include <fugio/file/filename_interface.h>

#include <fugio/core/uuid.h>

#include <fugio/nodecontrolbase.h>

class FilenameNode : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "GUI helper node to choose a filename" )
	Q_CLASSINFO( "URL", "http://wiki.bigfug.com/Filename" )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit FilenameNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~FilenameNode( void ) {}

	//-------------------------------------------------------------------------
	// NodeControlInterface interface
public:
	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual QWidget *gui( void ) Q_DECL_OVERRIDE;

protected slots:
	void onClick( void );

private:
	QSharedPointer<fugio::PinInterface>			 mPinFilename;
	fugio::FilenameInterface					*mValFilename;
};

#endif // FILENAMENODE_H

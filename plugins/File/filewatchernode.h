#ifndef FILEWATCHERNODE_H
#define FILEWATCHERNODE_H

#include <QObject>
#include <QDir>
#include <QFileSystemWatcher>
#include <QTemporaryFile>
#include <QProcess>

#include <fugio/node_interface.h>
#include <fugio/node_control_interface.h>
#include <fugio/pin_interface.h>
#include <fugio/pin_control_interface.h>

#include <fugio/core/variant_interface.h>

#include <fugio/core/uuid.h>

#include <fugio/nodecontrolbase.h>

class FileWatcherNode : public fugio::NodeControlBase
{
	Q_OBJECT

	Q_CLASSINFO( "Author", "Alex May" )
	Q_CLASSINFO( "Version", "1.0" )
	Q_CLASSINFO( "Description", "Loads the contents of a file, reloading if changes are detected" )
	Q_CLASSINFO( "URL", WIKI_NODE_URL( "File_Watcher" ) )
	Q_CLASSINFO( "Contact", "http://www.bigfug.com/contact/" )

public:
	Q_INVOKABLE explicit FileWatcherNode( QSharedPointer<fugio::NodeInterface> pNode );

	virtual ~FileWatcherNode( void );

	//-------------------------------------------------------------------------
	// NodeControlInterface

	virtual QWidget *gui( void ) Q_DECL_OVERRIDE;

	virtual void inputsUpdated( qint64 pTimeStamp ) Q_DECL_OVERRIDE;

	virtual bool initialise( void ) Q_DECL_OVERRIDE;

	virtual void loadSettings( QSettings &pSettings ) Q_DECL_OVERRIDE;

	virtual void saveSettings( QSettings &pSettings ) const Q_DECL_OVERRIDE;

private slots:
	void onEditClicked( void );

	void onFileUpdate( QString pFileName );

private:
	QSharedPointer<fugio::PinInterface>			 mPinString;
	fugio::VariantInterface						*mPinStringInterface;

	QFileSystemWatcher						mWatcher;
	QProcess								mProcess;
	QTemporaryFile							mTempFile;
};

#endif // FILEWATCHERNODE_H

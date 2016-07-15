#ifndef FILENAMEPIN_H
#define FILENAMEPIN_H

#include <QObject>

#include <fugio/core/uuid.h>

#include <fugio/pincontrolbase.h>

#include <fugio/file/filename_interface.h>

class FilenamePin : public fugio::PinControlBase, public fugio::FilenameInterface
{
	Q_OBJECT
	Q_INTERFACES( fugio::FilenameInterface )

public:
	Q_INVOKABLE explicit FilenamePin( QSharedPointer<fugio::PinInterface> pPin );

	virtual ~FilenamePin( void ) {}

	//-------------------------------------------------------------------------
	// fugio::PinControlInterface

	virtual QString toString( void ) const
	{
		return( mFilename );
	}

	virtual QString description( void ) const
	{
		return( "Filename" );
	}

	virtual void loadSettings(QSettings &pSettings);
	virtual void saveSettings(QSettings &pSettings) const;

	//-------------------------------------------------------------------------
	// InterfaceFilename

	virtual QString filename( void ) const
	{
		return( mFilename );
	}

	virtual void setFilename( const QString &pFilename )
	{
		mFilename = pFilename;
	}

private:
	QString		mFilename;
};

#endif // FILENAMEPIN_H
